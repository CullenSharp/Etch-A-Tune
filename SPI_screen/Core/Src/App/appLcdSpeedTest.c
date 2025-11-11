/* LCD speed test program
 * The results displayed in printf
 *
 * author: Roberto Benjami
 * version:  2022.12
 */

//-----------------------------------------------------------------------------
#include <Lcd/bmp.h>
#include <Lcd/lcd.h>
#include <Lcd/stm32_adafruit_lcd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "App/appLcdSpeedTest.h"

/* BSP_LCD_... */

/* Test photo */
#if BITMAP_TEST == 1
	#define rombitmap             hurricane_480x320_16
	#define ROMBITMAP_WIDTH       320
	#define ROMBITMAP_HEIGHT      480
#endif

#define GPIO_Port_(a)         a ## _GPIO_Port
#define GPIO_Port(a)          GPIO_Port_(a)
#define Pin_(a)               a ## _Pin
#define Pin(a)                Pin_(a)

#ifdef  __CC_ARM
	#define random()              rand()
#endif

//-----------------------------------------------------------------------------
#if BITMAP_TEST == 1
	extern const BITMAPSTRUCT rombitmap;
	#if READ_TEST == 1
		uint16_t bitmap[ROMBITMAP_WIDTH * ROMBITMAP_HEIGHT];
	#endif
#endif

//-----------------------------------------------------------------------------
uint32_t ClearTest(uint32_t n)
{
  uint32_t ctStartT = HAL_GetTick();
  for(uint32_t i = 0; i < n; i++)
    BSP_LCD_Clear(LCD_COLOR_BLACK);
  return(HAL_GetTick() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t PixelTest(uint32_t n)
{
  uint16_t x, y;

  uint32_t ctStartT = HAL_GetTick();
  for(uint32_t i = 0; i < n; i++)
  {
    x = random() % BSP_LCD_GetXSize();
    y = random() % BSP_LCD_GetYSize();
    BSP_LCD_DrawPixel(x, y, LCD_COLOR16(random() & 0xFFFF));
  }
  return(HAL_GetTick() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t FillRectTest(uint32_t n)
{
  uint16_t x, y, w, h;

  uint32_t ctStartT = HAL_GetTick();
  for(uint32_t i = 0; i < n; i++)
  {
    w = random() % (BSP_LCD_GetXSize() >> 1);
    h = random() % (BSP_LCD_GetYSize() >> 1);
    x = random() % (BSP_LCD_GetXSize() - w);
    y = random() % (BSP_LCD_GetYSize() - h);
    BSP_LCD_SetTextColor(LCD_COLOR16(random() & 0xFFFF));
    BSP_LCD_FillRect(x, y, w, h);
  }
  return(HAL_GetTick() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t CharTest(uint32_t n)
{
  uint16_t x, y;
  uint8_t  c;
  sFONT * fp;

  uint32_t ctStartT = HAL_GetTick();
  for(uint32_t i = 0; i < n; i++)
  {
    c = random() % 5;
    if(c == 0)
      BSP_LCD_SetFont(&Font8);
    else if(c == 1)
      BSP_LCD_SetFont(&Font12);
    else if(c == 2)
      BSP_LCD_SetFont(&Font16);
    else if(c == 3)
      BSP_LCD_SetFont(&Font20);
    else if(c == 4)
      BSP_LCD_SetFont(&Font24);
    fp = BSP_LCD_GetFont();

    x = random() % (BSP_LCD_GetXSize() - fp->Width);
    y = random() % (BSP_LCD_GetYSize() - fp->Height);
    BSP_LCD_SetTextColor(LCD_COLOR16(random() & 0xFFFF));
    BSP_LCD_SetBackColor(LCD_COLOR16(random() & 0xFFFF));

    c = random() % 96 + ' ';
    BSP_LCD_DisplayChar(x, y, c);
  }
  return(HAL_GetTick() - ctStartT);
}

//-----------------------------------------------------------------------------
#if BITMAP_TEST == 1
uint32_t BitmapTest(uint32_t n)
{
  extern const BITMAPSTRUCT hurricane_480x320_16;
  uint16_t x = 0;
  uint16_t y = 0;

  BSP_LCD_DrawBitmap(x, y, (uint8_t *)&rombitmap);
  return(0);
}

//-----------------------------------------------------------------------------
#if READ_TEST == 1
uint32_t ReadPixelTest(uint32_t n)
{
  uint16_t x, y, x0, y0, xsize, ysize;
  uint32_t error = 0;

  x0 = 20;
  y0 =  5;
  xsize = rombitmap.infoHeader.biWidth;
  ysize = rombitmap.infoHeader.biHeight;

  /* Draw bitmap */
  BSP_LCD_DrawBitmap(x0, y0, (uint8_t *)&rombitmap);

  /* Read bitmap (BSP_LCD_ReadPixel) */
  uint32_t ctStartT = HAL_GetTick();
  while(n--)
    for(y = 0; y < ysize; y++)
      for(x = 0; x < xsize; x++)
        bitmap[y * xsize + x] = BSP_LCD_ReadPixel(x0 + x, y0 + y);
  ctStartT = HAL_GetTick() - ctStartT;

  /* Check the read error */
  for(y = 0; y < ysize; y++)
    for(x = 0; x < xsize; x++)
    {
      if(bitmap[y * xsize + x] != rombitmap.data[(ysize - 1 - y) * xsize + x])
        error++;
    }

  if(error)
    printf("ReadPixelTest error: %d\r\n", (int)error);

  BSP_LCD_DrawRGB16Image(x0 + 45, y0 + 10, xsize, ysize, &bitmap[0]);
  return(ctStartT);
}

#endif /* #if READ_TEST == 1 */
#endif /* #if BITMAP_TEST == 1 */

//-----------------------------------------------------------------------------
void mainApp(void)
{
  uint32_t t;

  HAL_Delay(300);

  BSP_LCD_Init();

  t = random();

  HAL_Delay(100);
  printf("\r\nDisplay ID = %X\r\n", (unsigned int)BSP_LCD_ReadID());
  HAL_Delay(100);

  while(1)
  {
    #ifndef  __CC_ARM
    _impure_ptr->_r48->_rand_next = 0;
    #endif


    ClearTest(1);
    #if BITMAP_TEST == 1
    printf("\r\nrunning bitmap test\r\n");
    BitmapTest(1);
    printf("Bitmap Test (1 bitmap): %d ms", (int)t);
    #endif /* #if BITMAP_TEST == 1 */
    printf("\r\n");
  }
}
