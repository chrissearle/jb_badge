#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include "Screen.h"

class LowBattery : public Screen
{
public:
    LowBattery(Adafruit_SPITFT *tft) : Screen(tft) {}
    uint32_t draw()
    {
        // Black
        tft->fillScreen(GC9A01A_RED);
        yield();
        return 1000;
    }
};