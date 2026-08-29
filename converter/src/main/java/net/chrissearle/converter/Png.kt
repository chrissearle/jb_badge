package net.chrissearle.converter

import java.io.ByteArrayInputStream
import java.io.File
import java.io.IOException
import java.util.zip.InflaterInputStream
import kotlin.math.abs

/**
 * A decoded image. One pixel per entry, packed as `0xRRGGBB` in 8-bit sRGB, row-major.
 *
 * Alpha is deliberately absent: the badge display has no alpha channel, and the RGB565
 * conversion discards it anyway.
 */
class Image(
    val width: Int,
    val height: Int,
    private val pixels: IntArray,
) {
    fun rgb(
        x: Int,
        y: Int,
    ): Int = pixels[y * width + x]
}

class PngFormatException(
    message: String,
    cause: Throwable? = null,
) : IOException(message, cause)

/**
 * Minimal PNG reader, written to replace `javax.imageio.ImageIO`.
 *
 * ImageIO cannot be used from a GraalVM native image: it initialises AWT, which dynamically
 * loads `libawt` from `java.library.path`, and native-image only searches next to the
 * executable. This decoder is pure JDK (`java.util.zip` only) and needs no native libraries.
 *
 * It reproduces `BufferedImage.getRGB()` byte for byte; the colour rules that requires live
 * in `PngColor.kt`.
 *
 * Supported: non-interlaced and Adam7-interlaced images, colour types 0/2/3/4/6, bit depths
 * 1/2/4/8/16. Ancillary chunks (`gAMA`, `iCCP`, `sRGB`, `cHRM`, `tRNS`) are ignored, as
 * ImageIO also ignores them when producing pixel values.
 */
fun readPng(file: File): Image = readPng(file.readBytes())

/** Decodes a PNG held in memory. See [readPng]. */
fun readPng(bytes: ByteArray): Image = PngReader(bytes).read()

internal fun pngRequire(
    condition: Boolean,
    message: () -> String,
) {
    if (!condition) throw PngFormatException(message())
}

private val SIGNATURE = byteArrayOf(-119, 80, 78, 71, 13, 10, 26, 10)

private const val CHUNK_LENGTH_BYTES = 4
private const val CHUNK_TYPE_BYTES = 4
private const val CHUNK_CRC_BYTES = 4
private const val CHUNK_OVERHEAD = CHUNK_LENGTH_BYTES + CHUNK_TYPE_BYTES + CHUNK_CRC_BYTES

private const val IHDR_BYTES = 13
private const val PALETTE_ENTRY_BYTES = 3

/** The colour types defined by the PNG spec, with the bit depths each one permits. */
@Suppress("MagicNumber")
private enum class ColorType(
    val code: Int,
    val channels: Int,
    val bitDepths: Set<Int>,
    val kind: PixelFormat.Kind,
) {
    GRAYSCALE(0, 1, setOf(1, 2, 4, 8, 16), PixelFormat.Kind.GRAYSCALE),
    RGB(2, 3, setOf(8, 16), PixelFormat.Kind.TRUECOLOUR),
    PALETTE(3, 1, setOf(1, 2, 4, 8), PixelFormat.Kind.PALETTE),
    GRAYSCALE_ALPHA(4, 2, setOf(8, 16), PixelFormat.Kind.GRAYSCALE),
    RGB_ALPHA(6, 4, setOf(8, 16), PixelFormat.Kind.TRUECOLOUR),
    ;

    companion object {
        fun from(code: Int): ColorType =
            entries.firstOrNull { it.code == code }
                ?: throw PngFormatException("Unsupported PNG colour type: $code")
    }
}

/**
 * The per-scanline filters from PNG spec section 9.2.
 *
 * Each predicts a byte from its already reconstructed neighbours: `left` one pixel back,
 * `up` on the previous scanline, and `upperLeft` diagonally back.
 */
private enum class ScanlineFilter {
    NONE {
        override fun predict(
            left: Int,
            up: Int,
            upperLeft: Int,
        ) = 0
    },
    SUB {
        override fun predict(
            left: Int,
            up: Int,
            upperLeft: Int,
        ) = left
    },
    UP {
        override fun predict(
            left: Int,
            up: Int,
            upperLeft: Int,
        ) = up
    },
    AVERAGE {
        override fun predict(
            left: Int,
            up: Int,
            upperLeft: Int,
        ) = (left + up) / 2
    },
    PAETH {
        /** Picks whichever neighbour the linear estimate `left + up - upperLeft` lands closest to. */
        override fun predict(
            left: Int,
            up: Int,
            upperLeft: Int,
        ): Int {
            val estimate = left + up - upperLeft
            val toLeft = abs(estimate - left)
            val toUp = abs(estimate - up)
            val toUpperLeft = abs(estimate - upperLeft)

            return when {
                toLeft <= toUp && toLeft <= toUpperLeft -> left
                toUp <= toUpperLeft -> up
                else -> upperLeft
            }
        }
    },
    ;

    abstract fun predict(
        left: Int,
        up: Int,
        upperLeft: Int,
    ): Int

