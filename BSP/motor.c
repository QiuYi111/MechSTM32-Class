
#include "motor.h"

// Motor PWM FWD: PF9
// Motor PWM REV: PF10
// Motor Speed Feedbk: PF11 (EXTI11)

static volatile uint32_t pulse_count = 0;
static uint16_t current_rpm = 0;
static uint8_t current_dir = 0;             // 0: FWD, 1: REV
static volatile uint16_t current_speed = 0; // 0-100
static volatile uint16_t soft_pwm_cnt = 0;

// Filter for encoder crosstalk
static volatile uint32_t tick_100us = 0;
static volatile uint32_t last_pulse_tick = 0;
#define ENCODER_BLANKING_TICKS 5 // 500us (PWM noise is 100us)

void Motor_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // 1. Clock Enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // 2. GPIO Config: PF9, PF10 as Out_PP for Soft PWM
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);

  // 3. GPIO Config: PF11 (Speed Feedback) as IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  // 4. TIM3 Config (Software PWM Timer)
  // 72MHz / 72 = 1MHz clock -> 100 ticks = 100us (10kHz interrupt)
  TIM_TimeBaseStructure.TIM_Period = 99;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM3, ENABLE);

  // 5. EXTI Config for Speed Feedback (PF11)
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource11);
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Motor_SetSpeed(uint16_t speed) {
  if (speed > 100)
    speed = 100;
  current_speed = speed;
}

void Motor_SetDir(uint8_t dir) { current_dir = dir; }

uint16_t Motor_GetSpeedRPM(void) { return current_rpm; }

// Call this every 100ms (now updated from experiment_lcd.c)
void Motor_UpdateStats(void) {
  // RPM calculation over 100ms:
  // pulses_per_sec = pulse_count * 10
  // RPM = (pulses_per_sec * 60) / 40 = pulse_count * 15
  uint16_t new_rpm = (uint16_t)(pulse_count * 15);
  pulse_count = 0;

  // Simple low-pass filter: 70% old, 30% new
  current_rpm = (uint16_t)((current_rpm * 7 + new_rpm * 3) / 10);
}

// 10kHz Interrupt for Software PWM & Timing
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    tick_100us++; // Used for encoder filtering

    soft_pwm_cnt++;
    if (soft_pwm_cnt >= 100)
      soft_pwm_cnt = 0;

    if (soft_pwm_cnt < current_speed) {
      if (current_dir == 0) { // Forward
        GPIO_SetBits(GPIOF, GPIO_Pin_9);
        GPIO_ResetBits(GPIOF, GPIO_Pin_10);
      } else { // Reverse
        GPIO_ResetBits(GPIOF, GPIO_Pin_9);
        GPIO_SetBits(GPIOF, GPIO_Pin_10);
      }
    } else {
      GPIO_ResetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);
    }

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

void EXTI15_10_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
    // Software Filter: ignore pulses that are too close (noise from 10kHz PWM)
    uint32_t now = tick_100us;
    if ((now - last_pulse_tick) >= ENCODER_BLANKING_TICKS) {
      pulse_count++;
      last_pulse_tick = now;
    }
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
}
