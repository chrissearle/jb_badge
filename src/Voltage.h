#include <Arduino.h>

class Voltage
{
private:
    const bool inUse = false; // May not be needed - using battery protection board

    const float adc_scaling = 3.3 / 1024.0;
    const float adc_threshold = 3.0;

    // Potential divider - reduce voltage in half - two 10k resistors
    const int potential_divider_resistance = 10000;

    int pin;

    bool lowBattery = false;

    float readADC()
    {
        int adc_value;

        float adc_voltage;
        float measured_voltage;

        adc_value = analogRead(pin);
        adc_voltage = adc_value * adc_scaling;

        measured_voltage = (adc_voltage * (potential_divider_resistance * 2)) / potential_divider_resistance;

        return measured_voltage;
    }

public:
    Voltage(int voltagePin)
    {
        pin = voltagePin;
    }

    void process()
    {
        if (inUse)
        {
            float voltage = readADC();

            if (voltage < adc_threshold)
            {
                lowBattery = true;
            }
            else
            {
                lowBattery = false;
            }
        }
    }

    bool isLowBattery()
    {
        if (inUse)
        {
            return lowBattery;
        }
        else
        {
            return false;
        }
    }
};