#include "VL53L0XSensor.h"

VL53L0XSensor::VL53L0XSensor(int xShutPin, uint32_t measurementInterval) : m_distanceSensor(), m_lastDistance(-1), 
                            m_lastMeasurement(0), m_measurementInterval(measurementInterval), m_i2cAddress(0x29),
                            m_xShutPin(xShutPin) 
{
    pinMode(m_xShutPin, OUTPUT);
    digitalWrite(m_xShutPin, HIGH);
} 

void VL53L0XSensor::setI2CAddress(int8_t i2cAddress)
{
    if(i2cAddress < 0x29 || i2cAddress > 0x7f)
    {
        // error !
    }
    else
    {
        m_i2cAddress = i2cAddress;
    }
}

bool  VL53L0XSensor::bootSensor()
{
    return m_distanceSensor.begin(m_i2cAddress);
}

bool VL53L0XSensor::newDataAvailable()
{
    int newDistance = this->getSensorDistance();

    if(newDistance != m_lastDistance)
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
        VL53L0X_RangingMeasurementData_t measure;
        m_distanceSensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

        if (measure.RangeStatus != 4) {  // phase failures have incorrect data
            return measure.RangeMilliMeter;
        } else {
            return -1;
        }
        m_lastMeasurement = millis();
    }
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