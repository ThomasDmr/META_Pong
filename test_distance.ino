#include "VL53L0XSensor.h"

VL53L0XSensor sensor1(2, 100);
VL53L0XSensor sensor2(3, 100);
VL53L0XSensor sensor3(4, 100);
VL53L0XSensor sensor4(5, 100);

void setup() {
  Serial.begin(115200);

  Wire.begin();
  configureDistanceSensors();
}

void loop() 
{
  if(sensor1.newDataAvailable())
  {
    Serial.println("1:\t" + String(millis()) + "\t" + String(sensor1.getLastMeasuredDistance()));
  }

  if(sensor2.newDataAvailable())
  {
    Serial.println("2:\t" + String(millis()) + "\t" + String(sensor2.getLastMeasuredDistance()));
  }

  if(sensor3.newDataAvailable())
  {
    Serial.println("3:\t" + String(millis()) + "\t" + String(sensor3.getLastMeasuredDistance()));
  }

  if(sensor4.newDataAvailable())
  {
    Serial.println("4:\t" + String(millis()) + "\t" + String(sensor4.getLastMeasuredDistance()));
  }
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
