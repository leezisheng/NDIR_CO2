#ifndef __UART_H__
#define __UART_H__

#include <stm32f10x.h>
#include <stdio.h>

// 串口1-USART1
#define  uartx                   USART1
#define  uart_CLK                RCC_APB2Periph_USART1
#define  uart_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  uart_BAUDRATE           9600

// USART GPIO 引脚宏定义
#define  uart_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  uart_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  uart_TX_GPIO_PORT       GPIOA   
#define  uart_TX_GPIO_PIN        GPIO_Pin_9
#define  uart_RX_GPIO_PORT       GPIOA
#define  uart_RX_GPIO_PIN        GPIO_Pin_10

#define  USART_IRQ                USART1_IRQn
#define  USART_IRQHandler         USART1_IRQHandler
// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel5
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)
// 一次发送的数据量
#define  RECEIVEBUFF_SIZE            5
void USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);



#endif	 

