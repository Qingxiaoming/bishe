#ifndef __RECKON_H
#define __RECKON_H

#include "stm32f10x.h" 
#include "sys.h"

/* 棋盘相关常量定义 */
#define BOARD_SIZE 3  // 棋盘大小3x3

/* 棋盘相关变量声明 */
extern char nowchess[BOARD_SIZE][BOARD_SIZE];    // 当前棋盘状态
extern char recvchess[BOARD_SIZE][BOARD_SIZE];   // 接收的棋盘状态
extern int board_updated;                        // 棋盘更新标志
extern int processing_move;                      // 正在处理移动标志
extern int game_state;                           // 游戏状态

/* 棋盘处理函数声明 */
void DisplayChessboard(char board[BOARD_SIZE][BOARD_SIZE]);
int CompareChessboard(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE], int *row, int *col);
void SendChessboardToLogger(char board[BOARD_SIZE][BOARD_SIZE], const char* type);
int ProcessNewMove(char board[BOARD_SIZE][BOARD_SIZE], int player_row, int player_col);
int CheckGameResult(char board[BOARD_SIZE][BOARD_SIZE]);
void UpdateDisplayStatus(int status);
int8_t Parse_MqttCmd(uint8_t *data);

/* 游戏状态定义 */
#define GAME_READY      0  // 等待开始
#define GAME_PLAYING    1  // 游戏进行中
#define GAME_WIN        2  // 游戏结束(电脑赢)
#define GAME_LOSE       3  // 游戏结束(玩家赢)
#define GAME_DRAW       4  // 游戏结束(平局)

#endif /* __RECKON_H */ 