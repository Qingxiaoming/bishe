

#include "Stepper_motor.h"

Motor motor1,motor2;

int glob_lock=0;

void Stepper_motor_Init(int ARR,int Compare,int Period){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//使能与方向
	//PA1-4
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_1 |GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//pwm输出
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure1);

	TIM_InternalClockConfig(TIM3);//选择TIM3来设置内部时钟源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  //TIM定时器初始化结构体
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:0
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = ARR;	//ARV定时器的自动重装载值，计数值达到这个值时，定时器就会产生一个中断或更新事件。
	TIM_TimeBaseInitStructure.TIM_Prescaler = Period;	//PSC定时器的预分频值，它决定了定时器的时钟频率被分频的倍数，这可以用来控制定时器的分辨率和计数速度。
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //重复进入中断次数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure; //输出比较结构体
	TIM_OCStructInit(&TIM_OCInitStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //比较输出模式选择
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //比较输出极性，可选 OCx为高电平有效或低电平有效。它决定着定时器通道有效电平。
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;		//比较输出脉冲宽度，实际设定比较寄存器 CCR 的值，决定脉冲宽度。
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	

	TIM_Cmd(TIM3, ENABLE); //使能 TIM3
    
    TIM_SetCompare1(TIM3, Compare);
	TIM_SetCompare2(TIM3, Compare);
    
    Stepmotor1_DisEnable();
	Stepmotor2_DisEnable();

}

