#include "LCD.h"
#include "delay.h"
#include "font.h"

//  LCD的画笔颜色和背景色
u16 POINT_COLOR = 0x0000; //  画笔颜色
u16 BACK_COLOR = 0xFFFF;  //  背景色

//  管理LCD重要参数
//  默认为横屏
_lcd_dev lcddev;

//  写寄存器函数
//  regval:寄存器值
void LCD_WR_REG(u16 regval) {
  LCD->LCD_REG = regval; //  写入要写的寄存器序号
}

//  写LCD数据
//  data:要写入的值
void LCD_WR_DATA(u16 data) { LCD->LCD_RAM = data; }

//  读LCD数据
//  返回值:读到的值
u16 LCD_RD_DATA(void) {
  vu16 ram; //  防止被优化

  ram = LCD->LCD_RAM;

  return ram;
}

//  写寄存器
//  LCD_Reg:寄存器地址
//  LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue) {
  LCD->LCD_REG = LCD_Reg;      //  写入要写的寄存器序号
  LCD->LCD_RAM = LCD_RegValue; //  写入数据
}

//  读寄存器
//  LCD_Reg:寄存器地址
//  返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg) {
  LCD_WR_REG(LCD_Reg); //  写入要读的寄存器序号
  delay_us(5);

  return LCD_RD_DATA(); //  返回读到的值
}

//  开始写GRAM
void LCD_WriteRAM_Prepare(void) { LCD->LCD_REG = lcddev.wramcmd; }

//  LCD写GRAM
//  RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code) {
  LCD->LCD_RAM = RGB_Code; //  写十六位GRAM
}

//  从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//  通过该函数转换
//  c:GBR格式的颜色值
//  返回值：RGB格式的颜色值

//  设置光标位置
//  Xpos:横坐标
//  Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos) {
  if (lcddev.id == 0X1963) {
    if (lcddev.dir == 0) //  x坐标需要变换
    {
      Xpos = lcddev.width - 1 - Xpos;
      LCD_WR_REG(lcddev.setxcmd);
      LCD_WR_DATA(0);
      LCD_WR_DATA(0);
      LCD_WR_DATA(Xpos >> 8);
      LCD_WR_DATA(Xpos & 0XFF);
    } else {
      LCD_WR_REG(lcddev.setxcmd);
      LCD_WR_DATA(Xpos >> 8);
      LCD_WR_DATA(Xpos & 0XFF);
      LCD_WR_DATA((lcddev.width - 1) >> 8);
      LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    }
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
  }
}
//  设置LCD的自动扫描方向
//  注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//  所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//  dir:0~7,代表8个方向(具体定义见lcd.h)
//  9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
void LCD_Scan_Dir(u8 dir) {
  u16 regval = 0;
  u16 dirreg = 0;
  u16 temp;

  if (lcddev.dir == 0) // 横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
  {
    switch (dir) //  方向转换
    {
    case 0:
      dir = 6;
      break;

    case 1:
      dir = 7;
      break;

    case 2:
      dir = 4;
      break;

    case 3:
      dir = 5;
      break;

    case 4:
      dir = 1;
      break;

    case 5:
      dir = 0;
      break;

    case 6:
      dir = 3;
      break;

    case 7:
      dir = 2;
      break;
    }
  }
  if (lcddev.id == 0X1963) // 9341/6804/5310/5510/1963,特殊处理
  {
    switch (dir) {
    case L2R_U2D: //  从左到右,从上到下
      regval |= (0 << 7) | (0 << 6) | (0 << 5);
      break;

    case L2R_D2U: //  从左到右,从下到上
      regval |= (1 << 7) | (0 << 6) | (0 << 5);
      break;

    case R2L_U2D: //  从右到左,从上到下
      regval |= (0 << 7) | (1 << 6) | (0 << 5);
      break;

    case R2L_D2U: //  从右到左,从下到上
      regval |= (1 << 7) | (1 << 6) | (0 << 5);
      break;

    case U2D_L2R: //  从上到下,从左到右
      regval |= (0 << 7) | (0 << 6) | (1 << 5);
      break;

    case U2D_R2L: //  从上到下,从右到左
      regval |= (0 << 7) | (1 << 6) | (1 << 5);
      break;

    case D2U_L2R: //  从下到上,从左到右
      regval |= (1 << 7) | (0 << 6) | (1 << 5);
      break;

    case D2U_R2L: //  从下到上,从右到左
      regval |= (1 << 7) | (1 << 6) | (1 << 5);
      break;
    }

    LCD_WriteReg(dirreg, regval);

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
  }
}

