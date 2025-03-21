#include "sys.h"


/********************************** 任务句柄 ************************************/
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t DOG_Task_Handle = NULL;
static TaskHandle_t MOTOR_Task_Handle = NULL;
static TaskHandle_t Electro_Task_Handle = NULL;
/********************************** 内核对象句柄 *********************************/


/******************************* 全局变量声明 ************************************/
int dogdog=0,nowtime1=0,nowtime2=0;
int flag_gowhere=0;

// 电机目标位置变量
extern char MotorGoal_chess[];
extern char MotorGoal_place[];

// 注：将棋盘相关变量移动到reckon.c中
// 这里保留对这些变量的引用，通过reckon.h的extern声明

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/

static void AppTaskCreate(void);/* 用于创建任务 */

static void DOG_Task(void* pvParameters);/* DOG_Task任务实现 */
static void MOTOR_Task(void* pvParameters);/* MOTOR_Task任务实现 */
static void Electro_Task(void* pvParameters);/* MOTOR_Task任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

// 步进电机控制程序中断处理函数
void Motor_Move_TIM(MotorID motor);
void Motor_Debug_TIM(void);

// json拆装包
void getJsonData(void);

// 注：棋盘处理函数已移动到reckon.c

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
	/* 开发板硬件初始化 */
	BSP_Init();
  
	/* 创建AppTaskCreate任务 */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )256,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
	/* 启动任务调度 */           
	if(pdPASS == xReturn)
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
		return -1;  
  
	while(1);   /* 正常不会执行到这里 */ 

    // ChessMove(Chess_MODE, 1, 5, 9, 12);
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
	taskENTER_CRITICAL();           //进入临界区
  
	/* 创建DOG_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )DOG_Task, /* 任务入口函数 */
							(const char*    )"DOG_Task",/* 任务名字 */
							(uint16_t       )256,   /* 任务栈大小 */
							(void*          )NULL,	/* 任务入口函数参数 */
							(UBaseType_t    )2,	    /* 任务的优先级 */
							(TaskHandle_t*  )&DOG_Task_Handle);/* 任务控制块指针 */
	if(pdPASS == xReturn)
		USART2_printf("creat DOG_Task\r\n");
	/* 创建MOTOR_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )MOTOR_Task,  /* 任务入口函数 */
							(const char*    )"MOTOR_Task",/* 任务名字 */
							(uint16_t       )256,  /* 任务栈大小 */
							(void*          )NULL,/* 任务入口函数参数 */
							(UBaseType_t    )3, /* 任务的优先级 */
							(TaskHandle_t*  )&MOTOR_Task_Handle);/* 任务控制块指针 */ 
	if(pdPASS == xReturn)
		USART2_printf("creat MOTOR_Task\r\n");
  
	
	/* 创建Electro_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )Electro_Task,  /* 任务入口函数 */
							(const char*    )"Electro_Task",/* 任务名字 */
							(uint16_t       )256,  /* 任务栈大小 */
							(void*          )NULL,/* 任务入口函数参数 */
							(UBaseType_t    )4, /* 任务的优先级 */
							(TaskHandle_t*  )&Electro_Task_Handle);/* 任务控制块指针 */ 
	if(pdPASS == xReturn)
		USART2_printf("creat Electro_Task\r\n");

	
	vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
	taskEXIT_CRITICAL();            //退出临界区
}




/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	// 初始化串口
	USART1_Init();  // 日志通信串口
    USART2_Init(9600);  // OpenMV通信串口
	
	// 初始化定时器
    Timer1_Init();  // 系统时钟
	
	// 初始化OLED显示器
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(1, 1, "System Init...");
	
	// 初始化UI
	TestUI_Init();
    
	// 初始化步进电机
	Stepper_motor_Init(149, 75, 719);
	
	// 清空棋盘状态和初始化游戏状态
	// 注：直接使用reckon.h中声明的变量
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			nowchess[i][j] = '0';
			recvchess[i][j] = '0';
		}
	}
	
	// 初始化游戏状态
	game_state = GAME_READY;
	processing_move = 0;
	
	USART1_printf("System initialized.\r\n");
	OLED_ShowString(2, 1, "Ready!");
}



