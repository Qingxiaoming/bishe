#include "sys.h"
/********************************** 任务句柄 ************************************/
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t DOG_Task_Handle = NULL;
static TaskHandle_t MOTOR_Task_Handle = NULL;
static TaskHandle_t Electro_Task_Handle = NULL;
static TaskHandle_t OLED_Task_Handle = NULL;
/********************************** 内核对象句柄 *********************************/


/******************************* 全局变量声明 ************************************/
int dogdog=0,nowtime1=0,nowtime2=0;
int flag_gowhere=0;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/

//多任务初始化
static void AppTaskCreate(void);
//用于初始化板载相关资源
static void BSP_Init(void);

//时钟任务
static void DOG_Task(void* pvParameters);
//电机任务
static void MOTOR_Task(void* pvParameters);
//电磁铁任务
static void Electro_Task(void* pvParameters);
//UI显示任务
static void OLED_Task(void* pvParameters);


//步进电机控制程序
void Motor_Move_1_TIM(void);
void Motor_Move_2_TIM(void);
//json拆装包
void getJsonData(void);
int8_t Parse_MqttCmd(uint8_t *data);


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
  
	// 开发板硬件初始化 
	BSP_Init();
  
	// 创建AppTaskCreate任务 
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )256,  
                        (void*          )NULL,
                        (UBaseType_t    )1, 
                        (TaskHandle_t*  )&AppTaskCreate_Handle);
	// 启动任务调度            
	if(pdPASS == xReturn)
		vTaskStartScheduler();   
	else
		return -1;  
  
	while(1);   

    
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
	xReturn = xTaskCreate((TaskFunction_t )DOG_Task, 
							(const char*    )"DOG_Task",
							(uint16_t       )256,   
							(void*          )NULL,	
							(UBaseType_t    )2,	    
							(TaskHandle_t*  )&DOG_Task_Handle);
	if(pdPASS == xReturn)
		USART2_printf("creat DOG_Task\r\n");
	
	/* 创建MOTOR_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )MOTOR_Task,  
							(const char*    )"MOTOR_Task",
							(uint16_t       )256,  
							(void*          )NULL,
							(UBaseType_t    )3, 
							(TaskHandle_t*  )&MOTOR_Task_Handle);
	if(pdPASS == xReturn)
		USART2_printf("creat MOTOR_Task\r\n");
  
	
	/* 创建Electro_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )Electro_Task,  
							(const char*    )"Electro_Task",
							(uint16_t       )256,  
							(void*          )NULL,
							(UBaseType_t    )4, 
							(TaskHandle_t*  )&Electro_Task_Handle);
	if(pdPASS == xReturn)
		USART2_printf("creat Electro_Task\r\n");

	/* 创建Electro_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t )OLED_Task,  
							(const char*    )"OLED_Task",
							(uint16_t       )256,  
							(void*          )NULL,
							(UBaseType_t    )5, 
							(TaskHandle_t*  )&OLED_Task_Handle);
	if(pdPASS == xReturn)
		USART2_printf("creat OLED_Task\r\n");
	
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
	
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	USART1_Init();
    Timer1_Init();
	
	TestUI_Init();
    USART2_Init(9600); 
	
	//Timer2_Init();
	
	//Stepper_motor_Init(19999,10000,719);
	//Stepper_motor_Init(149,75,719);

}



/**********************************************************************
  * @ 函数名  ： DOG_Task
  * @ 功能说明： 定时器任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void DOG_Task(void* parameter)
{	
	while (1)
	{
		//getJsonData();  
	  
		uint8_t aaa[USART1_REC_LEN]= "\0";
		
		for(int i=3;i<USART1_RX_BUF[0]-1;i++){
			aaa[i-3] = USART1_RX_BUF[i];
		}
		
		//USART2_printf("%s",aaa);
		
		//Parse_MqttCmd(aaa);
	  
		vTaskDelay(1000); 
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

		//faialMove();
		if( USART1_RX_BUF[3] == '1' )
		{
			vTaskSuspend(DOG_Task_Handle);
		} 
		if( USART1_RX_BUF[3] == '2' )
		{
			vTaskResume(DOG_Task_Handle);
		}
		vTaskDelay(20);
	}
}


/**********************************************************************
  * @ 函数名  ： Electro_Task
  * @ 功能说明： 电磁铁任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Electro_Task(void* parameter)
{	
	while (1)
	{	
		//USART2_printf("aaaaa");

		vTaskDelay(20); 
	}
}

/**********************************************************************
  * @ 函数名  ： OLED_Task
  * @ 功能说明： UI任务,处理按键事件并用OLED显示
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void OLED_Task(void* parameter)
{	
	while (1)
	{	
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




int8_t Parse_MqttCmd(uint8_t *data)
{
	// 寻找JSON数据的开始位置  
	const char *json_start = strstr((char *)data, "{"); 
	if (json_start == NULL) 
	{  
		USART2_printf("JSON data not found in the received string.\n");  
		return -1;  
	}  
	size_t json_length = strlen(json_start);
	
	// 分配内存并复制JSON数据  
	char *json_data = (char *)malloc(json_length + 1);  
	if (json_data == NULL) {  
		USART2_printf("Memory allocation failed.\n");  
		return -1;  
	}  
	strncpy(json_data, json_start, json_length);  
	json_data[json_length] = '\0'; // 添加null终止符  
 
	//解析JSON数据  
	cJSON *root = cJSON_Parse(json_data);  
	if (root == NULL)
	{  
		USART2_printf("Failed to parse JSON data.\n");  
		cJSON_free(json_data);  
		return -1;  
	}  
	// ... 在这里处理JSON数据 ...  
	
	
	// 获取并打印"command_name"字段的值  
	cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
	cJSON *paras = cJSON_GetObjectItemCaseSensitive(root, "age");
	cJSON *sw_item = cJSON_GetObjectItemCaseSensitive(root, "is_running");
	//判断"command_name"字段的值选择控制类型
	char * command_name = name->valuestring;
	
	int command_name2 = paras->valueint;
	int command_name3 = sw_item->valueint;
	USART2_printf("Name: %s \r\n", command_name);
	USART2_printf("Name: %d \r\n", command_name2);
	USART2_printf("Name: %d \r\n", command_name3);


	
	// 释放资源 
	cJSON_Delete(root);  
	cJSON_free(json_data);  
	return 1;  
}












void Motor_Debug_TIM(){  
////////    switch(motor1.mode){
////////        case 1:{
////////            //当位置到达goal_1进入case3
////////            if(motor1.stepnum<=motor1.goal_1 && motor1.stepnum>=0 ){
////////                motor1.stepnum++;			
////////                if(motor1.stepnum == motor1.goal_1){
////////                    motor1.mode=2;
////////                }
////////            }
////////            break;
////////        }
////////    }  
////////    switch(motor2.mode){
////////        case 1:{
////////            //当位置到达goal_1进入case3
////////            if(motor2.stepnum<=motor2.goal_1 && motor2.stepnum>=0 ){
////////                motor2.stepnum++;			
////////                if(motor2.stepnum == motor2.goal_1){
////////                    motor2.mode=2;
////////                }
////////            }
////////            break;
////////        }
////////    }    
}


void Motor_Move_1_TIM(){  
////////    switch(motor1.mode){
////////        case 2:{
////////            //当位置到达goal_1进入case3
////////            if(motor1.stepnum<=motor1.goal_1 && motor1.stepnum>=0 && motor1.direct == motorForward){
////////                motor1.stepnum++;			
////////                if(motor1.stepnum == motor1.goal_1){
////////                    motor1.mode=3;
////////                }
////////            }
////////            break;
////////        }
////////        case 4:{
////////            //当位置到达goal_2进入case5    
////////            if(motor1.direct == motorForward){
////////                motor1.stepnum++;			
////////                if(motor1.stepnum == motor1.goal_2)
////////                    motor1.mode=5;
////////            }
////////            else if(motor1.direct == motorRetreat){
////////                motor1.stepnum--;			
////////                if(motor1.stepnum == motor1.goal_2)
////////                    motor1.mode=5;
////////            }
////////            else if(motor1.direct == motorStop){		
////////                if(motor1.stepnum == motor1.goal_2)
////////                    motor1.mode=5;
////////            }
////////            break;
////////        }  
////////        case 6:{  
////////            //在pwm中断，当步数回到0时进入case7  
////////            if(motor1.stepnum<=motor1.goal_2 && motor1.stepnum>=0 && motor1.direct == motorRetreat){
////////                motor1.stepnum--;			
////////                if(motor1.stepnum == 0)
////////                    motor1.mode=7;
////////            }
////////            break;
////////        }
////////    }            
}
			
void Motor_Move_2_TIM(){        
////////    switch(motor2.mode){
////////        case 2:{
////////            //当位置到达goal_1进入case3
////////            if(motor2.stepnum<=motor2.goal_1 && motor2.stepnum>=0 && motor2.direct == motorForward){
////////                motor2.stepnum++;			
////////                if(motor2.stepnum == motor2.goal_1){
////////                    motor2.mode=3;
////////                }
////////            }
////////            break;
////////        }
////////        case 4:{
////////            //当位置到达goal_2进入case5    
////////            if(motor2.direct == motorForward){
////////                motor2.stepnum++;			
////////                if(motor2.stepnum == motor2.goal_2)
////////                    motor2.mode=5;
////////            }
////////            else if(motor2.direct == motorRetreat){
////////                motor2.stepnum--;			
////////                if(motor2.stepnum == motor2.goal_2)
////////                    motor2.mode=5;
////////            }
////////            else if(motor2.direct == motorStop){		
////////                if(motor2.stepnum == motor2.goal_2)
////////                    motor2.mode=5;
////////            }
////////            break;
////////        }  
////////        case 6:{  
////////            //在pwm中断，当步数回到0时进入case7  
////////            if(motor2.stepnum<=motor2.goal_2 && motor2.stepnum>=0 && motor2.direct == motorRetreat){
////////                motor2.stepnum--;			
////////                if(motor2.stepnum == 0)
////////                    motor2.mode=7;
////////            }
////////            break;
////////        }
////////    }            
}


void TIM3_IRQHandler(void)
{
	//TIM_IT_Update
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源  
		
/********************************执行程序************************************************************/ 	
//////	USART2_printf("1234567\n");
//////    switch(Chess_MODE){
//////        case Chess_debug:{
//////            Motor_Debug_TIM();
//////            break;
//////        }
//////        case Chess_onlymove:{
//////            Motor_Move_1_TIM();        
//////            Motor_Move_2_TIM();
//////            break;
//////        }
//////        case Chess_Chess:{
//////            Motor_Move_1_TIM();        
//////            Motor_Move_2_TIM();
//////            break;
//////        }
//////    }

/****************************************************************************************************/
	}
}



void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源  
		
/********************************执行程序************************************************************/ 	

        
        
/****************************************************************************************************/
	}
}





void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源  
		
/********************************执行程序************************************************************/ 	
//		dogdog++;
//		USART2_printf("%d",dogdog);
		
		

		
		
/****************************************************************************************************/
	}
}




