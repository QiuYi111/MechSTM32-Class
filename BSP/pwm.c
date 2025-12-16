#include "pwm.h"
#include "led.h"

static uint8_t currentDutyCycle = 0;
static uint32_t pwmCounter = 0;

void PWM_Init(void)
{
    /* 软件PWM不需要额外初始化，LED_GPIO_Init已处理 */
    currentDutyCycle = 0;
    pwmCounter = 0;
}

void PWM_SetDutyCycle(uint8_t percent)
{
    /* 限制占空比范围 0-100% */
    if (percent > 100) percent = 0;

    currentDutyCycle = percent;
}

/* 软件PWM更新函数 - 需要在定时器中断中调用 */
void PWM_Update(void)
{
    pwmCounter++;

    if (pwmCounter >= 100) {
        pwmCounter = 0;
    }

    /* 根据占空比控制LED */
    if (pwmCounter < currentDutyCycle) {
        /* PWM周期内，LED打开 */
        LED_WriteMask(0x0F);  /* 所有LED打开 */
    } else {
        /* PWM周期内，LED关闭 */
        LED_WriteMask(0x00);  /* 所有LED关闭 */
    }
}