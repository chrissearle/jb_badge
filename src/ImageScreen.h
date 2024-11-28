#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SD.h>

#include "Screen.h"

class ImageScreen : public Screen
{
private:
    const uint16_t maxDim = 240;
    const uint16_t byteWidth = 2;

    uint16_t *pixels;
    const char *filename;
    const uint16_t background;
    const uint16_t imageWidth;
    const uint16_t imageHeight;

    void write()
    {
        if (SD.exists(filename))
        {
            SD.remove(filename);
        }

        File file = SD.open(filename, FILE_WRITE);
        file.write(pixels, imageWidth * imageHeight * byteWidth);
        file.close();
    }

    void read()
    {
        pixels = (uint16_t *)malloc(imageWidth * imageHeight * byteWidth);
        File file = SD.open(filename, FILE_READ);
        file.read(pixels, imageWidth * imageHeight * byteWidth);
        file.close();
    }

public:
    ImageScreen(Adafruit_SPITFT *tft, const char *filename, uint16_t background,
                uint16_t imageWidth, uint16_t imageHeight) : Screen(tft),
                                                             filename(filename),
                                                             background(background),
                                                             imageWidth(imageWidth),
                                                             imageHeight(imageHeight)
    {
        read();
    }

    ImageScreen(Adafruit_SPITFT *tft, const char *filename, uint16_t background) : Screen(tft),
                                                                                   filename(filename),
                                                                                   background(background),
                                                                                   imageWidth(170),
                                                                                   imageHeight(170)
    {
        read();
    }

    ~ImageScreen()
    {
        free(pixels);
    }

    void draw()
    {
        tft->fillScreen(background);

        yield();

        tft->drawRGBBitmap((maxDim - imageWidth) / 2, (maxDim - imageHeight) / 2, pixels, imageWidth, imageHeight);

        yield();

        delay(2000);
    }
};