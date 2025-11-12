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
#include <Lcd/ts.h>
#include <Lcd/stm32_adafruit_ts.h>
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

#define CALIBBOXSIZE          6
#define CALIBBOXPOS           15

#define CALIBDELAY            500
#define TOUCHDELAY            50

/* overflow limit (cindex 1, 2, 4, 5 and cindex 3, 6) */
#define MAXCINT1245           262144
#define MAXCINT36             1073741824

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

void touchcalib_drawBox(int32_t x, int32_t y, uint16_t cl)
{
  BSP_LCD_SetTextColor(cl);
  BSP_LCD_DrawRect(x - CALIBBOXSIZE / 2, y - CALIBBOXSIZE / 2, CALIBBOXSIZE, CALIBBOXSIZE);
}

//-----------------------------------------------------------------------------

extern  TS_DrvTypeDef         *ts_drv;


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void mainApp(void)
{
	uint16_t tx, ty;
	ts_three_points tc, dc; /* touchscreen and display corrdinates */
	ts_cindex ci;

	printf("\r\nPlease: set the LCD ORIENTATION to the value on which the program will run.\r\n");
	HAL_Delay(100);
	printf("Press the screen at the yellow squares that appear,\r\n");
	HAL_Delay(100);
	printf("then paste the following line into stm32_adafruit_ts.h :\r\n");

	BSP_LCD_Init();
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	dc.x0 = 20;
	dc.y0 = 20;
	dc.x1 = BSP_LCD_GetXSize() >> 1;
	dc.y1 = BSP_LCD_GetYSize() - 1 - 20;
	dc.x2 = BSP_LCD_GetXSize() - 1 - 20;
	dc.y2 = BSP_LCD_GetYSize() >> 1;

	touchcalib_drawBox(dc.x0, dc.y0, LCD_COLOR_YELLOW);
	while(!ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);
	ts_drv->GetXY(0, &tx, &ty);
	tc.x0 = tx; tc.y0 = ty;
	while(ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);

	touchcalib_drawBox(dc.x0, dc.y0, LCD_COLOR_GRAY);
	touchcalib_drawBox(dc.x1, dc.y1, LCD_COLOR_YELLOW);
	HAL_Delay(CALIBDELAY);
	while(!ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);
	ts_drv->GetXY(0, &tx, &ty);
	tc.x1 = tx; tc.y1 = ty;
	while(ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);

	touchcalib_drawBox(dc.x1, dc.y1, LCD_COLOR_GRAY);
	touchcalib_drawBox(dc.x2, dc.y2, LCD_COLOR_YELLOW);
	HAL_Delay(CALIBDELAY);
	while(!ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);
	ts_drv->GetXY(0, &tx, &ty);
	tc.x2 = tx; tc.y2 = ty;
	while(ts_drv->DetectTouch(0))
		HAL_Delay(TOUCHDELAY);

	BSP_TS_CalibCalc(&tc, &dc, &ci);

	printf("#define  TS_CINDEX            {%d, %d, %d, %d, %d, %d, %d}\r\n", (int)ci[0], (int)ci[1], (int)ci[2], (int)ci[3], (int)ci[4], (int)ci[5], (int)ci[6]);

	while(1);
}

//void mainApp(void)
//{
//  uint32_t t;
//
//  HAL_Delay(300);
//
//  BSP_LCD_Init();
//
//  t = random();
//
//  HAL_Delay(100);
//  printf("\r\nDisplay ID = %X\r\n", (unsigned int)BSP_LCD_ReadID());
//  HAL_Delay(100);
//
//  while(1)
//  {
//    #ifndef  __CC_ARM
//    _impure_ptr->_r48->_rand_next = 0;
//    #endif
//
//
//    ClearTest(1);
//    #if BITMAP_TEST == 1
//    printf("\r\nrunning bitmap test\r\n");
//    BitmapTest(1);
//    printf("Bitmap Test (1 bitmap): %d ms", (int)t);
//    #endif /* #if BITMAP_TEST == 1 */
//    printf("\r\n");
//  }
//}