/**********************************************************************
  * @ 函数名  ： DOG_Task
  * @ 功能说明： 定时器任务主体，接收OpenMV发送的棋盘数据
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void DOG_Task(void* parameter)
{	
	// 初始化游戏状态
	game_state = GAME_READY;
	processing_move = 0;
	
	// 清空棋盘状态
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			nowchess[i][j] = '0';
			recvchess[i][j] = '0';
		}
	}
	
	// 初始显示状态
	UpdateDisplayStatus(game_state);
	
	while (1)
	{
		// 检查UART2中是否有数据
		if (Usart_Rx_Buf[0] > 0) {
			uint8_t aaa[USART_RBUFF_SIZE] = "\0";
	  
			// 提取UART数据
			for (int i = 3; i < Usart_Rx_Buf[0] - 1; i++) {
			aaa[i-3] = Usart_Rx_Buf[i];
		}
		
			// 处理接收到的数据
		Parse_MqttCmd(aaa);
	  
			// 清空接收缓冲区
			for (int i = 0; i < USART_RBUFF_SIZE; i++) {
				Usart_Rx_Buf[i] = 0;
			}
		}
		
		// 电机移动完成后，更新棋盘显示
		if (processing_move && motor1.enable == 0 && motor2.enable == 0) {
			processing_move = 0;
			DisplayChessboard(nowchess);
		}
		
		// 等待下一次检查
		vTaskDelay(1000); // 每秒检查一次
	}
}

/**********************************************************************
  * @ 函数名  ： MOTOR_Task
  * @ 功能说明： 电机控制任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void MOTOR_Task(void* parameter)
{	
	while (1)
  {
    // 检查是否需要暂停DOG_Task
    if (Usart_Rx_Buf[3] == '1')
    {
      vTaskSuspend(DOG_Task_Handle);
    } 
    if (Usart_Rx_Buf[3] == '2')
    {
      vTaskResume(DOG_Task_Handle);
    }
    
    // 检查电机移动状态
    if (processing_move) {
      // 检查电机是否已完成移动
      if (motor1.mode == 0 && motor2.mode == 0 && 
          motor1.enable == 0 && motor2.enable == 0) {
        // 电机移动完成
        USART2_printf("Motor move completed.\n");
        processing_move = 0;
        
        // 更新OLED显示
        DisplayChessboard(nowchess);
      }
    }
    
    vTaskDelay(20);
  }
}


/**********************************************************************
  * @ 函数名  ： ElectromagnetDOG_Task
  * @ 功能说明： 电磁铁任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Electro_Task(void* parameter)
{	
	while (1)
	{	
		//USART2_printf("aaaaa");
		TestUI_Proc();
		
		if(USARTx_BUFF_AVAIBLE(USART2_RX_STA))
        {
			
			USART2_printf("a?");
			USART2_printf("%c\n",USART2_RX_BUF[0]);
            switch (USART2_RX_BUF[0]) //没有按键，使用串口代替
            {
				
                case 'a': OLED_MsgQueSend(msg_click); break;
                case 'b': OLED_MsgQueSend(msg_up); break;
                case 'c': OLED_MsgQueSend(msg_down); break;
                case 'd': OLED_MsgQueSend(msg_return); break;
                default: break;
            }
            USARTx_BUFF_RESET(USART2);//(&USART2_RX_STA);
        }
		
		
		vTaskDelay(20); 
	}
}



void getJsonData(void)
{
	// 添加参数到params对象，并检查每个调用的返回值 
	cJSON *pOrder = cJSON_CreateObject();  
    cJSON *params = cJSON_CreateObject();  
    char *json_body = NULL;  
	
	if (pOrder == NULL || params == NULL)  
    {  
        USART2_printf("Creat ENV JSONobj Err\n");  
        cJSON_Delete(pOrder);  
        cJSON_Delete(params);  
        return;  
    }  
	
	
    if (!cJSON_AddNumberToObject(params,"temp",23.5)||  
        !cJSON_AddNumberToObject(params,"o2",21))
    {  
        USART2_printf("Add ENV data to JSONobj Err\n");  
        cJSON_Delete(pOrder);  
        cJSON_Delete(params);  
        return;  
    }  
	
	
 
    cJSON_AddItemToObject(pOrder, "params", params);
	
	//
    json_body = cJSON_PrintUnformatted(pOrder); 
	
    if (json_body == NULL)  
    {  
        USART2_printf("Print ENV JSON Err\n");  
        cJSON_Delete(pOrder);  
        return;  
    }  
	
	//
    USART2_printf("ENV json: %s \r\n", json_body);  
	
	cJSON_free(json_body);
	cJSON_Delete(pOrder);  
}




/**
 * @brief  调试模式下的电机中断处理函数
 * @retval 无
 */
