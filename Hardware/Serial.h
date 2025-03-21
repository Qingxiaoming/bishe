#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h"
#include <stdarg.h>


void USART1_Init(void);//串口1初始化并启动
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);


// 串口对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL     DMA1_Channel5
// 外设寄存器地址
#define  USART_DR_ADDRESS        (&USART1->DR)

#define USART_RBUFF_SIZE 300


#define motorForward    'F'
#define motorRetreat    'B'
#define motorStop       'T'

#define Package_head1   'D'
#define Package_head2   '!'
#define Package_tail    'O'

#define motorReset      'R'
#define motor_t4_Reset  'P'
#define motorEnable     'E'

#define Chess_debug      1
#define Chess_onlymove   2
#define Chess_Chess      3
#define Chess_gote       4

//调试模式
//D! R T0 M=E,E 111,123 O
#define data_reset        RxBuffer[3]
#define data_testnum      RxBuffer[6]
#define data_motor1enable RxBuffer[10]	
#define data_motor2enable RxBuffer[12]	
#define data_M1_3         RxBuffer[14]
#define data_M1_2 	      RxBuffer[15]	
#define data_M1_1 	      RxBuffer[16]	
#define data_M2_3 	      RxBuffer[18]	
#define data_M2_2 	      RxBuffer[19]
#define data_M2_1 	      RxBuffer[20]	

//第1-3题
//D! R T1 M=E,E 4:[5,1][2,5][8,8][9,9] O
#define data_reset        RxBuffer[3]
#define data_testnum      RxBuffer[6]
#define data_motor1enable RxBuffer[10]	
#define data_motor2enable RxBuffer[12]	
#define data_amount       RxBuffer[14]
#define data_chess1 	  RxBuffer[17]	
#define data_place1 	  RxBuffer[19]	
#define data_chess2 	  RxBuffer[22]	
#define data_place2 	  RxBuffer[24]
#define data_chess3 	  RxBuffer[27]	
#define data_place3 	  RxBuffer[29]
#define data_chess4 	  RxBuffer[32]	
#define data_place4 	  RxBuffer[34]

//第四五题：
//黑先，在白失误的情况下赢
//白先，黑保证不输
//D! R T2 M=E,E S5 O
//D! R T2 M=E,E R1 O
#define data_reset        RxBuffer[3]
#define data_testnum      RxBuffer[6]
#define data_motor1enable RxBuffer[10]	
#define data_motor2enable RxBuffer[12]	
#define data_t4mode       RxBuffer[14]
#define data_t4_start     'S'
#define data_t4_response  'R'
#define data_t4_place     RxBuffer[15]	


extern u16 flag_rtos;
extern u16	flag_rtos_key1 ;
extern u16	flag_rtos_key2 ;

extern u16  MotorGoal_chess[6];
extern u16  MotorGoal_place[6];

extern u16 Chess_MODE;

extern u16 Usart_Rx_Buf[USART_RBUFF_SIZE];

#endif
