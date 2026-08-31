#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SD.h>

#include "Screen.h"

class ImageScreen : public Screen
{
private:
    static constexpr uint16_t maxDim = 240;
    static constexpr uint16_t byteWidth = 2;

    inline static DMAMEM uint16_t buffer[maxDim * maxDim];

    bool valid = false;

    const char *filename;
    const uint16_t background;
    const uint16_t imageWidth;
    const uint16_t imageHeight;

    void read()
    {
        Serial.printf("image to load: %s\n", filename);

        if (imageWidth > maxDim || imageHeight > maxDim)
        {
            Serial.printf("image too big: %s (%ux%u)\n", filename, imageWidth, imageHeight);
            return;
        }

        File file = SD.open(filename, FILE_READ);

        if (!file)
        {
            Serial.printf("image missing: %s\n", filename);
            return;
        }

        const size_t expected = static_cast<size_t>(imageWidth) * imageHeight * byteWidth;
        const size_t got = file.read(buffer, expected);

        file.close();

        if (got != expected)
        {
            Serial.printf("image short: %s (%u of %u bytes)\n", filename, got, expected);
            return;
        }

        valid = true;

        Serial.printf("image loaded successfully: %s\n", filename);
    }

public:
    ImageScreen(Adafruit_SPITFT *tft, const char *filename, uint16_t background,
                uint16_t imageWidth = 170, uint16_t imageHeight = 170) : Screen(tft),
                                                                         filename(filename),
                                                                         background(background),
                                                                         imageWidth(imageWidth),
                                                                         imageHeight(imageHeight)
    {
        read();
    }

    uint32_t draw() override
    {
        tft->fillScreen(background);

        yield();

        if (valid)
        {
            tft->drawRGBBitmap((maxDim - imageWidth) / 2, (maxDim - imageHeight) / 2, buffer, imageWidth, imageHeight);

            yield();
        }

        return 2000;
    }
};