/*
 * ST7796 LCD driver v2024.11
 */

#include <Lcd/bmp.h>
#include <Lcd/lcd.h>
#include <Lcd/lcd_io.h>
#include <Lcd/st7796.h>
#include "main.h"

LCD_DrvTypeDef   st7796_drv =
{
  st7796_Init,
  st7796_ReadID,
  st7796_DisplayOn,
  st7796_DisplayOff,
  st7796_SetCursor,
  st7796_WritePixel,
  st7796_ReadPixel,
  st7796_SetDrawRegion,
  st7796_DrawHLine,
  st7796_DrawVLine,
  st7796_GetLcdPixelWidth,
  st7796_GetLcdPixelHeight,
  st7796_DrawBitmap,
  st7796_DrawRGBImage,
  st7796_FillRect,
  st7796_ReadRGBImage,
  st7796_Scroll,
  st7796_UserCommand
};

LCD_DrvTypeDef  *lcd_drv = &st7796_drv;

/* transaction data */
#define TRANSDATAMAXSIZE  4
union
{
  char       c[TRANSDATAMAXSIZE];
  uint8_t   d8[TRANSDATAMAXSIZE];
  uint16_t d16[TRANSDATAMAXSIZE / 2];
}transdata;

static inline void st7796_WriteAddrWindow(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    transdata.d16[0] = __REVSH(x1);
    transdata.d16[1] = __REVSH(x2);
    LCD_IO_WriteCmd8MultipleData8(ST7796_CASET, &transdata, 4);

    transdata.d16[0] = __REVSH(y1);
    transdata.d16[1] = __REVSH(y2);
    LCD_IO_WriteCmd8MultipleData8(ST7796_PASET, &transdata, 4);
}

//-----------------------------------------------------------------------------
#define ST7796_LCD_INITIALIZED    0x01
#define ST7796_IO_INITIALIZED     0x02
static  uint8_t   Is_st7796_Initialized = 0;

const uint8_t EntryRightThenUp = ST7796_MAD_DATA_RIGHT_THEN_UP;
const uint8_t EntryRightThenDown = ST7796_MAD_DATA_RIGHT_THEN_DOWN;

/* the last set drawing direction is stored here */
uint8_t LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;

static  uint16_t  yStart, yEnd;

//-----------------------------------------------------------------------------
/* Pixel draw and read functions */

#if ST7796_WRITEBITDEPTH != ST7796_READBITDEPTH
// Static variables are only accessible within this source file
static uint8_t lastdir = 0;
#endif

/**
 * @brief Sets how many bits are written by the MCU to the st7796
 * @retval None
 */
static inline void LCD_IO_SetWriteDir(void)
{
	#if ST7796_WRITEBITDEPTH != ST7796_READBITDEPTH
		#if ST7796_WRITEDEPTH == 16
			/* 16-bit write */
			if (lastdir != 0) {
				LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, "\55", 1);
				lastdir = 0;
			}
		#elif ST7796_WRITEDEPTH == 24
			/* 24-bit write */
			if (lastdir != 0) {
				LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, "\66", 1);
				lastdir = 0;
			}
		#endif
	#endif
}

/**
 * @brief Sets how many bits are read from the st7796
 * @retval None
 */
static inline void LCD_IO_SetReadDir(void) {
#if ST7796_WRITEBITDEPTH != ST7796_READBITDEPTH
	#if ST7796_WRITEDEPTH == 16
		/* 16-bit write */
		if (lastdir != 0) {
			LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, "\55", 1);
			lastdir = 0;
		}
	#elif ST7796_WRITEDEPTH == 24
		/* 24-bit write */
		if (lastdir != 0) {
			LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, "\66", 1);
			lastdir = 0;
		}
	#endif
#endif
}


#if ST7796_WRITEBITDEPTH == 16

static inline void LCD_IO_DrawFill(uint8_t Color, uint32_t Size) {
	LCD_IO_SetWriteDir();
	LCD_IO_WriteCmd8DataFill16(ST7796_RAMWR, Color, Size);
}

