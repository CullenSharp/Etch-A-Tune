/* LCD speed test program
 * The results displayed in printf
 *
 * author: Roberto Benjami
 * version:  2022.12
 */

//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "Lcd/lcd.h"
#include "Lcd/bmp.h"

#include "App/appLcdSpeedTest.h"

/* BSP_LCD_... */
#include "Lcd/stm32_adafruit_lcd.h"

/* Test photo */
#if BITMAP_TEST == 1
#define rombitmap             beer_60x100_16
#define ROMBITMAP_WIDTH       60
#define ROMBITMAP_HEIGHT      100
#endif

#define GPIO_Port_(a)         a ## _GPIO_Port
#define GPIO_Port(a)          GPIO_Port_(a)
#define Pin_(a)               a ## _Pin
#define Pin(a)                Pin_(a)

#ifdef  __CC_ARM
#define random()              rand()
#endif

//-----------------------------------------------------------------------------
#ifndef  osCMSIS
/* no freertos */
#define Delay(t)              HAL_Delay(t)
#define GetTime()             HAL_GetTick()

#else
/* freertos */
#define Delay(t)              osDelay(t)
volatile uint32_t task02_count = 0, task02_run = 0;
uint32_t refcpuusage = 1;

#if osCMSIS < 0x20000
/* freertos V1 */

#define GetTime()             osKernelSysTick()
extern osThreadId defaultTaskHandle;
void StartTask02(void const * argument);
osThreadId Task2Handle;
osThreadDef(Task2, StartTask02, osPriorityLow, 0, 144);

#else
/* freertos V2 */

#define GetTime()             osKernelGetTickCount()
extern osThreadId_t defaultTaskHandle;
void StartTask02(void * argument);
osThreadId_t Task2Handle;
const osThreadAttr_t t2_attributes = {.name = "Task2", .stack_size = 256, .priority = (osPriority_t) osPriorityLow,};

#endif
/* common freertos V1 and freertos V2 */

#if     POWERMETER == 1
#define POWERMETER_START      {task02_count = 0; task02_run = 1;}
#define POWERMETER_STOP       task02_run = 0

#define POWERMETER_REF        {task02_run = 1; refcpuusage = task02_count / t;}
#define POWERMETER_PRINT      {Delay(10); if(t) printf(", cpu usage:%d%%\r\n", (int)cpuusage_calc(t)); else printf("\r\n");}
#endif

uint32_t cpuusage_calc(uint32_t t)
{
  uint32_t cpuusage;
  if(t)
  {
    cpuusage = ((100 * task02_count) / t) / refcpuusage;
    if(cpuusage > 100)
      cpuusage = 100;
    cpuusage = 100 - cpuusage;
  }
  else
    cpuusage = 0;

  #if STACKOWERFLOW_CHECK == 1
  uint32_t wm;
  wm = uxTaskGetStackHighWaterMark(Task2Handle);
  if(!wm)
    while(1);
  wm = uxTaskGetStackHighWaterMark(defaultTaskHandle);
  if(!wm)
    while(1);
  #endif

  task02_count = 0;
  return cpuusage;
}

#endif

//-----------------------------------------------------------------------------
#if BITMAP_TEST == 1
extern const BITMAPSTRUCT rombitmap;
#if READ_TEST == 1
uint16_t bitmap[ROMBITMAP_WIDTH * ROMBITMAP_HEIGHT];
#endif
#endif

//-----------------------------------------------------------------------------
uint32_t CharTest(uint32_t n) {
	uint16_t x, y;
	uint8_t c;
	sFONT *fp;

	uint32_t ctStartT = GetTime();
	for (uint32_t i = 0; i < n; i++) {
		c = random() % 5;
		if (c == 0)
			BSP_LCD_SetFont(&Font8);
		else if (c == 1)
			BSP_LCD_SetFont(&Font12);
		else if (c == 2)
			BSP_LCD_SetFont(&Font16);
		else if (c == 3)
			BSP_LCD_SetFont(&Font20);
		else if (c == 4)
			BSP_LCD_SetFont(&Font24);
		fp = BSP_LCD_GetFont();

		x = random() % (BSP_LCD_GetXSize() - fp->Width);
		y = random() % (BSP_LCD_GetYSize() - fp->Height);
		BSP_LCD_SetTextColor(LCD_COLOR16(random() & 0xFFFF));
		BSP_LCD_SetBackColor(LCD_COLOR16(random() & 0xFFFF));

		c = random() % 96 + ' ';
		BSP_LCD_DisplayChar(x, y, c);
	}
	return (GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t ColorTest(void) {
	uint16_t xs, ys;
	uint8_t cy;

	uint32_t ctStartT = GetTime();
	xs = BSP_LCD_GetXSize();
	ys = BSP_LCD_GetYSize();
	for (uint16_t x = 0; x < xs; x++) {
		cy = (uint32_t) (x << 8) / xs;
		BSP_LCD_SetTextColor(LCD_COLOR(cy, cy, cy));
		BSP_LCD_DrawVLine(x, 0, ys >> 2);

		BSP_LCD_SetTextColor(LCD_COLOR(cy, 0, 0));
		BSP_LCD_DrawVLine(x, ys >> 2, ys >> 2);

		BSP_LCD_SetTextColor(LCD_COLOR(0, cy, 0));
		BSP_LCD_DrawVLine(x, ys >> 1, ys >> 2);

		BSP_LCD_SetTextColor(LCD_COLOR(0, 0, cy));
		BSP_LCD_DrawVLine(x, (ys >> 1) + (ys >> 2), ys >> 2);
	}
	return (GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t BitmapTest(uint32_t n) {
	extern const BITMAPSTRUCT beer_60x100_16;
	uint16_t x, y;

	uint32_t ctStartT = GetTime();
	for (uint32_t i = 0; i < n; i++) {
		x = random() % (BSP_LCD_GetXSize() - rombitmap.infoHeader.biWidth);
		y = random() % (BSP_LCD_GetYSize() - rombitmap.infoHeader.biHeight);
		BSP_LCD_DrawBitmap(x, y, (uint8_t*) &rombitmap);
	}
	return (GetTime() - ctStartT);
}
//-----------------------------------------------------------------------------
void mainApp(void) {
	Delay(300);

	BSP_LCD_Init();

#ifndef  __CC_ARM
	_impure_ptr->_r48->_rand_next = 0;
#endif

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	ColorTest();
	Delay(DELAY_CHAPTER);
}
