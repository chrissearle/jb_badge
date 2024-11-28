#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include "Screen.h"

class Avatar : public Screen
{
public:
    Avatar(Adafruit_SPITFT *tft) : Screen(tft) {}
    void draw()
    {
        int16_t eyeballY = 70;
        int16_t pupilY = 83;

        int16_t eyeballR = 35;
        int16_t pupilR = 15;

        int16_t eyeballO = 50;
        int16_t pupilO = 35;

        int16_t center = tft->width() / 2;

        // Black
        tft->fillScreen(GC9A01A_BLACK);
        yield();
        // Fill blue
        tft->fillScreen(GC9A01A_BLUE);
        yield();
        // Draw both eyes
        tft->fillCircle(center - eyeballO, eyeballY, eyeballR, GC9A01A_YELLOW);
        tft->fillCircle(center + eyeballO, eyeballY, eyeballR, GC9A01A_YELLOW);
        tft->fillCircle(center - pupilO, pupilY, pupilR, GC9A01A_BLACK);
        tft->fillCircle(center + pupilO, pupilY, pupilR, GC9A01A_BLACK);
        yield();
        delay(1000);
        // Half close eye
        tft->fillRect(center + eyeballO - eyeballR, eyeballY - eyeballR, 2 * eyeballR + 2, eyeballR + 2, GC9A01A_BLUE);
        yield();
        delay(100);
        // Close eye
        tft->fillCircle(center + eyeballO, eyeballY, eyeballR, GC9A01A_BLUE);
        yield();
        delay(100);
        // Open eye
        tft->fillCircle(center + eyeballO, eyeballY, eyeballR, GC9A01A_YELLOW);
        tft->fillCircle(center + pupilO, pupilY, pupilR, GC9A01A_BLACK);
        yield();
        delay(1000);
    }
};