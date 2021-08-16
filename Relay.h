// Relay.h
/**
 * © Thomas Demmer - 2021 | www.thomasdmr.com
 */

#pragma once
#include "Arduino.h"

class Relay
{
    public:
    Relay(int pin, String name = "None", bool activeHigh = true);

    void init();

    void open();

    void close();

    bool state();

    private:
    int m_pin;
    bool m_state;
    String m_name;
    bool m_activeHigh;

};