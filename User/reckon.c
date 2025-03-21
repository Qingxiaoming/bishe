/**
 * @file    reckon.c
 * @brief   棋盘控制逻辑模块实现
 * @details 实现了棋盘状态处理、显示和AI对弈逻辑
 */

#include "reckon.h"
#include "Stepper_motor.h"
#include "oled.h"
#include "usart.h"
#include "cJSON.h"

/* 全局变量定义 */
char nowchess[BOARD_SIZE][BOARD_SIZE] = {{'0','0','0'},{'0','0','0'},{'0','0','0'}}; // 当前棋盘状态
char recvchess[BOARD_SIZE][BOARD_SIZE] = {{'0','0','0'},{'0','0','0'},{'0','0','0'}}; // 接收的棋盘状态
int board_updated = 0;  // 棋盘更新标志
int processing_move = 0; // 正在处理移动标志
int game_state = 0;     // 游戏状态

/* 外部变量引用 */
extern Motor motor1, motor2;
extern char MotorGoal_chess[];
extern char MotorGoal_place[];
extern int flag_gowhere;

/**
 * @brief  在OLED上显示棋盘状态
 * @param  board - 要显示的棋盘
 * @retval 无
 */
void DisplayChessboard(char board[BOARD_SIZE][BOARD_SIZE]) {
    OLED_Clear();
    OLED_ShowString(1, 1, "Chess Board:");
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char chessPiece[2] = {' ', '\0'};
            
            // 0表示空，1表示黑棋，2表示白棋
            if (board[i][j] == '1')
                chessPiece[0] = 'X'; // 黑棋用X表示
            else if (board[i][j] == '2')
                chessPiece[0] = 'O'; // 白棋用O表示
            else 
                chessPiece[0] = '.'; // 空位用.表示
                
            OLED_ShowString(i+2, j*2+1, chessPiece);
        }
    }
}

/**
 * @brief  比较两个棋盘状态，找出不同的位置
 * @param  board1 - 第一个棋盘
 * @param  board2 - 第二个棋盘
 * @param  row - 存储不同位置的行
 * @param  col - 存储不同位置的列
 * @retval 不同位置的数量，0表示完全相同
 */
int CompareChessboard(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE], int *row, int *col) {
    int diff_count = 0;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board1[i][j] != board2[i][j]) {
                if (diff_count == 0) {
                    *row = i;
                    *col = j;
                }
                diff_count++;
            }
        }
    }
    
    return diff_count;
}

/**
 * @brief  将棋盘状态发送到日志单片机
 * @param  board - 要发送的棋盘状态
 * @param  type - 消息类型，如"update"或"result"
 * @retval 无
 */
void SendChessboardToLogger(char board[BOARD_SIZE][BOARD_SIZE], const char* type) {
    cJSON *root = cJSON_CreateObject();
    cJSON *boardArray = cJSON_CreateArray();
    
    // 添加类型
    cJSON_AddStringToObject(root, "type", type);
    
    // 将棋盘转换为一维数组并添加到JSON
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char value[2] = {board[i][j], '\0'};
            cJSON_AddItemToArray(boardArray, cJSON_CreateString(value));
        }
    }
    
    // 添加棋盘数组到JSON对象
    cJSON_AddItemToObject(root, "board", boardArray);
    
    // 转换为字符串并发送
    char *json_string = cJSON_PrintUnformatted(root);
    if (json_string != NULL) {
        USART1_printf("%s\r\n", json_string);
        cJSON_free(json_string);
    }
    
    cJSON_Delete(root);
}

// 注：此处应为外部函数，在其他文件中定义的AI算法函数
extern int reckon(char board[BOARD_SIZE][BOARD_SIZE], int condition);

/**
 * @brief  处理新的移动请求，计算电脑的回应
 * @param  board - 当前棋盘状态
 * @param  player_row - 玩家下棋的行
 * @param  player_col - 玩家下棋的列
 * @retval 1-成功处理，0-处理失败
 */
