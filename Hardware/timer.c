#include "stm32f10x.h"                  // Device header
#include "sys.h"
uint16_t sum=0;
uint16_t Sum=0;

void Timer1_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
  NVIC_InitTypeDef NVIC_InitStructure;  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能  

  TIM_TimeBaseStructure.TIM_Period = 20000-1; //设置自动重装载寄存器周期值  
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;//设置预分频值  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式  
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数设置  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //参数初始化  
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清中断标志位  
  TIM_ITConfig(      //使能或者失能指定的TIM中断  
    TIM1,            //TIM1  
    TIM_IT_Update  | //TIM 更新中断源  
    TIM_IT_Trigger,  //TIM 触发中断源   
    ENABLE           //使能  
    );  
	
  //设置优先级  
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;    
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//先占优先级0级  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //从优先级0级  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);   

  TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设 
	
}  


void Timer2_Init(void)
{	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}



void Timer1_ON(void)
{
	TIM_Cmd(TIM1,ENABLE);
}

void Timer1_OFF(void)
{
	TIM_Cmd(TIM1,DISABLE);
}
