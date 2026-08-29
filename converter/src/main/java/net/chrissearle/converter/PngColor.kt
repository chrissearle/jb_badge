package net.chrissearle.converter

import kotlin.math.pow

internal const val MAX_8_BIT = 255
internal const val MAX_16_BIT = 65535
internal const val BITS_PER_BYTE = 8

private const val RED_SHIFT = 16
private const val GREEN_SHIFT = 8

/** Rounds a scaled value to nearest when truncated with `toInt()`. */
private const val ROUND_TO_NEAREST = 0.5

/**
 * The parts of the PNG header that decide how samples become colours.
 */
internal data class PixelFormat(
    val pixelCount: Int,
    val channels: Int,
    val bitDepth: Int,
    val kind: Kind,
) {
    internal enum class Kind {
        GRAYSCALE,
        PALETTE,
        TRUECOLOUR,
    }
}

/**
 * Maps raw PNG samples to packed `0xRRGGBB` sRGB pixels.
 *
 * Every rule here exists to reproduce `BufferedImage.getRGB()` exactly, because the badge
 * firmware consumes the resulting bytes and the committed `.dat` files must not shift.
 */
internal fun toRgb(
    samples: IntArray,
    format: PixelFormat,
    palette: IntArray?,
): IntArray =
    IntArray(format.pixelCount) { pixel ->
        val base = pixel * format.channels

        when (format.kind) {
            PixelFormat.Kind.GRAYSCALE -> {
                grayToRgb(samples[base], format.bitDepth)
            }

            PixelFormat.Kind.PALETTE -> {
                val entries = palette ?: throw PngFormatException("Indexed PNG has no PLTE chunk")

                entries.getOrNull(samples[base])
                    ?: throw PngFormatException("Palette index ${samples[base]} is out of range")
            }

            PixelFormat.Kind.TRUECOLOUR -> {
                packRgb(
                    scaleSample(samples[base], format.bitDepth),
                    scaleSample(samples[base + 1], format.bitDepth),
                    scaleSample(samples[base + 2], format.bitDepth),
                )
            }
        }
    }

/**
 * Converts a colour sample to 8 bits, matching `ComponentColorModel.getRGB()`.
 *
 * 16-bit samples are rescaled to 0..255 with rounding, *not* truncated with `shr 8`. Truncating
 * changes the RGB565 output of real 16-bit images, so this rounding is load-bearing.
 */
private fun scaleSample(
    sample: Int,
    bitDepth: Int,
): Int =
    if (bitDepth == BITS_PER_BYTE) {
        sample
    } else {
        (sample.toDouble() * MAX_8_BIT / MAX_16_BIT + ROUND_TO_NEAREST).toInt()
    }

/**
 * Converts a grayscale sample to a packed sRGB pixel, matching what ImageIO's `getRGB()` returns.
 *
 * This is the subtle one. ImageIO reads 8- and 16-bit grayscale into `TYPE_BYTE_GRAY` /
 * `TYPE_USHORT_GRAY`, whose colour space is *linear* gray, so `getRGB()` applies the linear ->
 * sRGB transfer function. Passing the sample straight through instead visibly darkens the image.
 *
 * Sub-byte depths do not take that path: they load as `TYPE_BYTE_BINARY` backed by an
 * `IndexColorModel` holding an sRGB ramp, so the sample is only rescaled. The 255/(2^d - 1)
 * factor is exact for depths 1, 2 and 4 (255, 85, 17), so no rounding is needed.
 */
private fun grayToRgb(
    sample: Int,
    bitDepth: Int,
): Int {
    val level =
        when {
            bitDepth < BITS_PER_BYTE -> sample * MAX_8_BIT / ((1 shl bitDepth) - 1)
            bitDepth == BITS_PER_BYTE -> linearToSrgb(sample.toDouble() / MAX_8_BIT)
            else -> linearToSrgb(sample.toDouble() / MAX_16_BIT)
        }

    return packRgb(level, level, level)
}

/** The sRGB transfer function, returning an 8-bit level. IEC 61966-2-1. */
@Suppress("MagicNumber")
private fun linearToSrgb(linear: Double): Int {
    val encoded =
        if (linear <= 0.0031308) {
            12.92 * linear
        } else {
            1.055 * linear.pow(1.0 / 2.4) - 0.055
        }

    return (encoded * MAX_8_BIT + ROUND_TO_NEAREST).toInt()
}

internal fun packRgb(
    red: Int,
    green: Int,
    blue: Int,
): Int = (red shl RED_SHIFT) or (green shl GREEN_SHIFT) or blue
