/*
 * encoder.c -- Adapted from linux/drivers/input/misc/rotary_encoder
 *
 * Credit to:
 * 		Johan Hovold <jhovold@gmail.com>
 *  	Daniel Mack <daniel@caiaq.de>
 *  	Tim Ruetz
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 */
#include "Encoder/encoder.h"
#include "Lcd/ili9341.h"

// private function prototypes
static enum enc_state ROTENC_Get_State(struct Rotary_Encoder *encoder);
static void ROTENC_Update_Pos(struct Rotary_Encoder *encoder);
static void ROTENC_IRQ_Handler(struct Rotary_Encoder *encoder);

// public function protoypes
static uint32_t ROTENC_Get_Pos(struct Rotary_Encoder *encoder);


// device initialization
Rotary_Encoder encoders[2] = {
	{
		ROTENC_Get_Pos,
		ROTENC_Get_State,
		ROTENC_Update_Pos,
		ROTENC_IRQ_Handler,
		ILI9341_LCD_PIXEL_WIDTH, 1, 0, 0, 0,
		IO_X,
		ROTENC_GRAY,
		ROTA1_Pin, ROTB1_Pin,
		ROTA1_GPIO_Port, ROTB1_GPIO_Port
	},
	{
		ROTENC_Get_Pos,
		ROTENC_Get_State,
		ROTENC_Update_Pos,
		ROTENC_IRQ_Handler,
		ILI9341_LCD_PIXEL_HEIGHT, 1, 0, 0, 0,
		IO_Y,
		ROTENC_GRAY,
		ROTA2_Pin, ROTB2_Pin,
		ROTA2_GPIO_Port, ROTB2_GPIO_Port
	}
};

/**
 * @brief samples channel a and channel b, then reports the state
 * @param encoder
 */
static enum enc_state ROTENC_Get_State(struct Rotary_Encoder *encoder) {
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

	// mask least significant 2 bits
	ret &= 0b11;
	switch(ret) {
	case 0x0:
		return S0_DISARM;
	case 0x1:
		return S1_TURN_CW;
	case 0x2:
		return S2_ARM;
	case 0x3:
		return S3_TURN_CCW;
	default:
		__NOP();
	}

	return S0_DISARM;
}


/**
 * @brief determines the direction and arming of the encoder
 * @param encoder
 */
static void ROTENC_IRQ_Handler(struct Rotary_Encoder *encoder) {
	enum enc_state state = encoder->Get_State(encoder);

	switch (state) {
	case S0_DISARM:
		if (encoder->armed) {
			encoder->Update_Pos(encoder);
			encoder->armed = 0;
		}
		break;
	case S1_TURN_CW:
	case S3_TURN_CCW:
		if (encoder->armed) {
			encoder->dir = 2 - state;
		}
		break;
	case S2_ARM:
		encoder->armed = 1;
	default:
		__NOP();
	}
}

/**
 * @brief calculates new position based on past position
 * 		  and direction of rotation.
 * @param encoder
 */
static void ROTENC_Update_Pos(struct Rotary_Encoder *encoder) {
	uint32_t pos = encoder->pos;

	/*  Suppose our initial position in screen space is p0
	 *
	 *  +--------------------+
	 *  |                    |
	 *  |<-p0                |
	 *  |                    |
	 *  |                    |
	 *  |                    |
	 *  +--------------------+
	 *
	 *  Turning the encoder ccw, results in a new position p1
	 *  if rollover is selected, otherwise it would remain at 0.
	 *
	 *	+--------------------+
	 *  |                    |
	 *  |               p1<--|
	 *  |                    |
	 *  |                    |
	 *  |                    |
	 *  +--------------------+
	 */
	if (encoder->dir < 0) {
		// turning counter clockwise
		if (encoder->rollover) {
			pos += encoder->steps;
		}
		// prevents underflow by only incrementing when pos > 0
		if (pos) {
			pos--;
		}
	} else {
		// turning clockwise
		if ((encoder->rollover) || (encoder->pos)) {
			pos++;
		}
	}

	if (encoder->rollover) {
		pos %= encoder->steps;
	}

	encoder->pos = pos;
}

static uint32_t ROTENC_Get_Pos(struct Rotary_Encoder *encoder) {
	return encoder->pos;
}
