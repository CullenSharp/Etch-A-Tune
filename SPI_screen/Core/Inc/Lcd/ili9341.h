/* To clear the screen before display turning on ?
   - 0: does not clear
   - 1: clear */
#define  ILI9341_INITCLEAR        1

/* Color order (0 = RGB, 1 = BGR) */
#define  ILI9341_COLORMODE        1

/* Draw and read bitdeph (16: RGB565, 24: RGB888)
   note: my SPI ILI9341 LCD only readable if ILI9341_READBITDEPTH 24 */
#define  ILI9341_WRITEBITDEPTH     16
#define  ILI9341_READBITDEPTH      24

// ILI9341 physic resolution (in 0 orientation)
#define  ILI9341_LCD_WIDTH  240
#define  ILI9341_LCD_HEIGHT 320
