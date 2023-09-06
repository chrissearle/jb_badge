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

const uint8_t screenCount = 17;

Screen *currentScreen;

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

  tft->fillScreen(GC9A01A_BLACK);

  currentScreen = new Avatar(tft);
}

void nextScreen()
{
  static uint32_t lastScreen = 0;
  uint32_t nextScreen = lastScreen;

  do
  {
    nextScreen = random(screenCount);
  } while (nextScreen == lastScreen);

  lastScreen = nextScreen;

  if (currentScreen != nullptr)
  {
    delete currentScreen;
  }

  switch (nextScreen)
  {
  case 0:
    currentScreen = new Avatar(tft);
    break;
  case 1:
    currentScreen = new ImageScreen(tft, "jz2021.dat", GC9A01A_BLACK);
    break;
  case 2:
    currentScreen = new ImageScreen(tft, "marius_duke.dat", GC9A01A_WHITE, 240, 234);
    break;
  case 3:
    currentScreen = new ImageScreen(tft, "rock_duke.dat", GC9A01A_WHITE, 240, 158);
    break;
  case 4:
    currentScreen = new ImageScreen(tft, "tetris_duke.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 5:
    currentScreen = new ImageScreen(tft, "tetris_duke2.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 6:
    currentScreen = new ImageScreen(tft, "viking_duke.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 7:
    currentScreen = new ImageScreen(tft, "viking_duke2.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 8:
    currentScreen = new ImageScreen(tft, "nisse_duke.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 9:
    currentScreen = new ImageScreen(tft, "nisse_duke2.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 10:
    currentScreen = new ImageScreen(tft, "itera.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 11:
    currentScreen = new ImageScreen(tft, "wheely.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 12:
    currentScreen = new ImageScreen(tft, "wendy.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 13:
    currentScreen = new ImageScreen(tft, "bendy.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 14:
    currentScreen = new ImageScreen(tft, "hero.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 15:
    currentScreen = new ImageScreen(tft, "umbrella_duke.dat", GC9A01A_WHITE, 240, 240);
    break;
  case 16:
    currentScreen = new ImageScreen(tft, "party_duke.dat", GC9A01A_WHITE, 240, 240);
    break;
  default:
    currentScreen = new Avatar(tft);
    break;
  }
}

void loop(void)
{
  currentScreen->draw();
  nextScreen();
}