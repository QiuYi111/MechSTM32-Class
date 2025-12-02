#include "led.h"

#define LED_GPIO_PORT GPIOC // LED 所在的 GPIO 端口
#define LED_GPIO_RCC RCC_APB2Periph_GPIOC // LED 所在的 GPIO 时钟

/* 高电平为激活*/
#define LED_ACTIVE_HIGH 1

//定义 LED 引脚
static const uint16_t led_pins[LED_COUNT] = {
    GPIO_Pin_0,
    GPIO_Pin_1,
    GPIO_Pin_2,
    GPIO_Pin_3
};

//返回所有 LED 引脚的掩码
static uint16_t led_all_pins(void)
{
    uint16_t mask = 0U;
    uint8_t i;
    for (i = 0U; i < LED_COUNT; ++i)
    {
        mask |= led_pins[i];
    }
    return mask;
}
//初始化 LED 引脚
void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(LED_GPIO_RCC, ENABLE); //开启 LED GPIO 时钟

    gpio_init.GPIO_Pin = led_all_pins();//所有 LED 引脚
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_PORT, &gpio_init);

    LED_AllOff();//初始化后全部熄灭
}

void LED_Set(uint8_t index, FunctionalState state)
{
    if (index >= LED_COUNT)
    {
        return; //越界，直接返回
    }

#if LED_ACTIVE_HIGH
    if (state == ENABLE)
    {
        GPIO_SetBits(LED_GPIO_PORT, led_pins[index]);//高电平点亮
    }
    else
    {
        GPIO_ResetBits(LED_GPIO_PORT, led_pins[index]);
    }
#else
    if (state == ENABLE)
    {
        GPIO_ResetBits(LED_GPIO_PORT, led_pins[index]);
    }
    else
    {
        GPIO_SetBits(LED_GPIO_PORT, led_pins[index]);
    }
#endif
}

void LED_WriteMask(uint8_t mask)
{
    uint16_t set_mask = 0U;//要点亮的 LED 掩码
    uint16_t clear_mask;//要熄灭的 LED 掩码
    uint8_t i;
    uint16_t all_pins = led_all_pins();

    for (i = 0U; i < LED_COUNT; ++i)
    {
        if ((mask & (1U << i)) != 0U)
        {
            set_mask |= led_pins[i];
        }
    }

    clear_mask = all_pins & (uint16_t)(~set_mask);

#if LED_ACTIVE_HIGH
    GPIO_SetBits(LED_GPIO_PORT, set_mask);
    GPIO_ResetBits(LED_GPIO_PORT, clear_mask);
#else
    GPIO_ResetBits(LED_GPIO_PORT, set_mask);
    GPIO_SetBits(LED_GPIO_PORT, clear_mask);
#endif
}

void LED_AllOff(void)
{
    LED_WriteMask(0U);
}
