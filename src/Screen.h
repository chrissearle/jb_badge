#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

word ConvertRGB(byte R, byte G, byte B)
{
    return (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3));
}

class Screen
{
protected:
    Adafruit_SPITFT *tft;

public:
    Screen(Adafruit_SPITFT *tft) : tft(tft) {};
    virtual ~Screen() {};
    virtual uint32_t draw() = 0;
};