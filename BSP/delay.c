#include "delay.h"


unsigned int TimingDelay = 0;

// 初始化SysTick定时器，使能滴答时钟，时间为1ms
void Systick_Init(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */
        while (1);
    }
}

//等待直到TimeDelay 被减为 0
void delay_ms(unsigned int time)
{
    TimingDelay = time;
    while(TimingDelay);
}

/**********************************************************************************************************
us延时函数  通过空循环消耗 CPU 时间
**********************************************************************************************************/
void delay_us(unsigned int time)
{  
   unsigned short i=0; 
    
   while(time--)
   {
      i = 8;  
      while(i--) ;    
   }
}

/**********************************************************************************************************
中断服务函数  每1ms调用一次，用于延时函数
**********************************************************************************************************/
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}
