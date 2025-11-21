/*
 * ST7789 LCD driver v2024.11
 */

#include <Lcd/bmp.h>
#include <Lcd/lcd.h>
#include <Lcd/lcd_io.h>
#include <Lcd/st7789.h>
#include "main.h"

void     st7789_Init(void);
uint32_t st7789_ReadID(void);
void     st7789_DisplayOn(void);
void     st7789_DisplayOff(void);
void     st7789_SetCursor(uint16_t Xpos, uint16_t Ypos);
void     st7789_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code);
uint16_t st7789_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void     st7789_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     st7789_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     st7789_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     st7789_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t RGBCode);
uint16_t st7789_GetLcdPixelWidth(void);
uint16_t st7789_GetLcdPixelHeight(void);
void     st7789_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
void     st7789_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData);
void     st7789_ReadRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData);
void     st7789_Scroll(int16_t Scroll, uint16_t TopFix, uint16_t BottonFix);
void     st7789_UserCommand(uint16_t Command, uint8_t * pData, uint32_t Size, uint8_t Mode);

LCD_DrvTypeDef   st7789_drv =
{
  st7789_Init,
  st7789_ReadID,
  st7789_DisplayOn,
  st7789_DisplayOff,
  st7789_SetCursor,
  st7789_WritePixel,
  st7789_ReadPixel,
  st7789_SetDisplayWindow,
  st7789_DrawHLine,
  st7789_DrawVLine,
  st7789_GetLcdPixelWidth,
  st7789_GetLcdPixelHeight,
  st7789_DrawBitmap,
  st7789_DrawRGBImage,
  st7789_FillRect,
  st7789_ReadRGBImage,
  st7789_Scroll,
  st7789_UserCommand
};

LCD_DrvTypeDef  *lcd_drv = &st7789_drv;

/* transaction data */
#define TRANSDATAMAXSIZE  8
union
{
  char       c[TRANSDATAMAXSIZE];
  uint8_t   d8[TRANSDATAMAXSIZE];
  uint16_t d16[TRANSDATAMAXSIZE / 2];
}transdata;

#define ST7789_NOP            0x00
#define ST7789_SWRESET        0x01

#define ST7789_RDDID          0x04
#define ST7789_RDDST          0x09
#define ST7789_RDMODE         0x0A
#define ST7789_RDMADCTL       0x0B
#define ST7789_RDPIXFMT       0x0C
#define ST7789_RDIMGFMT       0x0D
#define ST7789_RDSELFDIAG     0x0F

#define ST7789_SLPIN          0x10
#define ST7789_SLPOUT         0x11
#define ST7789_PTLON          0x12
#define ST7789_NORON          0x13

#define ST7789_INVOFF         0x20
#define ST7789_INVON          0x21
#define ST7789_GAMMASET       0x26
#define ST7789_DISPOFF        0x28
#define ST7789_DISPON         0x29

#define ST7789_CASET          0x2A
#define ST7789_PASET          0x2B
#define ST7789_RAMWR          0x2C
#define ST7789_RAMRD          0x2E

#define ST7789_PTLAR          0x30
#define ST7789_VSCRDEF        0x33
#define ST7789_MADCTL         0x36
#define ST7789_VSCRSADD       0x37      /* Vertical Scrolling Start Address */
#define ST7789_PIXFMT         0x3A      /* COLMOD: Pixel Format Set */

#define ST7796_IFMODE  		  0xB0     /* RGB Interface Signal Control */
#define ST7789_FRMCTR1        0xB1
#define ST7789_FRMCTR2        0xB2
#define ST7789_FRMCTR3        0xB3
#define ST7789_INVCTR         0xB4
#define ST7796_BPC			  0xB5
#define ST7789_DFUNCTR        0xB6      /* Display Function Control */
#define ST7796_EM			  0xB7

#define ST7789_PWCTR1         0xC0
#define ST7789_PWCTR2         0xC1
#define ST7789_PWCTR3         0xC2
#define ST7789_PWCTR4         0xC3
#define ST7789_PWCTR5         0xC4
#define ST7789_VMCTR1         0xC5
#define ST7789_VMCTR2         0xC7

#define ST7789_RDID1          0xDA
#define ST7789_RDID2          0xDB
#define ST7789_RDID3          0xDC
#define ST7789_RDID4          0xDD

#define ST7789_GMCTRP1        0xE0
#define ST7789_GMCTRN1        0xE1