int ProcessNewMove(char board[BOARD_SIZE][BOARD_SIZE], int player_row, int player_col) {
    // 检查玩家移动是否有效
    if (board[player_row][player_col] != '0') {
        return 0; // 该位置已有棋子
    }
    
    // 更新棋盘，玩家用'1'表示
    board[player_row][player_col] = '1';
    
    // 检查游戏是否结束
    int result = CheckGameResult(board);
    if (result != 0) {
        return result; // 游戏已结束
    }
    
    // 计算电脑的移动（使用reckon函数）
    int player_move = player_row * 3 + player_col + 1; // 转换为1-9的位置
    int computer_move = reckon(board, player_move);
    
    // 处理电脑移动结果
    if (computer_move >= 1 && computer_move <= 9) {
        // 成功计算出电脑移动
        int comp_row = (computer_move - 1) / 3;
        int comp_col = (computer_move - 1) % 3;
        
        // 设置电机移动到相应位置
        flag_gowhere = 0;  // 重置移动标志
        MotorGoal_chess[1] = comp_row + '0';  // 将行位置转换为字符
        MotorGoal_place[1] = comp_col + '0';  // 将列位置转换为字符
        MotorGoal_place[0] = '1';  // 设置只有一步移动
        
        // 启动电机
        motor1.enable = 1;
        motor2.enable = 1;
        motor1.t4_enable = 1;
        motor2.t4_enable = 1;
        Chess_MODE = Chess_Chess;  // 设置为下棋模式
        
        return 1;
    } else if (computer_move == -2) {
        // 玩家获胜
        game_state = GAME_LOSE; // 设置为输状态
        UpdateDisplayStatus(game_state);
        return GAME_LOSE;
    } else if (computer_move == 10) {
        // 电脑获胜
        game_state = GAME_WIN; // 设置为赢状态
        UpdateDisplayStatus(game_state);
        return GAME_WIN;
    } else if (computer_move == 11) {
        // 平局
        game_state = GAME_DRAW; // 设置为平局状态
        UpdateDisplayStatus(game_state);
        return GAME_DRAW;
    }
    
    return 0; // 处理失败
}

/**
 * @brief  检查游戏结果
 * @param  board - 当前棋盘状态
 * @retval 0-游戏继续，2-电脑赢，3-玩家赢，4-平局
 */
int CheckGameResult(char board[BOARD_SIZE][BOARD_SIZE]) {
    // 检查是否有玩家获胜
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 检查行
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '0') {
            return (board[i][0] == '1') ? GAME_LOSE : GAME_WIN; // 玩家赢或电脑赢
        }
        
        // 检查列
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '0') {
            return (board[0][i] == '1') ? GAME_LOSE : GAME_WIN; // 玩家赢或电脑赢
        }
    }
    
    // 检查对角线
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '0') {
        return (board[0][0] == '1') ? GAME_LOSE : GAME_WIN; // 玩家赢或电脑赢
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '0') {
        return (board[0][2] == '1') ? GAME_LOSE : GAME_WIN; // 玩家赢或电脑赢
    }
    
    // 检查平局（棋盘已满）
    int is_full = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == '0') {
                is_full = 0;
                break;
            }
        }
        if (!is_full) break;
    }
    
    if (is_full) return GAME_DRAW; // 平局
    
    return 0; // 游戏继续
}

/**
 * @brief  更新OLED显示游戏状态
 * @param  status - 游戏状态
 * @retval 无
 */
void UpdateDisplayStatus(int status) {
    OLED_Clear();
    
    switch (status) {
        case GAME_READY: // 等待开始
            OLED_ShowString(2, 1, "Ready to Start");
            break;
        case GAME_PLAYING: // 游戏进行中
            OLED_ShowString(2, 1, "Game in Progress");
            break;
        case GAME_WIN: // 电脑赢
            OLED_ShowString(2, 1, "You Win!");
            break;
        case GAME_LOSE: // 玩家赢
            OLED_ShowString(2, 1, "You Lost!");
            break;
        case GAME_DRAW: // 平局
            OLED_ShowString(2, 1, "Game Draw!");
            break;
    }
}

/**
 * @brief  解析从OpenMV接收的MQTT命令
 * @param  data - 接收到的数据
 * @retval 1-成功，-1-失败
 */
