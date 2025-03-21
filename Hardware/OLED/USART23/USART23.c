#include "sys.h"
#include "USART23.h"

//..\HAREWARE\USART23\USART23.c(96): warning:  #550-D: variable "res" was set but never used
//..\HAREWARE\USART23\USART23.c(270): warning:  #550-D: variable "res" was set but never used
//使用以下这个宏可以屏蔽掉以上的两个res未使用警告
#pragma diag_suppress 550 

#if EN_USART2  
u8 USART2_RX_BUF[USART2_REC_LEN];   
u16 USART2_RX_STA=0;         

void USART2_printf (char *fmt, ...)
{ 
	char buffer[USART2_REC_LEN+1];  
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART2_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART2_REC_LEN) && (i < strlen(buffer))){
        USART_SendData(USART2, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

void USART2_sendbuff(uint8_t * buffer,uint32_t buff_size)
{
	for(uint32_t i = 0 ; i < buff_size; i++)
	{
		USART_SendData(USART2, (u8) buffer[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
}

#if (USART2_USE_DMA==1)
//使能串口初始化和DMA接收初始化函数
void USART2_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	DMA_InitTypeDef DMA_InitStructure;
		 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能 DMA 时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART2, &USART_InitStructure); 
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); //开启空闲中断，用于dma接收
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //开启串口RX 的DMA接收请求

   //Usart2 NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	 

 
	DMA_DeInit(USART2_Rx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR)); //外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2_RX_BUF; //内存基地址
	//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //外设->内存
	DMA_InitStructure.DMA_BufferSize = USART2_REC_LEN; //内存缓冲区的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //外设自增单位字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //内存自增单位字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //dma循环发送
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 失能内存到内存
	DMA_Init(USART2_Rx_DMA_Channel, &DMA_InitStructure);

	// DMA_ITConfig(USART2_Rx_DMA_Channel, DMA_IT_TC, DISABLE);	 //dma传输完成中断
	// DMA_ITConfig(USART2_Rx_DMA_Channel, DMA_IT_TE, DISABLE);  // dma传输错误中断
	DMA_Cmd(USART2_Rx_DMA_Channel, ENABLE);	
    USART_Cmd(USART2, ENABLE);                     
}
//对应的中断函数
void USART2_IRQHandler(void)
{ 	
	uint16_t len;
	uint8_t res = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{  
		len = USART2_REC_LEN - DMA_GetCurrDataCounter(USART2_Rx_DMA_Channel);
		USART2_RX_STA += len; // 将收到的数据长度计入缓冲区长度
		USART2_RX_STA |= 0xC000; //标志接收到一帧
		len = USART2_RX_STA & (~0xC000); //让len等于现有总长度
		res = USART2->DR;//读取DR是为了清除接收标志位，可能会用一个没有使用的warning
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
	} 
} 
#elif (USART2_USE_DMA==2)

//使用接收中断的初始化和中断函数
void USART2_Init(uint32_t bound)
{ 

    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
		 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART2, &USART_InitStructure); 
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);                     
   //Usart2 NVIC 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	 
}

#if USART2_USE_INT_IRQn
void USART2_IRQHandler(void){ 	
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //0x0d 0x0a		
		Res =USART_ReceiveData(USART2);	
		// printf("%c",Res);	
		if((USART2_RX_STA&0x8000)==0){			
			if(USART2_RX_STA&0x4000){				
				if(Res!=0x0a)USART2_RX_STA=0;
				else USART2_RX_STA|=0x8000;	 
			}else{ 					
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ; 
					USART2_RX_STA++;	
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;
				}		 
			}
		}   		 
	} 
} 
#endif

#endif

#endif	


/*
USART3
*/
#if EN_USART3   