static inline void LCD_IO_DrawBitmap(const uint8_t *pData, uint32_t Size) {
	LCD_IO_SetWriteDir();
	LCD_IO_WriteCmd8MultipleData16(ST7796_RAMWR, pData, Size);
}

#elif ST7796_WRITEBITDEPTH == 24

static inline void LCD_IO_DrawFill(uint8_t Color, uint32_t Size) {
	LCD_IO_SetWriteDir();
	LCD_IO_WriteCmd8DataFill16to24(ST7796_RAMWR, Color, Size);
}

static inline void LCD_IO_DrawBitmap(const uint8_t *pData, uint32_t Size) {
	LCD_IO_SetWriteDir();
	LCD_IO_WriteCmd8MultipleData16to24(ST7796_RAMWR, pData, Size);
}
#endif

#if ST7796_READBITDEPTH == 16
static inline void LCD_IO_ReadBitmap(uint16_t *pData, uint32_t size)
{
	LCD_IO_SetReadDir();
	LCD_IO_ReadCmd8MultipleData16(ST7796_RAMRD, pData, size, 1);
}

#elif ST7796_READBITDEPTH == 24
static inline void LCD_IO_ReadBitmap(uint16_t *pData, uint32_t size)
{
	LCD_IO_SetReadDir();
	LCD_IO_ReadCmd8MultipleData24to16(ST7796_RAMRD, pData, size, 1);
}
#endif
//-----------------------------------------------------------------------------
/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void st7796_DisplayOn(void)
{
  LCD_IO_Bl_OnOff(1);
  LCD_IO_WriteCmd8MultipleData8(ST7796_SLPOUT, NULL, 0);    /* Exit Sleep */
}

