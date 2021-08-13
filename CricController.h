#pragma once
#include "Arduino.h"
#include "Relay.h"
#include "VL53L0XSensor.h"

class CricController
{
    public:
    CricController(uint8_t pinRelayIn, uint8_t pinRelayOut, String name);

    void init(VL53L0XSensor& distanceSensor);

    void stop();

    void setMinPosDistance(int minPosDistance);
    void setMaxPosDistance(int maxPosDistance);

    void updatePosition(bool print = false);

    int8_t goToPosition(int desiredPosition);

    bool relayMovementDetected();

    void forceAirIn();
    void forceAirOut();

    private:
    Relay           m_relayAirIn;
    Relay           m_relayAirOut;
    VL53L0XSensor   *m_distanceSensor;
    String          m_name;
    int             m_minPosDistance;
    int             m_maxPosDistance;
    int             m_currentPosition;
    uint8_t         m_systemState;
    bool            m_relayMovement;
};