#ifndef DELAY_H
#define DELAY_H

#include "stm32f10x.h"

//  º¯ÊıÉùÃ÷
void Systick_Init(void);
void delay_ms(unsigned int time);
void delay_us(unsigned int time);
void TimingDelay_Decrement(void);

#endif
