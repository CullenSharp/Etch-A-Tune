
/*---------------Exported Settings-------------------*/
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
