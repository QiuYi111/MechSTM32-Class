#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"
#include <stdint.h>

typedef enum
{
    ADC_CHANNEL_0 = 0,    /* PA0 */
    ADC_CHANNEL_1,        /* PA1 - 电位器 */
    ADC_CHANNEL_2,        /* PA2 */
    ADC_CHANNEL_3,        /* PA3 */
    ADC_CHANNEL_COUNT
} adc_channel_t;

void ADC_Config(void);
uint16_t ADC_ReadValue(adc_channel_t channel);
float ADC_ReadVoltage(adc_channel_t channel);

#endif

