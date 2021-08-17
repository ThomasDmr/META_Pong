#include "VL53L0XSensor.h"
#include "CricController.h"
#include "system_parameters.h"

// Defini les 4 capteurs de distance (pin, intervale entre chaque mesure en ms)
VL53L0XSensor sensor1(XSHUT_1, MEASUREMENT_INTERVAL);
VL53L0XSensor sensor2(XSHUT_2, MEASUREMENT_INTERVAL);
VL53L0XSensor sensor3(XSHUT_3, MEASUREMENT_INTERVAL);
VL53L0XSensor sensor4(XSHUT_4, MEASUREMENT_INTERVAL);

// Défini les 4 controleurs de cric (pin relai air IN, relai air OUT, nom de l'objet)
CricController Cric1(IN_1_CTRL, OUT_1_CTRL, "C1");
CricController Cric2(IN_2_CTRL, OUT_2_CTRL, "C2");
CricController Cric3(IN_3_CTRL, OUT_3_CTRL, "C3");
CricController Cric4(IN_4_CTRL, OUT_4_CTRL, "C4");

// Défini les position souhaitées de chaque cric en variable globale
int desiredPosCric1 = 0;
int desiredPosCric2 = 0;
int desiredPosCric3 = 0;
int desiredPosCric4 = 0;

// Définition de booléens d'état permettant d'activer ou désactiver certaines options
bool printPosition = false;
bool continuousRandomMovement = true; // !!! Mettre à false si on ne veut pas un démarrage automatique

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    // Configure les adresses i2c des capteurs de distance et initialise les objets
    configureDistanceSensors();

    int seed = analogRead(A7); // A7 est flottant
    randomSeed(seed);

    // Initialise les objets cric et associe un capteur de distance
    Cric1.init(sensor1);
    Cric2.init(sensor2);
    Cric3.init(sensor3);
    Cric4.init(sensor4);

    // Définition des distances min et max de chaque cric
    Cric1.setMaxPosDistance(MAX_DISTANCE_1);
    Cric1.setMinPosDistance(MIN_DISTANCE_1);
    Cric2.setMaxPosDistance(MAX_DISTANCE_2);
    Cric2.setMinPosDistance(MIN_DISTANCE_2);
    Cric3.setMaxPosDistance(MAX_DISTANCE_3);
    Cric3.setMinPosDistance(MIN_DISTANCE_3);
    Cric4.setMaxPosDistance(MAX_DISTANCE_4);
    Cric4.setMinPosDistance(MIN_DISTANCE_4);
}

// !!! Le loop est asynchrone, ne pas mettre des fonction bloquantes !!!! 
void loop() 
{
    // A laisser dans le loop pour actualiser la lecture des distances
    Cric1.updatePosition(printPosition);
    Cric2.updatePosition(printPosition);
    Cric3.updatePosition(printPosition);
    Cric4.updatePosition(printPosition);

    checkForSerialOrder();

    if(continuousRandomMovement)
    {
        runRandomMovements(false);
    }
    else 
    {
        if(moveTable(false))
        {
            Serial.println(String(millis()) + "\tPosition reached !");
        }
    }

    bool relayMovement = Cric1.relayMovementDetected() | Cric2.relayMovementDetected() | Cric3.relayMovementDetected() | Cric4.relayMovementDetected();
    
    if(relayMovement)
    {
        if(configureDistanceSensors() != 0)
        {
            if(configureDistanceSensors() != 0)
            {
                // On laisse deux tentatives de reconfiguration
                // Arrête les déplacements et ferme toutes les valves
                setTablePosition(0, 0, 0, 0);
                Cric1.stop();
                Cric2.stop();
                Cric3.stop();
                Cric4.stop();
                continuousRandomMovement = false;

            }
        }
    }
}

//====================================================================================
//==========================  OVERAL MOVEMENT FUNCTIONS ==============================
//====================================================================================

