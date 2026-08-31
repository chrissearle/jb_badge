#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include "Screen.h"

class ErrorScreen : public Screen
{
private:
    static constexpr uint16_t background = GC9A01A_YELLOW;
    static constexpr uint16_t foreground = GC9A01A_BLACK;
    static constexpr uint8_t textSize = 8;

    const char *message;

public:
    ErrorScreen(Adafruit_SPITFT *tft, const char *message) : Screen(tft), message(message) {}

    uint32_t draw() override
    {
        tft->fillScreen(background);

        tft->setTextColor(foreground);
        tft->setTextSize(textSize);

        int16_t x1, y1;
        uint16_t w, h;

        tft->getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
        tft->setCursor((tft->width() - w) / 2 - x1, (tft->height() - h) / 2 - y1);
        tft->print(message);

        yield();

        return 0;
    }

    // Draw, then stop the device. Never returns.
    [[noreturn]] void halt()
    {
        draw();

        while (true)
        {
            yield();
        }
    }
};