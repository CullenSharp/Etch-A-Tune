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
#define CALIBBOXSIZE          6
#define CALIBBOXPOS           15

#define CALIBDELAY            500
#define TOUCHDELAY            50

/* overflow limit (cindex 1, 2, 4, 5 and cindex 3, 6) */
#define MAXCINT1245           262144
#define MAXCINT36             1073741824

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

extern  TS_DrvTypeDef         *ts_drv;

//-----------------------------------------------------------------------------

extern const BITMAPSTRUCT hurricane_480x320_16;
uint16_t front[ROMBITMAP_WIDTH * ROMBITMAP_HEIGHT];
uint16_t back[ROMBITMAP_WIDTH * ROMBITMAP_HEIGHT];

//-----------------------------------------------------------------------------

void mainApp(void)
{
  BSP_LCD_Init();
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  HAL_Delay(100);
  printf("\r\nDisplay ID = %X\r\n", (unsigned int)BSP_LCD_ReadID());
  HAL_Delay(100);

  printf("\r\nWidth: %ld, Height: %ld\r\n", hurricane_480x320_16.infoHeader.biWidth, hurricane_480x320_16.infoHeader.biHeight);
  memcpy(front, hurricane_480x320_16.data, sizeof front);
  BSP_LCD_DrawRGB16Image(0, 0, 480, 320, front);
  while(1)
  {

  }
}
