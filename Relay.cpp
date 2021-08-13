#include "Relay.h"

Relay::Relay(int pin, String name, bool activeHigh) : m_pin(pin), m_name(name), m_state(0), m_activeHigh(activeHigh)
{}

void Relay::init()
{
    pinMode(m_pin, OUTPUT);
    if(m_activeHigh)
    {
        digitalWrite(m_pin, LOW);
    }
    else
    {
        digitalWrite(m_pin, HIGH);
    }
    m_state = false;
}

void Relay::open()
{
    if(m_activeHigh)
    {
        digitalWrite(m_pin, HIGH);
    }
    else
    {
        digitalWrite(m_pin, LOW);
    }

    if(m_name != "None" && m_state != true)
    {
        Serial.println(String(millis()) + "\t0\tOpen: " + m_name);
    }
    m_state = true;
}

void Relay::close()
{
    if(m_activeHigh)
    {
        digitalWrite(m_pin, LOW);
    }
    else
    {
        digitalWrite(m_pin, HIGH);
    }

    if(m_name != "None" && m_state != false)
    {
        Serial.println(String(millis()) + "\t0\tClose: " + m_name);
    }
    m_state = false;
}