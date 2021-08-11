#include "CricController.h"

CricController::CricController(uint8_t pinRelayIn, uint8_t pinRelayOut, String name): 
                                m_relayAirIn(pinRelayIn), m_relayAirOut(pinRelayOut),
                                m_name(name), m_maxPosDistance(0), m_minPosDistance(0)
{}

void CricController::init(VL53L0XSensor& distanceSensor)
{
    m_relayAirIn.init();
    m_relayAirOut.init();

    m_distanceSensor = &distanceSensor;
}

void CricController::updatePosition()
{
    if(m_distanceSensor->newDataAvailable())
    {
        m_currentPosition = m_distanceSensor->getLastMeasuredDistance();
    }
}

void CricController::setMinPosDistance(int minPosDistance)
{
    if(m_maxPosDistance != 0 && minPosDistance > m_maxPosDistance)
    {
        Serial.println(m_name + "\tErr: min>max pos");
    }
    else
    {
        m_minPosDistance = minPosDistance;
    }
}

void CricController::setMaxPosDistance(int maxPosDistance)
{
    if(m_minPosDistance != 0 && maxPosDistance < m_minPosDistance)
    {
        Serial.println(m_name + "\tErr: max<min pos");
    }
    else
    {
        m_maxPosDistance = maxPosDistance;
    }
}

int8_t CricController::goToPosition(int desiredPosition)
{
    if(desiredPosition > m_maxPosDistance || desiredPosition < m_minPosDistance)
    {
        Serial.println(String(millis()) + "\t" + m_name + "\tPosition order out of bound");
        return -1;
    }
    else
    {
        if(abs(m_currentPosition - desiredPosition) < 3)
        {
            // we reached the desired position
            m_relayAirIn.close();
            m_relayAirOut.close();
            Serial.println(String(millis()) + "\t" + m_name + "\tPosition reached: " + String(desiredPosition));
            return 1;
        }
        else if(m_currentPosition < desiredPosition)
        {
            m_relayAirIn.open();
            m_relayAirOut.close();
        }
        else if(m_currentPosition > desiredPosition)
        {
            m_relayAirOut.open();
            m_relayAirIn.close();
        }
        return 0;
    }
}