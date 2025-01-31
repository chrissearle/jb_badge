#include <Arduino.h>
#include <Adafruit_GC9A01A.h>

struct screenInfo
{
    const char *filename;
    uint16_t background;
    uint16_t imageWidth;
    uint16_t imageHeight;
};

screenInfo screens[] = {
    {"jz2021.dat", GC9A01A_BLACK, 240, 240},
    {"marius_duke.dat", GC9A01A_WHITE, 240, 234},
    {"rock_duke.dat", GC9A01A_WHITE, 240, 158},
    {"tetris_duke.dat", GC9A01A_WHITE, 240, 240},
    {"tetris_duke2.dat", GC9A01A_WHITE, 240, 240},
    {"viking_duke.dat", GC9A01A_WHITE, 240, 240},
    {"viking_duke2.dat", GC9A01A_WHITE, 240, 240},
    {"nisse_duke.dat", GC9A01A_WHITE, 240, 240},
    {"nisse_duke2.dat", GC9A01A_WHITE, 240, 240},
    {"itera.dat", GC9A01A_WHITE, 240, 240},
    {"wheely.dat", GC9A01A_WHITE, 240, 240},
    {"wendy.dat", GC9A01A_WHITE, 240, 240},
    {"bendy.dat", GC9A01A_WHITE, 240, 240},
    {"hero.dat", GC9A01A_WHITE, 240, 240},
    {"umbrella_duke.dat", GC9A01A_WHITE, 240, 240},
    {"party_duke.dat", GC9A01A_WHITE, 240, 240},
    {"bart_duke.dat", GC9A01A_WHITE, 240, 240},
    {"2024_circle.dat", GC9A01A_WHITE, 200, 200},
    {"jz24-disc.dat", GC9A01A_WHITE, 240, 240},
    {"jz24-usb-duke.dat", GC9A01A_WHITE, 240, 240},
    {"jz24-mc.dat", GC9A01A_WHITE, 240, 240},
    {"jz24-sky.dat", GC9A01A_WHITE, 240, 240},
    {"reg_vestfold.dat", GC9A01A_WHITE, 240, 240},
    {"reg_bergen.dat", GC9A01A_WHITE, 240, 240},
    {"reg_oslo.dat", GC9A01A_WHITE, 240, 240},
    {"reg_soerlandet.dat", GC9A01A_WHITE, 240, 240},
    {"reg_sogn.dat", GC9A01A_WHITE, 240, 240},
    {"reg_stavanger.dat", GC9A01A_WHITE, 240, 240},
    {"reg_tromsoe.dat", GC9A01A_WHITE, 240, 240},
    {"reg_trondheim.dat", GC9A01A_WHITE, 240, 240},
};

size_t screenCount = (sizeof(screens) / sizeof(screens[0])) + 1;