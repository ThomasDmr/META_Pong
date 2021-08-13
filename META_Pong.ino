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

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    // Configure les adresses i2c des capteurs de distance et initialise les objets
    configureDistanceSensors();

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
bool printPosition = false;
void loop() 
{
    // A laisser dans le loop pour actualiser la lecture des distances
    Cric1.updatePosition(printPosition);
    Cric2.updatePosition(printPosition);
    Cric3.updatePosition(printPosition);
    Cric4.updatePosition(printPosition);

    if(Serial.available())
    {
        char b = Serial.read();

        if(b == '1')
        {
            // Back to minimum
            setTablePosition(MIN_DISTANCE_1 + 5, MIN_DISTANCE_2 + 5, MIN_DISTANCE_3 + 5, MIN_DISTANCE_4 + 5);
        }
        else if(b == '2')
        {
            // Go to max 
            setTablePosition((MIN_DISTANCE_1 + MAX_DISTANCE_1)/2, (MIN_DISTANCE_2 + MAX_DISTANCE_2)/2, (MIN_DISTANCE_3 + MAX_DISTANCE_3)/2, (MIN_DISTANCE_4 + MAX_DISTANCE_4)/2);
        }
        else if(b == '3')
        {
            // Go to median
            setTablePosition(MAX_DISTANCE_1, MAX_DISTANCE_2, MAX_DISTANCE_3, MAX_DISTANCE_4);
        }
        else if(b == '4')
        {
            // Angle 1
            setTablePosition(MAX_DISTANCE_1, (MIN_DISTANCE_2 + MAX_DISTANCE_2)/2 - 30, (MIN_DISTANCE_3 + MAX_DISTANCE_3)/2 - 30, MAX_DISTANCE_4);
        }
        else if(b == '5')
        {
            // - Angle 1
            setTablePosition((MIN_DISTANCE_1 + MAX_DISTANCE_1)/2 - 30, MAX_DISTANCE_2, MAX_DISTANCE_3, (MIN_DISTANCE_4 + MAX_DISTANCE_4)/2 - 30);
        }
        else if(b == '0')
        {
            setTablePosition(0, 0, 0, 0);
            Cric1.stop();
            Cric2.stop();
            Cric3.stop();
            Cric4.stop();
        }
        else if(b == 'm')
        {
            printPosition = true;
        }
        else if(b == 'l')
        {
            printPosition = false;
        }
        else if(b == 'a')
        {
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
        else if(b == 'b')
        {
            configureDistanceSensors();
        }
        else if(b == 'r')
        {
            sensor1.turnOffSensor();
            delay(100);
            if(sensor1.quickBoot())
            {
                Serial.println("Done");
            }
        }
    }

    if(moveTable())
    {
        Serial.println(String(millis()) + "\tPosition reached !");
    }

    bool relayMovement = Cric1.relayMovementDetected() | Cric2.relayMovementDetected() | Cric3.relayMovementDetected() | Cric4.relayMovementDetected();
    
    if(relayMovement)
    {
        configureDistanceSensors();
    }

}

void setTablePosition(int posCric1, int posCric2, int posCric3, int posCric4)
{
    desiredPosCric1 = posCric1;
    desiredPosCric2 = posCric2;
    desiredPosCric3 = posCric3;
    desiredPosCric4 = posCric4;
}

bool moveTable()
{
    static int destinationReached = 0;

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

void configureDistanceSensors()
{
    int error = 0;
    //Serial.println("Start sensor configuration: ");
    //Serial.print("Sensor4: ");
    turnOffSensors();
    delay(5);
    if(!sensor4.quickBoot(I2C_ADDRESS_4))
        error += 4;

    //Serial.print("Sensor3: ");
    if(!sensor3.quickBoot(I2C_ADDRESS_3))
        error += 30;

    //Serial.print("Sensor2: ");
    if(!sensor2.quickBoot(I2C_ADDRESS_2))
        error += 200;
        

    //Serial.print("Sensor1: ");
    if(!sensor1.quickBoot(I2C_ADDRESS_1))
        error += 1000;
    
    if(error != 0)
    {
        Serial.println(error);
    }
}

void turnOffSensors()
{
    sensor1.turnOffSensor();
    sensor2.turnOffSensor();
    sensor3.turnOffSensor();
    sensor4.turnOffSensor();
}