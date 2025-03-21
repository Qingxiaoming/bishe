/**
 * @file    reckon_ai.c
 * @brief   井字棋AI算法实现
 * @details 实现了计算电脑下棋的最佳位置的函数
 */

#include "reckon.h"
#include "usart.h"

/**
 * @brief  检查是否有一方获胜
 * @param  board - 当前棋盘状态
 * @param  player - 玩家编号 ('1'或'2')
 * @retval 1-玩家获胜，0-未获胜
 */
static int CheckWin(char board[BOARD_SIZE][BOARD_SIZE], char player) {
    // 检查行
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return 1;
        }
    }
    
    // 检查列
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return 1;
        }
    }
    
    // 检查对角线
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return 1;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  检查棋盘是否已满
 * @param  board - 当前棋盘状态
 * @retval 1-已满，0-未满
 */
static int IsBoardFull(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == '0') {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * @brief  计算电脑下棋位置的极小极大算法
 * @param  board - 当前棋盘状态
 * @param  isMax - 是否为极大值阶段(电脑)
 * @retval 棋盘评分
 */
static int Minimax(char board[BOARD_SIZE][BOARD_SIZE], int isMax) {
    // 检查电脑是否赢了
    if (CheckWin(board, '2')) {
        return 10;
    }
    
    // 检查玩家是否赢了
    if (CheckWin(board, '1')) {
        return -10;
    }
    
    // 检查是否平局
    if (IsBoardFull(board)) {
        return 0;
    }
    
    // 极大值阶段：电脑寻找最大收益
    if (isMax) {
        int best = -1000;
        
        // 遍历所有空格
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == '0') {
                    // 尝试此位置
                    board[i][j] = '2';
                    
                    // 递归计算这步棋的得分
                    best = (best > Minimax(board, !isMax)) ? best : Minimax(board, !isMax);
                    
                    // 恢复棋盘
                    board[i][j] = '0';
                }
            }
        }
        
        return best;
    }
    // 极小值阶段：玩家寻找最小收益（对电脑来说）
    else {
        int best = 1000;
        
        // 遍历所有空格
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == '0') {
                    // 尝试此位置
                    board[i][j] = '1';
                    
                    // 递归计算这步棋的得分
                    best = (best < Minimax(board, !isMax)) ? best : Minimax(board, !isMax);
                    
                    // 恢复棋盘
                    board[i][j] = '0';
                }
            }
        }
        
        return best;
    }
}

/**
 * @brief  找到电脑最佳下棋位置
 * @param  board - 当前棋盘状态
 * @retval 1-9表示位置，其他值表示特殊情况
 */
static int FindBestMove(char board[BOARD_SIZE][BOARD_SIZE]) {
    int bestVal = -1000;
    int bestRow = -1;
    int bestCol = -1;
    
    // 遍历所有空格，尝试每一个可能的位置
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == '0') {
                // 尝试此位置
                board[i][j] = '2';
                
                // 计算此位置的评分
                int moveVal = Minimax(board, 0);
                
                // 恢复棋盘
                board[i][j] = '0';
                
                // 如果此位置评分更高，更新最佳位置
                if (moveVal > bestVal) {
                    bestRow = i;
                    bestCol = j;
                    bestVal = moveVal;
                }
            }
        }
    }
    
    // 更新棋盘状态并返回位置
    if (bestRow != -1 && bestCol != -1) {
        board[bestRow][bestCol] = '2';
        // 返回1-9的位置
        return bestRow * 3 + bestCol + 1;
    }
    
    // 没有可用位置
    return 0;
}

/**
 * @brief  计算电脑下棋的最佳位置
 * @param  board - 当前棋盘状态
 * @param  condition - 玩家最后下棋位置(1-9)
 * @retval 1-9表示电脑下棋位置，-2表示玩家获胜，10表示电脑获胜，11表示平局
 */
int reckon(char board[BOARD_SIZE][BOARD_SIZE], int condition) {
    USART2_printf("AI calculating next move. Player moved at position %d\n", condition);
    
    // 检查是否有一方获胜
    if (CheckWin(board, '1')) {
        return -2; // 玩家获胜
    }
    
    if (CheckWin(board, '2')) {
        return 10; // 电脑获胜
    }
    
    // 检查是否平局
    if (IsBoardFull(board)) {
        return 11; // 平局
    }
    
    // 计算电脑的最佳位置
    int move = FindBestMove(board);
    USART2_printf("AI selected position %d\n", move);
    
    return move;
} 