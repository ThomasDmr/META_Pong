#include "Relay.h"

Relay::Relay(int pin, String name = "None") : m_pin(pin), m_name(name), m_state(0){}

void Relay::init()
{
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
    m_state = false;
}

void Relay::open()
{
    digitalWrite(m_pin, HIGH);
    if(m_name != "None" && m_state != true)
    {
        Serial.println(String(millis()) + "\t0\tOpen: " + m_name);
    }
    m_state = true;
}

void Relay::close()
{
    digitalWrite(m_pin, LOW);

    if(m_name != "None" && m_state != false)
    {
        Serial.println(String(millis()) + "\t0\tClose: " + m_name);
    }
    m_state = false;
}