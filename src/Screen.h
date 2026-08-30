#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

class Screen
{
protected:
    Adafruit_SPITFT *tft;

public:
    Screen(Adafruit_SPITFT *tft) : tft(tft) {};
    virtual ~Screen() {};
    virtual uint32_t draw() = 0;
};