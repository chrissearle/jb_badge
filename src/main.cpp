#include <Arduino.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Adafruit_SPIDevice.h>

#include "Screen.h"
#include "Avatar.h"
#include "ImageScreen.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_BL 7

Adafruit_GC9A01A *tft;

const uint8_t screenCount = 8;

Screen *screens[screenCount];

void setup()
{
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

  tft->begin();

  if (!SD.begin(BUILTIN_SDCARD))
  {
    while (true)
      ;
  }

  // For some reason this both sets correct rotation and fixes colour order from BGR to RGB
  tft->setRotation(0);

  screens[0] = new Avatar(tft);
  screens[1] = new ImageScreen(tft, "jz2021.dat", GC9A01A_BLACK);
  screens[2] = new ImageScreen(tft, "marius_duke.dat", GC9A01A_WHITE);
  screens[3] = new ImageScreen(tft, "rock_duke.dat", GC9A01A_WHITE);
  screens[4] = new ImageScreen(tft, "tetris_duke2.dat", GC9A01A_WHITE);
  screens[5] = new ImageScreen(tft, "tetris_duke2.dat", GC9A01A_WHITE);
  screens[6] = new ImageScreen(tft, "viking_duke.dat", GC9A01A_WHITE);
  screens[7] = new ImageScreen(tft, "viking_duke2.dat", GC9A01A_WHITE);
}

uint32_t nextScreen() {
  static uint32_t lastScreen = 0;
  uint32_t nextScreen = lastScreen;
  
  do {
    nextScreen = random(screenCount);
  } while (nextScreen == lastScreen);

  lastScreen = nextScreen;

  return nextScreen;
}

void loop(void)
{
  screens[nextScreen()]->draw();
}