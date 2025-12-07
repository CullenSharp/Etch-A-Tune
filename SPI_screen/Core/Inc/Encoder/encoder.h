/*
 * encoder.h
 *
 *  Created on: Dec 6, 2025
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

typedef struct Rotary_Encoder{
	// public methods
	uint32_t (*Get_State)(struct Rotary_Encoder *);

	// public data
	int8_t dir;
	enum rotary_encoder_encoding encoding;

	// private data
	uint16_t 	 _rot_a_pin;
	uint16_t 	 _rot_b_pin;
	GPIO_TypeDef *_rot_a_port;
	GPIO_TypeDef *_rot_b_port;
} Rotary_Encoder;


#endif /* ENCODER_H_ */
