#ifndef LED_H
#define LED_H

#include "stm32f10x.h"
#include <stdint.h>

/* LED1 -> PC0, LED2 -> PC1, LED3 -> PC2, LED4 -> PC3 */
#define LED_COUNT 4U

void LED_GPIO_Init(void);
void LED_Set(uint8_t index, FunctionalState state);
void LED_WriteMask(uint8_t mask);
void LED_AllOff(void);

#endif
