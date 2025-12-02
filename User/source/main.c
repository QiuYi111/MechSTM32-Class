#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "led.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "adc.h"

#define EXPERIMENT 1  /* 修改这里来选择实验: 1, 2, 3 */
#define USART_Mode_Rx_Tx  USART_Mode_Rx | USART_Mode_Tx
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Systick_Init();
    LED_GPIO_Init();
    Key_Init();
    ADC_Config();
}

void Experiment1_DisplayVoltage(void)
{
    UART_InitTypeDef uartConfig = {
        .remap = UART_REMAP_0,
        .baudrate = 115200,
        .wordlength = USART_WordLength_8b,
        .stopbits = USART_StopBits_1,
        .parity = USART_Parity_No,
        .mode = USART_Mode_Tx,
        .enableInterrupt = false
    };

    UART_Init(USART1, &uartConfig);
    UART_SendString(USART1, "ADC Voltage Monitor\r\n");
    UART_SendString(USART1, "PA1 - Potentiometer Voltage:\r\n");

    while (1) {
        float voltage = ADC_ReadVoltage(ADC_CHANNEL_1);
        char voltageStr[32];

        sprintf(voltageStr, "ADC= %.2fV\r\n", voltage);
        UART_SendString(USART1, voltageStr);

        delay_ms(500);
    }
}

void Experiment2_LED_BrightnessControl(void)
{
    float voltage;
    uint32_t delayTime;

    LED_AllOff();

    while (1) {
        /* 读取电压 */
        voltage = ADC_ReadVoltage(ADC_CHANNEL_1);

        /* 电压映射到延迟时间 (亮度控制) */
        if (voltage <= 0.5f) {
            delayTime = 100;          /* 快速闪烁 */
        } else if (voltage <= 1.0f) {
            delayTime = 80;
        } else if (voltage <= 1.5f) {
            delayTime = 60;
        } else if (voltage <= 2.0f) {
            delayTime = 40;
        } else if (voltage <= 2.5f) {
            delayTime = 20;
        } else {
            delayTime = 5;            /* 慢速闪烁，看起来像常亮 */
        }

        /* 控制所有 LED 的亮度 */
        LED_WriteMask(0x0F);  /* 打开所有 LED */
        delay_ms(delayTime);
        LED_AllOff();
        delay_ms(delayTime);
    }
}

void Experiment3_RunningLightControl(void)
{
    float voltage;
    uint32_t speedDelay;
    uint8_t direction = 0;  /* 0: forward, 1: backward */
    uint8_t currentLED = 0;
    uint8_t ledPattern[4] = {0x01, 0x02, 0x04, 0x08};
        static uint8_t lastKeyState = 0;

    LED_AllOff();

    while (1) {
        /* 读取电位器电压控制速度 */
        voltage = ADC_ReadVoltage(ADC_CHANNEL_1);

        /* 电压映射到延迟时间 (速度控制) */
        if (voltage <= 0.5f) {
            speedDelay = 500;  /* 最慢速度 */
        } else if (voltage <= 1.0f) {
            speedDelay = 300;
        } else if (voltage <= 1.5f) {
            speedDelay = 200;
        } else if (voltage <= 2.0f) {
            speedDelay = 150;
        } else if (voltage <= 2.5f) {
            speedDelay = 100;
        } else {
            speedDelay = 50;   /* 最快速度 */
        }

        /* 检查按键控制方向 */
        uint8_t currentKeyMask = Key_ReadMask();

        /* 检测按键按下 (边沿检测) */
        if (currentKeyMask != lastKeyState) {
            /* 检查哪个按键被按下 */
            if ((currentKeyMask & (1 << KEY_S1)) && !(lastKeyState & (1 << KEY_S1))) {
                direction = 0;  /* 向前 */
            }
            if ((currentKeyMask & (1 << KEY_S2)) && !(lastKeyState & (1 << KEY_S2))) {
                direction = 1;  /* 向后 */
            }
            lastKeyState = currentKeyMask;
        }

        /* 显示当前状态 */
        LED_AllOff();
        LED_WriteMask(ledPattern[currentLED]);

        /* 延迟 */
        delay_ms(speedDelay);

        /* 更新 LED 位置 */
        if (direction == 0) {
            /* 向前 */
            currentLED = (currentLED + 1) % 4;
        } else {
            /* 向后 */
            currentLED = (currentLED + 3) % 4;  /* 相当于 -1，但避免负数 */
        }
    }
}


int main(void)
{
    System_Init();

    switch (EXPERIMENT) {
        case 1:
            Experiment1_DisplayVoltage();
            break;
        case 2:
            Experiment2_LED_BrightnessControl();
            break;
        case 3:
            Experiment3_RunningLightControl();
            break;
        default:
            break;
    }

    while (1);
}
