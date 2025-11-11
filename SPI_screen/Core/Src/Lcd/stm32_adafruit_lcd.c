/*
 * Modify: Roberto Benjami
 * date: 2023.02
 *
 * - BSP_LCD_DrawCircle : delete the BSP_LCD_SetFont(&LCD_DEFAULT_FONT); (interesting bug)
 * - BSP_LCD_Init : DrawProp.pFont = &Font24 change to DrawProp.pFont = &LCD_DEFAULT_FONT
 * - FillTriangle -> BSP_LCD_FillTriangle, change to public, changed to fast algorythm
 * - Add : BSP_LCD_ReadID
 * - Add : BSP_LCD_ReadPixel
 * - Add : BSP_LCD_DrawRGB16Image
 * - Add : BSP_LCD_ReadRGB16Image
 * - Modify : BSP_LCD_Init (default font from header file, default colors from header file, optional clear from header file)
 * - Modify : ReadID return type: uint16_t to uint32_t
 * - Modify : DrawChar function character size limit removed, and smaller bitmap array is sufficient for operation
 */

/* Dependencies
- fonts.h
- font24.c
- font20.c
- font16.c
- font12.c
- font8.c"
EndDependencies */
    
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <Lcd/lcd.h>
#include <Lcd/stm32_adafruit_lcd.h>
#include "Fonts/fonts.h"

/* @defgroup STM32_ADAFRUIT_LCD_Private_Defines */
#define POLY_X(Z)             ((int32_t)((Points + (Z))->X))
#define POLY_Y(Z)             ((int32_t)((Points + (Z))->Y))

/* Bitmap header size */
#define OFFSET_BITMAP         54

/* @defgroup STM32_ADAFRUIT_LCD_Private_Macros */
#define ABS(X) ((X) > 0 ? (X) : -(X))
#define SWAP16(a, b) {uint16_t t = a; a = b; b = t;}

/* @defgroup STM32_ADAFRUIT_LCD_Private_Variables */ 
LCD_DrawPropTypeDef DrawProp;

extern LCD_DrvTypeDef  *lcd_drv;

/* Font bitmap buffer */
static uint16_t fontbitmapbuf[FONTBITMAPBUFSIZE] = {0};

/* @defgroup STM32_ADAFRUIT_LCD_Private_FunctionPrototypes */ 
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
  
/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval LCD state
  */
uint8_t BSP_LCD_Init(void)
{ 
  uint8_t ret = LCD_ERROR;

  /* LCD Init */   
  lcd_drv->Init();
  
  /* Default value for draw propriety */
  DrawProp.BackColor = LCD_DEFAULT_BACKCOLOR;
  DrawProp.TextColor = LCD_DEFAULT_TEXTCOLOR;
  DrawProp.pFont     = &LCD_DEFAULT_FONT;
  
  /* Clear the LCD screen */
  #if LCD_INIT_CLEAR == 1
  BSP_LCD_Clear(LCD_DEFAULT_BACKCOLOR);
  #endif
  
  ret = LCD_OK;
  
  return ret;
}

/**
  * @brief  Gets the LCD X size.
  * @param  None    
  * @retval Used LCD X size
  */
uint16_t BSP_LCD_GetXSize(void)
{
  return(lcd_drv->GetLcdPixelWidth());
}

/**
  * @brief  Gets the LCD Y size.
  * @param  None   
  * @retval Used LCD Y size
  */
uint16_t BSP_LCD_GetYSize(void)
{
  return(lcd_drv->GetLcdPixelHeight());
}

/**
  * @brief  Gets the LCD text color.
  * @param  None 
  * @retval Used text color.
  */
uint16_t BSP_LCD_GetTextColor(void)
{
  return DrawProp.TextColor;
}

/**
  * @brief  Gets the LCD background color.
  * @param  None
  * @retval Used background color
  */
uint16_t BSP_LCD_GetBackColor(void)
{
  return DrawProp.BackColor;
}

/**
  * @brief  Sets the LCD text color.
  * @param  Color: Text color code RGB(5-6-5)
  * @retval None
  */
void BSP_LCD_SetTextColor(uint16_t Color)
{
  DrawProp.TextColor = Color;
}

/**
  * @brief  Sets the LCD background color.
  * @param  Color: Background color code RGB(5-6-5)
  * @retval None
  */
void BSP_LCD_SetBackColor(uint16_t Color)
{
  DrawProp.BackColor = Color;
}

/**
  * @brief  Sets the LCD text font.
  * @param  fonts: Font to be used
  * @retval None
  */
void BSP_LCD_SetFont(sFONT *pFonts)
{
  DrawProp.pFont = pFonts;
}

/**
  * @brief  Gets the LCD text font.
  * @param  None
  * @retval Used font
  */
