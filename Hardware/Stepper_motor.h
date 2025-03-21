#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "stm32f10x.h" 
#include "sys.h"

//row motor1
//col motor2

#define ChessBlack_COL 10
#define ChessWrite_COL 700
#define Chess1 660
#define Chess2 550
#define Chess3 370
#define Chess4 220
#define Chess5 130

#define Row1 600
#define Row2 400
#define Row3 220
#define COL1 200
#define COL2 370
#define COL3 570

typedef struct StrMotor{
    int stepnum;
    int direct;
    int goal_1;
    int goal_2;
    int enable;
    int t4_enable;
    int mode;
}Motor;

extern Motor motor1,motor2;

extern  int dogdog,nowtime1,nowtime2;

extern int flag_gowhere;

void Stepper_motor_Init(int ARR,int Compare,int Period);

void Stepmotor1_Enable(void);
void Stepmotor1_DisEnable(void);
void Stepmotor1_forward(void);
void Stepmotor1_retreat(void);
void motor1_onestep(void);

void Stepmotor2_Enable(void);
void Stepmotor2_DisEnable(void);
void Stepmotor2_forward(void);
void Stepmotor2_retreat(void);
void motor2_onestep(void);


void Motor_Debug(void);

void Motor_Move_1(int delay_1,int delay_2,int delay_3,int delay_4);
void Motor_Move_2(int delay_1,int delay_2,int delay_3,int delay_4);

void Motor_Chess_1(int delay_1,int delay_2,int delay_3,int delay_4);
void Motor_Chess_2(int delay_1,int delay_2,int delay_3,int delay_4);

//第几轮 第几个目标
int Getgoal_Motor1(int tempflag,int goal);
int Getgoal_Motor2(int tempflag,int goal);




void faialMove(void);


#endif












