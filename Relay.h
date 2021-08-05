#pragma once
#include "Arduino.h"

class Relay
{
    public:
    Relay(int pin, String name = "None");

    void init();

    void open();

    void close();

    bool state();

    private:
    int m_pin;
    bool m_state;
    String m_name;

};