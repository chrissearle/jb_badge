package net.chrissearle.converter;

import com.google.common.io.LittleEndianDataOutputStream;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;


public class Convert {
    public static void main(String[] args) {
        String pngFile = args[0];
        String datFile = args[1];

        try {
            BufferedImage image = ImageIO.read(new File(pngFile));

            try (
                    LittleEndianDataOutputStream outputStream = new LittleEndianDataOutputStream(new FileOutputStream(datFile))
            ) {

                for (int y = 0; y < image.getHeight(); y++) {
                    for (int x = 0; x < image.getWidth(); x++) {
                        int pixel = image.getRGB(x, y);
                        short rgb565 = (short) (((pixel & 0xf80000) >> 8) | ((pixel & 0xfc00) >> 5) | ((pixel & 0xf8) >> 3));
                        outputStream.writeShort(rgb565);
                    }
                }
            }
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }
    }
}
