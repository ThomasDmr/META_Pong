#include "CricController.h"

CricController::CricController(uint8_t pinRelayIn, uint8_t pinRelayOut, String name): 
                                m_relayAirIn(pinRelayIn), m_relayAirOut(pinRelayOut),
                                m_name(name), m_maxPosDistance(0), m_minPosDistance(0), 
                                m_systemState(0)
{
    m_relayMovement = false;
}

void CricController::init(VL53L0XSensor& distanceSensor)
{
    m_relayAirIn.init();
    m_relayAirOut.init();
    m_systemState = 0;
    m_distanceSensor = &distanceSensor;
    m_distanceTimer = 0;
    m_positionTimer = 0;
}

void CricController::stop()
{
    if(m_systemState != 0)
    {
        m_relayAirIn.close();
        m_relayAirOut.close();
        m_systemState = 0;
        m_relayMovement = true;
    }
}

bool CricController::updatePosition(bool print)
{
    if(m_distanceSensor->newDataAvailable())
    {
        m_distanceTimer = millis();
        m_currentPosition = m_distanceSensor->getLastMeasuredDistance();
        if(print)
        {
            Serial.println(String(millis()) + "\t" + m_name + "\t" + String(m_currentPosition));
        }
    }

    if(millis() - m_distanceTimer > 500)
    {
        Serial.println(m_name + "\tDistance error");
        return true;
    }

    return false;
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

bool CricController::relayMovementDetected()
{
    bool tmp = m_relayMovement;
    m_relayMovement = false;

    return tmp;
}

int8_t CricController::goToPosition(int desiredPosition, int tolerance)
{
    if(desiredPosition > m_maxPosDistance || desiredPosition < m_minPosDistance)
    {
        Serial.println(String(millis()) + "\t" + m_name + "\tPosition order out of bound");
        m_positionTimer = 0;
        return -1;
    }
    else
    {
        if(abs(m_currentPosition - desiredPosition) < tolerance || (m_positionTimer != 0 && millis() - m_positionTimer > 20000))
        {
            if(m_systemState != 0)
            {
                // we reached the desired position
                m_relayAirIn.close();
                m_relayAirOut.close();
                m_systemState = 0;
                m_relayMovement = true;
            }
            if (m_positionTimer != 0 && millis() - m_positionTimer > 25000)
                Serial.println(String(millis()) + "\t" + m_name + "\tPosition timeout");
            else
                Serial.println(String(millis()) + "\t" + m_name + "\tPosition reached: " + String(desiredPosition));
            
            m_positionTimer = 0;

            return 1;
        }
        else if(m_currentPosition < desiredPosition)
        {
            if(m_positionTimer == 0)
                m_positionTimer = millis();

            if(m_systemState != 1)
            {
                m_relayAirIn.open();
                m_relayAirOut.close();
                m_relayMovement = true;
                m_systemState = 1;
            }
        }
        else if(m_currentPosition > desiredPosition)
        {
            if(m_positionTimer == 0)
                m_positionTimer = millis();

             if(m_systemState != 2)
            {
                m_relayAirIn.close();
                m_relayAirOut.open();
                m_relayMovement = true;
                m_systemState = 2;
            }
        }

        return 0;
    }
}

void CricController::forceAirIn()
{
    m_relayAirIn.open();
    m_relayAirOut.close();
    m_relayMovement = true;
    m_systemState = 1;
}

void CricController::forceAirOut()
{
    m_relayAirIn.close();
    m_relayAirOut.open();
    m_relayMovement = true;
    m_systemState = 2;
}