#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>
#include "gpio.h"

#define SPEEDISRCNT 10
#define Encoder_LeftDir()  GPIO_ReadBit(HW_GPIOB,21)
#define Encoder_RightDir() GPIO_ReadBit(HW_GPIOB,22)

typedef enum
{
	LeftMotor,
	RightMotor,
}Encoder_WhichMotor_Type;

typedef struct
{
	int value;
	uint8_t direction;
}Encoder_Speed_Type;

void Encoder_Init(void);
void Encoder_GetPulse(void);

#endif
