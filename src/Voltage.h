#pragma once

#include <Arduino.h>

class Voltage
{
private:
    static constexpr bool inUse = false; // May not be needed - using battery protection board

    const float adc_scaling = 3.3 / 1024.0;
    const float adc_threshold = 3.0;

    // Potential divider - reduce voltage in half - two 10k resistors
    const int potential_divider_resistance = 10000;

    int pin;

    bool lowBattery = false;

    float readADC()
    {
        const float adc_voltage = analogRead(pin) * adc_scaling;

        return (adc_voltage * (potential_divider_resistance * 2)) / potential_divider_resistance;
    }

public:
    Voltage(int voltagePin) : pin(voltagePin) {}

    void process()
    {
        if (inUse)
        {
            lowBattery = readADC() < adc_threshold;
        }
    }

    bool isLowBattery()
    {
        return inUse && lowBattery;
    }
};