// CricController.h
/**
 * © Thomas Demmer - 2021 | www.thomasdmr.com
 */

#pragma once
#include "Arduino.h"
#include "Relay.h"
#include "VL53L0XSensor.h"

class CricController
{
    public:
    /**
     * CricController : default constructor initializing the attributes
     */
    CricController(uint8_t pinRelayIn, uint8_t pinRelayOut, String name);

    /**
     * init : initialises the object and links the distance sensor pointer
     * to the externally defined distance sensor
     * @param distanceSensor the VL053l0X sensor that is related to this cric
     */
    void init(VL53L0XSensor& distanceSensor);

    /**
     * stop : stops any activity on the cric by closing both relays and resetting the 
     * system to an idle state
     */
    void stop();

    /**
     * setMinPosDistance : defines the distance corresponding to the lowest position of the cric
     * @param minPosDistance minimum measured distance by the VL053L0X sensor (in mm)
     */
    void setMinPosDistance(int minPosDistance);

    /**
     * setMaxPosDistance : defines the distance corresponding to the highest position of the cric
     * @param maxPosDistance maximum measured distance by the VL053L0X sensor (in mm)
     */
    void setMaxPosDistance(int maxPosDistance);

    /**
     * updatePosition : updates the objects current position by checking on the VL53l0X sensor
     * @warning : this needs to be called regularly and placed within a non-blocking loop
     * @param print if true, the method also prints the position on the Serial display
     */
    void updatePosition(bool print = false);

    /**
     * goToPosition : calls a movement of the cric. This will make sure the cric moves to the desired position
     * @warning : this needs to be called regularly and placed within a non-blocking loop
     * @param desiredPosition position the cric needs to move to (in mm)
     */
    int8_t goToPosition(int desiredPosition);

    /**
     * relayMovementDetected : returns true if a relay has changed its state during the last cycle
     */
    bool relayMovementDetected();

    /**
     * forceAirIn : opens the Air In relay
     * @warning : there is no safety on this function and à stop() will have to be called afterwards
     */
    void forceAirIn();

    /**
     * forceAirOut : opens the Air Out relay
     * @warning : there is no safety on this function and à stop() will have to be called afterwards
     */
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