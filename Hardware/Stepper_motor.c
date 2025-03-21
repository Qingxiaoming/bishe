/**
 * @file    Stepper_motor.c
 * @brief   步进电机控制模块实现
 * @details 实现了步进电机的初始化、基本控制和棋子移动逻辑
 */

#include "Stepper_motor.h"
#include <stdio.h>

/* 全局变量定义 */
Motor motor1, motor2;  // 定义两个电机控制结构体
int glob_lock = 0;     // 全局锁变量

/**
 * @brief  步进电机初始化函数
 * @param  ARR     - 自动重装载值
 * @param  Compare - 比较器值
 * @param  Period  - 定时周期
 * @retval 无
 */
void Stepper_motor_Init(int ARR, int Compare, int Period) {
	// 使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 配置使能与方向控制引脚(PA1-4)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 使能定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 配置PWM输出引脚(PA6-7)
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure1);

	// 配置定时器内部时钟
	TIM_InternalClockConfig(TIM3);
	
	// 初始化定时器基本参数
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = ARR;
	TIM_TimeBaseInitStructure.TIM_Prescaler = Period;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	// 初始化输出比较参数
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	// 配置定时器中断
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	// 设置中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	// 启动定时器
	TIM_Cmd(TIM3, ENABLE);
	
	// 设置比较输出
	TIM_SetCompare1(TIM3, Compare);
	TIM_SetCompare2(TIM3, Compare);
	
	// 初始禁用电机
	Stepmotor_DisEnable(MOTOR_1);
	Stepmotor_DisEnable(MOTOR_2);
}

/**
 * @brief  电机使能函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Stepmotor_Enable(MotorID motor) {
	if (motor == MOTOR_1) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	} else if (motor == MOTOR_2) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}
}

/**
 * @brief  电机禁用函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Stepmotor_DisEnable(MotorID motor) {
	if (motor == MOTOR_1) {
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	} else if (motor == MOTOR_2) {
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	}
}

/**
 * @brief  电机正向运动函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Stepmotor_Forward(MotorID motor) {
	if (motor == MOTOR_1) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	} else if (motor == MOTOR_2) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	}
}

/**
 * @brief  电机反向运动函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Stepmotor_Retreat(MotorID motor) {
	if (motor == MOTOR_1) {
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	} else if (motor == MOTOR_2) {
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}
}

/**
 * @brief  电机单步运动函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Motor_OneStep(MotorID motor) {
	if (motor == MOTOR_1) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	} else if (motor == MOTOR_2) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
	}
}

/**
 * @brief  获取电机目标位置
 * @param  motor    - 电机ID (MOTOR_1 或 MOTOR_2)
 * @param  tempflag - 当前轮次标志
 * @param  goal     - 目标类型 (1:棋子, 2:棋盘)
 * @retval 目标位置值
 */
int GetMotorGoal(MotorID motor, int tempflag, int goal) {
	int tempsum = 0;
	
	if (motor == MOTOR_1) {
		if (goal == 1) { // 目标1：棋子
			switch(MotorGoal_chess[tempflag]) {
				case '0': tempsum = CHESS_POS_1; break;
				case '1': tempsum = CHESS_POS_2; break;
				case '2': tempsum = CHESS_POS_3; break;
				case '3': tempsum = CHESS_POS_4; break;
				case '4': tempsum = CHESS_POS_5; break;
				case '5': tempsum = CHESS_POS_1; break;
				case '6': tempsum = CHESS_POS_2; break;
				case '7': tempsum = CHESS_POS_3; break;
				case '8': tempsum = CHESS_POS_4; break;
				case '9': tempsum = CHESS_POS_5; break;
			}
		} else if (goal == 2) { // 目标2：棋盘
			switch(MotorGoal_place[tempflag]) {
				case '1': case '2': case '3': tempsum = ROW_1; break;
				case '4': case '5': case '6': tempsum = ROW_2; break;
				case '7': case '8': case '9': tempsum = ROW_3; break;
			}
		}
	} else if (motor == MOTOR_2) {
		if (goal == 1) { // 目标1：棋子
			switch(MotorGoal_chess[tempflag]) {
				case '0': case '1': case '2': case '3': case '4':
					tempsum = CHESS_BLACK_COL; 
					break;
				case '5': case '6': case '7': case '8': case '9':
					tempsum = CHESS_WHITE_COL; 
					break;
			}
		} else if (goal == 2) { // 目标2：棋盘
			switch(MotorGoal_place[tempflag]) {
				case '1': case '4': case '7': tempsum = COL_1; break;
				case '2': case '5': case '8': tempsum = COL_2; break;
				case '3': case '6': case '9': tempsum = COL_3; break;
			}
		}
	}
	
	return tempsum;
}

