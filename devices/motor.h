#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

#define PWM_MAX  10000
#define PWM_MIN -10000

void Motor_Init(uint32_t freq);
void Motor_PWM(int32_t Left_PWM,int32_t Right_PWM);
void Motor_STOP(void);
void Motor_START(void);

#endif
