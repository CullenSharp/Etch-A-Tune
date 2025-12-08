/*
 * ts.c -- adapted from adafruit/Adafruit_TouchScreen
 *
 *	Credit to ladyada / adafruit
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 */

#include "Lcd/ts.h"
#include "main.h"

// private variables
extern ADC_HandleTypeDef hadc1;

// private functions

// generate 5 samples, sort, then return median
static inline uint16_t read_adc_filtered(void) {
	uint16_t v[5];
	for (uint8_t i = 0; i < 5; ++i) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		v[i] = HAL_ADC_GetValue(&hadc1);
	}

	for (uint8_t i = 0; i < 5; ++i) {
		for (uint8_t j = i + 1; j < 5; ++j) {
			if (v[j] < v[i]) {uint16_t t = v[i]; v[i] = v[j]; v[j] = t;}
		}
	}

	return v[2];
}

// function prototypes
void 	 GRTS_Drv_init(void);
uint8_t  GRTS_Drv_Read_Touch_Y(uint32_t* y);
uint8_t  GRTS_Drv_Read_Touch_X(uint32_t* x);
TS_Point GRTS_Drv_Get_Point(void);

GRTS_Drv grts_drv = {
	GRTS_Drv_init,
	GRTS_Drv_Read_Touch_Y,
	GRTS_Drv_Read_Touch_X,
	GRTS_Drv_Get_Point,
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
GRTS_Drv  *ts_drv = &grts_drv;


void GRTS_Drv_init(void) {
	// not implemented yet
}

uint8_t GRTS_Drv_Read_Touch_X(uint32_t *x) {
	uint8_t valid = 1;

	// |              | X+   | X-         | Y+   | Y-            |
	// | X-Coordinate | Hi-Z | Hi-Z / ADC | Gnd  | Vcc           |
	LL_GPIO_SetPinMode(grts_drv._xp_port, grts_drv._xp_pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(grts_drv._xm_port, grts_drv._xm_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(grts_drv._yp_port, grts_drv._yp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(grts_drv._ym_port, grts_drv._ym_pin, LL_GPIO_MODE_OUTPUT);

	// Y- = Vcc
	LL_GPIO_SetOutputPin(grts_drv._ym_port, grts_drv._ym_pin);

	// Y+ = Gnd
	LL_GPIO_ResetOutputPin(grts_drv._yp_port, grts_drv._yp_pin);

	// delay for voltages to settle
	// 75000 iterations roughly translates to 300us
	for (volatile uint32_t i = 0; i < 75000; ++i);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 14
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_14);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);

	// assign result
	*x = read_adc_filtered();
	return valid;
}

uint8_t GRTS_Drv_Read_Touch_Y(uint32_t *y) {
	uint8_t valid = 1;

	//|              | X+   | X-         | Y+   	  | Y-      |
	//| Y-Coordinate | Gnd  | Vcc        | Hi-Z / ADC | Hi-Z    |
	LL_GPIO_SetPinMode(grts_drv._xp_port, grts_drv._xp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(grts_drv._xm_port, grts_drv._xm_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(grts_drv._yp_port, grts_drv._yp_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(grts_drv._ym_port, grts_drv._ym_pin, LL_GPIO_MODE_INPUT);

	// X- = Vcc
	LL_GPIO_SetOutputPin(grts_drv._xm_port, 	grts_drv._xm_pin);

	// X+ = Gnd
	LL_GPIO_ResetOutputPin(grts_drv._xp_port, grts_drv._xp_pin);

	// delay for voltages to settle
	// 75000 iterations roughly translates to 300us
	for (volatile uint32_t i = 0; i < 75000; ++i);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 17
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_17);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_17);

	// assign result
	*y = 1023 - read_adc_filtered();
	return valid;
}

TS_Point GRTS_Drv_Get_Point(void) {
	uint32_t x, y, z;
	uint32_t s1, s0;
	uint8_t valid;

	valid = grts_drv.Read_Touch_X(&x);
	valid = grts_drv.Read_Touch_Y(&y);

	//|              | X+   | X-         | Y+   	  | Y-      |
	//| Pressure     | Gnd  | ADC        | ADC 		  | Vcc    	|
	LL_GPIO_SetPinMode(grts_drv._xp_port, grts_drv._xp_pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(grts_drv._xm_port, grts_drv._xm_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(grts_drv._yp_port, grts_drv._yp_pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(grts_drv._ym_port, grts_drv._ym_pin, LL_GPIO_MODE_OUTPUT);

	// Y- = Vcc
	LL_GPIO_SetOutputPin(grts_drv._ym_port, grts_drv._ym_pin);

	// X+ = Gnd
	LL_GPIO_ResetOutputPin(grts_drv._xp_port, grts_drv._xp_pin);

	// delay for voltages to settle
	// 75000 iterations roughly translates to 300us
	for (volatile uint32_t i = 0; i < 75000; ++i);

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 17 (y+)
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_17);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_17);

	s0 = read_adc_filtered();

	// Stop ADC since channel selection is only allowed when stopped
	HAL_ADC_Stop(&hadc1);

	// only read channel 14 (x-)
	LL_ADC_SetChannelPreselection(ADC1, LL_ADC_CHANNEL_14);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);

	s1 = read_adc_filtered();

	// calculate the pressure
	if (grts_drv._rxplate != 0) {
		float scratch;
		scratch  = s0;
		scratch /= s1;
		scratch -= 1;
		scratch *= x;
		scratch *= grts_drv._rxplate;
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