void Stepmotor1_Enable(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}
void Stepmotor1_DisEnable(){
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void Stepmotor1_forward(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}
void Stepmotor1_retreat(){
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
}
void motor1_onestep(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

void Stepmotor2_Enable(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}
void Stepmotor2_DisEnable(){
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void Stepmotor2_forward(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}
void Stepmotor2_retreat(){
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
void motor2_onestep(){
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
}


int Getgoal_Motor1(int tempflag,int goal){
    int tempsum=0;
    if(goal == 1){//目标1：棋子
        switch(MotorGoal_chess[tempflag]){
            case '0':
                tempsum= Chess1;
                break;
            case '1':
                tempsum= Chess2;
                break;
            case '2':
                tempsum= Chess3;
                break;
            case '3':
                tempsum= Chess4;
                break;
            case '4':
                tempsum= Chess5;
                break;
            case '5':
                tempsum= Chess1;
                break;
            case '6':
                tempsum= Chess2;
                break;
            case '7':
                tempsum= Chess3;
                break;
            case '8':
                tempsum= Chess4;
                break;
            case '9':
                tempsum= Chess5;
                break;

        }
    }
    if(goal == 2){//目标1：棋盘
        switch(MotorGoal_place[tempflag]){
            case '1':
                tempsum= Row1;
                break;
            case '2':
                tempsum= Row1;
                break;
            case '3':
                tempsum= Row1;
                break;
            case '4':
                tempsum= Row2;
                break;
            case '5':
                tempsum= Row2;
                break;
            case '6':
                tempsum= Row2;
                break;
            case '7':
                tempsum= Row3;
                break;
            case '8':
                tempsum= Row3;
                break;
            case '9':
                tempsum= Row3;
                break;
        }
    } 
    return tempsum;
}


int Getgoal_Motor2(int tempflag,int goal){
    int tempsum=0;
    if(goal == 1){//目标1：棋子
        switch(MotorGoal_chess[tempflag]){
            case '0':
                tempsum= ChessBlack_COL;
                break;
            case '1':
                tempsum= ChessBlack_COL;
                break;
            case '2':
                tempsum= ChessBlack_COL;
                break;
            case '3':
                tempsum= ChessBlack_COL;
                break;
            case '4':
                tempsum= ChessBlack_COL;
                break;
            case '5':
                tempsum= ChessWrite_COL;
                break;
            case '6':
                tempsum= ChessWrite_COL;
                break;
            case '7':
                tempsum= ChessWrite_COL;
                break;
            case '8':
                tempsum= ChessWrite_COL;
                break;
            case '9':
                tempsum= ChessWrite_COL;
                break;
        }
    }
    if(goal == 2){//目标1：棋盘
        switch(MotorGoal_place[tempflag]){
            case '1':
                tempsum= COL1;
                break;
            case '2':
                tempsum= COL2;
                break;
            case '3':
                tempsum= COL3;
                break;
            case '4':
                tempsum= COL1;
                break;
            case '5':
                tempsum= COL2;
                break;
            case '6':
                tempsum= COL3;
                break;
            case '7':
                tempsum= COL1;
                break;
            case '8':
                tempsum= COL2;
                break;
            case '9':
                tempsum= COL3;
                break;

        }
    }
    return tempsum;
}



void Motor_Debug(){
    if(motor1.enable==1){
        switch(motor1.mode){
            case 0:
                Stepmotor1_forward();
                Stepmotor1_Enable();
                motor1.mode=1;
                break;
        
            case 2:
                Stepmotor1_DisEnable();
                break;
        }
    }
    else 
        Stepmotor1_DisEnable();
    
    if(motor2.enable==1){
        switch(motor2.mode){
            case 0:
                Stepmotor2_forward();
                Stepmotor2_Enable();
                motor2.mode=1;
                break;
            case 2:
                Stepmotor2_DisEnable();
                break;
        }
    }
    else 
        Stepmotor2_DisEnable();
}

void Motor_Move_1(int delay_1,int delay_2,int delay_3,int delay_4)
{
    motor1.goal_1 = Getgoal_Motor1(flag_gowhere+1,1);
    motor1.goal_2 = Getgoal_Motor1(flag_gowhere+1,2);
    printf("motor1::%d:%d,%d,%d\n",motor1.enable,motor1.goal_1,motor1.goal_2,motor1.stepnum);
//    if(flag_gowhere==MotorGoal_place[0]){
//        motor1.enable=0;
//		printf("asdfghjklsdfghjkl\n");
//    }
    if(motor1.enable==1){     
        switch(motor1.mode){
            case 0:
                //初始状态，记录此时时间
                nowtime1 = dogdog;
                motor1.mode = 1;
                break;
            case 1:
                //延时一秒后电机去第一个目标
                if(nowtime1+delay_1 == dogdog && motor1.enable==1){
                    Stepmotor1_forward();
                    Stepmotor1_Enable();
                    motor1.direct = motorForward;
                    glob_lock=1;
                    motor1.mode = 2;
                    
                }
                break;
            //case2:  
                //在pwm中断，当位置到达goal_1进入case3
            case 3:
                //到达第一个目标，延时,然后去第二个目标
                Stepmotor1_DisEnable();
                if(glob_lock==1){
                    printf("glob!\n");
                    glob_lock=0;
                }
                if(nowtime1+delay_2 == dogdog){
                    if(motor1.goal_1<motor1.goal_2){
                        Stepmotor1_forward();
                        Stepmotor1_Enable();         
                        motor1.direct = motorForward;
                    }
                    else if(motor1.goal_1>motor1.goal_2){
                        Stepmotor1_retreat();
                        Stepmotor1_Enable();
                        motor1.direct = motorRetreat;
                    }
                    else if(motor1.goal_1==motor1.goal_2){
                        motor1.direct = motorStop;
                        Stepmotor1_DisEnable();
                    }
                    motor1.mode = 4;
                }
                break;
            //case4:  
                //在pwm中断，当位置到达goal_2进入case5    
            case 5://到达第二个目标，延时
                Stepmotor1_DisEnable();
                if(nowtime1+delay_3 == dogdog){
                    printf("1:drop!\n");
                    Stepmotor1_retreat();
                    Stepmotor1_Enable();
                    motor1.direct = motorRetreat;
                    motor1.mode = 6;
                }
                break;
            //case6:  
                //在pwm中断，当位置回到0时进入case7      
            case 7://到达原点
                Stepmotor1_DisEnable();
                if(nowtime1+delay_4 == dogdog){
                    flag_gowhere++;
                    motor1.mode = 0;
                }
                break;
            }
    }
    else 
        Stepmotor1_DisEnable();
}



void Motor_Move_2(int delay_1,int delay_2,int delay_3,int delay_4)
{
    motor2.goal_1 = Getgoal_Motor2(flag_gowhere+1,1);
    motor2.goal_2 = Getgoal_Motor2(flag_gowhere+1,2);
    //printf("%d: %d,%d,%d %d %d-%d\n",motor2.mode,motor2.stepnum,motor2.goal_1,motor2.goal_2,flag_gowhere,nowtime2,dogdog);
    if(flag_gowhere==MotorGoal_place[0]){
        motor2.enable=0;
    }
    if(motor2.enable==1){    
        switch(motor2.mode){
            case 0:
                //初始状态，记录此时时间
                nowtime2 = dogdog;
                motor2.mode = 1;
                break;
            case 1:
                //延时一秒后电机去第一个目标
                if(nowtime2+delay_1 == dogdog ){
                    Stepmotor2_forward();
                    Stepmotor2_Enable();
                    motor2.direct = motorForward;
                    motor2.mode = 2;
                    
                }
                break;
            //case2:  
                //在pwm中断，当位置到达goal_1进入case3
            case 3:
                //到达第一个目标，延时,然后去第二个目标
                Stepmotor2_DisEnable();
                if(nowtime2+delay_2 == dogdog){
                    if(motor2.goal_1<motor2.goal_2){
                        Stepmotor2_forward();
                        Stepmotor2_Enable();         
                        motor2.direct = motorForward;
                    }
                    else if(motor2.goal_1>motor2.goal_2){
                        Stepmotor2_retreat();
                        Stepmotor2_Enable();
                        motor2.direct = motorRetreat;
                    }
                    else if(motor2.goal_1==motor2.goal_2){
                        motor2.direct = motorStop;
                        Stepmotor2_DisEnable();
                    }
                    motor2.mode = 4;
                }
                break;
            //case4:  
                //在pwm中断，当位置到达goal_2进入case5    
            case 5://到达第二个目标，延时
                Stepmotor2_DisEnable();
                if(nowtime2+delay_3 == dogdog){
                    Stepmotor2_retreat();
                    Stepmotor2_Enable();
                    motor2.direct = motorRetreat;
                    motor2.mode = 6;
                }
                break;
            //case6:  
                //在pwm中断，当位置回到0时进入case7      
            case 7://到达原点
                Stepmotor2_DisEnable();
                if(nowtime2+delay_4 == dogdog){
                    //flag_gowhere++;
                    motor2.mode = 0;
                }
                break;
            }
    }
    else 
        Stepmotor2_DisEnable();
}



void Motor_Chess_1(int delay_1,int delay_2,int delay_3,int delay_4)
{
    motor1.goal_1 = Getgoal_Motor1(flag_gowhere+1,1);
    motor1.goal_2 = Getgoal_Motor1(flag_gowhere+1,2);
    printf("%d: %d,%d,%d %d %d-%d\n",motor1.mode,motor1.stepnum,motor1.goal_1,motor1.goal_2,flag_gowhere,nowtime1,dogdog);
    if(flag_gowhere==MotorGoal_place[0]){
        motor1.enable=0;
    }
    if(motor1.enable==1 && motor1.t4_enable==1){     
        switch(motor1.mode){
            case 0:
                //初始状态，记录此时时间
                nowtime1 = dogdog;
                motor1.mode = 1;
                break;
            case 1:
                //延时一秒后电机去第一个目标
                if(nowtime1+delay_1 == dogdog && motor1.enable==1){
                    Stepmotor1_forward();
                    Stepmotor1_Enable();
                    motor1.direct = motorForward;
                    glob_lock=1;
                    motor1.mode = 2;
                    
                }
                break;
            //case2:  
                //在pwm中断，当位置到达goal_1进入case3
            case 3:
                //到达第一个目标，延时,然后去第二个目标
                Stepmotor1_DisEnable();
                if(glob_lock==1){
                    printf("glob!\n");
                    glob_lock=0;
                }
                if(nowtime1+delay_2 == dogdog){
                    if(motor1.goal_1<motor1.goal_2){
                        Stepmotor1_forward();
                        Stepmotor1_Enable();         
                        motor1.direct = motorForward;
                    }
                    else if(motor1.goal_1>motor1.goal_2){
                        Stepmotor1_retreat();
                        Stepmotor1_Enable();
                        motor1.direct = motorRetreat;
                    }
                    else if(motor1.goal_1==motor1.goal_2){
                        motor1.direct = motorStop;
                        Stepmotor1_DisEnable();
                    }
                    motor1.mode = 4;
                }
                break;
            //case4:  
                //在pwm中断，当位置到达goal_2进入case5    
            case 5://到达第二个目标，延时
                Stepmotor1_DisEnable();
                if(nowtime1+delay_3 == dogdog){
                    printf("drop!\n");
                    Stepmotor1_retreat();
                    Stepmotor1_Enable();
                    motor1.direct = motorRetreat;
                    motor1.mode = 6;
                }
                break;
            //case6:  
                //在pwm中断，当位置回到0时进入case7      
            case 7://到达原点
                Stepmotor1_DisEnable();
                if(nowtime1+delay_4 == dogdog){
                    flag_gowhere++;
                    printf("OK!!\n");
                    motor1.mode = 0;
                    motor1.t4_enable=0;
                }
                break;
            }
    }
    else 
        Stepmotor1_DisEnable();
}



void Motor_Chess_2(int delay_1,int delay_2,int delay_3,int delay_4)
{
    motor2.goal_1 = Getgoal_Motor2(flag_gowhere+1,1);
    motor2.goal_2 = Getgoal_Motor2(flag_gowhere+1,2);
    printf("%d: %d,%d,%d %d %d-%d\n",motor2.mode,motor2.stepnum,motor2.goal_1,motor2.goal_2,flag_gowhere,nowtime2,dogdog);
    if(flag_gowhere==MotorGoal_place[0]){
        motor2.enable=0;
    }
    if(motor2.enable==1 && motor2.t4_enable==1){    
        switch(motor2.mode){
            case 0:
                //初始状态，记录此时时间
                nowtime2 = dogdog;
                motor2.mode = 1;
                break;
            case 1:
                //延时一秒后电机去第一个目标
                if(nowtime2+delay_1 == dogdog ){
                    Stepmotor2_forward();
                    Stepmotor2_Enable();
                    motor2.direct = motorForward;
                    motor2.mode = 2;
                }
                break;
            //case2:  
                //在pwm中断，当位置到达goal_1进入case3
            case 3:
                //到达第一个目标，延时,然后去第二个目标
                Stepmotor2_DisEnable();
                if(nowtime2+delay_2 == dogdog){
                    if(motor2.goal_1<motor2.goal_2){
                        Stepmotor2_forward();
                        Stepmotor2_Enable();         
                        motor2.direct = motorForward;
                    }
                    else if(motor2.goal_1>motor2.goal_2){
                        Stepmotor2_retreat();
                        Stepmotor2_Enable();
                        motor2.direct = motorRetreat;
                    }
                    else if(motor2.goal_1==motor2.goal_2){
                        motor2.direct = motorStop;
                        Stepmotor2_DisEnable();
                    }
                    motor2.mode = 4;
                }
                break;
            //case4:  
                //在pwm中断，当位置到达goal_2进入case5    
            case 5://到达第二个目标，延时
                Stepmotor2_DisEnable();
                if(nowtime2+delay_3 == dogdog){
                    Stepmotor2_retreat();
                    Stepmotor2_Enable();
                    motor2.direct = motorRetreat;
                    motor2.mode = 6;
                }
                break;
            //case6:  
                //在pwm中断，当位置回到0时进入case7      
            case 7://到达原点
                Stepmotor2_DisEnable();
                if(nowtime2+delay_4 == dogdog){
                    //flag_gowhere++;
                    motor2.mode = 0;
                    motor2.t4_enable=0;
                }
                break;
            }
    }
    else 
        Stepmotor2_DisEnable();
}

void faialMove(){
    switch(Chess_MODE){
        case Chess_debug:{
            Motor_Debug();
            break;
        }
        case Chess_onlymove:{
            Motor_Move_1(1,5,9,12);        
            Motor_Move_2(1,5,9,12);
            break;
        }
        case Chess_Chess:{
            Motor_Chess_1(1,5,9,12);        
            Motor_Chess_2(1,5,9,12);
            break;
        }
        case Chess_gote:{
            
            break;
        }
    }
}



