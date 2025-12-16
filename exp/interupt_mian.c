#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "delay.h"
#include "LED.h"
#include "key.h"

typedef enum
{
    EXPERIMENT_INTERRUPT_TOGGLE = 1,
    EXPERIMENT_STOPWATCH_BINARY,
    EXPERIMENT_VARIABLE_BLINK,
    EXPERIMENT_PWM_BRIGHTNESS,
    EXPERIMENT_BREATHING_LIGHT
} Experiment_Mode;

#define ACTIVE_EXPERIMENT EXPERIMENT_VARIABLE_BLINK
#define VARIABLE_BLINK_DEFAULT_INTERVAL_MS (2000U)
#define VARIABLE_BLINK_FAST_INTERVAL_MS    (1000U)
#define VARIABLE_BLINK_SLOW_INTERVAL_MS    (4000U)
#define VARIABLE_BLINK_TIMER              TIM3
#define VARIABLE_BLINK_TIMER_CLK          RCC_APB1Periph_TIM3
#define VARIABLE_BLINK_TIMER_IRQ          TIM3_IRQn

volatile uint8_t g_active_experiment = ACTIVE_EXPERIMENT;

static volatile uint32_t variable_blink_interval_ms = VARIABLE_BLINK_DEFAULT_INTERVAL_MS;
static volatile uint32_t variable_blink_elapsed_ms = 0U;
static volatile uint8_t variable_blink_led_state = 0U;

static void experiment_interrupt_toggle(void);
static void experiment_stopwatch_binary(void);
static void experiment_variable_blink(void);
static void experiment_pwm_brightness(void);
static void experiment_breathing_light(void);
static void variable_blink_timer_init(void);
static void variable_blink_set_interval(uint32_t interval_ms);
static void variable_blink_handle_key_edge(Key_Id key);

void VariableBlink_HandleKeyInterrupt(Key_Id key);
void VariableBlink_TimerTick(void);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Systick_Init();
    LED_GPIO_Init();

    switch (ACTIVE_EXPERIMENT)
    {
        case EXPERIMENT_INTERRUPT_TOGGLE:
            experiment_interrupt_toggle();
            break;
        case EXPERIMENT_STOPWATCH_BINARY:
            experiment_stopwatch_binary();
            break;
        case EXPERIMENT_VARIABLE_BLINK:
            experiment_variable_blink();
            break;
        case EXPERIMENT_PWM_BRIGHTNESS:
            experiment_pwm_brightness();
            break;
        case EXPERIMENT_BREATHING_LIGHT:
        default:
            experiment_breathing_light();
            break;
    }

    while (1)
    {
    }
}

static void experiment_interrupt_toggle(void)
{
    g_active_experiment = EXPERIMENT_INTERRUPT_TOGGLE;
    uint8_t led_mask = 0U;
    uint64_t cnt;
    Key_Init(KEY_MODE_EXTI);
    LED_WriteMask(led_mask);

    while (1)
    {
        if (Key_GetEvent(KEY_ID_S1))
        {
            for in (cnt = 0U; cnt < 1000U)
            {
               if (Key_IsPressed(KEY_ID_S1) == 0U)
               {
                   break;
               }
               else
               {
                   cnt++;
               }
            }
            if (cnt >= 600U)
            {
            led_mask ^= LED_ALL_MASK;
            LED_WriteMask(led_mask);
            }
            
        }
    }
}

static void experiment_stopwatch_binary(void)
{
    g_active_experiment = EXPERIMENT_STOPWATCH_BINARY;
    uint8_t seconds = 0U;

    Key_Init(KEY_MODE_GPIO);
    LED_WriteMask(0U);

    while (1)
    {
        LED_WriteMask(seconds & LED_ALL_MASK);
        delay_ms(1000U);
        seconds = (uint8_t)((seconds + 1U) & LED_ALL_MASK);
    }
}

static void experiment_variable_blink(void)
{
    g_active_experiment = EXPERIMENT_VARIABLE_BLINK;
    variable_blink_set_interval(VARIABLE_BLINK_DEFAULT_INTERVAL_MS);

    Key_Init(KEY_MODE_EXTI);
    LED_WriteMask(0U);
    variable_blink_led_state = 0U;
    variable_blink_timer_init();

    while (1)
    {
    }
}

