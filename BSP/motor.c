#include "motor.h"

// Motor PWM FWD: PF9 (TIM14_CH1)
// Motor PWM REV: PF10 (TIM12_CH1)
// Motor Speed Feedbk: PF11 (EXTI11)

static volatile uint32_t pulse_count = 0;
static uint16_t current_rpm = 0;
static uint8_t current_dir = 0; // 0: FWD, 1: REV

void Motor_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // 1. Clock Enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14 | RCC_APB1Periph_TIM12, ENABLE);

  // 2. GPIO Config: PF9 (TIM14_CH1), PF10 (TIM12_CH1) as AF_PP
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  // 3. GPIO Config: PF11 (Speed Feedback) as IPU
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  // 4. TIM14 & TIM12 Config (PWM)
  // 72MHz / 72 = 1MHz clock -> 1000 steps = 1kHz PWM
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

  // PWM Config (CH1 for both)
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM14, &TIM_OCInitStructure);
  TIM_OC1Init(TIM12, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);

  TIM_Cmd(TIM14, ENABLE);
  TIM_Cmd(TIM12, ENABLE);

  // 5. EXTI Config for Speed Feedback (PF11)
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource11);
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger =
      EXTI_Trigger_Falling; // Assuming active low pulse
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
  uint16_t pulse = (uint16_t)(speed * 10); // 100% -> 1000 pulse

  if (current_dir == 0) { // FWD
    TIM_SetCompare1(TIM14, pulse);
    TIM_SetCompare1(TIM12, 0);
  } else { // REV
    TIM_SetCompare1(TIM14, 0);
    TIM_SetCompare1(TIM12, pulse);
  }
}

void Motor_SetDir(uint8_t dir) {
  current_dir = dir;
  // Re-apply speed to switch pins
  Motor_SetSpeed(0); // Optional: stop briefly if needed, or just switch
}

uint16_t Motor_GetSpeedRPM(void) { return current_rpm; }

// Call this every 1 second (e.g. from main loop or timer)
void Motor_UpdateStats(void) {
  // Simple RPM calculation: (pulses per second * 60) / pulses_per_rev
  // Assuming 20 pulses per revolution for a typical small DC motor encoder
  current_rpm = (pulse_count * 60) / 20;
  pulse_count = 0;
}

void EXTI15_10_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
    pulse_count++;
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
}
