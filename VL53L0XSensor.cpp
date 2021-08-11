#include "VL53L0XSensor.h"

VL53L0XSensor::VL53L0XSensor(uint8_t xShutPin, uint16_t measurementInterval) : m_distanceSensor(), m_lastDistance(-1), 
                            m_lastMeasurement(0), m_measurementInterval(measurementInterval),
                            m_xShutPin(xShutPin) 
{
    pinMode(m_xShutPin, OUTPUT);
    digitalWrite(m_xShutPin, LOW);
} 

void VL53L0XSensor::setI2CAddress(uint8_t i2cAddress)
{
    if(i2cAddress < 0x29 || i2cAddress > 0x7f)
    {
        // error !
    }
    else
    {
        m_distanceSensor.setAddress(i2cAddress);
    }
}

bool  VL53L0XSensor::bootSensor()
{
    m_distanceSensor.setTimeout(500);
    m_distanceSensor.setMeasurementTimingBudget(60000);

    if(!m_distanceSensor.init())
    {
        return false;
    }
    else
    {
        m_distanceSensor.startContinuous();
        return true;
    }
}

bool VL53L0XSensor::newDataAvailable()
{
    int newDistance = this->getSensorDistance();

    if(newDistance != 0 && newDistance != -1)
    {
        m_lastDistance = newDistance;
        return true;
    }
    else
    {
        return false;
    }
}

int VL53L0XSensor::getSensorDistance()
{
    if(millis() - m_lastMeasurement > m_measurementInterval)
    {
        int measure = m_distanceSensor.readRangeContinuousMillimeters();
        m_lastMeasurement = millis();

        if (m_distanceSensor.timeoutOccurred()) {  // phase failures have incorrect data
            return -1;
        } else {
            return measure;
        }
    }

    return 0;
}

int VL53L0XSensor::getLastMeasuredDistance() const
{
    return m_lastDistance;
}

void VL53L0XSensor::turnOffSensor()
{
    digitalWrite(m_xShutPin, LOW);
}

void VL53L0XSensor::turnOnSensor()
{
    digitalWrite(m_xShutPin, HIGH);
}