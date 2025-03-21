#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "stm32f10x.h" 
#include "sys.h"

/* 常量定义区 */
// 电机运动方向定义
#define MOTOR_FORWARD     'F'    // 电机前进
#define MOTOR_RETREAT     'B'    // 电机后退
#define MOTOR_STOP        'T'    // 电机停止

// 棋子位置定义
#define CHESS_BLACK_COL   10     // 黑棋列位置
#define CHESS_WHITE_COL   700    // 白棋列位置

// 棋子行位置定义
#define CHESS_POS_1       660    // 第1个棋子位置
#define CHESS_POS_2       550    // 第2个棋子位置
#define CHESS_POS_3       370    // 第3个棋子位置 
#define CHESS_POS_4       220    // 第4个棋子位置
#define CHESS_POS_5       130    // 第5个棋子位置

// 棋盘行位置定义
#define ROW_1             600    // 第1行位置
#define ROW_2             400    // 第2行位置
#define ROW_3             220    // 第3行位置

// 棋盘列位置定义
#define COL_1             200    // 第1列位置
#define COL_2             370    // 第2列位置
#define COL_3             570    // 第3列位置

/* 数据结构定义区 */
// 电机控制结构体
typedef struct {
    int stepnum;        // 步进电机步数
    int direct;         // 电机运动方向
    int goal_1;         // 第一目标位置
    int goal_2;         // 第二目标位置
    int enable;         // 电机使能标志
    int t4_enable;      // 定时器4使能标志
    int mode;           // 电机运行模式
} Motor;

/* 外部变量声明 */
extern Motor motor1, motor2;
extern int dogdog, nowtime1, nowtime2;
extern int flag_gowhere;
extern int glob_lock;

/* 函数声明区 */
// 电机通用控制接口
typedef enum {
    MOTOR_1 = 1,
    MOTOR_2 = 2
} MotorID;

// 初始化函数
void Stepper_motor_Init(int ARR, int Compare, int Period);

// 统一的电机控制函数
void Stepmotor_Enable(MotorID motor);
void Stepmotor_DisEnable(MotorID motor);
void Stepmotor_Forward(MotorID motor);
void Stepmotor_Retreat(MotorID motor);
void Motor_OneStep(MotorID motor);

// 获取电机目标位置
int GetMotorGoal(MotorID motor, int tempflag, int goal);

// 统一的电机运动控制函数
void Motor_Move(MotorID motor, int delay_1, int delay_2, int delay_3, int delay_4);
void Motor_Chess(MotorID motor, int delay_1, int delay_2, int delay_3, int delay_4);

// 调试模式
void Motor_Debug(void);

// 下棋动作函数
void ChessMove(int mode, int delay_1, int delay_2, int delay_3, int delay_4);

#endif /* __STEPPER_MOTOR_H */












