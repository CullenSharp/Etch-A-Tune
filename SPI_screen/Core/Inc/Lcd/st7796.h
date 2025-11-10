
/*---------------Exported Settings-------------------*/
/* Orientation
   - 0: 320x480 portrait (plug on top)
   - 1: 480x320 landscape (plug on left)
   - 2: 320x480 portrait (plug on bottom)
   - 3: 480x320 landscape (plug on right) */
#define  ST7796_ORIENTATION             1

/* To clear the screen before display turning on ?
   - 0: does not clear
   - 1: clear */
#define  ST7796_INITCLEAR               1

/* Color order (0 = RGB, 1 = BGR) */
#define  ST7796_COLORMODE               2

/* Draw and read bit-depth (16: RGB565, 24: RGB888)
   note: my SPI ST7796 LCD only readable if ST7796_READBITDEPTH 24 */
#define  ST7796_WRITEBITDEPTH           16
#define  ST7796_READBITDEPTH            24

// ST7796 physical resolution (in 0 orientation)
#define  ST7796_LCD_PIXEL_WIDTH         320
#define  ST7796_LCD_PIXEL_HEIGHT        480

/* -------------------Exported Functions--------------*/
void     st7796_Init(void);
uint32_t st7796_ReadID(void);
void     st7796_DisplayOn(void);
void     st7796_DisplayOff(void);
void     st7796_SetCursor(uint16_t Xpos, uint16_t Ypos);
void     st7796_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code);
uint16_t st7796_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void     st7796_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     st7796_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     st7796_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     st7796_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t RGBCode);
uint16_t st7796_GetLcdPixelWidth(void);
uint16_t st7796_GetLcdPixelHeight(void);
void     st7796_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
void     st7796_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData);
void     st7796_ReadRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData);
void     st7796_Scroll(int16_t Scroll, uint16_t TopFix, uint16_t BottonFix);
void     st7796_UserCommand(uint16_t Command, uint8_t * pData, uint32_t Size, uint8_t Mode);

/* ------------------- Registers and Command Words ----*/

#define ST7796_NOP            0x00
#define ST7796_SWRESET        0x01

#define ST7796_RDDID          0x04
#define ST7796_RDDST          0x09
#define ST7796_RDMODE         0x0A
#define ST7796_RDMADCTL       0x0B
#define ST7796_RDPIXFMT       0x0C
#define ST7796_RDIMGFMT       0x0D
#define ST7796_RDSELFDIAG     0x0F

#define ST7796_SLPIN          0x10
#define ST7796_SLPOUT         0x11
#define ST7796_PTLON          0x12
#define ST7796_NORON          0x13

#define ST7796_INVOFF         0x20
#define ST7796_INVON          0x21
#define ST7796_DISPOFF        0x28
#define ST7796_DISPON         0x29

#define ST7796_CASET          0x2A
#define ST7796_PASET          0x2B
#define ST7796_RAMWR          0x2C
#define ST7796_RAMRD          0x2E

#define ST7796_PTLAR          0x30
#define ST7796_VSCRDEF        0x33
#define ST7796_MADCTL         0x36
#define ST7796_VSCRSADD       0x37      /* Vertical Scrolling Start Address */
#define ST7796_PIXFMT         0x3A      /* COLMOD: Pixel Format Set */

#define ST7796_IFMODE  		  0xB0     /* RGB Interface Signal Control */
#define ST7796_FRMCTR1        0xB1
#define ST7796_FRMCTR2        0xB2
#define ST7796_FRMCTR3        0xB3
#define ST7796_INVCTR         0xB4
#define ST7796_BPC			  0xB5
#define ST7796_DFUNCTR        0xB6      /* Display Function Control */
#define ST7796_EM			  0xB7

#define ST7796_PWCTR1         0xC0
#define ST7796_PWCTR2         0xC1
#define ST7796_PWCTR3         0xC2
#define ST7796_VMCTR1         0xC5
#define ST7796_VMCTR2         0xC7

#define ST7796_RDID1          0xDA
#define ST7796_RDID2          0xDB
#define ST7796_RDID3          0xDC

