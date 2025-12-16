#include "delay.h"
#include "key.h"

#define KEY_ACTIVE_STATE Bit_RESET
#define KEY_DEBOUNCE_SAMPLE_COUNT 5U
#define KEY_DEBOUNCE_INTERVAL_US 1000U

//描述按键硬件信息的结构体
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} key_hw_t;

//定义按键
static const key_hw_t key_hw[KEY_COUNT] = {
    {GPIOA, GPIO_Pin_0},  /* S1 */
    {GPIOA, GPIO_Pin_8},  /* S2 */
    {GPIOC, GPIO_Pin_9},  /* S3 */
    {GPIOC, GPIO_Pin_13}  /* S4 */
};

static uint8_t key_read_raw_state(key_id_t key);
static uint8_t key_read_debounced_state(key_id_t key);

//
void Key_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    uint8_t i;
    uint16_t gpioa_pins = 0U;
    uint16_t gpioc_pins = 0U;

    //开启 GPIOA、GPIOC、AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    for (i = 0U; i < KEY_COUNT; ++i)
    {
        if (key_hw[i].port == GPIOA)
        {
            gpioa_pins |= key_hw[i].pin; //取出 GPIOA 的按键引脚
        }
        else if (key_hw[i].port == GPIOC)
        {
            gpioc_pins |= key_hw[i].pin;//取出 GPIOC 的按键引脚
        }
    }

    gpio_init.GPIO_Mode = GPIO_Mode_IPU; //按下为低电平
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz; //引脚速度 2MHz

    //分别初始化 GPIOA 和 GPIOC 的按键引脚
    if (gpioa_pins != 0U)
    {
        gpio_init.GPIO_Pin = gpioa_pins;
        GPIO_Init(GPIOA, &gpio_init);
    }

    if (gpioc_pins != 0U)
    {
        gpio_init.GPIO_Pin = gpioc_pins;
        GPIO_Init(GPIOC, &gpio_init);
    }
}

//判断按键是否按下
uint8_t Key_IsPressed(key_id_t key)
{
    if (key >= KEY_COUNT) //越界，直接返回未按下
    {
        return 0U;
    }

    return key_read_debounced_state(key);
}

//读取所有按键状态，返回按键掩码。例如，返回 0b00001001 表示按键 S1 和 S4 被按下。

uint8_t Key_ReadMask(void)
{
    uint8_t mask = 0U;
    uint8_t i;

    for (i = 0U; i < KEY_COUNT; ++i)
    {
        if (key_read_debounced_state((key_id_t)i) != 0U)
        {
            mask |= (1U << i);
        }
    }

    return mask;
}

static uint8_t key_read_raw_state(key_id_t key)
{
    if (GPIO_ReadInputDataBit(key_hw[key].port, key_hw[key].pin) == KEY_ACTIVE_STATE)
    {
        return 1U;
    }

    return 0U;
}

//多次采样按键状态，只有在大多数采样为按下时才判定为按下
static uint8_t key_read_debounced_state(key_id_t key)
{
    uint8_t i;
    uint8_t pressed_samples = 0U;

    for (i = 0U; i < KEY_DEBOUNCE_SAMPLE_COUNT; ++i)
    {
        pressed_samples += key_read_raw_state(key);

        if ((i + 1U) < KEY_DEBOUNCE_SAMPLE_COUNT)
        {
            delay_us(KEY_DEBOUNCE_INTERVAL_US);
        }
    }

    if (pressed_samples >= ((KEY_DEBOUNCE_SAMPLE_COUNT / 2U) + 1U))
    {
        return 1U;
    }

    return 0U;
}
