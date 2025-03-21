#include "LED.h"                 

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
}

void LED1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
}

void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_2) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	}
}