#define ST7796_GMCTRP1        0xE0
#define ST7796_GMCTRN1        0xE1

#define ST7796_CSCON		  0xF0

/* Extend register commands */
#define ST7796_DTCA           0xE8    /* Driver timing control A */

//-----------------------------------------------------------------------------
#define ST7796_MAD_RGB        0x00
#define ST7796_MAD_BGR        0x08

#define ST7796_MAD_VERTICAL   0x20
#define ST7796_MAD_HORIZONTAL 0x00
#define ST7796_MAD_X_LEFT     0x00
#define ST7796_MAD_X_RIGHT    0x40
#define ST7796_MAD_Y_UP       0x00
#define ST7796_MAD_Y_DOWN     0x80

#if ST7796_COLORMODE == 0
#define ST7796_MAD_COLORMODE  ST7796_MAD_RGB
#else
#define ST7796_MAD_COLORMODE  ST7796_MAD_BGR
#endif

#if (ST7796_ORIENTATION == 0)
#define ST7796_SIZE_X                     ST7796_LCD_PIXEL_WIDTH
#define ST7796_SIZE_Y                     ST7796_LCD_PIXEL_HEIGHT
#define ST7796_MAD_DATA_RIGHT_THEN_UP     (ST7796_MAD_COLORMODE | ST7796_MAD_X_RIGHT | ST7796_MAD_Y_UP | ST7796_MAD_HORIZONTAL)
#define ST7796_MAD_DATA_RIGHT_THEN_DOWN   (ST7796_MAD_COLORMODE | ST7796_MAD_X_RIGHT | ST7796_MAD_Y_DOWN | ST7796_MAD_HORIZONTAL)
#define XSIZE                              Xsize
#define YSIZE                              Ysize
#elif (ST7796_ORIENTATION == 1)
#define ST7796_SIZE_X                     ST7796_LCD_PIXEL_HEIGHT
#define ST7796_SIZE_Y                     ST7796_LCD_PIXEL_WIDTH
#define ST7796_MAD_DATA_RIGHT_THEN_UP     (ST7796_MAD_COLORMODE | ST7796_MAD_X_RIGHT | ST7796_MAD_Y_DOWN | ST7796_MAD_VERTICAL)
#define ST7796_MAD_DATA_RIGHT_THEN_DOWN   (ST7796_MAD_COLORMODE | ST7796_MAD_X_LEFT  | ST7796_MAD_Y_DOWN | ST7796_MAD_VERTICAL)
#define XSIZE                              Ysize
#define YSIZE                              Xsize
#elif (ST7796_ORIENTATION == 2)
#define ST7796_SIZE_X                     ST7796_LCD_PIXEL_WIDTH
#define ST7796_SIZE_Y                     ST7796_LCD_PIXEL_HEIGHT
#define ST7796_MAD_DATA_RIGHT_THEN_UP     (ST7796_MAD_COLORMODE | ST7796_MAD_X_LEFT  | ST7796_MAD_Y_DOWN | ST7796_MAD_HORIZONTAL)
#define ST7796_MAD_DATA_RIGHT_THEN_DOWN   (ST7796_MAD_COLORMODE | ST7796_MAD_X_LEFT  | ST7796_MAD_Y_UP | ST7796_MAD_HORIZONTAL)
#define XSIZE                              Xsize
#define YSIZE                              Ysize
#elif (ST7796_ORIENTATION == 3)
#define ST7796_SIZE_X                     ST7796_LCD_PIXEL_HEIGHT
#define ST7796_SIZE_Y                     ST7796_LCD_PIXEL_WIDTH
#define ST7796_MAD_DATA_RIGHT_THEN_UP     (ST7796_MAD_COLORMODE | ST7796_MAD_X_LEFT  | ST7796_MAD_Y_UP | ST7796_MAD_VERTICAL)
#define ST7796_MAD_DATA_RIGHT_THEN_DOWN   (ST7796_MAD_COLORMODE | ST7796_MAD_X_RIGHT | ST7796_MAD_Y_UP | ST7796_MAD_VERTICAL)
#define XSIZE                              Ysize
#define YSIZE                              Xsize
#endif
