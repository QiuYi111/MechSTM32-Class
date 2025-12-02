#ifndef UART_H
#define UART_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    UART_REMAP_0,  /* PA9(TX), PA10(RX) */
    UART_REMAP_1   /* PB6(TX), PB7(RX) */
} UART_Remap_TypeDef;

typedef struct {
    UART_Remap_TypeDef remap;           /* Pin remapping configuration */
    uint32_t baudrate;                  /* Baud rate, e.g., 115200 */
    uint16_t wordlength;                /* Word length: USART_WordLength_8b or USART_WordLength_9b */
    uint16_t stopbits;                  /* Stop bits: USART_StopBits_1, USART_StopBits_0_5, etc. */
    uint16_t parity;                    /* Parity: USART_Parity_No, USART_Parity_Even, USART_Parity_Odd */
    uint16_t mode;                      /* Mode: USART_Mode_Rx, USART_Mode_Tx, USART_Mode_Rx_Tx */
    bool enableInterrupt;               /* Enable RX interrupt */
} UART_InitTypeDef;

void UART_GPIO_Init(UART_Remap_TypeDef remap);
void UART_Init(USART_TypeDef* USARTx, UART_InitTypeDef* uartConfig);
void UART_SendChar(USART_TypeDef* USARTx, char ch);
void UART_SendString(USART_TypeDef* USARTx, const char* str);
void UART_SendData(USART_TypeDef* USARTx, const uint8_t* data, uint16_t length);
uint8_t UART_ReceiveChar(USART_TypeDef* USARTx);
bool UART_ReceiveAvailable(USART_TypeDef* USARTx);
void UART_EnableInterrupt(USART_TypeDef* USARTx, uint8_t irqPriority);
void UART_DisableInterrupt(USART_TypeDef* USARTx);

#endif
