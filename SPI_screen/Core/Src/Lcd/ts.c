/*
 * ts.c
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 *
 *	todo: write adc reads to be non-blocking
 */

#include "Lcd/ts.h"
#include "main.h"

// private variables
extern ADC_HandleTypeDef hadc1;

// function prototypes
void 	 TS_Drv_init(void);
uint8_t  TS_Drv_Read_Touch_Y(uint32_t* y);
uint8_t  TS_Drv_Read_Touch_X(uint32_t* x);
TS_Point TS_Drv_Get_Point(void);

TS_Drv ili9341_ts_drv = {
	TS_Drv_init,
	TS_Drv_Read_Touch_Y,
	TS_Drv_Read_Touch_X,
	TS_Drv_Get_Point,
	330,				// nominal resistance between plates
	XP_Pin, 			// _xp
	YP_Pin, 			// _yp
	XM_Pin, 			// _xm
	YM_Pin, 			// _ym
	XP_GPIO_Port,		// _xp_port
	YP_GPIO_Port,		// _yp_port
	XM_GPIO_Port,		// _xm_port
	YM_GPIO_Port		// _ym_port
};

// expose generic pointer to register level touch screen driver
TS_Drv  *ts_drv = &ili9341_ts_drv;


void TS_Drv_init(void) {
	// not implemented yet
}

uint8_t TS_Drv_Read_Touch_X(uint32_t *x) {
	uint32_t s0, s1;
	uint8_t valid = 1;
	int32_t diff;

	// |              | X+   | X-         | Y+   | Y-            |
	// | X-Coordinate | Hi-Z | Hi-Z / ADC | Gnd  | Vcc           |
	LL_GPIO_SetPinMode(ili9341_ts_drv._xp_port, ili9341_ts_drv._xp_pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(ili9341_ts_drv._xm_port, ili9341_ts_drv._xm_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ili9341_ts_drv._yp_port, ili9341_ts_drv._yp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ili9341_ts_drv._ym_port, ili9341_ts_drv._ym_pin, LL_GPIO_MODE_OUTPUT);

	// Y- = Vcc
	LL_GPIO_SetOutputPin(ili9341_ts_drv._ym_port, ili9341_ts_drv._ym_pin);

	// Y+ = Gnd
	LL_GPIO_ResetOutputPin(ili9341_ts_drv._yp_port, ili9341_ts_drv._yp_pin);

	// delay for voltages to settle
	// 5600 iterations roughly translates to 20us
	for (volatile uint32_t i = 0; i < 5600; ++i);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 14
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_14);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s0 = HAL_ADC_GetValue(&hadc1);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s1 = HAL_ADC_GetValue(&hadc1);

	// assign result
	*x = (s0 + s1) >> 1;

	// check for validity
	diff = (int32_t)s1 - (int32_t)s0;
	if ((diff < -8) || (diff > 8)) {
		return !valid;
	} else {
		return  valid;
	}
}

uint8_t TS_Drv_Read_Touch_Y(uint32_t *y) {
	uint32_t s0, s1;
	uint8_t valid = 1;
	int32_t diff;

	//|              | X+   | X-         | Y+   	  | Y-      |
	//| Y-Coordinate | Gnd  | Vcc        | Hi-Z / ADC | Hi-Z    |
	LL_GPIO_SetPinMode(ili9341_ts_drv._xp_port, ili9341_ts_drv._xp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ili9341_ts_drv._xm_port, ili9341_ts_drv._xm_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ili9341_ts_drv._yp_port, ili9341_ts_drv._yp_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ili9341_ts_drv._ym_port, ili9341_ts_drv._ym_pin, LL_GPIO_MODE_INPUT);

	// X- = Vcc
	LL_GPIO_SetOutputPin(ili9341_ts_drv._xm_port, 	ili9341_ts_drv._xm_pin);

	// X+ = Gnd
	LL_GPIO_ResetOutputPin(ili9341_ts_drv._xp_port, ili9341_ts_drv._xp_pin);

	// delay for voltages to settle
	// 5600 iterations roughly translates to 20us
	for (volatile uint32_t i = 0; i < 5600; ++i);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 17
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_17);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_17);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s0 = HAL_ADC_GetValue(&hadc1);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s1 = HAL_ADC_GetValue(&hadc1);

	// assign result
	*y = 1023 - ((s0 + s1) >> 1);

	// check for validity
	diff = (int32_t)s1 - (int32_t)s0;
	if ((diff < -8) || (diff > 8)) {
		return !valid;
	} else {
		return  valid;
	}
}

TS_Point TS_Drv_Get_Point(void) {
	uint32_t x, y, z;
	uint32_t s1, s0;
	uint8_t valid;

	valid = ili9341_ts_drv.Read_Touch_X(&x);
	valid = ili9341_ts_drv.Read_Touch_Y(&y);

	//|              | X+   | X-         | Y+   	  | Y-      |
	//| Pressure     | Gnd  | ADC        | ADC 		  | Vcc    	|
	LL_GPIO_SetPinMode(ili9341_ts_drv._xp_port, ili9341_ts_drv._xp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ili9341_ts_drv._xm_port, ili9341_ts_drv._xm_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ili9341_ts_drv._yp_port, ili9341_ts_drv._yp_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ili9341_ts_drv._ym_port, ili9341_ts_drv._ym_pin, LL_GPIO_MODE_OUTPUT);

	// Y- = Vcc
	LL_GPIO_SetOutputPin(ili9341_ts_drv._ym_port, 	ili9341_ts_drv._ym_pin);

	// X+ = Gnd
	LL_GPIO_ResetOutputPin(ili9341_ts_drv._xp_port, ili9341_ts_drv._xp_pin);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 17 (y+)
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_17);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_17);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s0 = HAL_ADC_GetValue(&hadc1);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 14 (x-)
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_14);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	s1 = HAL_ADC_GetValue(&hadc1);

	// calculate the pressure
	if (ili9341_ts_drv._rxplate != 0) {
		float scratch;
		scratch  = s0;
		scratch /= s1;
		scratch -= 1;
		scratch *= x;
		scratch *= ili9341_ts_drv._rxplate;
		scratch /= 1024;
		z = (uint32_t) scratch;
	} else {
		z = (1023 - (s0 - s1));
	}

	if (!valid) {
		z = 0;
	}

	TS_Point result = {x, y, z};
	return result;
}
