#include "experiment_lcd.h"
#include "adc.h"
#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "motor.h"
#include <stdio.h>
#include <stdlib.h>

// Helper: Swap integers
void swap(u16 *a, u16 *b) {
  u16 t = *a;
  *a = *b;
  *b = t;
}

// Draw outline triangle
void DrawTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3, u16 color) {
  LCD_DrawLine(x1, y1, x2, y2, color);
  LCD_DrawLine(x2, y2, x3, y3, color);
  LCD_DrawLine(x3, y3, x1, y1, color);
}

// Simple scanline triangle fill (Reference implementation)
void FillTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3, u16 color) {
  u16 a, b, y;
  // Sort vertices by Y
  if (y1 > y2) {
    swap(&y1, &y2);
    swap(&x1, &x2);
  }
  if (y2 > y3) {
    swap(&y2, &y3);
    swap(&x2, &x3);
  }
  if (y1 > y2) {
    swap(&y1, &y2);
    swap(&x1, &x2);
  }

  if (y1 == y3)
    return; // Zero height

  // Upper part
  for (y = y1; y <= y2; y++) {
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1 + 1); // +1 to avoid div0
    b = x1 + (x3 - x1) * (y - y1) / (y3 - y1 + 1);
    if (a > b)
      swap(&a, &b);
    LCD_DrawLine(a, y, b, y, color);
  }
  // Lower part
  for (; y <= y3; y++) {
    a = x2 + (x3 - x2) * (y - y2) / (y3 - y2 + 1);
    b = x1 + (x3 - x1) * (y - y1) / (y3 - y1 + 1);
    if (a > b)
      swap(&a, &b);
    LCD_DrawLine(a, y, b, y, color);
  }
}

// Scanline Circle Fill
void FillCircle(u16 x0, u16 y0, u16 r, u16 color) {
  int x = 0, y = r;
  int d = 3 - 2 * r;
  while (x <= y) {
    LCD_DrawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
    LCD_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
    LCD_DrawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);
    LCD_DrawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
    if (d < 0)
      d = d + 4 * x + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Exp_DrawShapes(void) {
  LCD_Init();
  LCD_Clear(WHITE);
  POINT_COLOR = RED;

  LCD_ShowString(10, 10, 300, 24, 24, (u8 *)"Experiment 1: Shapes (Grid)", RED);

  // --- Row 1: Triangles ---
  // Outline
  DrawTriangle(50, 120, 100, 20, 150, 120, BLUE);
  // Filled
  FillTriangle(300, 120, 350, 20, 400, 120, RED);

  LCD_ShowString(50, 130, 100, 16, 16, (u8 *)"Triangles", BLACK);

  // --- Row 2: Rectangles ---
  // Outline
  LCD_DrawRectangle(50, 160, 150, 260, BLUE);
  // Filled
  LCD_Fill(300, 160, 400, 260, GREEN);

  LCD_ShowString(50, 270, 100, 16, 16, (u8 *)"Rectangles", BLACK);

  // --- Row 3: Circles ---
  // Outline
  LCD_DrawCircle(100, 350, 50, BLUE);
  // Filled
  FillCircle(350, 350, 50, ORANGE);

  LCD_ShowString(50, 410, 100, 16, 16, (u8 *)"Circles", BLACK);
}

void Exp_MotorControl(void) {
  u16 speed = 0;
  u8 dir = 0; // 0: FWD, 1: REV
  float voltage = 0.0f;
  char buf[30];
  u32 time_sec = 0;
  u32 tick_count = 0;

  LCD_Init();
  Motor_Init();
  ADC_Config(); // Assuming init needed
  // Assuming Key_Init() and ADC_Config() called in System_Init of main, but
  // safe to call here if needed? main.c System_Init usually calls them.

  LCD_Clear(BLACK);
  POINT_COLOR = WHITE;
  BACK_COLOR = BLACK;

  LCD_ShowString(10, 10, 300, 24, 24, (u8 *)"Exp 2: Motor & Timer", WHITE);
  LCD_ShowString(10, 40, 300, 16, 16, (u8 *)"Member: Qiu Jingyi",
                 WHITE); // Example

  while (1) {
    // 1. Timer Logic (Approximate 1 sec)
    delay_ms(100); // 100ms update rate
    tick_count++;
    if (tick_count >= 10) {
      tick_count = 0;
      time_sec++;
      if (time_sec >= 3600)
        time_sec = 0; // Wrap at 1 hour
    }

    // Display Timer
    sprintf(buf, "Time: %02d:%02d", time_sec / 60, time_sec % 60);
    LCD_ShowString(10, 70, 200, 24, 24, (u8 *)buf, YELLOW);

    // 2. Motor Control
    voltage = ADC_ReadVoltage(ADC_CHANNEL_1); // PA1
    speed = (u16)(voltage / 3.3f * 100.0f);
    if (speed > 100)
      speed = 100;

    Motor_SetSpeed(speed);

    // Direction (Toggle with Key if available, or just fixed)
    // Guide says "Status Display... Forward/Reverse", doesn't explicitly ask
    // for Key control in Exp 2 description summary but Exp 3 in PROJECT_INDEX
    // had keys. I'll read keys if available. Assuming Key_ReadMask() is
    // available.
    /*
    if (Key_ReadMask() & KEY_S1_MASK) dir = 0;
    if (Key_ReadMask() & KEY_S2_MASK) dir = 1;
    */
    // For simplicity, just display Forward unless logic changes
    Motor_SetDir(dir);

    // Display Status
    sprintf(buf, "Speed: %3d %%", speed);
    LCD_ShowString(10, 110, 200, 24, 24, (u8 *)buf, GREEN);

    if (dir == 0)
      LCD_ShowString(10, 140, 200, 24, 24, (u8 *)"Dir: Forward ", CYAN);
    else
      LCD_ShowString(10, 140, 200, 24, 24, (u8 *)"Dir: Reverse ", CYAN);
  }
}
