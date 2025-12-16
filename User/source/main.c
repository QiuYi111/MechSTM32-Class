#include "adc.h"
#include "delay.h"
#include "experiment_lcd.h"
#include "key.h"
#include "led.h"
#include "stm32f10x.h"

// Select Experiment:
// 1: Geometric Shapes
// 2: Motor Control & Timer
#define EXPERIMENT 1

void System_Init(void) {
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Systick_Init();
  LED_GPIO_Init();
  Key_Init();
  // Monitor (ADC/Motor) drivers init inside experiment or here
  // LCD_Init is called inside experiment functions usually to ensure fresh
  // state
}

int main(void) {
  System_Init();

  // Loop
  while (1) {
#if EXPERIMENT == 1
    Exp_DrawShapes();
    // Stop here or loop? Shapes are static, so maybe just stop or blink LED
    while (1) {
      LED_Set(LED1, LED_ON);
      delay_ms(500);
      LED_Set(LED1, LED_OFF);
      delay_ms(500);
    }
#elif EXPERIMENT == 2
    Exp_MotorControl(); // Contains its own loop
#endif
  }
}
