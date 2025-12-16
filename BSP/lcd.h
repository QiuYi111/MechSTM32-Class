#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"
#include <stdlib.h>

typedef uint16_t u16;
typedef uint8_t u8;
typedef uint32_t u32;
typedef volatile uint16_t vu16;

#define LCD_W 480
#define LCD_H 272

// LCD地址结构体
typedef struct {
  volatile u16 LCD_REG;
  volatile u16 LCD_RAM;
} LCD_TypeDef;
// 使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A10作为数据命令区分线
// 注：这是16位操作，STM32内部地址要左移一位！
#define LCD_BASE ((u32)(0x6C000000 | 0x000007FE))
#define LCD ((LCD_TypeDef *)LCD_BASE)

/*********************************************************************************************************
液晶硬件引脚定义
LCD_BL(PE3)
LCD_RESET(PE4)
LCD_CS(FMSC_NE4)
LCD_RS(FMSC_A10)
LCD_WR(FMSC_NWE)
LCD_RD(FMSC_NOE)
LCD_D0--LCD_D15(FMSC_D0--FSMC_D15)
*********************************************************************************************************/
// LCD_BL(PE3)
#define LCD_BL_L GPIO_ResetBits(GPIOE, GPIO_Pin_3)
#define LCD_BL_H GPIO_SetBits(GPIOE, GPIO_Pin_3)

// LCD_RESET(PE4)
#define LCD_RESET_L GPIO_ResetBits(GPIOE, GPIO_Pin_4)
#define LCD_RESET_H GPIO_SetBits(GPIOE, GPIO_Pin_4)

//	LCD重要参数集
typedef struct {
  u16 width;   //	LCD 宽度
  u16 height;  //	LCD 高度
  u16 id;      //	LCD ID
  u8 dir;      //	横屏还是竖屏控制：0，竖屏；1，横屏。
  u16 wramcmd; //	开始写gram指令
  u16 setxcmd; //	设置x坐标指令
  u16 setycmd; //	设置y坐标指令
} _lcd_dev;

//	LCD参数
extern _lcd_dev lcddev; //	管理LCD重要参数

//	LCD的画笔颜色和背景色
extern u16 POINT_COLOR; //	默认红色
extern u16 BACK_COLOR;  //	背景颜色.默认为白色

//	扫描方向定义
#define L2R_U2D 0 //	从左到右,从上到下
#define L2R_D2U 1 //	从左到右,从下到上
#define R2L_U2D 2 //	从右到左,从上到下
#define R2L_D2U 3 //	从右到左,从下到上

#define U2D_L2R 4 //	从上到下,从左到右
#define U2D_R2L 5 //	从上到下,从右到左
#define D2U_L2R 6 //	从下到上,从左到右
#define D2U_R2L 7 //	从下到上,从右到左

#define DFT_SCAN_DIR L2R_U2D //	默认的扫描方向

//	画笔颜色
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40  //	棕色
#define BRRED 0XFC07  //	棕红色
#define GRAY 0X8430   //	灰色
#define ORANGE 0xFD20 // 橙色
//	GUI颜色

#define DARKBLUE 0X01CF  //	深蓝色
#define LIGHTBLUE 0X7D7C //	浅蓝色
#define GRAYBLUE 0X5458  //	灰蓝色
//	以上三色为PANEL的颜色

#define LIGHTGREEN 0X841F //	浅绿色
#define LGRAY 0XC618      //	浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE 0XA651 //	浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12    //	浅棕蓝色(选择条目的反色)

//	LCD分辨率设置
#define SSD_HOR_RESOLUTION (480 - 1) //	LCD水平分辨率
#define SSD_VER_RESOLUTION (272 - 1) //	LCD垂直分辨率

//	LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH 10 //	水平脉宽
#define SSD_HOR_BACK_PORCH 8   //	水平前廊
#define SSD_HOR_FRONT_PORCH 43 //	水平后廊

#define SSD_VER_PULSE_WIDTH 10 //	垂直脉宽
#define SSD_VER_BACK_PORCH 12  //	垂直前廊
#define SSD_VER_FRONT_PORCH 4  //	垂直前廊

//	如下几个参数，自动计算
#define SSD_HT (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_FRONT_PORCH)
#define SSD_VT (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//	函数声明
void LCD_WR_REG(u16 regval);
void LCD_WR_DATA(u16 data);
u16 LCD_RD_DATA(void);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_Display_Dir(u8 dir);
void LCD_Init(void);
void LCD_Clear(u16 color);

// Graphics Primitives
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_Fill(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawCircle(u16 x0, u16 y0, u8 r, u16 color);
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u16 color, u16 mode);
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p,
                    u16 color);

#endif
