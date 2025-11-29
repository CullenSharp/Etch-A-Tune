/*
 * ts.h
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TS_H_
#define __TS_H_

/* Interface section */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32h7a3xxq.h"
#include "stm32h7xx_ll_gpio.h"

typedef struct {
    // State variables for x and y
    uint32_t x;
    uint32_t y;
} TS_Point;

typedef struct {
    /**
     * @brief Construct a new TS_Drv object
     *
     * @param xp X+ pin. Must be an analog pin
     * @param yp Y+ pin. Must be an analog pin
     * @param xm X- pin. Can be a digital pin
     * @param ym Y- pin. Can be a digital pin
     */
    void 	(*Init)(void);

    // public methods
    uint32_t 	(*Read_Touch_Y)();
    uint32_t 	(*Read_Touch_X)();
    TS_Point 	(*Get_Point)();

    // private variables
    // pins
    uint32_t  _xp_pin,  _yp_pin,  _xm_pin,  _ym_pin;
    // ports
    GPIO_TypeDef *_xp_port, *_yp_port, *_xm_port, *_ym_port;
} TS_Drv;


#endif /* INC_LCD_TS_H_ */
