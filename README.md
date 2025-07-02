# javaBin badge

Uses a GC9A01A display and a Teensy 4.1 microcontroller (since the 4.1 has a micro-SD card slot).

Displays different images on screen.

## Display

GC9A01A round display - I used this part from waveshare: https://www.waveshare.com/1.28inch-lcd-module.htm

![Display Front](docs/front.png)
![Display Back](docs/back.png)

## Teensy

PlatformIO Project - see `platformio.ini` and `src` folder.

The Teensy 4 version of the SD library is not compatible with the Adafruit_GFX image loader classes
so loading from bitmap on the SD card does not work out of the box.

Adafruit_GFX is expecting image data as rgb565 (16 bit number - 5 bits red, 6 bits green 5 bits blue).

## Connections

- Screen -> Teensy

- VCC -> 3.3V
- GND -> GND
- DIN -> 11 (MOSI)
- CLK -> 13 (SCK)
- CS -> 10 (CS)
- DC -> 9
- RST -> 8
- BL -> 7

- Voltage monitor

- pin 18
- Voltage divide VCC in half - two 10k resistors or similar

## Image conversion/preparation

The java project in the `converter` directory will read a PNG and spit out a dat file of data in that format - a list of bytes that are really 16 bits in little endian order.

These can then be read from the SD card to a `uint16_t *` and passed to `drawRGBBitmap`.

Note that when converting it must write the bytes in little-endian.

### Image Conversion

- All images **MUST** be max of 240 pixels per side
- Default size is 170 pixel square
- Format - PNG
- Transparent pixels will be dropped (black pixel) - alpha is ignored.

### Build converter

Expects Graal JDK 22.

```shell
cd converter
./gradlew clean nativeCompile
```

This builds a native executable in converter/build/native/nativeCompile/jb_badge_converter

### Convert images

```shell
cd images
for F in *png; do
    ./converter/build/native/nativeCompile/jb_badge_converter $F ${F%%.png}.dat
done
```
