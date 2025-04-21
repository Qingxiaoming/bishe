#ifndef __GAME_H_
#define __GAME_H_


#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "stdbool.h"
#include "OLED.h"
#include "stm32f10x.h" 

#define ROW 3//行数为三
#define COL 3//列数为三
#define SIZE 3  

void print_board(char board[3][3]); 

void SetBoard(char board[ROW][COL], int row, int col);//创建棋盘
void ShowBoard(char board[ROW][COL], int row, int col);//展示棋盘
void PlayerMove(char board[ROW][COL], int row, int col);//玩家下棋
int computer_move(char board[ROW][COL], char computer_mark,char player_mark);//电脑下棋
int is_win(char board[3][3], char player);//判断输赢
//下棋!
int reckon(char board[SIZE][SIZE],int condition);
#endif