void Motor_Debug_TIM() {
    // 电机1调试控制中断处理
    if (motor1.mode == 1) {
        // 当位置到达goal_1进入case3
        if (motor1.stepnum <= motor1.goal_1 && motor1.stepnum >= 0) {
                motor1.stepnum++;			
            if (motor1.stepnum == motor1.goal_1) {
                motor1.mode = 2;
            }
        }
    }
    
    // 电机2调试控制中断处理
    if (motor2.mode == 1) {
        // 当位置到达goal_1进入case3
        if (motor2.stepnum <= motor2.goal_1 && motor2.stepnum >= 0) {
                motor2.stepnum++;			
            if (motor2.stepnum == motor2.goal_1) {
                motor2.mode = 2;
            }
        }
    }
}

/**
 * @brief  电机移动中断处理函数
 * @param  motor - 电机ID (MOTOR_1 或 MOTOR_2)
 * @retval 无
 */
void Motor_Move_TIM(MotorID motor) {
    Motor *pMotor = (motor == MOTOR_1) ? &motor1 : &motor2;
    
    switch (pMotor->mode) {
        case 2:
            // 当位置到达goal_1进入case3
            if (pMotor->stepnum <= pMotor->goal_1 && pMotor->stepnum >= 0 && pMotor->direct == MOTOR_FORWARD) {
                pMotor->stepnum++;
                if (pMotor->stepnum == pMotor->goal_1) {
                    pMotor->mode = 3;
                }
            }
            break;
            
        case 4:
            // 当位置到达goal_2进入case5
            if (pMotor->direct == MOTOR_FORWARD) {
                pMotor->stepnum++;
                if (pMotor->stepnum == pMotor->goal_2)
                    pMotor->mode = 5;
            } else if (pMotor->direct == MOTOR_RETREAT) {
                pMotor->stepnum--;
                if (pMotor->stepnum == pMotor->goal_2)
                    pMotor->mode = 5;
            } else if (pMotor->direct == MOTOR_STOP) {
                if (pMotor->stepnum == pMotor->goal_2)
                    pMotor->mode = 5;
            }
            break;
            
        case 6:
            // 在PWM中断，当步数回到0时进入case7
            if (pMotor->stepnum <= pMotor->goal_2 && pMotor->stepnum >= 0 && pMotor->direct == MOTOR_RETREAT) {
                pMotor->stepnum--;
                if (pMotor->stepnum == 0)
                    pMotor->mode = 7;
            }
            break;
        }  
}

/**
 * @brief  TIM3中断处理函数
 * @retval 无
 */
void TIM3_IRQHandler(void) {
    // 检查指定的TIM中断发生与否
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        // 清除TIMx的中断待处理位
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        // 执行电机控制程序
        USART2_printf("1234567\n");
        
        switch (Chess_MODE) {
            case Chess_debug:
                Motor_Debug_TIM();
                break;
                
            case Chess_onlymove:
            case Chess_Chess:
                Motor_Move_TIM(MOTOR_1);
                Motor_Move_TIM(MOTOR_2);
            break;
        }
    }            
}
			
/**
 * @brief  TIM2中断处理函数
 * @retval 无
 */
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        
        // 执行程序
    }
}

/**
 * @brief  TIM1_UP中断处理函数
 * @retval 无
 */
void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        
        // 执行程序
        // dogdog++;
        // USART2_printf("%d",dogdog);
    }
}

// 注：以下原有的棋盘处理函数已移动到reckon.c
// DisplayChessboard
// CompareChessboard
// SendChessboardToLogger
// ProcessNewMove
// CheckGameResult
// UpdateDisplayStatus
// Parse_MqttCmd