#define ST7789_PWCTR6         0xFC
#define ST7789_INTERFACE      0xF6    /* Interface control register */
#define ST7796_CSCON		  0xF0

/* Extend register commands */
#define ST7789_POWERA         0xCB    /* Power control A register */
#define ST7789_POWERB         0xCF    /* Power control B register */
#define ST7789_DTCA           0xE8    /* Driver timing control A */
#define ST7789_DTCB           0xEA    /* Driver timing control B */
#define ST7789_POWER_SEQ      0xED    /* Power on sequence register */
#define ST7789_3GAMMA_EN      0xF2    /* 3 Gamma enable register */
#define ST7789_PRC            0xF7    /* Pump ratio control register */

//-----------------------------------------------------------------------------
#define ST7789_MAD_RGB        0x00
#define ST7789_MAD_BGR        0x08

#define ST7789_MAD_VERTICAL   0x20
#define ST7789_MAD_HORIZONTAL 0x00
#define ST7789_MAD_X_LEFT     0x00
#define ST7789_MAD_X_RIGHT    0x40
#define ST7789_MAD_Y_UP       0x00
#define ST7789_MAD_Y_DOWN     0x80

#if ST7789_COLORMODE == 0
#define ST7789_MAD_COLORMODE  ST7789_MAD_RGB
#else
#define ST7789_MAD_COLORMODE  ST7789_MAD_BGR
#endif

#if (ST7789_ORIENTATION == 0)
#define ST7789_SIZE_X                     ST7789_LCD_PIXEL_WIDTH
#define ST7789_SIZE_Y                     ST7789_LCD_PIXEL_HEIGHT
#define ST7789_MAD_DATA_RIGHT_THEN_UP     (ST7789_MAD_COLORMODE | ST7789_MAD_X_RIGHT | ST7789_MAD_Y_UP | ST7789_MAD_HORIZONTAL)
#define ST7789_MAD_DATA_RIGHT_THEN_DOWN   (ST7789_MAD_COLORMODE | ST7789_MAD_X_RIGHT | ST7789_MAD_Y_DOWN | ST7789_MAD_HORIZONTAL)
#define XSIZE                              Xsize
#define YSIZE                              Ysize
#elif (ST7789_ORIENTATION == 1)
#define ST7789_SIZE_X                     ST7789_LCD_PIXEL_HEIGHT
#define ST7789_SIZE_Y                     ST7789_LCD_PIXEL_WIDTH
#define ST7789_MAD_DATA_RIGHT_THEN_UP     (ST7789_MAD_COLORMODE | ST7789_MAD_X_RIGHT | ST7789_MAD_Y_DOWN | ST7789_MAD_VERTICAL)
#define ST7789_MAD_DATA_RIGHT_THEN_DOWN   (ST7789_MAD_COLORMODE | ST7789_MAD_X_LEFT  | ST7789_MAD_Y_DOWN | ST7789_MAD_VERTICAL)
#define XSIZE                              Ysize
#define YSIZE                              Xsize
#elif (ST7789_ORIENTATION == 2)
#define ST7789_SIZE_X                     ST7789_LCD_PIXEL_WIDTH
#define ST7789_SIZE_Y                     ST7789_LCD_PIXEL_HEIGHT
#define ST7789_MAD_DATA_RIGHT_THEN_UP     (ST7789_MAD_COLORMODE | ST7789_MAD_X_LEFT  | ST7789_MAD_Y_DOWN | ST7789_MAD_HORIZONTAL)
#define ST7789_MAD_DATA_RIGHT_THEN_DOWN   (ST7789_MAD_COLORMODE | ST7789_MAD_X_LEFT  | ST7789_MAD_Y_UP | ST7789_MAD_HORIZONTAL)
#define XSIZE                              Xsize
#define YSIZE                              Ysize
#elif (ST7789_ORIENTATION == 3)
#define ST7789_SIZE_X                     ST7789_LCD_PIXEL_HEIGHT
#define ST7789_SIZE_Y                     ST7789_LCD_PIXEL_WIDTH
#define ST7789_MAD_DATA_RIGHT_THEN_UP     (ST7789_MAD_COLORMODE | ST7789_MAD_X_LEFT  | ST7789_MAD_Y_UP | ST7789_MAD_VERTICAL)
#define ST7789_MAD_DATA_RIGHT_THEN_DOWN   (ST7789_MAD_COLORMODE | ST7789_MAD_X_RIGHT | ST7789_MAD_Y_UP | ST7789_MAD_VERTICAL)
#define XSIZE                              Ysize
#define YSIZE                              Xsize
#endif