    companion object {
        fun from(code: Int): ScanlineFilter =
            entries.getOrNull(code) ?: throw PngFormatException("Unsupported PNG filter type: $code")
    }
}

/** One Adam7 interlace pass: the sub-image starts at ([xOffset], [yOffset]) and steps by ([xStep], [yStep]). */
private data class Adam7Pass(
    val xOffset: Int,
    val yOffset: Int,
    val xStep: Int,
    val yStep: Int,
)

@Suppress("MagicNumber")
private val ADAM7_PASSES =
    listOf(
        Adam7Pass(0, 0, 8, 8),
        Adam7Pass(4, 0, 8, 8),
        Adam7Pass(0, 4, 4, 8),
        Adam7Pass(2, 0, 4, 4),
        Adam7Pass(0, 2, 2, 4),
        Adam7Pass(1, 0, 2, 2),
        Adam7Pass(0, 1, 1, 2),
    )

private val NO_INTERLACE = listOf(Adam7Pass(0, 0, 1, 1))

private data class Header(
    val width: Int,
    val height: Int,
    val bitDepth: Int,
    val colorType: ColorType,
    val interlaced: Boolean,
) {
    val channels: Int get() = colorType.channels

    val pixelFormat: PixelFormat get() = PixelFormat(width * height, channels, bitDepth, colorType.kind)
}

@Suppress("MagicNumber")
private class PngReader(
    private val bytes: ByteArray,
) {
    fun read(): Image {
        verifySignature()

        var header: Header? = null
        var palette: IntArray? = null
        val compressed = mutableListOf<ByteArray>()

        forEachChunk { type, offset, length ->
            when (type) {
                "IHDR" -> header = readHeader(offset, length)
                "PLTE" -> palette = readPalette(offset, length)
                "IDAT" -> compressed += bytes.copyOfRange(offset, offset + length)
                else -> Unit
            }
        }

        val ihdr = header ?: throw PngFormatException("PNG has no IHDR chunk")
        pngRequire(compressed.isNotEmpty()) { "PNG has no IDAT chunk" }

        val samples = decodeSamples(ihdr, inflate(compressed))

        return Image(ihdr.width, ihdr.height, toRgb(samples, ihdr.pixelFormat, palette))
    }

    private fun verifySignature() =
        pngRequire(bytes.size >= SIGNATURE.size && SIGNATURE.contentEquals(bytes.copyOf(SIGNATURE.size))) {
            "Not a PNG file"
        }

    /** Walks the chunk list, handing each chunk's type and payload range to [action]. */
    private inline fun forEachChunk(action: (type: String, offset: Int, length: Int) -> Unit) {
        var position = SIGNATURE.size

        while (position + CHUNK_OVERHEAD <= bytes.size) {
            val length = readInt(position)
            val type = String(bytes, position + CHUNK_LENGTH_BYTES, CHUNK_TYPE_BYTES, Charsets.US_ASCII)
            val payload = position + CHUNK_LENGTH_BYTES + CHUNK_TYPE_BYTES

            pngRequire(length >= 0 && payload + length + CHUNK_CRC_BYTES <= bytes.size) {
                "Truncated PNG: chunk $type claims $length bytes"
            }

            action(type, payload, length)
            if (type == "IEND") return
            position = payload + length + CHUNK_CRC_BYTES
        }

        throw PngFormatException("Truncated PNG: no IEND chunk")
    }

    private fun readInt(offset: Int): Int =
        (bytes[offset].toInt() and 0xff shl 24) or
            (bytes[offset + 1].toInt() and 0xff shl 16) or
            (bytes[offset + 2].toInt() and 0xff shl 8) or
            (bytes[offset + 3].toInt() and 0xff)

    private fun readHeader(
        offset: Int,
        length: Int,
    ): Header {
        pngRequire(length == IHDR_BYTES) { "Malformed IHDR chunk" }

        val width = readInt(offset)
        val height = readInt(offset + 4)
        val bitDepth = bytes[offset + 8].toInt() and 0xff
        val colorType = ColorType.from(bytes[offset + 9].toInt() and 0xff)
        val compression = bytes[offset + 10].toInt() and 0xff
        val filter = bytes[offset + 11].toInt() and 0xff
        val interlace = bytes[offset + 12].toInt() and 0xff

        pngRequire(width > 0 && height > 0) { "PNG has non-positive dimensions" }
        pngRequire(bitDepth in colorType.bitDepths) {
            "Bit depth $bitDepth is not valid for colour type ${colorType.code}"
        }
        pngRequire(compression == 0) { "Unsupported PNG compression method: $compression" }
        pngRequire(filter == 0) { "Unsupported PNG filter method: $filter" }
        pngRequire(interlace <= 1) { "Unsupported PNG interlace method: $interlace" }

        return Header(width, height, bitDepth, colorType, interlaced = interlace == 1)
    }

    private fun readPalette(
        offset: Int,
        length: Int,
    ): IntArray {
        pngRequire(length % PALETTE_ENTRY_BYTES == 0) { "Malformed PLTE chunk" }

        return IntArray(length / PALETTE_ENTRY_BYTES) { entry ->
            val at = offset + entry * PALETTE_ENTRY_BYTES

            packRgb(
                bytes[at].toInt() and 0xff,
                bytes[at + 1].toInt() and 0xff,
                bytes[at + 2].toInt() and 0xff,
            )
        }
    }

    private fun inflate(compressed: List<ByteArray>): ByteArray {
        // IDAT chunks form one zlib stream; the split points are arbitrary, so join before inflating.
        val joined =
            ByteArray(compressed.sumOf { it.size }).also { target ->
                var at = 0

                compressed.forEach { chunk ->
                    chunk.copyInto(target, at)
                    at += chunk.size
                }
            }

        return try {
            InflaterInputStream(ByteArrayInputStream(joined)).use { it.readBytes() }
        } catch (e: IOException) {
            throw PngFormatException("Corrupt PNG pixel data: ${e.message}", e)
        }
    }
}

