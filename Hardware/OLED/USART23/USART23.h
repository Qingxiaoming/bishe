#ifndef __USART23_H__
#define __USART23_H__
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"	
#include "sys.h" 

/*
库：__USART23_H__
版本：v0.03
说明：新增DMA的支持，同时增加宏控制是否使用内部‘\r\n’作为帧尾的中断接收函数，
		方便使用JY901之类需要串口自定义协议的外设屏蔽中断函数，避免重定义
作者:Sheep
日期：23/07/31
*/

//是硬件规定死的两个DMA通道,不需要理会
#define USART2_Rx_DMA_Channel DMA1_Channel6 
#define USART3_Rx_DMA_Channel DMA1_Channel3


//***************************配置宏*****************************************//
//缓冲区大小(buff大小)
#define USART2_REC_LEN  			128  	
#define USART3_REC_LEN  			128  	
// 是否使用串口2、3，以及是DMA接收还是中断接收
//其他地方使用到某一个串口，可以通过使能(EN)屏蔽对应的串口初始化和中断函数
//默认使用dma+空闲中断接收，如果需要使用接收中断,直接将USART2_USE_DMA宏设为0即可
//!!需要注意，若是使用中断接收，则需要以‘\r\n’作为帧尾
#define EN_USART2 			1	
    #define USART2_USE_DMA  1 //是否使能DMA，1为dma，2为INT，
		#define USART2_USE_INT_IRQn 1 
		//当上方的宏为2时，这里可以选择是否使用其中断函数，0时需要自定义中断函数
#define EN_USART3 			1		
    #define USART3_USE_DMA  1//是否使能DMA，1为dma，2为INT
		#define USART3_USE_INT_IRQn 1
		//当上方的宏为2时，这里可以选择是否使用其中断函数，0时需要自定义中断函数


//*************************外部使用的全局变量和函数、宏******************************//
//缓冲数组，检查有数据后可以直接读
extern u8  USART2_RX_BUF[USART2_REC_LEN]; 
extern u8  USART3_RX_BUF[USART3_REC_LEN]; 
//缓冲区中数据长度&标志位，高两位一帧数据已收到的标志位
extern u16 USART2_RX_STA;         			
extern u16 USART3_RX_STA;         			
//初始化函数以及串口打印和发送函数
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);
void USART2_printf(char* fmt,...); 
void USART3_printf(char* fmt,...); 
void USART2_sendbuff(uint8_t * buffer,uint32_t buff_size);
void USART3_sendbuff(uint8_t * buffer,uint32_t buff_size);

//******************************用于检查缓冲区的两个接口**********************************//
//检查是否收到一帧数据
#define USARTx_BUFF_AVAIBLE(USARTx_BUFF_STA) (USARTx_BUFF_STA&0xC000)
//检查缓冲区中数据的多少
#define USARTx_BUFF_LEN(USARTx_BUFF_STA)     (USARTx_BUFF_STA&0x3FFF)
//根据是否使能DMA条件编译 缓冲区清空函数
#if ((USART2_USE_DMA==1) || (USART3_USE_DMA ==1))

static __INLINE void USARTx_BUFF_RESET(USART_TypeDef * USARTx)
{
	if(USARTx == USART2)
	{
		USART2_RX_STA = 0;
		DMA_Cmd(USART2_Rx_DMA_Channel, DISABLE);
		USART2_Rx_DMA_Channel->CNDTR = USART2_REC_LEN; //复位初始接收地址
		DMA_Cmd(USART2_Rx_DMA_Channel, ENABLE);		
	}
	else if(USARTx == USART3)
	{
		USART3_RX_STA = 0;
		DMA_Cmd(USART3_Rx_DMA_Channel, DISABLE);
		USART3_Rx_DMA_Channel->CNDTR = USART3_REC_LEN; //复位初始接收地址
		DMA_Cmd(USART3_Rx_DMA_Channel, ENABLE);		
	}
}

#else
static __INLINE void USARTx_BUFF_RESET(uint16_t* USARTx_RX_STA)
{
	*USARTx_RX_STA = 0;
}
#endif

#endif