#define ST7789_SETWINDOW(x1, x2, y1, y2) \
  { transdata.d16[0] = __REVSH(x1); transdata.d16[1] = __REVSH(x2); LCD_IO_WriteCmd8MultipleData8(ST7789_CASET, &transdata, 4); \
    transdata.d16[0] = __REVSH(y1); transdata.d16[1] = __REVSH(y2); LCD_IO_WriteCmd8MultipleData8(ST7789_PASET, &transdata, 4); }

#define ST7789_SETCURSOR(x, y)            ST7789_SETWINDOW(x, x, y, y)

//-----------------------------------------------------------------------------
#define ST7789_LCD_INITIALIZED    0x01
#define ST7789_IO_INITIALIZED     0x02
static  uint8_t   Is_st7789_Initialized = 0;

const uint8_t EntryRightThenUp = ST7789_MAD_DATA_RIGHT_THEN_UP;
const uint8_t EntryRightThenDown = ST7789_MAD_DATA_RIGHT_THEN_DOWN;

/* the last set drawing direction is stored here */
uint8_t LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;

static  uint16_t  yStart, yEnd;

//-----------------------------------------------------------------------------
/* Pixel draw and read functions */

#if ST7789_WRITEBITDEPTH == ST7789_READBITDEPTH
/* 16/16 and 24/24 bit, no need to change bitdepth data */
#define SetWriteDir()
#define SetReadDir()
#else /* #if ST7789_WRITEBITDEPTH == ST7789_READBITDEPTH */
uint8_t lastdir = 0;
#if ST7789_WRITEBITDEPTH == 16
/* 16/24 bit */
#define SetWriteDir() {                                      \
  if(lastdir != 0)                                           \
  {                                                          \
    LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, "\55", 1);  \
    lastdir = 0;                                             \
  }                                                          }
#define SetReadDir() {                                       \
  if(lastdir == 0)                                           \
  {                                                          \
    LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, "\66", 1);  \
    lastdir = 1;                                             \
  }                                                          }
#elif ST7789_WRITEBITDEPTH == 24
/* 24/16 bit */
#define SetWriteDir() {                                      \
  if(lastdir != 0)                                           \
  {                                                          \
    LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, "\66", 1);  \
    lastdir = 0;                                             \
  }                                                          }
#define SetReadDir() {                                       \
  if(lastdir == 0)                                           \
  {                                                          \
    LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, "\55", 1);  \
    lastdir = 1;                                             \
  }                                                          }
#endif /* #elif ILI9488_WRITEBITDEPTH == 24 */
#endif /* #else ILI9488_WRITEBITDEPTH == ILI9488_READBITDEPTH */

#if ST7789_WRITEBITDEPTH == 16
#define  LCD_IO_DrawFill(Color, Size) { \
  SetWriteDir(); \
  LCD_IO_WriteCmd8DataFill16(ST7789_RAMWR, Color, Size); }            /* Fill 16 bit pixel(s) */
#define  LCD_IO_DrawBitmap(pData, Size) { \
  SetWriteDir(); \
  LCD_IO_WriteCmd8MultipleData16(ST7789_RAMWR, pData, Size); }        /* Draw 16 bit bitmap */
#elif ST7789_WRITEBITDEPTH == 24
#define  LCD_IO_DrawFill(Color, Size) { \
  SetWriteDir(); \
  LCD_IO_WriteCmd8DataFill16to24(ST7789_RAMWR, Color, Size); }        /* Fill 24 bit pixel(s) from 16 bit color code */
#define  LCD_IO_DrawBitmap(pData, Size) { \
  SetWriteDir(); \
  LCD_IO_WriteCmd8MultipleData16to24(ST7789_RAMWR, pData, Size); }    /* Draw 24 bit Lcd bitmap from 16 bit bitmap data */
#endif /* #elif ST7789_WRITEBITDEPTH == 24 */

#if ST7789_READBITDEPTH == 16
#define  LCD_IO_ReadBitmap(pData, Size) { \
  SetReadDir(); \
  LCD_IO_ReadCmd8MultipleData16(ST7789_RAMRD, pData, Size, 1); }      /* Read 16 bit LCD */
