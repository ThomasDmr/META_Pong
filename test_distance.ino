#include "VL53L0XSensor.h"

VL53L0XSensor sensor1(13, 100);
VL53L0XSensor sensor2(3, 100);
VL53L0XSensor sensor3(4, 100);
VL53L0XSensor sensor4(5, 100);

void setup() {
  Serial.begin(115200);

  sensor1.setI2CAddress(0x29);
  sensor2.setI2CAddress(0x39);
  sensor3.setI2CAddress(0x49);
  sensor4.setI2CAddress(0x59);

  configureDistanceSensors();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void configureDistanceSensors()
{
  Serial.println("Start sensor configuration: ");
  Serial.print("Sensor4: ");
  turnOffSensors();
  delay(500);
  sensor4.turnOnSensor();
  delay(500);
  if(sensor4.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor3: ");
  turnOffSensors();
  delay(500);
  sensor3.turnOnSensor();
  delay(500);
  if(sensor3.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor2: ");
  turnOffSensors();
  delay(500);
  sensor2.turnOnSensor();
  delay(500);
  if(sensor2.bootSensor())
    Serial.println("DONE !");

  Serial.print("Sensor1: ");
  turnOffSensors();
  delay(500);
  sensor1.turnOnSensor();
  delay(500);
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
