#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h"
#include <stdarg.h>


void USART1_Init(void);//串口1初始化并启动
void USART1_printf(char *format, ...);


// 串口对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL     DMA1_Channel5
// 外设寄存器地址
#define  USART_DR_ADDRESS        (&USART1->DR)

#define USART1_REC_LEN 300

extern u16 USART1_RX_BUF[USART1_REC_LEN];

#endif