#elif ST7789_READBITDEPTH == 24
#define  LCD_IO_ReadBitmap(pData, Size) { \
  SetReadDir(); \
  LCD_IO_ReadCmd8MultipleData24to16(ST7789_RAMRD, pData, Size, 1); }  /* Read 24 bit Lcd and convert to 16 bit bitmap */
#endif /* #elif ST7789_READBITDEPTH == 24 */

//-----------------------------------------------------------------------------
/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void st7789_DisplayOn(void)
{
  LCD_IO_Bl_OnOff(1);
  LCD_IO_WriteCmd8MultipleData8(ST7789_SLPOUT, NULL, 0);    /* Exit Sleep */
}

//-----------------------------------------------------------------------------
/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void st7789_DisplayOff(void)
{
  LCD_IO_WriteCmd8MultipleData8(ST7789_SLPIN, NULL, 0);     /* Sleep */
  LCD_IO_Bl_OnOff(0);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the LCD pixel Width.
  * @param  None
  * @retval The Lcd Pixel Width
  */
uint16_t st7789_GetLcdPixelWidth(void)
{
  return ST7789_SIZE_X;
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the LCD pixel Height.
  * @param  None
  * @retval The Lcd Pixel Height
  */
uint16_t st7789_GetLcdPixelHeight(void)
{
  return ST7789_SIZE_Y;
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the ST7789 ID.
  * @param  None
  * @retval The ST7789 ID
  */
uint32_t st7789_ReadID(void)
{
  uint32_t dt = 0;
  LCD_IO_ReadCmd8MultipleData8(0xD3, (uint8_t *)&dt, 3, 1);
  return dt;
}

//-----------------------------------------------------------------------------
void st7789_Init(void)
{
  if((Is_st7789_Initialized & ST7789_LCD_INITIALIZED) == 0)
  {
    Is_st7789_Initialized |= ST7789_LCD_INITIALIZED;
    if((Is_st7789_Initialized & ST7789_IO_INITIALIZED) == 0)
      LCD_IO_Init();
    Is_st7789_Initialized |= ST7789_IO_INITIALIZED;
  }

  LCD_Delay(50);
  LCD_IO_WriteCmd8MultipleData8(ST7789_SWRESET, NULL, 0);
  LCD_Delay(150);

  /* color mode (16 or 24 bit) */
  #if ST7789_WRITEBITDEPTH == 16
  // RGB 16 bit depth
  LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, (uint8_t *)"\x55", 1);
  #elif ST7789_WRITEBITDEPTH == 24
  LCD_IO_WriteCmd8MultipleData8(ST7789_PIXFMT, (uint8_t *)"\x66", 1);
  #endif
  LCD_Delay(50);

  // begin my code

  // write to cscon
  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\xC3\x96", 1);

  // write to madctl
  LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, (uint8_t *)"\x68",1);

  // write interface mode (IFMODE)
  // 0x80 => spi enable
  LCD_IO_WriteCmd8MultipleData8(ST7796_IFMODE, (uint8_t *)"\x80",1);

  // first parameter
  // RM interface for ram: 0 => system interface
  // RCM RGB interface selection: 0 => DE mode
  // Direct memory
  // Normal scan
  // V63
  // second parameter:
  // GS gate output scan direction:
  // SS S1 -> S960
  // SM ISC[3:0]
  LCD_IO_WriteCmd8MultipleData8(ST7789_DFUNCTR, (uint8_t *)"\x00\x02", 2);


  // blanking porch control
  LCD_IO_WriteCmd8MultipleData8(ST7796_BPC, (uint8_t *)"\x02\x03\x00\x04", 1);

  // frame control 1
  // first parameter
  // FRS = 0b100 => frame frequency in full color
  // Diva = 0 => inversion mode is Fosc
  // second parameter
  // RTNA = 0x10 = 16
  LCD_IO_WriteCmd8MultipleData8(ST7789_FRMCTR1, (uint8_t *)"\x80\x10", 2);

  // Display Inversion Control
  // DINV = 00 => Column inversion
  LCD_IO_WriteCmd8MultipleData8(ST7789_INVCTR, (uint8_t *)"\x00", 1);

  // Entry mode set EM
  // epf = 0b11 => set data format r(0) = b(0) = G(0)
  // GON/DTE = 0b11 => normal display
  // DTSB = 0b0 => deep standby off
  LCD_IO_WriteCmd8MultipleData8(ST7796_EM, (uint8_t *)"\xC6", 1);

  // vcom control
  // VCMP = 36 => VCOM 1.2
  LCD_IO_WriteCmd8MultipleData8(ST7789_VMCTR1, (uint8_t *)"\x24",1);


  // might be mistake
  LCD_IO_WriteCmd8MultipleData8(0xE4, (uint8_t *)"\x31",1);

  // Source timing control: 0x09 22.5us
  // gate start = 0x19 => 25Tclk
  // gate end = 0x25 => 37Tclk
  // G_eq = on
  // tclk = 4/osc
  LCD_IO_WriteCmd8MultipleData8(ST7789_DTCA, (uint8_t *)"\x40\x8A\x00\x00\x29\x19\xA5\x33", 8);


  // set current level
  LCD_IO_WriteCmd8MultipleData8(ST7789_PWCTR3, (uint8_t *)"\xA7", 1);

  // positive gamma
  LCD_IO_WriteCmd8MultipleData8(ST7789_GMCTRP1, (uint8_t *)"\xF0\x09\x13\x12\x12\x2B\x3C\x44\x4B\x1B\x18\x17\x1D\x21", 14);

  // negative gamma
  LCD_IO_WriteCmd8MultipleData8(ST7789_GMCTRN1, (uint8_t *)"\xF0\x09\x13\x0C\x0D\x27\x3B\x44\x4D\x0B\x17\x17\x1D\x21", 14);

  LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  LCD_Delay(10);

  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\xC3",1);
  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\x69",1);
  LCD_IO_WriteCmd8MultipleData8(ST7789_NORON, NULL, 0);
  LCD_IO_WriteCmd8MultipleData8(ST7789_SLPOUT, NULL, 0);
  LCD_IO_WriteCmd8MultipleData8(ST7789_DISPON, NULL, 0);


  #if ST7789_INITCLEAR == 1
  st7789_FillRect(0, 0, ST7789_SIZE_X, ST7789_SIZE_Y, 0x0000);
  LCD_Delay(10);
  #endif
  
  LCD_Delay(10);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Set Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void st7789_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  ST7789_SETCURSOR(Xpos, Ypos);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Write pixel.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color
  * @retval None
  */
void st7789_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  ST7789_SETCURSOR(Xpos, Ypos);
  LCD_IO_DrawFill(RGBCode, 1);
}


//-----------------------------------------------------------------------------
/**
  * @brief  Read pixel.
  * @param  None
  * @retval the RGB pixel color
  */
uint16_t st7789_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint16_t ret;
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  ST7789_SETCURSOR(Xpos, Ypos);
  LCD_IO_ReadBitmap(&ret, 1);
  return(ret);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Sets a display window
  * @param  Xpos:   specifies the X bottom left position.
  * @param  Ypos:   specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width:  display window width.
  * @retval None
  */
void st7789_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  yStart = Ypos; yEnd = Ypos + Height - 1;
  ST7789_SETWINDOW(Xpos, Xpos + Width - 1, Ypos, Ypos + Height - 1);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Draw vertical line.
  * @param  RGBCode: Specifies the RGB color
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.
  * @retval None
  */
void st7789_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  ST7789_SETWINDOW(Xpos, Xpos + Length - 1, Ypos, Ypos);
  LCD_IO_DrawFill(RGBCode, Length);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Draw vertical line.
  * @param  RGBCode: Specifies the RGB color
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.
  * @retval None
  */
void st7789_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  ST7789_SETWINDOW(Xpos, Xpos, Ypos, Ypos + Length - 1);
  LCD_IO_DrawFill(RGBCode, Length);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Draw Filled rectangle
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Xsize:    specifies the X size
  * @param  Ysize:    specifies the Y size
  * @param  RGBCode:  specifies the RGB color
  * @retval None
  */
void st7789_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t RGBCode)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  ST7789_SETWINDOW(Xpos, Xpos + Xsize - 1, Ypos, Ypos + Ysize - 1);
  LCD_IO_DrawFill(RGBCode, Xsize * Ysize);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Displays a 16bit bitmap picture..
  * @param  BmpAddress: Bmp picture address.
  * @param  Xpos:  Bmp X position in the LCD
  * @param  Ypos:  Bmp Y position in the LCD
  * @retval None
  * @brief  Draw direction: right then up
  */
