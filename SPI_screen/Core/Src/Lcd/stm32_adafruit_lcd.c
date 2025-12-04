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
 * - Add : BSP_LCD_FillTriangle (faster algorithm)
 * - Modify : BSP_LCD_Init (default font from header file, default colors from header file, otptional clear from header file)
 * - Modify : ReadID return type: uint16_t to uint32_t
 * - Modify : DrawChar function character size limit removed, and smaller bitmap array is sufficient for operation
 * - Add : BSP_LCD_DisplayMultilayerChar function (mainly for drawing icons and buttons)
 * - Add : BSP_LCD_DisplayStringOnMultilayerChar function (mainly for drawing texticons and textbuttons)
 * */

/**
  ******************************************************************************
  * @file    stm32_adafruit_lcd.c
  * @author  MCD Application Team
  * @brief   This file includes the driver for Liquid Crystal Display (LCD) module
  *          mounted on the Adafruit 1.8" TFT LCD shield (reference ID 802),
  *          that is used with the STM32 Nucleo board through SPI interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - The LCD st7735 component driver MUST be included with this driver.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the LCD using the BSP_LCD_Init() function.

  + Display on LCD
     o Clear the whole LCD using the BSP_LCD_Clear() function or only one specified
       string line using the BSP_LCD_ClearStringLine() function.
     o Display a character on the specified line and column using the BSP_LCD_DisplayChar()
       function or a complete string line using the BSP_LCD_DisplayStringAtLine() function.
     o Display a string line on the specified position (x,y in pixel) and align mode
       using the BSP_LCD_DisplayStringAtLine() function.
     o Draw and fill a basic shapes (dot, line, rectangle, circle, ellipse, ..)
       on LCD using a set of functions.

------------------------------------------------------------------------------*/

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
#include "Lcd/lcd.h"
#include "Lcd/stm32_adafruit_lcd.h"
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
  * @brief  Draws a full ellipse.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  XRadius: Ellipse X radius
  * @param  YRadius: Ellipse Y radius
  * @retval None
  */
void BSP_LCD_FillEllipse(uint16_t Xpos, uint16_t Ypos, uint16_t XRadius, uint16_t YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float K = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  K = (float)(rad2/rad1);

  do
  {
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1));
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1));

    e2 = err;
    if (e2 <= x)
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