sFONT *BSP_LCD_GetFont(void)
{
  return DrawProp.pFont;
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: Color of the background
  * @retval None
  */
void BSP_LCD_Clear(uint16_t Color)
{
  lcd_drv->FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), Color);
}

/**
  * @brief  Clears the selected line.
  * @param  Line: Line to be cleared
  *          This parameter can be one of the following values:
  *            @arg  0..9: if the Current fonts is Font16x24
  *            @arg  0..19: if the Current fonts is Font12x12 or Font8x12
  *            @arg  0..29: if the Current fonts is Font8x8
  * @retval None
  */
void BSP_LCD_ClearStringLine(uint16_t Line)
{
  uint32_t color_backup = DrawProp.TextColor;
  DrawProp.TextColor = DrawProp.BackColor;;

  /* Draw a rectangle with background color */
  BSP_LCD_FillRect(0, (Line * DrawProp.pFont->Height), BSP_LCD_GetXSize(), DrawProp.pFont->Height);

  DrawProp.TextColor = color_backup;
  BSP_LCD_SetTextColor(DrawProp.TextColor);
}

/**
  * @brief  Displays one character.
  * @param  Xpos: Start column address
  * @param  Ypos: Line where to display the character shape.
  * @param  Ascii: Character ascii code
  *   This parameter must be a number between Min_Data = 0x20 and Max_Data = 0xFF (or while the font last)
  * @retval None
  */
void BSP_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  DrawChar(Xpos, Ypos, &DrawProp.pFont->table[(Ascii-' ') *\
                        DrawProp.pFont->Height * ((DrawProp.pFont->Width + 7) / 8)]);
}

/**
  * @brief  Displays characters on the LCD.
  * @param  Xpos: X position (in pixel)
  * @param  Ypos: Y position (in pixel)   
  * @param  Text: Pointer to string to display on LCD
  * @param  Mode: Display mode
  *          This parameter can be one of the following values:
  *            @arg  CENTER_MODE
  *            @arg  RIGHT_MODE
  *            @arg  LEFT_MODE   
  * @retval None
  */
void BSP_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Line_ModeTypdef Mode)
{
  uint16_t refcolumn = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* Get the text size */
  while (*ptr++) size ++ ;
  
  /* Characters number per line */
  xsize = (BSP_LCD_GetXSize()/DrawProp.pFont->Width);
  
  switch (Mode)
  {
  case CENTER_MODE:
    {
      refcolumn = Xpos + ((xsize - size)* DrawProp.pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      refcolumn = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      refcolumn =  - Xpos + ((xsize - size)*DrawProp.pFont->Width);
      break;
    }    
  default:
    {
      refcolumn = Xpos;
      break;
    }
  }
  
  /* Send the string character by character on lCD */
  while ((*Text != 0) & (((BSP_LCD_GetXSize() - (i*DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width))
  {
    /* Display one character on LCD */
    BSP_LCD_DisplayChar(refcolumn, Ypos, *Text);
    /* Decrement the column position by 16 */
    refcolumn += DrawProp.pFont->Width;
    /* Point on the next character */
    Text++;
    i++;
  }
}

/**
  * @brief  Displays a character on the LCD.
  * @param  Line: Line where to display the character shape
  *          This parameter can be one of the following values:
  *            @arg  0..19: if the Current fonts is Font8
  *            @arg  0..12: if the Current fonts is Font12
  *            @arg  0...9: if the Current fonts is Font16
  *            @arg  0...7: if the Current fonts is Font20
  *            @arg  0...5: if the Current fonts is Font24
  * @param  ptr: Pointer to string to display on LCD
  * @retval None
  */
void BSP_LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr)
{
  BSP_LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}

/**
  * @brief  Draws a pixel on LCD.
  * @param  Xpos: X position 
  * @param  Ypos: Y position
  * @param  RGB_Code: Pixel color in RGB mode (5-6-5)  
  * @retval None
  */
void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code)
{
  lcd_drv->WritePixel(Xpos, Ypos, RGB_Code);
}
  
/**
  * @brief  Draws an horizontal line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  * @retval None
  */
void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  lcd_drv->DrawHLine(DrawProp.TextColor, Xpos, Ypos, Length);
}

/**
  * @brief  Draws a vertical line.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  * @retval None
  */
void BSP_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  lcd_drv->DrawVLine(DrawProp.TextColor, Xpos, Ypos, Length);
}

/**
  * @brief  Draws an uni-line (between two points).
  * @param  x1: Point 1 X position
  * @param  y1: Point 1 Y position
  * @param  x2: Point 2 X position
  * @param  y2: Point 2 Y position
  * @retval None
  */
void BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
  curpixel = 0;

  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */

  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    BSP_LCD_DrawPixel(x, y, DrawProp.TextColor);  /* Draw the current pixel */
    num += numadd;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

/**
  * @brief  Draws a bitmap picture loaded in the STM32 MCU internal memory.
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD
  * @param  pBmp: Pointer to Bmp picture address
  * @retval None
  */