/**
 * Un-filters every scanline and scatters the raw samples into one `width * height * channels` grid.
 *
 * Interlaced images are stored as seven sub-images; each is filtered independently and its pixels
 * land on a sparse lattice of the full image.
 */
private fun decodeSamples(
    header: Header,
    inflated: ByteArray,
): IntArray {
    val samples = IntArray(header.width * header.height * header.channels)
    val passes = if (header.interlaced) ADAM7_PASSES else NO_INTERLACE
    var offset = 0

    passes.forEach { pass ->
        val passWidth = ceilDiv(header.width - pass.xOffset, pass.xStep)
        val passHeight = ceilDiv(header.height - pass.yOffset, pass.yStep)

        if (passWidth > 0 && passHeight > 0) {
            offset = readPass(header, inflated, offset, pass, passWidth, passHeight, samples)
        }
    }

    return samples
}

@Suppress("LongParameterList")
private fun readPass(
    header: Header,
    inflated: ByteArray,
    start: Int,
    pass: Adam7Pass,
    passWidth: Int,
    passHeight: Int,
    samples: IntArray,
): Int {
    val bitsPerPixel = header.channels * header.bitDepth
    // Filters operate on whole bytes, one pixel apart -- rounded up to 1 for sub-byte pixels.
    val filterStride = maxOf(1, bitsPerPixel / BITS_PER_BYTE)
    val lineBytes = (passWidth * bitsPerPixel + BITS_PER_BYTE - 1) / BITS_PER_BYTE

    var offset = start
    var previous = ByteArray(lineBytes)

    repeat(passHeight) { passY ->
        pngRequire(offset + 1 + lineBytes <= inflated.size) { "Truncated PNG pixel data" }

        val filter = ScanlineFilter.from(inflated[offset].toInt() and 0xff)
        val line = inflated.copyOfRange(offset + 1, offset + 1 + lineBytes)
        offset += 1 + lineBytes

        unfilter(line, previous, filter, filterStride)
        scatter(line, header, pass, passWidth, pass.yOffset + passY * pass.yStep, samples)
        previous = line
    }

    return offset
}

/** Reverses a scanline's filter, in place. */
private fun unfilter(
    line: ByteArray,
    previous: ByteArray,
    filter: ScanlineFilter,
    stride: Int,
) = line.indices.forEach { i ->
    val left = if (i >= stride) line[i - stride].toInt() and 0xff else 0
    val up = previous[i].toInt() and 0xff
    val upperLeft = if (i >= stride) previous[i - stride].toInt() and 0xff else 0

    line[i] = (line[i] + filter.predict(left, up, upperLeft)).toByte()
}

/** Unpacks one scanline's samples and writes them to their positions in the full-image grid. */
@Suppress("MagicNumber", "LongParameterList")
private fun scatter(
    line: ByteArray,
    header: Header,
    pass: Adam7Pass,
    passWidth: Int,
    y: Int,
    samples: IntArray,
) {
    val channels = header.channels
    val depth = header.bitDepth
    val rowStart = y * header.width * channels

    for (passX in 0 until passWidth) {
        val target = rowStart + (pass.xOffset + passX * pass.xStep) * channels

        for (channel in 0 until channels) {
            val index = passX * channels + channel

            samples[target + channel] =
                when (depth) {
                    16 -> {
                        ((line[index * 2].toInt() and 0xff) shl 8) or (line[index * 2 + 1].toInt() and 0xff)
                    }

                    8 -> {
                        line[index].toInt() and 0xff
                    }

                    else -> {
                        // Sub-byte samples are packed most-significant-bit first, left to right.
                        val perByte = BITS_PER_BYTE / depth
                        val shift = BITS_PER_BYTE - depth * (index % perByte + 1)

                        (line[index / perByte].toInt() and 0xff shr shift) and ((1 shl depth) - 1)
                    }
                }
        }
    }
}

private fun ceilDiv(
    value: Int,
    divisor: Int,
): Int = if (value <= 0) 0 else (value + divisor - 1) / divisor