//  设置LCD显示方向
//  dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir) {
  if (dir == 0) //  竖屏
  {
    lcddev.dir = 0; //  竖屏
    lcddev.width = 240;
    lcddev.height = 320;
    if (lcddev.id == 0X1963) {
      lcddev.wramcmd = 0x2C; //  设置写入GRAM的指令
      lcddev.setxcmd = 0x2A; //  设置写X坐标指令
      lcddev.setycmd = 0x2B; //  设置写Y坐标指令
      lcddev.width = 480;    //  设置宽度480
      lcddev.height = 800;   //  设置高度800
    }

  } else //  横屏
  {
    lcddev.dir = 1; //  横屏
    lcddev.width = 320;
    lcddev.height = 240;
    if (lcddev.id == 0X1963) {
      lcddev.wramcmd = 0x2C; //  设置写入GRAM的指令
      lcddev.setxcmd = 0x2A; //  设置写X坐标指令
      lcddev.setycmd = 0x2B; //  设置写Y坐标指令
      lcddev.width = 800;    //  设置宽度800
      lcddev.height = 480;   //  设置高度480
    }
  }
  LCD_Scan_Dir(DFT_SCAN_DIR); //  默认扫描方向
}

void LCD_Clear(u16 color) {
  u32 index = 0;
  u32 totalpoint = lcddev.width;
  totalpoint *= lcddev.height; // 得到总点数
  LCD_SetCursor(0x00, 0x0000); // 设置光标位置
  LCD_WriteRAM_Prepare();      // 开始写入GRAM
  for (index = 0; index < totalpoint; index++) {
    LCD->LCD_RAM = color;
  }
}

void LCD_DrawPoint(u16 x, u16 y, u16 color) {
  LCD_SetCursor(x, y);    // 设置光标位置
  LCD_WriteRAM_Prepare(); // 开始写入GRAM
  LCD->LCD_RAM = color;
}

void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color) {
  u16 i, j;
  u16 xlen = 0;
  xlen = ex - sx + 1;
  for (i = sy; i <= ey; i++) {
    LCD_SetCursor(sx, i);   // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    for (j = 0; j < xlen; j++)
      LCD->LCD_RAM = color; // 显示颜色
  }
}

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color) {
  u16 t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; // 计算坐标增量
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0)
    incx = 1; // 设置单步方向
  else if (delta_x == 0)
    incx = 0; // 垂直线
  else {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0; // 水平线
  else {
    incy = -1;
    delta_y = -delta_y;
  }
  if (delta_x > delta_y)
    distance = delta_x; // 选取基本增量坐标轴
  else
    distance = delta_y;
  for (t = 0; t <= distance + 1; t++) // 画线输出
  {
    LCD_DrawPoint(uRow, uCol, color); // 画点
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      uCol += incy;
    }
  }
}

void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 color, u16 mode) {
  u8 temp, t1, t;
  u16 y0 = y;
  u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) *
             (size / 2); // 得到字体一个字符对应点阵集所占的字节数
  num = num - ' ';       // 得到偏移后的值
  for (t = 0; t < csize; t++) {
    if (size == 12)
      temp = asc2_1206[num][t]; // 调用1206字体
    else if (size == 16)
      temp = asc2_1608[num][t]; // 调用1608字体
    else if (size == 24)
      temp = asc2_2412[num][t]; // 调用2412字体
    else
      return; // 没有的字库
    for (t1 = 0; t1 < 8; t1++) {
      if (temp & 0x80)
        LCD_DrawPoint(x, y, color);
      else if (mode == 0)
        LCD_DrawPoint(x, y, BACK_COLOR);
      temp <<= 1;
      y++;
      if (y >= lcddev.height)
        return; // 超区域了
      if ((y - y0) == size) {
        y = y0;
        x++;
        if (x >= lcddev.width)
          return; // 超区域了
        break;
      }
    }
  }
}

void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p,
                    u16 color) {
  u8 x0 = x;
  width += x;
  height += y;
  while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
  {
    if (x >= width) {
      x = x0;
      y += size;
    }
    if (y >= height)
      break; // 退出
    LCD_ShowChar(x, y, *p, size, color, 0);
    x += size / 2;
    p++;
  }
}

