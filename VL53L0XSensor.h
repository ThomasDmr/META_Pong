// VL53L0XSensor.h
/**
 * © Thomas Demmer - 2021 | www.thomasdmr.com
 */

#pragma once

#include <Wire.h>
#include "VL53L0X.h" // inclusion de la librairie du capteur
#include "Arduino.h"

class VL53L0XSensor
{
public:

    /**
     * VL53L0XDetector : default constructor initializing the attributes
     */
    VL53L0XSensor(uint8_t xShutPin, uint16_t measurementInterval = 100);

    /**
     * setI2CAddress : set the desired I2C address
     * @warning has to be called before the bootSensor() method
     */
    void setI2CAddress(uint8_t i2cAddress);

    /**
     * bootSensor : initialises the communication with the VL35L0X
     * @warning Should be called within a loop to test regularly before working with the sensor
     * @return true if the sensor is correctly booted. 
     */
    bool bootSensor();

    bool quickBoot(uint8_t i2cAddress);
    bool quickBoot();

    /**
     * newDataAvailable : check if there is a new measurement available
	 * @warning This method needs to be called within the main loop
     * @return true if we have a new distance reading
     */
    bool newDataAvailable();

    /**
     * turnOffSensor : turns off the sensor through the XSHUT pin
     */
    void turnOffSensor();

    /**
     * turnOnSensor : turns off the sensor through the XSHUT pin
     */
    void turnOnSensor();

    /**
     * getLastMeasuredDistance : returns the last measured distance by the sensor.
     * @return last measured distance in cm (centimeters) in double format
     */
    int  getLastMeasuredDistance() const;

private:
    VL53L0X             m_distanceSensor;
    int                 getSensorDistance();
    int                 m_lastDistance;
    uint32_t            m_lastMeasurement;
    uint16_t            m_measurementInterval;
    uint8_t             m_xShutPin;
    uint8_t             m_i2cAddress;
};