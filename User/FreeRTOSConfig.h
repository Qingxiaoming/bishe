

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"
#include "Serial.h"


//针对不同的编译器调用不同的 stdint.h 文件 
//#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__) 
//#include <stdint.h> 
//extern uint32_t SystemCoreClock; 
//#endif

//断言 
//#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int) 
//#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

/* 置 1：RTOS 使用抢占式调度器；置 0：RTOS 使用协作式调度器（时间片） 
* 注：在多任务管理机制上，操作系统可以分为抢占式和协作式两种。 
* 协作式操作系统是任务主动释放 CPU 后，切换到下一个任务。 
* 任务切换的时机完全取决于正在运行的任务。 
*/ 
//置1时系统永远先运行优先级高的
#define configUSE_PREEMPTION		1

//1 使能时间片调度(默认式使能的) 
//这个源码是没有的
//#define configUSE_TIME_SLICING      1 

/**************************************************************** 
FreeRTOS 与中断服务函数有关的配置选项 
****************************************************************/ 
#define xPortPendSVHandler PendSV_Handler 
#define vPortSVCHandler SVC_Handler

//否则无法使用计数信号量
#define configUSE_COUNTING_SEMAPHORES 	1
//使用互斥量
#define configUSE_MUTEXES    1 
//使用递归互斥量
#define configUSE_RECURSIVE_MUTEXES 1
//使用软件定时器
#define configUSE_TIMERS 1
#define configUSE_TIMERconfigSUPPORT_DYNAMIC_ALLOCATIONS 1

//软件定时器优先级
#define configTIMER_TASK_PRIORITY		        (configMAX_PRIORITIES-1)   
//软件定时器队列长度
#define configTIMER_QUEUE_LENGTH		        10                               
//软件定时器任务堆栈大小
#define configTIMER_TASK_STACK_DEPTH	      (configMINIMAL_STACK_SIZE*2) 

//cpu占用率
#define configGENERATE_RUN_TIME_STATS 1
#define configUSE_TRACE_FACILITY  1 
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
extern volatile uint32_t CPU_RunTime; 
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()     (CPU_RunTime = 0ul) 
#define portGET_RUN_TIME_COUNTER_VALUE()              CPU_RunTime 

#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) 72000000 )	
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
//可使用的最大优先级
#define configMAX_PRIORITIES		( 5 )
//空闲任务使用的堆栈大小
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 17 * 512) )
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 		255
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 /* equivalent to 0xb0, or priority 11. */


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15

#endif /* FREERTOS_CONFIG_H */
