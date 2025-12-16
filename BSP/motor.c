#include "motor.h"

// Motor PWM: PA6 (TIM3_CH1)
// Motor Dir: PA7

void Motor_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // GPIO Config: PA6 (PWM Output), PA7 (Direction)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // TIM3 Config
  TIM_TimeBaseStructure.TIM_Period = 99; // 100 steps
  TIM_TimeBaseStructure.TIM_Prescaler =
      719; // 72MHz / 720 = 100kHz clock -> 1kHz PWM
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  // PWM Config (CH1)
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_Cmd(TIM3, ENABLE);
}

void Motor_SetSpeed(uint16_t speed) {
  if (speed > 100)
    speed = 100;
  TIM_SetCompare1(TIM3, speed);
}

void Motor_SetDir(uint8_t dir) {
  if (dir) {
    GPIO_SetBits(GPIOA, GPIO_Pin_7);
  } else {
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
  }
}
