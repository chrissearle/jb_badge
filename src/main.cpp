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

  delay(5000);
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
  jz2021 = new ImageScreen(tft, "jz2021.dat", GC9A01A_BLACK);
  dukeMarius = new ImageScreen(tft, "marius_duke.dat", GC9A01A_WHITE);
  dukeRock = new ImageScreen(tft, "rock_duke.dat", GC9A01A_WHITE);
  dukeTetris = new ImageScreen(tft, "tetris_duke2.dat", GC9A01A_WHITE);
  dukeTetris2 = new ImageScreen(tft, "tetris_duke2.dat", GC9A01A_WHITE);
  dukeViking = new ImageScreen(tft, "viking_duke.dat", GC9A01A_WHITE);
  dukeViking2 = new ImageScreen(tft, "viking_duke2.dat", GC9A01A_WHITE);
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