void st7789_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index, size;
  /* Read bitmap size */
  size = ((BITMAPSTRUCT *)pbmp)->fileHeader.bfSize;
  /* Get bitmap data address offset */
  index = ((BITMAPSTRUCT *)pbmp)->fileHeader.bfOffBits;
  size = (size - index) / 2;
  pbmp += index;

  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_UP)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_UP;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenUp, 1);
  }
  transdata.d16[0] = __REVSH(ST7789_SIZE_Y - 1 - yEnd);
  transdata.d16[1] = __REVSH(ST7789_SIZE_Y - 1 - yStart);
  LCD_IO_WriteCmd8MultipleData8(ST7789_PASET, &transdata, 4);
  LCD_IO_DrawBitmap(pbmp, size);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Displays 16bit/pixel picture..
  * @param  pdata: picture address.
  * @param  Xpos:  Image X position in the LCD
  * @param  Ypos:  Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  * @brief  Draw direction: right then down
  */
void st7789_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  st7789_SetDisplayWindow(Xpos, Ypos, Xsize, Ysize);
  LCD_IO_DrawBitmap(pData, Xsize * Ysize);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Read 16bit/pixel picture from Lcd and store to RAM
  * @param  pdata: picture address.
  * @param  Xpos:  Image X position in the LCD
  * @param  Ypos:  Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  * @brief  Draw direction: right then down
  */
