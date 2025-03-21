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
	
	TIM_InternalClockConfig(TIM3);//ѡ��TIM3�������ڲ�ʱ��Դ
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  //TIM��ʱ����ʼ���ṹ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:0
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 19999;	//ARV��ʱ�����Զ���װ��ֵ������ֵ�ﵽ���ֵʱ����ʱ���ͻ����һ���жϻ�����¼���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;	//PSC��ʱ����Ԥ��Ƶֵ���������˶�ʱ����ʱ��Ƶ�ʱ���Ƶ�ı�����������������ƶ�ʱ���ķֱ��ʺͼ����ٶȡ�
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //�ظ������жϴ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure; //����ȽϽṹ��
	TIM_OCStructInit(&TIM_OCInitStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //�Ƚ����ģʽѡ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�Ƚ�������ԣ���ѡ OCxΪ�ߵ�ƽ��Ч��͵�ƽ��Ч���������Ŷ�ʱ��ͨ����Ч��ƽ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;		//�Ƚ���������ȣ�ʵ���趨�ȽϼĴ��� CCR ��ֵ�����������ȡ�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM3
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
