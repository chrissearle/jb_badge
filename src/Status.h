#pragma once

#include <Bounce2.h>

class Status
{
private:
    static constexpr bool btnInUse = false; // Currently disabled - we pull the battery when needed

    bool active = true;

    Bounce bounce;
    int blPin;

    // TODO - try to use #define GC9A01A_DISPOFF 0x28   ///< Display OFF
    // and #define GC9A01A_DISPON 0x29    ///< Display ON
    void setBacklight()
    {
        digitalWrite(blPin, active ? HIGH : LOW);
    }

public:
    Status(int pin, int backlightPin) : blPin(backlightPin)
    {
        if (btnInUse)
        {
            bounce.attach(pin, INPUT_PULLUP);
            bounce.interval(5);
        }
    }

    void process()
    {
        if (!btnInUse)
        {
            return;
        }

        bounce.update();

        if (bounce.changed())
        {
            // TODO - check if we pull low or high - think this is release of button
            // which is what is wanted.
            if (bounce.read() == HIGH)
            {
                active = !active;
                setBacklight();
            }
        }
    }

    bool isActive() const
    {
        return !btnInUse || active;
    }
};