/**
 * @brief  调试模式下的电机控制
 * @retval 无
 */
void Motor_Debug(void) {
	// 电机1调试控制
	if (motor1.enable == 1) {
		switch (motor1.mode) {
			case 0:
				Stepmotor_Forward(MOTOR_1);
				Stepmotor_Enable(MOTOR_1);
				motor1.mode = 1;
				break;
			case 2:
				Stepmotor_DisEnable(MOTOR_1);
				break;
		}
	} else {
		Stepmotor_DisEnable(MOTOR_1);
	}
	
	// 电机2调试控制
	if (motor2.enable == 1) {
		switch (motor2.mode) {
			case 0:
				Stepmotor_Forward(MOTOR_2);
				Stepmotor_Enable(MOTOR_2);
				motor2.mode = 1;
				break;
			case 2:
				Stepmotor_DisEnable(MOTOR_2);
				break;
		}
	} else {
		Stepmotor_DisEnable(MOTOR_2);
	}
}

/**
 * @brief  电机移动控制函数
 * @param  motor    - 电机ID (MOTOR_1 或 MOTOR_2)
 * @param  delay_1  - 第一阶段延时
 * @param  delay_2  - 第二阶段延时
 * @param  delay_3  - 第三阶段延时
 * @param  delay_4  - 第四阶段延时
 * @retval 无
 */
