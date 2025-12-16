#ifndef USART1_H
#define USART1_H	 

#include "stm32f10x.h"
#include <stdio.h>

//  º¯ÊıÉùÃ÷
void UART1_Configuration(void);
void USART1_Senddata(unsigned char *Data, unsigned int length);

#endif
