package net.chrissearle.converter

import com.google.common.io.LittleEndianDataOutputStream
import java.io.File
import java.io.FileOutputStream
import javax.imageio.ImageIO
import kotlin.jvm.Throws

@Suppress("MagicNumber")
fun LittleEndianDataOutputStream.writePixel(pixel: Int) =
    this.writeShort(
        ((pixel and 0xf80000 shr 8) or (pixel and 0xfc00 shr 5) or (pixel and 0xf8 shr 3)),
    )

@Suppress("UnstableApiUsage") // LittleEndianDataOutputStream is marked @Beta
@Throws
fun main(args: Array<String>) {
    val (pngFile, datFile) = args

    val image = ImageIO.read(File(pngFile))

    LittleEndianDataOutputStream(FileOutputStream(datFile)).use { outputStream ->
        for (y in 0 until image.height) {
            for (x in 0 until image.width) {
                val pixel = image.getRGB(x, y)

                outputStream.writePixel(pixel)
            }
        }
    }
}
