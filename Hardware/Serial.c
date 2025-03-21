#include "Serial.h"                  // Device header
#include "USART23.h"
char board[3][3] = {{'0', '0', '0'}, {'0', '0', '0'}, {'0', '0', '0'}};

uint8_t flag_game=0;

u16 Chess_MODE = 0;

u16  MotorGoal_chess[6];
u16  MotorGoal_place[6];

u16 flag_rtos = 1;
u16	flag_rtos_key1 = 0;
u16	flag_rtos_key2 = 0;

u16 Usart_Rx_Buf[USART_RBUFF_SIZE] = {0};


void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;     //串口端口配置结构体变量
    USART_InitTypeDef USART_InitStructure;   //串口参数配置结构体变量
    NVIC_InitTypeDef NVIC_InitStructure;     //串口中断配置结构体变量

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //打开PA端口时钟
    
    //USART1_TX:PA 9   
    //USART1_RX:PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          		 //PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  		 //设定IO口的输出速度为50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		 //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);             	 	 //初始化PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //初始化PA10 

    //USART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;  //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		    //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	                          //根据指定的参数初始化VIC寄存器

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = 9600;                  //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;        //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;           //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                  //收发模式
    USART_Init(USART1, &USART_InitStructure);                     //初始化串口1

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //使能中断
    USART_Cmd(USART1, ENABLE);                     //使能串口1
    USART_ClearFlag(USART1, USART_FLAG_TC);        //清串口1发送标志
		
}



void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int Cy1, FILE *f)
{
	Serial_SendByte(Cy1);
	return Cy1;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}



//USART1 全局中断服务函数
void USART1_IRQHandler(void)			 
{
		u8 com_data; 
		int i;
		static u8 RxCounter=0;
		static u16 RxBuffer[100]={0};
		static u8 RxState = 0;	
		static u8 RxFlag1 = 0;

		if( USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)  	   //接收中断  
		{			
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //清除中断标志
			
			com_data = USART_ReceiveData(USART1);
        
            if(RxState==0&&com_data==Package_head1) 
            {
                RxState=1;
                RxBuffer[RxCounter]=com_data;
                RxCounter++;
                
            }
            if(RxState==1&&com_data==Package_head2) 
            {
                RxState=2;
                RxBuffer[RxCounter]=com_data;
            }
    
            if(RxState==2) //0x5b结束位
            {
                RxBuffer[RxCounter]=com_data;
                RxCounter++;
                if(com_data == Package_tail)
                {
                    RxState=3;
                    RxFlag1=1;
                }
            }
        }
    
            if(RxState==3)		//检测是否接受到结束标志
            {
                if(RxBuffer[RxCounter-1] == Package_tail)
                {
                    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//关闭DTSABLE中断
                    if(RxFlag1)
                    {
/********************************执行程序************************************************************/ 	
              
		Usart_Rx_Buf[0]=RxCounter+1;				
        for(i=0;i<RxCounter;i++){
			Usart_Rx_Buf[i+1] = RxBuffer[i];
            USART2_printf("%c",RxBuffer[i]);
        }
        USART2_printf("\n");   
		
		

				
/****************************************************************************************************/	
                                    
                    }
                        RxFlag1 = 0;
                        RxCounter = 0;
                        RxState = 0;
                        USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
                }
                else   //接收错误
                {
                    RxState = 0;
                    RxCounter=0;
                    for(i=0;i<USART_RBUFF_SIZE;i++)
                    {
                        RxBuffer[i]=0x00;      //将存放数据数组清零
                    }
                }
            } 
}