void Motor_Move(MotorID motor, int delay_1, int delay_2, int delay_3, int delay_4) {
	Motor *pMotor;
	int *nowtime;
	
	// 设置对应的电机指针和时间指针
	if (motor == MOTOR_1) {
		pMotor = &motor1;
		nowtime = &nowtime1;
		
		// 更新电机目标位置
		pMotor->goal_1 = GetMotorGoal(MOTOR_1, flag_gowhere + 1, 1);
		pMotor->goal_2 = GetMotorGoal(MOTOR_1, flag_gowhere + 1, 2);
		
		// 调试输出
		printf("motor1::%d:%d,%d,%d\n", pMotor->enable, pMotor->goal_1, pMotor->goal_2, pMotor->stepnum);
	} else {
		pMotor = &motor2;
		nowtime = &nowtime2;
		
		// 更新电机目标位置
		pMotor->goal_1 = GetMotorGoal(MOTOR_2, flag_gowhere + 1, 1);
		pMotor->goal_2 = GetMotorGoal(MOTOR_2, flag_gowhere + 1, 2);
	}
	
	// 检查是否达到最后一个位置
	if (flag_gowhere == MotorGoal_place[0]) {
		pMotor->enable = 0;
	}
	
	// 执行电机运动控制
	if (pMotor->enable == 1) {
		switch (pMotor->mode) {
			case 0:
				// 初始状态，记录当前时间
				*nowtime = dogdog;
				pMotor->mode = 1;
				break;
				
			case 1:
				// 延时后开始移动到第一个目标
				if (*nowtime + delay_1 == dogdog) {
					Stepmotor_Forward(motor);
					Stepmotor_Enable(motor);
					pMotor->direct = MOTOR_FORWARD;
					
					if (motor == MOTOR_1) {
						glob_lock = 1;
					}
					
					pMotor->mode = 2;
				}
				break;
				
			// case 2: 在PWM中断中处理，当位置到达goal_1进入case 3
			
			case 3:
				// 到达第一个目标，延时后移动到第二个目标
				Stepmotor_DisEnable(motor);
				
				if (motor == MOTOR_1 && glob_lock == 1) {
					printf("glob!\n");
					glob_lock = 0;
				}
				
				if (*nowtime + delay_2 == dogdog) {
					if (pMotor->goal_1 < pMotor->goal_2) {
						Stepmotor_Forward(motor);
						Stepmotor_Enable(motor);
						pMotor->direct = MOTOR_FORWARD;
					} else if (pMotor->goal_1 > pMotor->goal_2) {
						Stepmotor_Retreat(motor);
						Stepmotor_Enable(motor);
						pMotor->direct = MOTOR_RETREAT;
					} else {
						pMotor->direct = MOTOR_STOP;
						Stepmotor_DisEnable(motor);
					}
					
					pMotor->mode = 4;
				}
				break;
				
			// case 4: 在PWM中断中处理，当位置到达goal_2进入case 5
			
			case 5:
				// 到达第二个目标，延时后返回原点
				Stepmotor_DisEnable(motor);
				
				if (*nowtime + delay_3 == dogdog) {
					if (motor == MOTOR_1) {
						printf("1:drop!\n");
					}
					
					Stepmotor_Retreat(motor);
					Stepmotor_Enable(motor);
					pMotor->direct = MOTOR_RETREAT;
					pMotor->mode = 6;
				}
				break;
				
			// case 6: 在PWM中断中处理，当位置回到0进入case 7
			
			case 7:
				// 到达原点，延时后结束
				Stepmotor_DisEnable(motor);
				
				if (*nowtime + delay_4 == dogdog) {
					if (motor == MOTOR_1) {
						flag_gowhere++;
					}
					
					pMotor->mode = 0;
				}
				break;
		}
	} else {
		Stepmotor_DisEnable(motor);
	}
}

/**
 * @brief  电机下棋控制函数
 * @param  motor    - 电机ID (MOTOR_1 或 MOTOR_2)
 * @param  delay_1  - 第一阶段延时
 * @param  delay_2  - 第二阶段延时
 * @param  delay_3  - 第三阶段延时
 * @param  delay_4  - 第四阶段延时
 * @retval 无
 */
