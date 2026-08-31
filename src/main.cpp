#include <Arduino.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Adafruit_SPIDevice.h>

#include "Screen.h"
#include "Avatar.h"
#include "LowBattery.h"
#include "ImageScreen.h"
#include "Status.h"
#include "Voltage.h"
#include "ScreenList.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_BL 7

#define VOLT_PIN 18

// TODO - set button pin
#define BOUNCE_PIN 2

Adafruit_GC9A01A *tft;
Screen *currentScreen;
Status *status;
Voltage *voltage;

unsigned long previousMillis = 0;
uint32_t interval;

void setupScreen()
{
  tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

  tft->begin();

  // For some reason this both sets correct rotation and fixes colour order from BGR to RGB
  tft->setRotation(0);

  tft->fillScreen(GC9A01A_BLACK);
}

void initializeSD()
{
  if (!SD.begin(BUILTIN_SDCARD))
  {
    tft->fillScreen(GC9A01A_YELLOW);

    while (true)
      ;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(VOLT_PIN, INPUT);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  setupScreen();
  initializeSD();

  currentScreen = new Avatar(tft);

  status = new Status(BOUNCE_PIN, TFT_BL);
  voltage = new Voltage(VOLT_PIN);

  previousMillis = millis();
}

void nextScreen()
{
  if (voltage->isLowBattery())
  {
    currentScreen = new LowBattery(tft);
    return;
  }

  static uint32_t lastScreen = 0;
  uint32_t pick = lastScreen;

  do
  {
    pick = random(screenCount + 1);
  } while (pick == lastScreen);

  lastScreen = pick;

  if (currentScreen != nullptr)
  {
    delete currentScreen;
  }

  if (pick == 0)
  {
    currentScreen = new Avatar(tft);
  }
  else
  {
    const screenInfo &info = screens[pick - 1];
    currentScreen = new ImageScreen(tft, info.filename, info.background, info.imageWidth, info.imageHeight);
  }
}

void loop(void)
{
  status->process();

  static bool wasActive = true;
  const bool active = status->isActive();

  if (active)
  {
    voltage->process();
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      nextScreen();

      if (currentScreen != nullptr)
      {
        interval = currentScreen->draw();
      }

      previousMillis = millis();
    }
  }
  else if (wasActive)
  {
    tft->fillScreen(GC9A01A_BLACK);
  }

  wasActive = active;
}