#include "Mat_key.h" 

uint8_t MatrixKey_determine(void);


void MatrixKey_Init()
{
	RCC_APB2PeriphClockCmd(MKey_Periph, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin =MKey_Row1 | MKey_Row2 | MKey_Row3 | MKey_Row4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MKey_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = MKey_Col1 | MKey_Col2 | MKey_Col3 | MKey_Col4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MKey_GPIO, &GPIO_InitStructure);


}



uint8_t MatrixKey_GetValue()
{
	uint8_t Key;
//	uint8_t Real_Key;
	
	GPIO_ResetBits(MKey_GPIO, GPIO_Pin_4);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_5);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_15);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_14);

	Key =  MatrixKey_determine();
	
	GPIO_ResetBits(MKey_GPIO, GPIO_Pin_5); 
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_4);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_15);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_14);
	
	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 4;
	}
	
	GPIO_ResetBits(MKey_GPIO, GPIO_Pin_15);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_4);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_5);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_14);
	

	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 8;
	}
	
	GPIO_ResetBits(MKey_GPIO, GPIO_Pin_14);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_4);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_5);
	GPIO_SetBits(MKey_GPIO, GPIO_Pin_15);
	
	
	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 12;
	}
	
//	switch(Key){
//		case 0: Real_Key=0;break;
//		case 1: Real_Key=16;break;
//		case 2: Real_Key=15;break;
//		case 3: Real_Key=14;break;
//		case 4: Real_Key=13;break;
//		case 5: Real_Key=12;break;
//		case 6: Real_Key=11;break;
//		case 7: Real_Key=10;break;
//		case 8: Real_Key=9;break;
//		case 9: Real_Key=8;break;
//		case 10: Real_Key=7;break;
//		case 11: Real_Key=6;break;
//		case 12: Real_Key=5;break;
//		case 13: Real_Key=4;break;
//		case 14: Real_Key=3;break;
//		case 15: Real_Key=2;break;
//		case 16: Real_Key=1;break;
//	}
	
	return Key;
}


uint8_t MatrixKey_determine()
{
	uint8_t Key = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){Key = 4;}
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0){Key = 3;}
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){Key = 2;}
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0){Key = 1;}
	}
	return Key;
}