void Motor_Chess(MotorID motor, int delay_1, int delay_2, int delay_3, int delay_4) {
	Motor *pMotor;
	int *nowtime;
	
	// 设置对应的电机指针和时间指针
	if (motor == MOTOR_1) {
		pMotor = &motor1;
		nowtime = &nowtime1;
		
		// 更新电机目标位置
		pMotor->goal_1 = GetMotorGoal(MOTOR_1, flag_gowhere + 1, 1);
		pMotor->goal_2 = GetMotorGoal(MOTOR_1, flag_gowhere + 1, 2);
		
		// 调试输出
		printf("%d: %d,%d,%d %d %d-%d\n", pMotor->mode, pMotor->stepnum, pMotor->goal_1, pMotor->goal_2, 
				flag_gowhere, *nowtime, dogdog);
	} else {
		pMotor = &motor2;
		nowtime = &nowtime2;
		
		// 更新电机目标位置
		pMotor->goal_1 = GetMotorGoal(MOTOR_2, flag_gowhere + 1, 1);
		pMotor->goal_2 = GetMotorGoal(MOTOR_2, flag_gowhere + 1, 2);
		
		// 调试输出
		printf("%d: %d,%d,%d %d %d-%d\n", pMotor->mode, pMotor->stepnum, pMotor->goal_1, pMotor->goal_2, 
				flag_gowhere, *nowtime, dogdog);
	}
	
	// 检查是否达到最后一个位置
	if (flag_gowhere == MotorGoal_place[0]) {
		pMotor->enable = 0;
	}
	
	// 执行电机运动控制
	if (pMotor->enable == 1 && pMotor->t4_enable == 1) {
		switch (pMotor->mode) {
			case 0:
				// 初始状态，记录当前时间
				*nowtime = dogdog;
				pMotor->mode = 1;
				break;
				
			case 1:
				// 延时后开始移动到第一个目标
				if (*nowtime + delay_1 == dogdog) {
					Stepmotor_Forward(motor);
					Stepmotor_Enable(motor);
					pMotor->direct = MOTOR_FORWARD;
					
					if (motor == MOTOR_1) {
						glob_lock = 1;
					}
					
					pMotor->mode = 2;
				}
				break;
				
			// case 2: 在PWM中断中处理，当位置到达goal_1进入case 3
			
			case 3:
				// 到达第一个目标，延时后移动到第二个目标
				Stepmotor_DisEnable(motor);
				
				if (motor == MOTOR_1 && glob_lock == 1) {
					printf("glob!\n");
					glob_lock = 0;
				}
				
				if (*nowtime + delay_2 == dogdog) {
					if (pMotor->goal_1 < pMotor->goal_2) {
						Stepmotor_Forward(motor);
						Stepmotor_Enable(motor);
						pMotor->direct = MOTOR_FORWARD;
					} else if (pMotor->goal_1 > pMotor->goal_2) {
						Stepmotor_Retreat(motor);
						Stepmotor_Enable(motor);
						pMotor->direct = MOTOR_RETREAT;
					} else {
						pMotor->direct = MOTOR_STOP;
						Stepmotor_DisEnable(motor);
					}
					
					pMotor->mode = 4;
				}
				break;
				
			// case 4: 在PWM中断中处理，当位置到达goal_2进入case 5
			
			case 5:
				// 到达第二个目标，延时后返回原点
				Stepmotor_DisEnable(motor);
				
				if (*nowtime + delay_3 == dogdog) {
					if (motor == MOTOR_1) {
						printf("drop!\n");
					}
					
					Stepmotor_Retreat(motor);
					Stepmotor_Enable(motor);
					pMotor->direct = MOTOR_RETREAT;
					pMotor->mode = 6;
				}
				break;
				
			// case 6: 在PWM中断中处理，当位置回到0进入case 7
			
			case 7:
				// 到达原点，延时后结束
				Stepmotor_DisEnable(motor);
				
				if (*nowtime + delay_4 == dogdog) {
					if (motor == MOTOR_1) {
						flag_gowhere++;
						printf("OK!!\n");
					}
					
					pMotor->mode = 0;
					pMotor->t4_enable = 0;
				}
				break;
		}
	} else {
		Stepmotor_DisEnable(motor);
	}
}

/**
 * @brief  统一的下棋移动函数，根据不同模式调用不同的运动控制函数
 * @param  mode     - 下棋模式
 * @param  delay_1  - 第一阶段延时
 * @param  delay_2  - 第二阶段延时
 * @param  delay_3  - 第三阶段延时
 * @param  delay_4  - 第四阶段延时
 * @retval 无
 */
void ChessMove(int mode, int delay_1, int delay_2, int delay_3, int delay_4) {
	switch (mode) {
		case Chess_debug:
			// 调试模式
			Motor_Debug();
			break;
			
		case Chess_onlymove:
			// 仅移动模式
			Motor_Move(MOTOR_1, delay_1, delay_2, delay_3, delay_4);
			Motor_Move(MOTOR_2, delay_1, delay_2, delay_3, delay_4);
			break;
			
		case Chess_Chess:
			// 下棋模式
			Motor_Chess(MOTOR_1, delay_1, delay_2, delay_3, delay_4);
			Motor_Chess(MOTOR_2, delay_1, delay_2, delay_3, delay_4);
			break;
			
		case Chess_gote:
			// 预留模式
			break;
	}
}



