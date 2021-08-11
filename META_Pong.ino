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
    Cric2.setMinPosDistance(MIN_DISTANCE_2 );
    Cric3.setMaxPosDistance(MAX_DISTANCE_3);
    Cric3.setMinPosDistance(MIN_DISTANCE_3);
    Cric4.setMaxPosDistance(MAX_DISTANCE_4);
    Cric4.setMinPosDistance(MIN_DISTANCE_4);
}

// !!! Le loop est asynchrone, ne pas mettre des fonction bloquantes !!!! 

void loop() 
{
    // A laisser dans le loop pour actualiser la lecture des distances
    Cric1.updatePosition();
    Cric2.updatePosition();
    Cric3.updatePosition();
    Cric4.updatePosition();

    if(Serial.available())
    {
        char b = Serial.read();

        if(b == '0')
        {
            setTablePosition(MIN_DISTANCE_1 + 10, MIN_DISTANCE_2 + 10, MIN_DISTANCE_3 + 10, MIN_DISTANCE_4 + 10);
        }
        else if(b == '1')
        {
            setTablePosition((MIN_DISTANCE_1 + MAX_DISTANCE_1)/2, (MIN_DISTANCE_2 + MAX_DISTANCE_2)/2, (MIN_DISTANCE_3 + MAX_DISTANCE_3)/2, (MIN_DISTANCE_4 + MAX_DISTANCE_4)/2);
        }
        else if(b == '2')
        {
            setTablePosition(MAX_DISTANCE_1, MAX_DISTANCE_2, MAX_DISTANCE_3, MAX_DISTANCE_4);
        }
    }

    if(moveTable())
    {
        Serial.println(String(millis()) + "\tPosition reached !");
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
        if(Cric2.goToPosition(desiredPosCric1) == 1)
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
  Serial.println("Start sensor configuration: ");
  Serial.print("Sensor4: ");
  turnOffSensors();
  sensor4.turnOnSensor();
  delay(10);
  sensor4.setI2CAddress(0x59);
  delay(10);
  if(sensor4.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor3: ");
  sensor3.turnOnSensor();
  delay(10);
  sensor3.setI2CAddress(0x49);
  delay(10);
  if(sensor3.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor2: ");
  sensor2.turnOnSensor();
  delay(10);
  sensor2.setI2CAddress(0x39);
  delay(10);
  if(sensor2.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor1: ");
  sensor1.turnOnSensor();
  delay(10);
  sensor1.setI2CAddress(0x29);
  delay(10);
  if(sensor1.bootSensor())
    Serial.println("DONE !");
}

void turnOffSensors()
{
  sensor1.turnOffSensor();
  sensor2.turnOffSensor();
  sensor3.turnOffSensor();
  sensor4.turnOffSensor();
}