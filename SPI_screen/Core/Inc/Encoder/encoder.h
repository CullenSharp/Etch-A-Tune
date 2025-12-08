/*
 * encoder.h -- linux/drivers/input/misc/rotary_encoder
 *
 * Credit to:
 *	Johan Hovold <jhovold@gmail.com>
 *  Daniel Mack <daniel@caiaq.de>
 *  Tim Ruetz
 *
 *  Created on: Nov 28, 2025
 *      Author: cullen-sharp
 */
#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include "main.h"


enum rotary_encoder_encoding {
	ROTENC_GRAY,
	ROTENC_BINARY,
};

enum axis {
	IO_X,
	IO_Y,
};

enum enc_state {
	S0_DISARM,
	S1_TURN_CW,
	S2_ARM,
	S3_TURN_CCW
};

typedef struct Rotary_Encoder{
	// public methods
	uint32_t (*Get_Pos)(struct Rotary_Encoder *);

	// private methods
	enum enc_state (*Get_State)(struct Rotary_Encoder *);
	void (*Update_Pos)(struct Rotary_Encoder *);
	void (*irq_handler)(struct Rotary_Encoder *);

	// public data
	uint32_t steps;				// max number of steps allowed
	uint8_t rollover;			// steps wrap on overflow
	int8_t 	dir;				// 1 -> CW, -1 -> CCW
	uint8_t armed;				// 0 -> disarmed, 1 -> armed
	uint32_t pos;				// current position

	// IO input axis to control
	enum axis axis;				// 0 -> X, 1 -> Y

	// use either gray or binary encoding
	enum rotary_encoder_encoding encoding;

	// private data
	uint16_t 	 _rot_a_pin;
	uint16_t 	 _rot_b_pin;
	GPIO_TypeDef *_rot_a_port;
	GPIO_TypeDef *_rot_b_port;
} Rotary_Encoder;

#endif /* ENCODER_H_ */
