#ifndef PWM_H
#define PWM_H

#include "stm32f10x.h"
#include <stdint.h>

void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t percent);

#endif