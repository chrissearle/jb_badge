package net.chrissearle.converter

import java.io.ByteArrayOutputStream
import java.io.File
import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import kotlin.test.assertTrue
import kotlin.test.fail

/**
 * The expectation files were produced by the previous `javax.imageio.ImageIO` implementation.
 * They are the contract: the badge firmware reads these bytes, so the decoder must stay
 * byte-compatible with ImageIO, not merely "close enough".
 */
class PngTest {
    private val pngDirectory = File("src/test/resources/png")
    private val expectedDirectory = File("src/test/resources/expected")

    private fun convert(png: File): ByteArray =
        ByteArrayOutputStream().also { output -> writeRgb565(readPng(png), output) }.toByteArray()

    @Test
    fun `every supported png variant converts exactly as ImageIO did`() {
        val pngs = pngDirectory.listFiles { file -> file.extension == "png" }?.sorted().orEmpty()
        assertTrue(pngs.isNotEmpty(), "no test images found in $pngDirectory")

        val failures =
            pngs.mapNotNull { png ->
                val expected = File(expectedDirectory, "${png.nameWithoutExtension}.dat").readBytes()
                val actual = convert(png)

                when {
                    actual.size != expected.size -> {
                        "${png.name}: produced ${actual.size} bytes, expected ${expected.size}"
                    }

                    !actual.contentEquals(expected) -> {
                        "${png.name}: ${actual.indices.count { actual[it] != expected[it] }} of " +
                            "${expected.size} bytes differ"
                    }

                    else -> {
                        null
                    }
                }
            }

        if (failures.isNotEmpty()) fail("Output no longer matches ImageIO:\n" + failures.joinToString("\n"))
    }

    /**
     * The badge images live outside this project, so this only runs from a full checkout.
     * They are the real corpus, and their committed `.dat` files are what the badge actually ships.
     */
    @Test
    fun `the badge images convert exactly as ImageIO did`() {
        val images = File("../images")
        val pngs =
            images
                .listFiles { file -> file.extension == "png" }
                ?.filter { File(images, "${it.nameWithoutExtension}.dat").exists() }
                ?.sorted()
                .orEmpty()

        if (pngs.isEmpty()) return

        val failures =
            pngs.mapNotNull { png ->
                val expected = File(images, "${png.nameWithoutExtension}.dat").readBytes()
                if (convert(png).contentEquals(expected)) null else png.name
            }

        assertEquals(emptyList(), failures, "badge images no longer round-trip to their committed .dat")
    }

    @Test
    fun `image dimensions come from the header`() {
        val image = readPng(File(pngDirectory, "odd_gray1.png"))

        assertEquals(13, image.width)
        assertEquals(7, image.height)
    }

    @Test
    fun `a file that is not a png is rejected`() {
        assertFailsWith<PngFormatException> { readPng("not a png at all".toByteArray()) }
    }

    @Test
    fun `an empty file is rejected`() {
        assertFailsWith<PngFormatException> { readPng(ByteArray(0)) }
    }

    @Test
    fun `a truncated png is rejected rather than silently half-decoded`() {
        val whole = File(pngDirectory, "rgb8.png").readBytes()

        assertFailsWith<PngFormatException> { readPng(whole.copyOf(whole.size / 2)) }
    }

    @Test
    fun `a png with corrupt pixel data is rejected`() {
        val corrupt =
            File(pngDirectory, "rgb8.png").readBytes().also { bytes ->
                // Scribble over the middle of the compressed stream, leaving the chunk framing intact.
                for (i in bytes.size / 2 until bytes.size / 2 + 8) bytes[i] = 0
            }

        assertFailsWith<PngFormatException> { readPng(corrupt) }
    }
}
