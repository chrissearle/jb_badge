# java badge

Uses a GC9A01A and a Teensy 4.1 microcontroller

## Image Conversion

* All images are 170 pixels square PNG

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

