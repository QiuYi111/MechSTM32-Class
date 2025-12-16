#ifndef LED_H
#define LED_H	 

#include "stm32f10x.h"

/*********************************************************************************************************
//LED硬件资源引脚定义
//LED1(PC0)
//LED2(PC1)
//LED3(PC2)
//LED4(PC3)
*********************************************************************************************************/
//LED1(PC0)
#define  	LED1_ON      	    GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define  	LED1_OFF      	    GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define  	LED1_REVERSE  	    GPIO_WriteBit(GPIOC, GPIO_Pin_0, (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0))

//LED2(PC1)
#define  	LED2_ON      	    GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define  	LED2_OFF      	    GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define  	LED2_REVERSE  	    GPIO_WriteBit(GPIOC, GPIO_Pin_1, (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_1))

//LED3(PC2)
#define  	LED3_ON      	    GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define  	LED3_OFF      	    GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define  	LED3_REVERSE  	    GPIO_WriteBit(GPIOC, GPIO_Pin_2, (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_2))

//LED4(PC3)
#define  	LED4_ON      	    GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define  	LED4_OFF      	    GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define  	LED4_REVERSE  	    GPIO_WriteBit(GPIOC, GPIO_Pin_3, (BitAction)!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_3))


//  函数声明
void LED_GPIO_Init(void);
 				    
#endif