//-----------------------------------------------------------------------------
/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void st7796_DisplayOff(void)
{
  LCD_IO_WriteCmd8MultipleData8(ST7796_SLPIN, NULL, 0);     /* Sleep */
  LCD_IO_Bl_OnOff(0);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the LCD pixel Width.
  * @param  None
  * @retval The Lcd Pixel Width
  */
uint16_t st7796_GetLcdPixelWidth(void)
{
  return ST7796_SIZE_X;
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the LCD pixel Height.
  * @param  None
  * @retval The Lcd Pixel Height
  */
uint16_t st7796_GetLcdPixelHeight(void)
{
  return ST7796_SIZE_Y;
}

//-----------------------------------------------------------------------------
/**
  * @brief  Get the ST7796 ID.
  * @param  None
  * @retval The ST7796 ID
  */
uint32_t st7796_ReadID(void)
{
  uint32_t dt = 0;
  LCD_IO_ReadCmd8MultipleData8(0xD3, (uint8_t *)&dt, 3, 1);
  return dt;
}

//-----------------------------------------------------------------------------
void st7796_Init(void)
{
  if((Is_st7796_Initialized & ST7796_LCD_INITIALIZED) == 0)
  {
    Is_st7796_Initialized |= ST7796_LCD_INITIALIZED;
    if((Is_st7796_Initialized & ST7796_IO_INITIALIZED) == 0)
      LCD_IO_Init();
    Is_st7796_Initialized |= ST7796_IO_INITIALIZED;
  }

  LCD_Delay(50);
  LCD_IO_WriteCmd8MultipleData8(ST7796_SWRESET, NULL, 0);
  LCD_Delay(150);

  /* color mode (16 or 24 bit) */
  #if ST7796_WRITEBITDEPTH == 16
  // RGB 16 bit depth
  LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, (uint8_t *)"\x55", 1);
  #elif ST7796_WRITEBITDEPTH == 24
  LCD_IO_WriteCmd8MultipleData8(ST7796_PIXFMT, (uint8_t *)"\x66", 1);
  #endif
  LCD_Delay(50);

  // write to cscon
  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\xC3\x96", 1);

  // write to madctl
//  LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, (uint8_t *)"\x68",1);

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
  LCD_IO_WriteCmd8MultipleData8(ST7796_DFUNCTR, (uint8_t *)"\x00\x02", 2);


  // blanking porch control
  LCD_IO_WriteCmd8MultipleData8(ST7796_BPC, (uint8_t *)"\x02\x03\x00\x04", 1);

  // frame control 1
  // first parameter
  // FRS = 0b100 => frame frequency in full color
  // Diva = 0 => inversion mode is Fosc
  // second parameter
  // RTNA = 0x10 = 16
  LCD_IO_WriteCmd8MultipleData8(ST7796_FRMCTR1, (uint8_t *)"\x80\x10", 2);

  // Display Inversion Control
  // DINV = 00 => Column inversion
  LCD_IO_WriteCmd8MultipleData8(ST7796_INVCTR, (uint8_t *)"\x00", 1);

  // Entry mode set EM
  // epf = 0b11 => set data format r(0) = b(0) = G(0)
  // GON/DTE = 0b11 => normal display
  // DTSB = 0b0 => deep standby off
  LCD_IO_WriteCmd8MultipleData8(ST7796_EM, (uint8_t *)"\xC6", 1);

  // vcom control
  // VCMP = 36 => VCOM 1.2
  LCD_IO_WriteCmd8MultipleData8(ST7796_VMCTR1, (uint8_t *)"\x24",1);


  // might be mistake
  LCD_IO_WriteCmd8MultipleData8(0xE4, (uint8_t *)"\x31",1);

  // Source timing control: 0x09 22.5us
  // gate start = 0x19 => 25Tclk
  // gate end = 0x25 => 37Tclk
  // G_eq = on
  // tclk = 4/osc
  LCD_IO_WriteCmd8MultipleData8(ST7796_DTCA, (uint8_t *)"\x40\x8A\x00\x00\x29\x19\xA5\x33", 8);

  // set current level
  LCD_IO_WriteCmd8MultipleData8(ST7796_PWCTR3, (uint8_t *)"\xA7", 1);

  // positive gamma
  LCD_IO_WriteCmd8MultipleData8(ST7796_GMCTRP1, (uint8_t *)"\xF0\x09\x13\x12\x12\x2B\x3C\x44\x4B\x1B\x18\x17\x1D\x21", 14);

  // negative gamma
  LCD_IO_WriteCmd8MultipleData8(ST7796_GMCTRN1, (uint8_t *)"\xF0\x09\x13\x0C\x0D\x27\x3B\x44\x4D\x0B\x17\x17\x1D\x21", 14);

  LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  LCD_Delay(10);

  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\xC3",1);
  LCD_IO_WriteCmd8MultipleData8(ST7796_CSCON, (uint8_t *)"\x69",1);
  LCD_IO_WriteCmd8MultipleData8(ST7796_NORON, NULL, 0);
  LCD_IO_WriteCmd8MultipleData8(ST7796_SLPOUT, NULL, 0);
  LCD_IO_WriteCmd8MultipleData8(ST7796_DISPON, NULL, 0);


  #if ST7796_INITCLEAR == 1
  st7796_FillRect(0, 0, ST7796_SIZE_X, ST7796_SIZE_Y, 0x0000);
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
void st7796_SetCursor(uint16_t x, uint16_t y) {
	st7796_WriteAddrWindow(x, x, y, y);
}

//-----------------------------------------------------------------------------
/**
  * @brief  Write pixel.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color
  * @retval None
  */
void st7796_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_SetCursor(Xpos, Ypos);
  LCD_IO_DrawFill(RGBCode, 1);
}


//-----------------------------------------------------------------------------
/**
  * @brief  Read pixel.
  * @param  None
  * @retval the RGB pixel color
  */
uint16_t st7796_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint16_t ret;
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_SetCursor(Xpos, Ypos);
  LCD_IO_ReadBitmap(&ret, 1);
  return(ret);
}

//-----------------------------------------------------------------------------
/**
 * @brief Define the active drawing region on the display.
 * @param x: Left coordinate of the region.
 * @param y: Top coordinate of the region.
 * @param width: Width of the region in pixels.
 * @param height: Height of the region in pixels.
 */
