#include "motor.h"

// Motor PWM FWD: PF9
// Motor PWM REV: PF10
// Motor Speed Feedbk: PF11

static volatile uint32_t pulse_count = 0;
static uint16_t current_rpm = 0;
static uint8_t current_dir = 0;             // 0: FWD, 1: REV
static volatile uint16_t current_speed = 0; // 0-100
static volatile uint16_t soft_pwm_cnt = 0;

// Filter for encoder crosstalk
static uint8_t sample_history = 0xFF;
static uint8_t last_stable_state = 1;

void Motor_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
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

  // 3. GPIO Config: PF11 (Speed Feedback) as IPU (No EXTI needed)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  // 4. TIM3 Config (Sampling & PWM Timer)
  // 72MHz / 72 = 1MHz clock -> 25 ticks = 25us (40kHz interrupt)
  // 40kHz / 20 steps = 2kHz PWM frequency
  TIM_TimeBaseStructure.TIM_Period = 24;
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
}

void Motor_SetSpeed(uint16_t speed) {
  if (speed > 100)
    speed = 100;
  current_speed = speed;
}

void Motor_SetDir(uint8_t dir) { current_dir = dir; }

uint16_t Motor_GetSpeedRPM(void) { return current_rpm; }

// Call this every 100ms from main loop
void Motor_UpdateStats(void) {
  // RPM calculation over 100ms
  // pulses_per_sec = pulse_count * 10
  // RPM = (pulses_per_sec * 60) / 40 = pulse_count * 15
  uint16_t new_rpm = (uint16_t)(pulse_count * 15);
  pulse_count = 0;

  // Simple low-pass filter: 70% old, 30% new
  current_rpm = (uint16_t)((current_rpm * 7 + new_rpm * 3) / 10);
}

// 40kHz Interrupt for Filtering & Software PWM
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

    // 1. Encoder Sampling and Majority Voting (Top of IRQ to avoid PWM
    // transient)
    sample_history = (uint8_t)((sample_history << 1) |
                               GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11));

    // Check for stable 0/1 over last 4 samples (100us stability)
    if ((sample_history & 0x0F) == 0x00) { // 4 consecutive 0s
      if (last_stable_state == 1) {        // Falling edge pulse detected
        pulse_count++;
        last_stable_state = 0;
      }
    } else if ((sample_history & 0x0F) == 0x0F) { // 4 consecutive 1s
      last_stable_state = 1;
    }

    // 2. Software PWM (2kHz Frequency, 20 steps resolution)
    soft_pwm_cnt++;
    if (soft_pwm_cnt >= 20)
      soft_pwm_cnt = 0;

    // current_speed (0-100) -> threshold (0-20)
    uint16_t threshold = (uint16_t)(current_speed / 5);

    if (soft_pwm_cnt < threshold) {
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
