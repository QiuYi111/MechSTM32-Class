#include "delay.h"
#include "LCD.h"
#include "LED.h"
#include "picture.h"
#include "USART1.h"


/**********************************************************************************************************
函数名称：main函数
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
int main(void)
{     
    SystemInit();                                                       //  系统时钟初始化
    
    Systick_Init();	                                                    //  系统滴答定时器初始化
    
    LED_GPIO_Init();                                                    //  LED初始化
        
	
   
    
    LCD_Init();
    //printf("LCD ID = ");
    //printf("%d",lcddev.id);
    LCD_Clear(RED);	 
    delay_ms(1000);
    
    LCD_Clear(GREEN);	 
    delay_ms(1000);
    
    LCD_Clear(BLUE);	 
    delay_ms(1000);
    
    LCD_Clear(YELLOW);	 
    delay_ms(1000);
    
    LCD_Clear(WHITE);	 
    
    POINT_COLOR = RED;
    //LCD_DrawRectangle(5, 5, 475, 267);
	//LCD_DrawRectangle(10, 10, 470, 262);
	//LCD_ShowString(80, 10, 400, 24, 24,"--STM32F103ZET6 Platform--");   //  实验开发平台	
    //LCD_ShowString(150, 150, 400, 24, 24, "Hello Word!"); 
    delay_ms(1000);
    
    LCD_Clear(GREEN);                                                   //  设置液晶背景
    //LCD_DrawPicture(80, 20, 400, 260, (u8*)gImage_1);	  			    //  显示图片
    delay_ms(1000);
    //LCD_DrawRectangle(80,80,150,150);
        
    while(1);		
}


