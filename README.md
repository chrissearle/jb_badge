# java badge

Uses a GC9A01A and a Teensy 4.1 microcontroller

Displays different images on screen.

The Teensy 4 version of the SD library is not compatible with the Adafruit_GFX image loader classes
so loading from bitmap on the SD card does not work out of the box.

Adafruit_GFX is expecting image data as rgb565 (16 bit number - 5 bits red, 6 bits green 5 bits blue).

The java project in convert will read a PNG and spit out a dat file of data in that format - a list of bytes that are really 16 bits in little endian order.

These can then be read from the SD card to a `uint16_t *` and passed to `drawRGBBitmap`.

## Image Conversion

* All images **MUST** be 170 pixels square PNG
* Transparent pixels will be dropped (black pixel) - alpha is ignored.

### Build converter

```shell
cd converter
mvn clean install
```

### Convert images

```shell
cd images
for F in *png; do
    java -jar ../converter/target/converter-1.0-SNAPSHOT.jar $F ${F%%.png}.dat
done
```

