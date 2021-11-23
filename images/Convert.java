import javax.imageio.ImageIO;
import java.io.File;
import java.awt.image.BufferedImage;

class Convert {
    public static void main(String[] args) {
        try {
            BufferedImage image = ImageIO.read(new File(args[0]));

            System.out.printf("uint16_t pcolors[%d] = {", image.getHeight() * image.getWidth());

            String sep = "";
            for (int y = 0; y < image.getHeight(); y++) {
                for (int x = 0; x < image.getWidth(); x++) {
                    int pixel = image.getRGB(x, y);
                    int rgb565 = ((pixel & 0xf80000) >> 8) | ((pixel & 0xfc00) >> 5) | ((pixel & 0xf8) >> 3);
                    System.out.printf("%s%d", sep, rgb565);
                    sep = ", ";
                }
            }

            System.out.printf("};%n");

        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }
}