/* Orientation
   - 0: 320x480 portrait (plug in top)
   - 1: 480x320 landscape (plug in left)
   - 2: 320x480 portrait (plug in botton)
   - 3: 480x320 landscape (plug in right) */
#define  ST7796_ORIENTATION             0

/* To clear the screen before display turning on ?
   - 0: does not clear
   - 1: clear */
#define  ST7796_INITCLEAR               1

/* Color order (0 = RGB, 1 = BGR) */
#define  ST7796_COLORMODE               1

/* Draw and read bitdeph (16: RGB565, 24: RGB888) 
   note: my SPI ST7796 LCD only readable if ST7796_READBITDEPTH 24 */
#define  ST7796_WRITEBITDEPTH           16
#define  ST7796_READBITDEPTH            24

// ST7796 physical resolution (in 0 orientation)
#define  ST7796_LCD_PIXEL_WIDTH         320
#define  ST7796_LCD_PIXEL_HEIGHT        480