void computeRandomMovement()
{
    int movementType;
    if(ALLOW_SIMPLE_TRANSLATION)
        movementType = random(1, 6); // random number between 1 and 5
    else
        movementType = random(1, 5); // random number between 1 and 4

    int movementAmplitude = random(MIN_AMPLITUDE, MAX_AMPLITUDE + 1); // random number between 50 and 100
    int movementOrientation = random(2); // 0 or 1

    if(movementOrientation == 0)
    {
        movementAmplitude = -movementAmplitude;
    }

    switch(movementType)
    {
        case 1:
            pitchAngle(movementAmplitude);
            break;
        case 2:
            pitchAngle(movementAmplitude);
            break;
        case 3:
            commonPitchAndRoll(movementAmplitude);
            break;
        case 4:
            oppositePitchAndRoll(movementAmplitude);
            break;
        case 5:
            upDown(movementAmplitude);
            break;
        default:
            Serial.println("Mvt Error");
            upDown(0);
            break;
    }
}

void runRandomMovements(bool reset)
{
    static uint32_t maintainTime = 2;
    static uint32_t movementInterval = 0;
    static uint32_t lastMovement = 0;

    if(reset)
    {
        maintainTime = 2;
        movementInterval = 0;
        lastMovement = 0;
        return;
    }

    if(maintainTime != 0)
    {
        if(maintainTime != 1 && millis() - lastMovement > maintainTime)
        {
            Serial.println(String(millis()) + "\tBack to neutral");
            // on ramène le plateau à sa position "neutre"
            upDown(0);
            maintainTime = 1;
        }

        if(moveTable(false))
        {
            // Le plateau a atteint sa position neutre
            maintainTime = 0;
            lastMovement = millis();
            movementInterval = random(MIN_POS_INTERVAL, MAX_POS_INTERVAL + 1);
            Serial.println(String(millis()) + "\tPos_Int\t"  + String(movementInterval));
        }
    }

    if(movementInterval != 0)
    {
        if(movementInterval != 1 && millis() - lastMovement > movementInterval)
        {
            // on demande un nouveau mouvement aléatoire
            computeRandomMovement();
            movementInterval = 1;
        }

        if(moveTable(false))
        {
            // Le plateau a atteint sa position neutre
            movementInterval = 0;
            lastMovement = millis();
            maintainTime = random(MIN_MAINTAIN_TIME, MAX_MAINTAIN_TIME + 1);
            Serial.println(String(millis()) + "\tMaintain\t"  + String(maintainTime));
        }
    }
}

//====================================================================================
//=================  HIGH LEVEL FUNCTIONS (you can play with them) ===================
//====================================================================================
void checkForSerialOrder()
{
    if(Serial.available())
    {
        char b = Serial.read();

        continuousRandomMovement = false;

        if(b == '1')
        {
            // Back to minimum
            setTablePosition(MIN_DISTANCE_1 + 5, MIN_DISTANCE_2 + 5, MIN_DISTANCE_3 + 5, MIN_DISTANCE_4 + 5);
        }
        else if(b == '0')
        {
            // Arrête les déplacements et ferme toutes les valves
            setTablePosition(0, 0, 0, 0);
            Cric1.stop();
            Cric2.stop();
            Cric3.stop();
            Cric4.stop();
        }
        else if(b == 'm')
        {
            // Active l'affichage des distances de chaque crics
            printPosition = !printPosition;
        }
        else if(b == 'r')
        {
            // Permet d'envoyer une consigne de roll ('r-100' à 'r100')
            int i = Serial.parseInt();
            rollAngle(i);
        }
        else if(b == 'p')
        {
            // Permet d'envoyer une consigne de pitch ('p-100' à 'p100')
            int i = Serial.parseInt();
            pitchAngle(i);
        }
        else if(b == 't')
        {
            // Permet d'envoyer une consigne de translation ('t-100' à 't100')
            int i = Serial.parseInt();
            upDown(i);
        }
        else if(b == 'o')
        {
            // Permet d'envoyer une consigne de roll + pitch opposés ('o-100' à 'o100')
            int i = Serial.parseInt();
            oppositePitchAndRoll(i);
        }
        else if(b == 'c')
        {
            // Permet d'envoyer une consigne de roll + pitch commun ('c-100' à 'c100')
            int i = Serial.parseInt();
            commonPitchAndRoll(i);
        }
        else if(b == 'b')
        {
            // Permet de réinitialiser les capteurs de distance
            configureDistanceSensors();
        }
        else if(b == 'g')
        {
            // Permet de réinitialiser les capteurs de distance
            continuousRandomMovement = true;
            runRandomMovements(true);
        }
        else if(b == 'a')
        {
            // Permet de contrôler l'ouverture indépendante de chaque valve:
            // 'aX1' permet de faire entrer l'air dans le cric X (ex : 'a21' fait entrer l'air dans le cric 2)
            // 'aX0' permet de faire sortir l'air dans le cric X (ex : 'a40' fait sortir l'air du cric 4)
            // Utiliser la commande '0' pour fermer les crics
            int i = Serial.parseInt();
            if(i == 11)
            {
                Cric1.forceAirIn();
            }
            else if(i == 10)
            {
                Cric1.forceAirOut();
            }
            else if(i == 21)
            {
                Cric2.forceAirIn();
            }
            else if(i == 20)
            {
                Cric2.forceAirOut();
            }
            else if(i == 31)
            {
                Cric3.forceAirIn();
            }
            else if(i == 30)
            {
                Cric3.forceAirOut();
            }
            else if(i == 41)
            {
                Cric4.forceAirIn();
            }
            else if(i == 40)
            {
                Cric4.forceAirOut();
            }
            else
            {
                setTablePosition(0, 0, 0, 0);
                Cric1.stop();
                Cric2.stop();
                Cric3.stop();
                Cric4.stop();
            }
        }
    }
}