int8_t Parse_MqttCmd(uint8_t *data) {
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
 
    // 解析JSON数据  
    cJSON *root = cJSON_Parse(json_data);  
    if (root == NULL)
    {  
        USART2_printf("Failed to parse JSON data.\n");  
        free(json_data);  
        return -1;  
    }  
    
    // 获取棋盘数据
    cJSON *board = cJSON_GetObjectItemCaseSensitive(root, "board");
    cJSON *pieces = cJSON_GetObjectItemCaseSensitive(root, "pieces");
    
    // 如果有棋盘数据
    if (board != NULL && cJSON_IsArray(board)) {
        // 检查数组长度是否符合预期
        int board_size = cJSON_GetArraySize(board);
        if (board_size == BOARD_SIZE * BOARD_SIZE) {
            // 当处于移动状态时不接受新的棋盘更新
            if (processing_move) {
                USART2_printf("Moving in progress, ignoring board update.\n");
                cJSON_Delete(root);
                free(json_data);
                return 0;
            }
            
            // 把接收到的棋盘数据保存到recvchess
            int index = 0;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    cJSON *value = cJSON_GetArrayItem(board, index++);
                    if (cJSON_IsString(value)) {
                        recvchess[i][j] = value->valuestring[0];
                    }
                }
            }
            
            // 比较棋盘状态
            int row = 0, col = 0;
            int diff_count = CompareChessboard(nowchess, recvchess, &row, &col);
            
            // 如果只有一个位置不同，处理这个移动
            if (diff_count == 1) {
                // 检查是否为游戏开始
                int is_empty = 1;
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        if (nowchess[i][j] != '0') {
                            is_empty = 0;
                            break;
                        }
                    }
                    if (!is_empty) break;
                }
                
                if (is_empty && recvchess[row][col] == '1') {
                    // 游戏开始，玩家先手
                    USART2_printf("Game started with player's move at [%d,%d]\n", row, col);
                    game_state = GAME_PLAYING; // 设置为游戏进行中
                    
                    // 复制棋盘状态
                    for (int i = 0; i < BOARD_SIZE; i++) {
                        for (int j = 0; j < BOARD_SIZE; j++) {
                            nowchess[i][j] = recvchess[i][j];
                        }
                    }
                    
                    // 发送初始棋盘状态到日志
                    SendChessboardToLogger(nowchess, "update");
                    
                    // 处理游戏移动
                    processing_move = 1;
                    int result = ProcessNewMove(nowchess, row, col);
                    
                    // 更新OLED显示
                    if (result == 0) {
                        // 处理失败
                        USART2_printf("Failed to process move.\n");
                        UpdateDisplayStatus(game_state);
                    } else {
                        // 显示当前棋盘
                        DisplayChessboard(nowchess);
                    }
                    
                    processing_move = 0;
                } else if (game_state == GAME_PLAYING && recvchess[row][col] != nowchess[row][col]) {
                    // 游戏进行中，玩家进行了移动
                    USART2_printf("Player moved at [%d,%d]\n", row, col);
                    
                    // 复制棋盘状态
                    for (int i = 0; i < BOARD_SIZE; i++) {
                        for (int j = 0; j < BOARD_SIZE; j++) {
                            nowchess[i][j] = recvchess[i][j];
                        }
                    }
                    
                    // 发送更新后的棋盘状态到日志
                    SendChessboardToLogger(nowchess, "update");
                    
                    // 处理游戏移动
                    processing_move = 1;
                    int result = ProcessNewMove(nowchess, row, col);
                    
                    // 更新OLED显示
                    if (result >= GAME_WIN) {
                        // 游戏结束
                        game_state = result;
                        UpdateDisplayStatus(game_state);
                        SendChessboardToLogger(nowchess, "result");
                    } else if (result == 0) {
                        // 处理失败
                        USART2_printf("Failed to process move.\n");
                        UpdateDisplayStatus(game_state);
                    } else {
                        // 显示当前棋盘
                        DisplayChessboard(nowchess);
                    }
                    
                    processing_move = 0;
                }
            } else if (diff_count > 1) {
                // 多处差异，可能是重置或者非法移动
                USART2_printf("Multiple board differences detected: %d\n", diff_count);
                
                // 检查是否全部为空（重置）
                int is_all_empty = 1;
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        if (recvchess[i][j] != '0') {
                            is_all_empty = 0;
                            break;
                        }
                    }
                    if (!is_all_empty) break;
                }
                
                if (is_all_empty) {
                    // 重置游戏
                    USART2_printf("Game reset detected.\n");
                    for (int i = 0; i < BOARD_SIZE; i++) {
                        for (int j = 0; j < BOARD_SIZE; j++) {
                            nowchess[i][j] = '0';
                        }
                    }
                    game_state = GAME_READY; // 设置为等待开始
                    UpdateDisplayStatus(game_state);
                }
            }
        }
    }
    
    // 释放资源 
    cJSON_Delete(root);  
    free(json_data);  
    return 1;  
} 