/*
 * ts.h
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __TS_H_
#define __TS_H_

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "stm32h7a3xxq.h"
#include "stm32h7xx_ll_gpio.h"

/* Defines ------------------------------------------------------------------*/
// These will be used to map the touch screen to the coords to screen space
// They were found experimentally
#define MIN_X	70u
#define MIN_Y	100u
#define MAX_X	930u
#define MAX_Y	910u
/* Interface ----------------------------------------------------------------*/

typedef struct {
    // State variables for x and y
    uint32_t x;
    uint32_t y;
    uint32_t z;
} TS_Point;

typedef struct {
    void 	(*Init)(void);

    // public methods
    uint8_t 	(*Read_Touch_Y)(uint32_t*);
    uint8_t 	(*Read_Touch_X)(uint32_t*);
    TS_Point 	(*Get_Point)();

    // private variables
    uint32_t _rxplate;
    // pins
    uint32_t  _xp_pin,  _yp_pin,  _xm_pin,  _ym_pin;
    // ports
    GPIO_TypeDef *_xp_port, *_yp_port, *_xm_port, *_ym_port;
} GRTS_Drv;


#endif /* INC_LCD_TS_H_ */