void st7789_ReadRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData)
{
  if(LastEntry != ST7789_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7789_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7789_MADCTL, &EntryRightThenDown, 1);
  }
  st7789_SetDisplayWindow(Xpos, Ypos, Xsize, Ysize);
  LCD_IO_ReadBitmap(pData, Xsize * Ysize);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Set display scroll parameters
  * @param  Scroll    : Scroll size [pixel]
  * @param  TopFix    : Top fix size [pixel]
  * @param  BottonFix : Botton fix size [pixel]
  * @retval None
  */
void st7789_Scroll(int16_t Scroll, uint16_t TopFix, uint16_t BottonFix)
{
  static uint16_t scrparam[4] = {0, 0, 0, 0};
  #if (ST7789_ORIENTATION == 0)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[3] = __REVSH(TopFix);
    scrparam[1] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7789_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7789_VSCRDEF, &scrparam[1], 6);
  }
  Scroll = (0 - Scroll) % __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7789_ORIENTATION == 1)
  if((TopFix != __REVSH(scrparam[3])) || (BottonFix != __REVSH(scrparam[1])))
  {
    scrparam[3] = __REVSH(TopFix);
    scrparam[1] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7789_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7789_VSCRDEF, &scrparam[1], 6);
  }
  Scroll %= __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7789_ORIENTATION == 2)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[1] = __REVSH(TopFix);
    scrparam[3] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7789_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7789_VSCRDEF, &scrparam[1], 6);
  }
  Scroll %= __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7789_ORIENTATION == 3)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[1] = __REVSH(TopFix);
    scrparam[3] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7789_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7789_VSCRDEF, &scrparam[1], 6);
  }
  Scroll = (0 - Scroll) % __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #endif
  if(Scroll != __REVSH(scrparam[0]))
  {
    scrparam[0] = __REVSH(Scroll);
    LCD_IO_WriteCmd8MultipleData8(ST7789_VSCRSADD, &scrparam[0], 2);
  }
}

//-----------------------------------------------------------------------------
/**
  * @brief  User command
  * @param  Command   : Lcd command
  * @param  pData     : data pointer
  * @param  Size      : data number
  * @param  Mode      : 0=write 8bits datas, 1=0=write 16bits datas, 2=read 8bits datas, 3=read 16bits datas
  * @retval None
  */
void st7789_UserCommand(uint16_t Command, uint8_t* pData, uint32_t Size, uint8_t Mode)
{
  if(Mode == 0)
    LCD_IO_WriteCmd8MultipleData8((uint8_t)Command, pData, Size);
  else if(Mode == 1)
    LCD_IO_WriteCmd8MultipleData16((uint8_t)Command, pData, Size);
  else if(Mode == 2)
    LCD_IO_ReadCmd8MultipleData8((uint8_t)Command, pData, Size, 1);
  else if(Mode == 3)
    LCD_IO_ReadCmd8MultipleData16((uint8_t)Command, pData, Size, 1);
}
