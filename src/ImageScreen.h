#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SD.h>

#include "Screen.h"

class ImageScreen : public Screen
{
private:
    const uint16_t imageSide = 170;
    const uint16_t byteWidth = 2;

    uint16_t *pixels;
    const char *filename;
    const uint16_t background;

    void write()
    {
        if (SD.exists(filename))
        {
            SD.remove(filename);
        }

        File file = SD.open(filename, FILE_WRITE);
        file.write(pixels, imageSide * imageSide * byteWidth);
        file.close();
    }

    void read()
    {
        pixels = (uint16_t *)malloc(imageSide * imageSide * byteWidth);
        File file = SD.open(filename, FILE_READ);
        file.read(pixels, imageSide * imageSide * byteWidth);
        file.close();
    }

public:
    ImageScreen(Adafruit_SPITFT *tft, const char *filename, uint16_t background) : Screen(tft), filename(filename), background(background)
    {
        read();
    }

    void draw()
    {
        tft->fillScreen(background);

        yield();

        tft->drawRGBBitmap(35, 35, pixels, 170, 170);

        yield();

        delay(2000);
    }
};