#include "uart.h"

void UART_GPIO_Init(UART_Remap_TypeDef remap) //Init GPIO函数
{ 
    GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体

    if (remap == UART_REMAP_0) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //Set GPIOA clock
        
        // Configure PA9 as USART1_TX and PA10 as USART1_RX
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

        GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

void UART_Init(USART_TypeDef* USARTx, UART_InitTypeDef* uartConfig) // Init USART function
{
    USART_InitTypeDef USART_InitStructure;
    // Enable the USART clock based on the USARTx parameter
    if (USARTx == USART1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    } else if (USARTx == USART2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    } else if (USARTx == USART3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }

    // 提取配置
    UART_GPIO_Init(uartConfig->remap);

    USART_InitStructure.USART_BaudRate = uartConfig->baudrate;
    USART_InitStructure.USART_WordLength = uartConfig->wordlength;
    USART_InitStructure.USART_StopBits = uartConfig->stopbits;
    USART_InitStructure.USART_Parity = uartConfig->parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = uartConfig->mode;

    // 初始化USART
    USART_Init(USARTx, &USART_InitStructure);

    if (uartConfig->enableInterrupt) {
        UART_EnableInterrupt(USARTx, 1);
    }
    // 启用USART
    USART_Cmd(USARTx, ENABLE);
}

void UART_SendChar(USART_TypeDef* USARTx, char ch)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, (uint8_t)ch);
}

void UART_SendString(USART_TypeDef* USARTx, const char* str)
{
    while (*str) {
        UART_SendChar(USARTx, *str++);
    }
}

void UART_SendData(USART_TypeDef* USARTx, const uint8_t* data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++) {
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(USARTx, data[i]);
    }
}

uint8_t UART_ReceiveChar(USART_TypeDef* USARTx)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USARTx);
}

bool UART_ReceiveAvailable(USART_TypeDef* USARTx)
{
    return USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET;
}

void UART_EnableInterrupt(USART_TypeDef* USARTx, uint8_t irqPriority)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

    if (USARTx == USART1) {
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    } else if (USARTx == USART2) {
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    } else if (USARTx == USART3) {
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = irqPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UART_DisableInterrupt(USART_TypeDef* USARTx)
{
    USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    if (USARTx == USART1) {
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    } else if (USARTx == USART2) {
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    } else if (USARTx == USART3) {
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    }

    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}