static void experiment_pwm_brightness(void)
{
    g_active_experiment = EXPERIMENT_PWM_BRIGHTNESS;

    Key_Init(KEY_MODE_GPIO);
    LED_PWM_Init();

    while (1)
    {
    }
}

static void experiment_breathing_light(void)
{
    g_active_experiment = EXPERIMENT_BREATHING_LIGHT;
    uint8_t duty = 0U;
    uint8_t increasing = 1U;

    Key_Init(KEY_MODE_GPIO);
    LED_PWM_Init();

    while (1)
    {
        LED_PWM_SetDuty(duty);
        delay_ms(20U);

        if (increasing != 0U)
        {
            if (duty < LED_PWM_RESOLUTION)
            {
                duty++;
            }
            else
            {
                increasing = 0U;
                if (duty > 0U)
                {
                    duty--;
                }
            }
        }
        else
        {
            if (duty > 0U)
            {
                duty--;
            }
            else
            {
                increasing = 1U;
                if (duty < LED_PWM_RESOLUTION)
                {
                    duty++;
                }
            }
        }
    }
}

void VariableBlink_HandleKeyInterrupt(Key_Id key)
{
    if (g_active_experiment != EXPERIMENT_VARIABLE_BLINK)
    {
        return;
    }

    variable_blink_handle_key_edge(key);
}

void VariableBlink_TimerTick(void)
{
    static uint16_t debug_led2_divider = 0U;

    if (g_active_experiment != EXPERIMENT_VARIABLE_BLINK)
    {
        return;
    }

    variable_blink_elapsed_ms++;
    if (variable_blink_elapsed_ms >= variable_blink_interval_ms)
    {
        variable_blink_elapsed_ms = 0U;
        variable_blink_led_state ^= 1U;
        LED_Set(LED_INDEX_1, variable_blink_led_state ? ENABLE : DISABLE);
    }

    /* Debug heartbeat: toggle LED2 about every 250 ms to confirm TIM3 is firing. 
    debug_led2_divider++;
    if (debug_led2_divider >= 250U)
    {
        debug_led2_divider = 0U;
        LED_Toggle(LED_INDEX_2);
    }*/
}

static void variable_blink_timer_init(void)
{
    TIM_TimeBaseInitTypeDef tim_init;
    NVIC_InitTypeDef nvic_init;

    RCC_APB1PeriphClockCmd(VARIABLE_BLINK_TIMER_CLK, ENABLE);

    variable_blink_elapsed_ms = 0U;
    variable_blink_led_state = 0U;

    TIM_TimeBaseStructInit(&tim_init);
    tim_init.TIM_Period = 9U;
    tim_init.TIM_Prescaler = 7199U;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(VARIABLE_BLINK_TIMER, &tim_init);

    TIM_ClearFlag(VARIABLE_BLINK_TIMER, TIM_FLAG_Update);
    TIM_ITConfig(VARIABLE_BLINK_TIMER, TIM_IT_Update, ENABLE);

    nvic_init.NVIC_IRQChannel = VARIABLE_BLINK_TIMER_IRQ;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    TIM_Cmd(VARIABLE_BLINK_TIMER, ENABLE);
}

static void variable_blink_set_interval(uint32_t interval_ms)
{
    if (variable_blink_interval_ms != interval_ms)
    {
        variable_blink_interval_ms = interval_ms;
        variable_blink_elapsed_ms = 0U;
    }
}

static void variable_blink_handle_key_edge(Key_Id key)
{
    if ((key == KEY_ID_S1) && (Key_IsPressed(KEY_ID_S1) != 0U))
    {
        variable_blink_set_interval(VARIABLE_BLINK_FAST_INTERVAL_MS);
    }
    else if ((key == KEY_ID_S2) && (Key_IsPressed(KEY_ID_S2) != 0U))
    {
        variable_blink_set_interval(VARIABLE_BLINK_SLOW_INTERVAL_MS);
    }
}
