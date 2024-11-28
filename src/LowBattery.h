#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

#include "Screen.h"

class LowBattery : public Screen
{
public:
    LowBattery(Adafruit_SPITFT *tft) : Screen(tft) {}
    void draw()
    {
        // Black
        tft->fillScreen(GC9A01A_RED);
        yield();
        delay(1000);
    }
};