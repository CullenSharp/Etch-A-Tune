/*
 * ts.c
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 *
 *	todo: create a macro or ts_drv attribute for adc channel
 *	todo: write adc reads to be non-blocking
 */

#include "Lcd/ts.h"
#include "main.h"

// private variables
extern ADC_HandleTypeDef hadc1;

// function prototypes
void 	 TS_Drv_init(void);
uint32_t TS_Drv_Read_Touch_Y(void);
uint32_t TS_Drv_Read_Touch_X(void);
TS_Point TS_Drv_Get_Point(void);

TS_Drv ts_drv = {
	TS_Drv_init,
	TS_Drv_Read_Touch_Y,
	TS_Drv_Read_Touch_X,
	TS_Drv_Get_Point,
	XP_Pin, 			// _xp
	YP_Pin, 			// _yp
	XM_Pin, 			// _xm
	YM_Pin, 			// _ym
	XP_GPIO_Port,		// _xp_port
	YP_GPIO_Port,		// _yp_port
	XM_GPIO_Port,		// _xm_port
	YM_GPIO_Port		// _ym_port
};


void TS_Drv_init(void) {
	// not implemented yet
}

uint32_t TS_Drv_Read_Touch_Y(void) {
	uint32_t s0, s1;

	// |              | X+   | X-         | Y+   | Y-            |
	// | Y-Coordinate | Hi-Z | Hi-Z / ADC | Gnd  | Vcc           |
	LL_GPIO_SetPinMode(ts_drv._xp_port, ts_drv._xp_pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(ts_drv._xm_port, ts_drv._xm_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ts_drv._yp_port, ts_drv._yp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ts_drv._ym_port, ts_drv._ym_pin, LL_GPIO_MODE_OUTPUT);

	// Y- = Vcc
	LL_GPIO_SetOutputPin(ts_drv._ym_port, ts_drv._ym_pin);

	// Y+ = Gnd
	LL_GPIO_ResetOutputPin(ts_drv._yp_port, ts_drv._yp_pin);

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

	return (1023 - ((s0 + s1) >> 1));
}

uint32_t TS_Drv_Read_Touch_X(void) {
	uint32_t s0, s1;

	//|              | X+   | X-         | Y+   	  | Y-      |
	//| X-Coordinate | Gnd  | Vcc        | Hi-Z / ADC | Hi-Z    |
	LL_GPIO_SetPinMode(ts_drv._xp_port, ts_drv._xp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ts_drv._xm_port, ts_drv._xm_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(ts_drv._yp_port, ts_drv._yp_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(ts_drv._ym_port, ts_drv._ym_pin, LL_GPIO_MODE_INPUT);

	// X- = Vcc
	LL_GPIO_SetOutputPin(ts_drv._xm_port, 	ts_drv._xm_pin);

	// X+ = Gnd
	LL_GPIO_ResetOutputPin(ts_drv._xp_port, ts_drv._xp_pin);

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

	return (1023 - ((s0 + s1) >> 1));
}

TS_Point TS_Drv_Get_Point(void) {
	TS_Point result = {0, 0};

	result.x = ts_drv.Read_Touch_X();
	result.y = ts_drv.Read_Touch_Y();

	return result;
}