void BSP_LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pBmp)
{
  uint32_t height = 0;
  uint32_t width  = 0;
  
  /* Read bitmap width */
  width = pBmp[18] + (pBmp[19] << 8) + (pBmp[20] << 16)  + (pBmp[21] << 24);

  /* Read bitmap height */
  height = pBmp[22] + (pBmp[23] << 8) + (pBmp[24] << 16)  + (pBmp[25] << 24);
  
  SetDisplayWindow(Xpos, Ypos, width, height);
  
  lcd_drv->DrawBitmap(Xpos, Ypos, pBmp);
}

/**
  * @brief  Draws a full rectangle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Width: Rectangle width  
  * @param  Height: Rectangle height
  * @retval None
  */
void BSP_LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  lcd_drv->FillRect(Xpos, Ypos, Width, Height, DrawProp.TextColor);
}

/**
  * @brief  Enables the display.
  * @param  None
  * @retval None
  */
void BSP_LCD_DisplayOn(void)
{
  lcd_drv->DisplayOn();
}

/**
  * @brief  Disables the display.
  * @param  None
  * @retval None
  */
void BSP_LCD_DisplayOff(void)
{
  lcd_drv->DisplayOff();
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: Line where to display the character shape
  * @param  Ypos: Start column address
  * @param  pChar: Pointer to the character data
  * @retval None
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *pChar)
{
  uint32_t x, y, ay = 0;
  uint16_t *pb;                         /* bitmap pointer */
  uint32_t bmsy;                        /* bitmap buffer y size */
  uint8_t  c;                           /* character set actual byte */
  uint8_t  cbm;                         /* character set bitmap mask */

  if((FONTBITMAPBUFSIZE) < DrawProp.pFont->Width)
    return;
  bmsy = (FONTBITMAPBUFSIZE) / DrawProp.pFont->Width;

  cbm = 0x80;
  pb = fontbitmapbuf;
  c = *pChar;

  for(y = 0; y < DrawProp.pFont->Height; y++)
  {
    for(x = 0; x < DrawProp.pFont->Width; x++)
    {
      if(!cbm)
      { /* byte step */
        cbm = 0x80;
        pChar++;
        c = *pChar;
      }

      if(c & cbm)
        *pb = DrawProp.TextColor;
      else
        *pb = DrawProp.BackColor;
      pb++;
      cbm >>= 1;
    }
    cbm = 0;
    ay++;
    if(ay >= bmsy - 1)
    {
      BSP_LCD_DrawRGB16Image(Xpos, Ypos + y + 1 - ay, DrawProp.pFont->Width, ay, fontbitmapbuf);
      ay = 0;
      pb = fontbitmapbuf;
    }
  }
  if(ay)
  {
    BSP_LCD_DrawRGB16Image(Xpos, Ypos + y - ay, DrawProp.pFont->Width, ay, fontbitmapbuf);
  }
}

/**
  * @brief  Sets display window.
  * @param  LayerIndex: layer index
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height  
  * @retval None
  */
static void SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  lcd_drv->SetDisplayWindow(Xpos, Ypos, Width, Height);
}

/**
  * @brief  Get display ID
  * @param  none
  * @retval ID number
  */
uint32_t BSP_LCD_ReadID(void)
{
  return lcd_drv->ReadID();
}

/**
  * @brief  Get pixel
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @retval RGB565 pixel color
  */
uint16_t BSP_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  return lcd_drv->ReadPixel(Xpos, Ypos);
}

/**
  * @brief  Draw RGB565 image (draw direction: right then down)
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: image width
  * @param  Height: image height
  * @param  *pData: image data pointer
  * @retval None
  */
void BSP_LCD_DrawRGB16Image(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint16_t *pData)
{
  lcd_drv->DrawRGBImage(Xpos, Ypos, Xsize, Ysize, pData);
}

/**
  * @brief  User direct Lcd write and read
  * @param  Cmd       : Lcd command
  * @param  ptr       : data pointer
  * @param  Size      : data number
  * @retval None
  */
void BSP_LCD_DataWrite8(uint16_t Cmd, uint8_t *ptr, uint32_t Size)
{
  lcd_drv->UserCommand(Cmd, ptr, Size, 0);
}
void BSP_LCD_DataWrite16(uint16_t Cmd, uint16_t *ptr, uint32_t Size)
{
  lcd_drv->UserCommand(Cmd, (uint8_t *)ptr, Size, 1);
}
void BSP_LCD_DataRead8(uint16_t Cmd, uint8_t *ptr, uint32_t Size)
{
  lcd_drv->UserCommand(Cmd, ptr, Size, 2);
}
void BSP_LCD_DataRead16(uint16_t Cmd, uint16_t *ptr, uint32_t Size)
{
  lcd_drv->UserCommand(Cmd, (uint8_t *)ptr, Size, 2);
}

