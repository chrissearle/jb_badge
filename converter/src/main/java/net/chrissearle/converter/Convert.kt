package net.chrissearle.converter

import com.google.common.io.LittleEndianDataOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.OutputStream

@Suppress("MagicNumber")
fun LittleEndianDataOutputStream.writePixelAsRGB565(pixel: Int) =
    this.writeShort(
        ((pixel and 0xf80000 shr 8) or (pixel and 0xfc00 shr 5) or (pixel and 0xf8 shr 3)),
    )

/** Writes [image] as little-endian RGB565, the format the badge display expects. */
fun writeRgb565(
    image: Image,
    output: OutputStream,
) = LittleEndianDataOutputStream(output).use { outputStream ->
    for (y in 0 until image.height) {
        for (x in 0 until image.width) {
            val pixel = image.rgb(x, y)

            outputStream.writePixelAsRGB565(pixel)
        }
    }
}

@Throws
fun main(args: Array<String>) {
    val (pngFile, datFile) = args

    val image = readPng(File(pngFile))

    FileOutputStream(datFile).use { output -> writeRgb565(image, output) }
}
