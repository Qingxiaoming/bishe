#ifndef __MAT_KEY_H
#define __MAT_KEY_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void MatrixKey_Init(void);
uint8_t MatrixKey_GetValue(void);



#define MKey_Row1    GPIO_Pin_5
#define MKey_Row2    GPIO_Pin_7
#define MKey_Row3    GPIO_Pin_9
#define MKey_Row4    GPIO_Pin_13
#define MKey_Col1    GPIO_Pin_6
#define MKey_Col2    GPIO_Pin_8
#define MKey_Col3    GPIO_Pin_12
#define MKey_Col4    GPIO_Pin_14
#define MKey_GPIO    GPIOB
#define MKey_Periph  RCC_APB2Periph_GPIOB


#endif