//  初始化lcd
//  该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//  在其他型号的驱动芯片上没有测试!
void LCD_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef readWriteTiming;
  FSMC_NORSRAMTimingInitTypeDef writeTiming;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE); //  使能FSMC时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
                             RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG,
                         ENABLE); // 使能PORTB,D,E,G以及AFIO复用功能时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; //  PE3 推挽输出 背光
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //  推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  LCD_RESET_L;
  delay_ms(500);
  LCD_RESET_H;

  //	PORTD复用推挽输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |
                                GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 |
                                GPIO_Pin_15;      //	//PORTD复用推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //	复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //	PORTE复用推挽输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15;      //	//PORTD复用推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //	复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  //	PORTG12复用推挽输出 A0
  GPIO_InitStructure.GPIO_Pin =
      GPIO_Pin_0 | GPIO_Pin_12;                   //	PORTD复用推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //	复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  readWriteTiming.FSMC_AddressSetupTime =
      0x01; //	地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime =
      0x00; //	地址保持时间（ADDHLD）模式A未用到
  readWriteTiming.FSMC_DataSetupTime =
      0x0f; // 	数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A; //	模式A

  writeTiming.FSMC_AddressSetupTime = 0x00; //	地址建立时间（ADDSET）为1个HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;  //	地址保持时间（A
  writeTiming.FSMC_DataSetupTime = 0x03;    //	数据保存时间为4个HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A; //	模式A

  FSMC_NORSRAMInitStructure.FSMC_Bank =
      FSMC_Bank1_NORSRAM4; //  这里我们使用NE4 ，也就对应BTCR[6],[7]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux =
      FSMC_DataAddressMux_Disable; // 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =
      FSMC_MemoryType_SRAM; // 	FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth =
      FSMC_MemoryDataWidth_16b; // 存储器数据宽度为16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =
      FSMC_BurstAccessMode_Disable; // FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity =
      FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait =
      FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive =
      FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation =
      FSMC_WriteOperation_Enable; //  存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode =
      FSMC_ExtendedMode_Enable; // 读写使用不同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct =
      &readWriteTiming;                                            // 读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming; //	写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //	初始化FSMC配置

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); // 	使能BANK1

  delay_ms(100); // 	delay 50 ms

  LCD_WR_REG(0XA1);
  lcddev.id = LCD_RD_DATA();
  lcddev.id = LCD_RD_DATA(); //	读回0X57
  lcddev.id <<= 8;
  lcddev.id |= LCD_RD_DATA(); //	读回0X61
  if (lcddev.id == 0X5761) {
    lcddev.id =
        0X1963; //	SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
  }

  if (lcddev.id == 0X1963) {
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0);
    delay_us(10);

    //  Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO
    //  < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x1D); //  参数1
    LCD_WR_DATA(0x02); //  参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WR_DATA(0x04); //  参数3 Validate M and N values
    delay_us(100);

    LCD_WR_REG(0xE0);  //  Start PLL command
    LCD_WR_DATA(0x01); //  enable PLL
    delay_ms(10);

    LCD_WR_REG(0xE0);  //  Start PLL command again
    LCD_WR_DATA(0x03); //  now, use PLL output as system clock
    delay_ms(12);

    LCD_WR_REG(0x01); //  软复位
    delay_ms(10);

    LCD_WR_REG(0xE6); //  设置像素频率,33Mhz
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x16);

    LCD_WR_REG(0xB0);  //  设置LCD模式
    LCD_WR_DATA(0x20); //  24位模式
    LCD_WR_DATA(0x00); //  TFT 模式

    LCD_WR_DATA((SSD_HOR_RESOLUTION) >> 8); //  设置LCD水平像素
    LCD_WR_DATA(SSD_HOR_RESOLUTION);
    LCD_WR_DATA((SSD_VER_RESOLUTION) >> 8); //  设置LCD垂直像素
    LCD_WR_DATA(SSD_VER_RESOLUTION);
    LCD_WR_DATA(0x00); //  RGB序列

    LCD_WR_REG(0xB4); //  Set horizontal period
    LCD_WR_DATA((SSD_HT) >> 8);
    LCD_WR_DATA(SSD_HT);
    LCD_WR_DATA(SSD_HPS >> 8);
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xB6); //  Set vertical period
    LCD_WR_DATA((SSD_VT) >> 8);
    LCD_WR_DATA(SSD_VT);
    LCD_WR_DATA(SSD_VPS >> 8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_PULSE_WIDTH);
    LCD_WR_DATA((SSD_VER_FRONT_PORCH >> 8) & 0x00FF);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH & 0x00FF);

    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0X05); //  GPIO[1:0]=01,控制LCD方向

    LCD_WR_REG(0xB8);  //  设置GPIO配置
    LCD_WR_DATA(0x07); //  2个IO口设置成输出
    LCD_WR_DATA(0x01); //  GPIO使用正常的IO功能

    // LCD_WR_REG(0x36); //rotation
    // LCD_WR_DATA(0x00);
    // delay_ms(50);

    LCD_WR_REG(0xF0);  //  设置SSD1963与CPU接口为16bit
    LCD_WR_DATA(0x03); //  16-bit(565 format) data for 16bpp

    LCD_WR_REG(0x29); //  开启显示

    //  设置PWM输出  背光通过占空比可调
    LCD_WR_REG(0xD0);  //  设置自动白平衡DBC
    LCD_WR_DATA(0x0d); //  disable

    LCD_WR_REG(0xBE);  //  配置PWM输出
    LCD_WR_DATA(0x06); //  1设置PWM频率
    LCD_WR_DATA(0x80); //  2设置PWM占空比
    LCD_WR_DATA(0x01); //  3设置C
    LCD_WR_DATA(0xF0); //  4设置D
    LCD_WR_DATA(0x00); //  5设置E
    LCD_WR_DATA(0x00); //  6设置F

    LCD_WR_REG(0xB8);  //  设置GPIO配置
    LCD_WR_DATA(0x03); //  2个IO口设置成输出
    LCD_WR_DATA(0x01); //  GPIO使用正常的IO功能

    LCD_WR_REG(0x29); //  开启显示
  }
  LCD_Display_Dir(1); //	默认为横屏
  LCD_BL_H;           //	点亮背光
  LCD_Clear(WHITE);
}
