#include <Arduino.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Adafruit_SPIDevice.h>

#include "Screen.h"
#include "Avatar.h"
#include "JZ2021Screen.h"
#include "DukeVikingScreen.h"
#include "DukeViking2Screen.h"
#include "DukeRockScreen.h"
#include "DukeTetrisScreen.h"
#include "DukeTetris2Screen.h"
#include "DukeMariusScreen.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_BL 7

Adafruit_GC9A01A *tft;

Screen *avatar;
Screen *jz2021;
Screen *dukeMarius;
Screen *dukeTetris;
Screen *dukeTetris2;
Screen *dukeViking;
Screen *dukeViking2;
Screen *dukeRock;

void setup()
{
  Serial.begin(11520);
  Serial.println(F("GC9A01A Test!"));

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  Serial.println("Starting TFT");

  tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

  tft->begin();

  Serial.println(F("Starting SD"));

  if (!SD.begin(BUILTIN_SDCARD))
  {
    Serial.println(F("SD init failed"));

    while (true)
      ;
  }

  Serial.println(F("Setup Done!"));

  // For some reason this both sets correct rotation and fixes colour order from BGR to RGB
  tft->setRotation(0);

  avatar = new Avatar(tft);
  jz2021 = new JZ2021Screen(tft);
  dukeMarius = new DukeMariusScreen(tft);
  dukeRock = new DukeRockScreen(tft);
  dukeTetris = new DukeTetrisScreen(tft);
  dukeTetris2 = new DukeTetris2Screen(tft);
  dukeViking = new DukeVikingScreen(tft);
  dukeViking2 = new DukeViking2Screen(tft);
}

void loop(void)
{
  avatar->draw();
  jz2021->draw();
  dukeMarius->draw();
  dukeRock->draw();
  dukeTetris->draw();
  dukeTetris2->draw();
  dukeViking->draw();
  dukeViking2->draw();
}