u8 USART3_RX_BUF[USART3_REC_LEN];  
u16 USART3_RX_STA=0;       	  
void USART3_printf (char *fmt, ...)
{ 
	char buffer[USART3_REC_LEN+1];  
	u8 i = 0;	
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, USART3_REC_LEN+1, fmt, arg_ptr);
	while ((i < USART3_REC_LEN) && (i < strlen(buffer))){
        USART_SendData(USART3, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

void USART3_sendbuff(uint8_t * buffer,uint32_t buff_size)
{
	for(uint32_t i = 0 ; i < buff_size; i++)
	{
		USART_SendData(USART3, (u8) buffer[i]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
	}
}


#if (USART3_USE_DMA==1) 
//使能串口初始化和DMA接收初始化函数
void USART3_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	DMA_InitTypeDef DMA_InitStructure;
		 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能 DMA 时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART3, &USART_InitStructure); 
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); //开启空闲中断，用于dma接收
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE); //开启串口RX 的DMA接收请求

   //Usart3 NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	 

 
	DMA_DeInit(USART3_Rx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR)); //外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_RX_BUF; //内存基地址
	//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //外设->内存
	DMA_InitStructure.DMA_BufferSize = USART3_REC_LEN; //内存缓冲区的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //外设自增单位字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //内存自增单位字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //dma循环发送
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 失能内存到内存
	DMA_Init(USART3_Rx_DMA_Channel, &DMA_InitStructure);

	// DMA_ITConfig(USART3_Rx_DMA_Channel, DMA_IT_TC, DISABLE);	 //dma传输完成中断
	// DMA_ITConfig(USART3_Rx_DMA_Channel, DMA_IT_TE, DISABLE);  // dma传输错误中断
	DMA_Cmd(USART3_Rx_DMA_Channel, ENABLE);	
    USART_Cmd(USART3, ENABLE);                     
}


//对应的中断函数
void USART3_IRQHandler(void)
{ 	
	uint16_t len;
	uint8_t res = 0;
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{  
		len = USART3_REC_LEN - DMA_GetCurrDataCounter(USART3_Rx_DMA_Channel);
		USART3_RX_STA += len; // 将收到的数据长度计入缓冲区长度
		USART3_RX_STA |= 0xC000; //标志接收到一帧
		len = USART3_RX_STA & (~0xC000); //让len等于现有总长度
		res = USART3->DR; //读取DR是为了清除接收标志位，可能会用一个没有使用的warning
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	} 
} 

#elif (USART3_USE_DMA == 2)

void USART3_Init(u32 BaudRate){ 
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure; 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 

   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   
   USART_InitStructure.USART_BaudRate = BaudRate;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

   USART_Init(USART3, &USART_InitStructure);
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
   //USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
   USART_Cmd(USART3, ENABLE);

   
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}

#if USART3_USE_INT_IRQn
void USART3_IRQHandler(void){ 	
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){  
		Res =USART_ReceiveData(USART3);		
		if((USART3_RX_STA&0x8000)==0){			
			if(USART3_RX_STA&0x4000){				
				if(Res!=0x0a)USART3_RX_STA=0;
				else USART3_RX_STA|=0x8000;	 
			}else{ 					
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ; 
					USART3_RX_STA++;	
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;	  
				}		 
			}
		}   		 
	}
} 
#endif
#endif
#endif	



/***************************************************************/


/*示例：
//检测串口三是否有收到格式以'\r\n'结尾的数据,若使用dma接收，则不需要
uint16_t len = 0;
if(USARTx_BUFF_AVAIBLE(USART3_RX_STA))
{
	len = USARTx_BUFF_LEN(USART3_RX_STA);

	//读取USART3_RX_BUF数组做数据操作
	USARTx_BUFF_RESET(USART3) = 0;
}
//打印示例
USART3_printf(" === %s===","sheep");
//打包发送示例，依赖pack库,使用USART3_sendbuff()函数更方便
static uint32_t pack_test = 0;
uint8_t send_buff[12] = {0};
pack_test++;
struct_pack((char*)send_buff,12,"<III",pack_test,pack_test+1,pack_test+2);
USART3_sendbuff(send_buff,12);
*/

/*
//示例2：
uint32_t temp_int = 0,temp_char = 0;
float temp_float = 0;
uint32_t temp_int_arr[4] = {0};
if(USARTx_BUFF_AVAIBLE(USART2_RX_STA))
{
	struct_unpack((char*)USART2_RX_BUF,9,">Icf",&temp_int,&temp_char,&temp_float);
	if(log_en)logInfo("USART2_recv======%d,%d,%f",temp_int,temp_char,temp_float);
	// logInfo("buff = %s",USART2_RX_BUF);
	USARTx_BUFF_RESET(USART2);
}
if(USARTx_BUFF_AVAIBLE(USART3_RX_STA))
{
	struct_unpack((char*)USART3_RX_BUF,16,"<IIII",temp_int_arr,&temp_int_arr[1],&temp_int_arr[2],&temp_int_arr[3]);
	if(log_en)logDebug("USART3_recv:%d,%d,%d,%d",temp_int_arr[0],temp_int_arr[1],temp_int_arr[2],temp_int_arr[3]);
	USARTx_BUFF_RESET(USART3);
}
*/

/***************************************************************/


