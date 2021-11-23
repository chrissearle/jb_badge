#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include "Screen.h"

class ImageScreen : public Screen
{
protected:
    uint16_t *pixels;
    uint16_t background;

public:
    ImageScreen(Adafruit_SPITFT *tft) : Screen(tft) {}
    void draw()
    {
        tft->fillScreen(background);

        yield();

        tft->drawRGBBitmap(35, 35, pixels, 170, 170);

        yield();

        delay(2000);
    }
};