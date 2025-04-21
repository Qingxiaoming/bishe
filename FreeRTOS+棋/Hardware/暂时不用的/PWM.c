#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PA6 PA7
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure1);
	
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //PB0 pB1
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure2);
	
	TIM_InternalClockConfig(TIM3);//选择TIM3来设置内部时钟源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  //TIM定时器初始化结构体
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:0
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 19999;	//ARV定时器的自动重装载值，计数值达到这个值时，定时器就会产生一个中断或更新事件。
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;	//PSC定时器的预分频值，它决定了定时器的时钟频率被分频的倍数，这可以用来控制定时器的分辨率和计数速度。
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //重复进入中断次数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure; //输出比较结构体
	TIM_OCStructInit(&TIM_OCInitStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //比较输出模式选择
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //比较输出极性，可选 OCx为高电平有效或低电平有效。它决定着定时器通道有效电平。
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;		//比较输出脉冲宽度，实际设定比较寄存器 CCR 的值，决定脉冲宽度。
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM3, Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM3, Compare);
}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM3, Compare);
}

void PWM_SetCompare4(uint16_t Compare)
{
	TIM_SetCompare4(TIM3, Compare);
}