void st7796_SetDrawRegion(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  yStart = Ypos; yEnd = Ypos + Height - 1;
  st7796_WriteAddrWindow(Xpos, Xpos + Width - 1, Ypos, Ypos + Height - 1);
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
void st7796_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }

  st7796_WriteAddrWindow(Xpos, Xpos + Length - 1, Ypos, Ypos);
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
void st7796_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_WriteAddrWindow(Xpos, Xpos, Ypos, Ypos + Length - 1);
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
void st7796_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t RGBCode)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_WriteAddrWindow(Xpos, Xpos + Xsize - 1, Ypos, Ypos + Ysize - 1);
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
void st7796_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index, size;
  /* Read bitmap size */
  size = ((BITMAPSTRUCT *)pbmp)->fileHeader.bfSize;
  /* Get bitmap data address offset */
  index = ((BITMAPSTRUCT *)pbmp)->fileHeader.bfOffBits;
  size = (size - index) / 2;
  pbmp += index;

  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_UP)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_UP;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenUp, 1);
  }
  transdata.d16[0] = __REVSH(ST7796_SIZE_Y - 1 - yEnd);
  transdata.d16[1] = __REVSH(ST7796_SIZE_Y - 1 - yStart);
  LCD_IO_WriteCmd8MultipleData8(ST7796_PASET, &transdata, 4);
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
void st7796_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_SetDrawRegion(Xpos, Ypos, Xsize, Ysize);
  LCD_IO_DrawBitmap((uint8_t *) pData, Xsize * Ysize);
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
void st7796_ReadRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData)
{
  if(LastEntry != ST7796_MAD_DATA_RIGHT_THEN_DOWN)
  {
    LastEntry = ST7796_MAD_DATA_RIGHT_THEN_DOWN;
    LCD_IO_WriteCmd8MultipleData8(ST7796_MADCTL, &EntryRightThenDown, 1);
  }
  st7796_SetDrawRegion(Xpos, Ypos, Xsize, Ysize);
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
void st7796_Scroll(int16_t Scroll, uint16_t TopFix, uint16_t BottonFix)
{
  static uint16_t scrparam[4] = {0, 0, 0, 0};
  #if (ST7796_ORIENTATION == 0)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[3] = __REVSH(TopFix);
    scrparam[1] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7796_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7796_VSCRDEF, &scrparam[1], 6);
  }
  Scroll = (0 - Scroll) % __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7796_ORIENTATION == 1)
  if((TopFix != __REVSH(scrparam[3])) || (BottonFix != __REVSH(scrparam[1])))
  {
    scrparam[3] = __REVSH(TopFix);
    scrparam[1] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7796_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7796_VSCRDEF, &scrparam[1], 6);
  }
  Scroll %= __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7796_ORIENTATION == 2)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[1] = __REVSH(TopFix);
    scrparam[3] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7796_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7796_VSCRDEF, &scrparam[1], 6);
  }
  Scroll %= __REVSH(scrparam[2]);
  if(Scroll < 0)
    Scroll = __REVSH(scrparam[2]) + Scroll + __REVSH(scrparam[1]);
  else
    Scroll = Scroll + __REVSH(scrparam[1]);
  #elif (ST7796_ORIENTATION == 3)
  if((TopFix != __REVSH(scrparam[1])) || (BottonFix != __REVSH(scrparam[3])))
  {
    scrparam[1] = __REVSH(TopFix);
    scrparam[3] = __REVSH(BottonFix);
    scrparam[2] = __REVSH(ST7796_LCD_PIXEL_HEIGHT - TopFix - BottonFix);
    LCD_IO_WriteCmd8MultipleData8(ST7796_VSCRDEF, &scrparam[1], 6);
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
    LCD_IO_WriteCmd8MultipleData8(ST7796_VSCRSADD, &scrparam[0], 2);
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
void st7796_UserCommand(uint16_t Command, uint8_t* pData, uint32_t Size, uint8_t Mode)
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
