#include "USART1.h"
  
/**********************************************************************************************************
函数名称：UART1配置
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
// USART1_TX	 PA9	// out
// USART1_RX	 PA10	// in 
void UART1_Configuration(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
        
	// USARTx configuration --------------- first open APB2_peripheral
	/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE ); // USART1时钟使能
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // I/O端口A时钟使能
	//Configure the USART1_Tx PA9 as Alternate function Push-Pull 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	//Configure the USART1_Rx PA10 as input power-UP
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Enable USART1 Receive  interrupts 
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	// DisEnable USART1 SendEmpt  interrupts 
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	// DisEnable USART1 SendComplete  interrupts 
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	// Enable the USART1 
	USART_Cmd(USART1, ENABLE); //  USART1使能	  
}

/**********************************************************************************************************
函数名称：putchar函数重定义
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
int fputc(int ch, FILE *f)
{
    USART1->SR; // 防止复位后无法打印首字符
    
    USART_SendData(USART1, (u8) ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
    return (ch);
}

/**********************************************************************************************************
函数名称：USART1发送数据函数
输入参数：发送数据首地址和数据长度
输出参数：无
**********************************************************************************************************/
void USART1_Senddata(unsigned char *Data, unsigned int length)
{
    while(length--)
    {
        USART_SendData(USART1, *Data++);
        
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
    }
}
