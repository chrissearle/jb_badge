#include <Bounce2.h>

class Status
{
private:
    const bool inUse = false;
    bool active = true;
    Bounce bounce;
    int blPin;

    // TODO - try to use #define GC9A01A_DISPOFF 0x28   ///< Display OFF
    // and #define GC9A01A_DISPON 0x29    ///< Display ON
    void setBacklight()
    {
        if (active)
        {
            digitalWrite(blPin, HIGH);
        }
        else
        {
            digitalWrite(blPin, LOW);
        }
    }

public:
    Status(int pin, int backlightPin)
    {
        bounce = Bounce();
        bounce.attach(pin, INPUT_PULLUP);
        bounce.interval(5);

        blPin = backlightPin;
    }

    void process()
    {
        if (!inUse)
        {
            return;
        }

        bounce.update();

        if (bounce.changed())
        {
            int deboucedInput = bounce.read();

            // TODO - check if we pull low or high - think this is release of button which is what is wanted.
            if (deboucedInput == HIGH)
            {
                active = !active;
                setBacklight();
            }
        }
    }

    bool isActive()
    {
        if (inUse)
        {
            return active;
        }
        else
        {
            return true;
        }
    }
};