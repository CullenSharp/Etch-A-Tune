/*
 * encoder.c
 *
 *  Created on: Dec 6, 2025
 *      Author: cullen-sharp
 */
#include "Encoder/encoder.h"

// function prototypes
uint32_t ROTENC_Get_State(struct Rotary_Encoder *encoder);

Rotary_Encoder encoders[2] = {
	{
		ROTENC_Get_State,
		0,
		ROTENC_GRAY,
		ROTA1_Pin, ROTB1_Pin,
		ROTA1_GPIO_Port, ROTB1_GPIO_Port
	},
	{
		ROTENC_Get_State,
		0,
		ROTENC_GRAY,
		ROTA2_Pin, ROTB2_Pin,
		ROTA2_GPIO_Port, ROTB2_GPIO_Port
	}
};

uint32_t ROTENC_Get_State(struct Rotary_Encoder *encoder) {
	uint32_t ch_a_raw, ch_b_raw;
	uint32_t ret = 0;

	// convert from gray code
	ch_a_raw = HAL_GPIO_ReadPin(encoder->_rot_a_port, encoder->_rot_a_pin);
	if ((encoder->encoding == ROTENC_GRAY) && (ret & 1)) {
		ch_a_raw = !ch_a_raw;
	}

	ret = ret << 1 | ch_a_raw;

	ch_b_raw = HAL_GPIO_ReadPin(encoder->_rot_b_port, encoder->_rot_b_pin);
	if ((encoder->encoding == ROTENC_GRAY) && (ret & 1)) {
		ch_b_raw = !ch_b_raw;
	}

	ret = ret << 1 | ch_b_raw;

	// mask least significant 3 bits
	return ret & 3;
}