void rollAngle(int percentage)
{   
    Serial.println("Roll angle : " + String(percentage));
    if(abs(percentage) > 100)
    {
        Serial.println("Roll Angle out of bound");
    }
    else if(percentage <= 0)
    {
        double tmp = -(double)percentage / 100;
        int pos1 = ((double)MAX_DISTANCE_1 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_1 * (1 - tmp)) / 2;
        int pos2 = ((double)MIN_DISTANCE_2 + (double)MAX_DISTANCE_2)/2 - ((double)MIN_LOW_2 * tmp);
        int pos3 = ((double)MIN_DISTANCE_3 + (double)MAX_DISTANCE_3)/2 - ((double)MIN_LOW_3 * tmp);
        int pos4 = ((double)MAX_DISTANCE_4 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_4 * (1 - tmp)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }
    else if(percentage >= 0)
    {
        double tmp = (double)percentage / 100;
        int pos1 = ((double)MIN_DISTANCE_1 + (double)MAX_DISTANCE_1)/2 - ((double)MIN_LOW_1 * tmp);
        int pos2 = ((double)MAX_DISTANCE_2 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_2 * (1 - tmp)) / 2;
        int pos3 = ((double)MAX_DISTANCE_3 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_3 * (1 - tmp)) / 2;
        int pos4 = ((double)MIN_DISTANCE_4 + (double)MAX_DISTANCE_4)/2 - ((double)MIN_LOW_4 * tmp);
        setTablePosition(pos1, pos2, pos3, pos4);
    }
}

void pitchAngle(int percentage)
{
    Serial.println("Pitch angle : " + String(percentage));
    if(abs(percentage) > 100)
    {
        Serial.println("Pitch Angle out of bound");
    }
    else if(percentage >= 0)
    {
        double tmp = (double)percentage/100;
        int pos1 = ((double)MIN_DISTANCE_1 + (double)MAX_DISTANCE_1)/2 - ((double)MIN_LOW_1 * tmp);
        int pos2 = ((double)MIN_DISTANCE_2 + (double)MAX_DISTANCE_2)/2 - ((double)MIN_LOW_2 * tmp);
        int pos3 = ((double)MAX_DISTANCE_3 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_3 * (1 - tmp)) / 2;
        int pos4 = ((double)MAX_DISTANCE_4 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_4 * (1 - tmp)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }
    else if(percentage <= 0)
    {
        double tmp = -(double)percentage / 100;
        int pos1 = ((double)MAX_DISTANCE_1 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_1 * (1 - tmp)) / 2;
        int pos2 = ((double)MAX_DISTANCE_2 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_2 * (1 - tmp)) / 2;
        int pos3 = ((double)MIN_DISTANCE_3 + (double)MAX_DISTANCE_3)/2 - ((double)MIN_LOW_3 * tmp);
        int pos4 = ((double)MIN_DISTANCE_4 + (double)MAX_DISTANCE_4)/2 - ((double)MIN_LOW_4 * tmp);
        setTablePosition(pos1, pos2, pos3, pos4);
    }
}

void upDown(int percentage)
{
    Serial.println("Up/Down : " + String(percentage));
    if(abs(percentage) > 100)
    {
        Serial.println("Up-Down percentage out of bound");
    }
    else
    {
        double tmp = (double)percentage / 100;
        int pos1 = ((double)MAX_DISTANCE_1 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_1 * (1-tmp)) / 2;
        int pos2 = ((double)MAX_DISTANCE_2 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_2 * (1-tmp)) / 2;
        int pos3 = ((double)MAX_DISTANCE_3 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_3 * (1-tmp)) / 2;
        int pos4 = ((double)MAX_DISTANCE_4 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_4 * (1-tmp)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }

}

void oppositePitchAndRoll(int percentage)
{
    Serial.println("Common P&R : " + String(percentage));
    if(abs(percentage) > 100)
    {
        Serial.println("Common Pitch-and-Roll Angle out of bound");
    }
    else if(percentage >= 0)
    {
        double tmp = (double)percentage/100;
        int pos1 = ((double)MIN_DISTANCE_1 + (double)MAX_DISTANCE_1)/2 + (tmp * (((double)MAX_DISTANCE_1 - (double)MIN_DISTANCE_1)/2 - (double)MIN_LOW_1)) / 2;
        int pos2 = ((double)MAX_DISTANCE_2 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_2 * (1 - tmp)) / 2;
        int pos3 = ((double)MIN_DISTANCE_3 + (double)MAX_DISTANCE_3)/2 + (tmp * (((double)MAX_DISTANCE_3 - (double)MIN_DISTANCE_3)/2 - (double)MIN_LOW_3)) / 2;
        int pos4 = ((double)MIN_DISTANCE_4 + (double)MAX_DISTANCE_4)/2 - ((double)MIN_LOW_4 * tmp);
        setTablePosition(pos1, pos2, pos3, pos4);
    }
    else if(percentage <= 0)
    {
        double tmp = -(double)percentage / 100;
        int pos1 = ((double)MIN_DISTANCE_1 + (double)MAX_DISTANCE_1)/2 + (tmp * (((double)MAX_DISTANCE_1 - (double)MIN_DISTANCE_1)/2 - (double)MIN_LOW_1)) / 2;
        int pos2 = ((double)MIN_DISTANCE_2 + (double)MAX_DISTANCE_2)/2 - ((double)MIN_LOW_2 * tmp);
        int pos3 = ((double)MIN_DISTANCE_3 + (double)MAX_DISTANCE_3)/2 + (tmp * (((double)MAX_DISTANCE_3 - (double)MIN_DISTANCE_3)/2 - (double)MIN_LOW_3)) / 2;
        int pos4 = ((double)MAX_DISTANCE_4 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_4 * (1 - tmp)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }
}

void commonPitchAndRoll(int percentage)
{
    Serial.println("Opposite P&R : " + String(percentage));
    if(abs(percentage) > 100)
    {
        Serial.println("Opposite Pitch-and-Roll Angle out of bound");
    }
    else if(percentage <= 0)
    {
        double tmp = -(double)percentage/100;
        int pos1 = ((double)MAX_DISTANCE_1 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_1 * (1 - tmp)) / 2;
        int pos2 = ((double)MIN_DISTANCE_2 + (double)MAX_DISTANCE_2)/2 + (tmp * (((double)MAX_DISTANCE_2 - (double)MIN_DISTANCE_2)/2 - (double)MIN_LOW_2)) / 2;
        int pos3 = ((double)MIN_DISTANCE_3 + (double)MAX_DISTANCE_3)/2 - ((double)MIN_LOW_3 * tmp);
        int pos4 = ((double)MIN_DISTANCE_4 + (double)MAX_DISTANCE_4)/2 + (tmp * (((double)MAX_DISTANCE_4 - (double)MIN_DISTANCE_4)/2 - (double)MIN_LOW_4)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }
    else if(percentage > 0)
    {
        double tmp = (double)percentage / 100;
        int pos1 = ((double)MIN_DISTANCE_1 + (double)MAX_DISTANCE_1)/2 - ((double)MIN_LOW_1 * tmp);
        int pos2 = ((double)MIN_DISTANCE_2 + (double)MAX_DISTANCE_2)/2 + (tmp * (((double)MAX_DISTANCE_2 - (double)MIN_DISTANCE_2)/2 - (double)MIN_LOW_2)) / 2;
        int pos3 = ((double)MAX_DISTANCE_3 * (1 + tmp)) / 2 + ((double)MIN_DISTANCE_3 * (1 - tmp)) / 2;
        int pos4 = ((double)MIN_DISTANCE_4 + (double)MAX_DISTANCE_4)/2 + (tmp * (((double)MAX_DISTANCE_4 - (double)MIN_DISTANCE_4)/2 - (double)MIN_LOW_4)) / 2;
        setTablePosition(pos1, pos2, pos3, pos4);
    }
}
//====================================================================================
//==============================  LOW LEVEL FUNCTIONS ================================
//====================================================================================

/**
 * setTablePosition : method to require a position for all 4 crics.
 */
void setTablePosition(int posCric1, int posCric2, int posCric3, int posCric4)
{
    desiredPosCric1 = posCric1;
    desiredPosCric2 = posCric2;
    desiredPosCric3 = posCric3;
    desiredPosCric4 = posCric4;
    moveTable(true);
}

/**
 * moveTable : calls the required movements for each cric after a setTablePosition is called
 * @warning : needs to be called within an non blocking loop
 * @return true when the position in reached
 */
bool moveTable(bool reset)
{
    static int destinationReached = 0;

    if(reset)
    {
        destinationReached = 0;
        return false;
    }

    if(desiredPosCric1 != 0)
    {
        if(Cric1.goToPosition(desiredPosCric1) == 1)
        {
            // Cric 1 a atteint sa position
            desiredPosCric1 = 0;
            destinationReached++;
        }
    }

    if(desiredPosCric2 != 0)
    {
        if(Cric2.goToPosition(desiredPosCric2) == 1)
        {
            // Cric 1 a atteint sa position
            desiredPosCric2 = 0;
            destinationReached++;
        }
    }

    if(desiredPosCric3 != 0)
    {
        if(Cric3.goToPosition(desiredPosCric3) == 1)
        {
            // Cric 1 a atteint sa position
            desiredPosCric3 = 0;
            destinationReached++;
        }
    }

    if(desiredPosCric4 != 0)
    {
        if(Cric4.goToPosition(desiredPosCric4) == 1)
        {
            // Cric 1 a atteint sa position
            desiredPosCric4 = 0;
            destinationReached++;
        }
    }

    if(destinationReached == 4)
    {
        destinationReached = 0;
        return true;
    }

    return false;
}

/**
 * configureDistanceSensors : initialises and defines the I2C adresses of each distance sensor
 * @warning if no issue is encountered this functions doesn't show anything. If one of the sensors
 * encountered an error it will print the number of the faulty sensor :
 * "1234" --> all sensors faileds
 * "1004" --> sensor 1 and 4 failed
 * "30"   --> sensor 3 only failed  
 */
int configureDistanceSensors()
{
    int error = 0;
    turnOffSensors();
    delay(5);
    if(!sensor4.quickBoot(I2C_ADDRESS_4))
        error += 4;

    if(!sensor3.quickBoot(I2C_ADDRESS_3))
        error += 30;

    if(!sensor2.quickBoot(I2C_ADDRESS_2))
        error += 200;
        

    if(!sensor1.quickBoot(I2C_ADDRESS_1))
        error += 1000;
    
    if(error != 0)
    {
        Serial.println(error);
    }
    return error;
}

/**
 * turnOffSensors : turns off all 4 sensors
 */
void turnOffSensors()
{
    sensor1.turnOffSensor();
    sensor2.turnOffSensor();
    sensor3.turnOffSensor();
    sensor4.turnOffSensor();
}