#include "LED.h"

  
/**********************************************************************************************************
函数名称：LED GPIO初始化
输入参数：无
输出参数：无
函数返回：无
//LED硬件资源引脚定义
//LED1(PC0)
//LED2(PC1)
//LED3(PC2)
//LED4(PC3)
**********************************************************************************************************/
void LED_GPIO_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	                        //  使能PC端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;//  LED端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 						//  推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 						//  IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 						//  根据设定参数初始化GPIOC
    
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;
}
