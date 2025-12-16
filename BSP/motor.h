#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

void Motor_Init(void);
void Motor_SetSpeed(uint16_t speed); // 0-100
void Motor_SetDir(uint8_t dir);      // 0: Forward, 1: Reverse

#endif
