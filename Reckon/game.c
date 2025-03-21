#include "game.h" 



void print_board(char board[SIZE][SIZE]) {  
    for (int i = 0; i < SIZE; i++) {  
        for (int j = 0; j < SIZE; j++) {  
            printf("%c ", board[i][j]);  
        }  
        printf("\n");  
    }    
}  
  
int is_valid_move(char board[3][3], int row, int col) {  
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == '0';  
}  
  
int is_win(char board[3][3], char player) {  
    // 检查行  
    for (int i = 0; i < SIZE; i++) {  
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {  
            return 1;  
        }  
    }  
    // 检查列  
    for (int j = 0; j < SIZE; j++) {  
        if (board[0][j] == player && board[1][j] == player && board[2][j] == player) {  
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


int compareMatrix(char matrix1[3][3], char matrix2[3][3]) {  
  for (int i = 0; i < 3; i++) {  
    for (int j = 0; j < 3; j++) {  
      if (matrix1[i][j] != matrix2[i][j]) {  
        return 0; // 矩阵不同  
      }  
    }  
  }  
  return 1; // 矩阵相同  
}  

  
int computer_move(char board[ROW][COL], char computer_mark,char player_mark) {  
    char best_row = '3', best_col = '3';  
    int player = computer_mark; 
	int oppoplayer = player_mark;
	int flag_find = 0;
	char board_1[3][3] = {'0',oppoplayer,'0','0',player,'0','0','0','0'}; //2,0
	char board_2[3][3] = {'0','0','0','0',player,'0','0',oppoplayer,'0'}; //0,0
	char board_3[3][3] = {'0','0','0',oppoplayer,player,'0','0','0','0'}; //0,2
	char board_4[3][3] = {'0','0','0','0',player,oppoplayer,'0','0','0'}; //0,0

    // 优先检查是否有立即获胜的走法  
    for (int row = 0; row < SIZE; row++) {  
        for (int col = 0; col < SIZE; col++) {  
            if (board[row][col] == '0' && flag_find==0) { 
                board[row][col] = player; 
                if (is_win(board, player) ) {  
                    best_row = row;  
                    best_col = col;  
                    flag_find = 1;
                }  
                board[row][col] = '0';  
            }  
        }  
    }  
    //寻找是否有敌对玩家立即获胜的方法
		for (int row = 0; row < SIZE; row++) {  
        for (int col = 0; col < SIZE; col++) {  
            if (board[row][col] == '0' && flag_find==0) {   
							board[row][col] = oppoplayer; 
                if (is_win(board, oppoplayer) ) {  
                    best_row = row;  
                    best_col = col;  
                    flag_find = 1;
                }   
                board[row][col] = '0';  
            }  
        }  
    }  

    // 假设我们选择中心位置（如果可用）  
    if (board[1][1] == '0' && flag_find != 1) {  
        best_row = 1;  
        best_col = 1;  
		flag_find = 1;
    } 
		//写死先手胜(对面失误)前两步
    if (flag_find != 1) {  
        if(compareMatrix(board,board_1)){
            best_row = 2;  
            best_col = 0; 
            flag_find = 1;
        }
        if(compareMatrix(board,board_2)){
            best_row = 0;  
            best_col = 0; 
            flag_find = 1;
        }
        if(compareMatrix(board,board_3)){
            best_row = 0;  
            best_col = 2; 
            flag_find = 1;
        }
        if(compareMatrix(board,board_4)){
            best_row = 0;  
            best_col = 0; 
            flag_find = 1;
        }
    } 
    if (flag_find != 1) {  
        for (int row = 0; row < SIZE; row++) {  
            for (int col = 0; col < SIZE; col++) {  
                if (board[row][col] == '0') {  
                    best_row = row;  
                    best_col = col;  
                    break;  
                }  
            }  
        }  
    }  
    // 走棋  
    board[best_row][best_col] = player;  
	return (((int)best_row<<8)+((int)best_col));
}  


//返回值:
//1-9 有效值,输出机械臂姿态
//-1 输入值错误
//-2 玩家胜利
//10 装置胜利
//11 平局
int reckon(char board[SIZE][SIZE],int condition)
{
    char player_mark='1',computer_mark='2';  
	int best_step = -1;
	char best_row='0';
	char best_col='0';
    char t_r,t_l;
    int  t_f;

	
	//Serial_SendByte(condition);
	int temp_l = (condition-1)%3;
	int temp_r = (condition-1)/3;
	
	if(board[temp_r][temp_l]!= '0' || !(condition>=1&&condition<=9) )
		return -1;
  
	board[temp_r][temp_l] = player_mark;

	
	//玩家胜利
    if (is_win(board, player_mark)) {  
    return -2;  
	}
	
    
    
	//装置回合
	best_step = computer_move(board, computer_mark,player_mark);  
	
	best_row = (int)best_step>>8;
	best_col = (int)best_step&0xff;
	
	t_r = best_row;
	t_l = best_col;
	
    //printf("4444:%d %d\n",best_row,best_col);
    
	board[t_r][t_l]=computer_mark;
	
    
    

    //判断是否平局  
    int all_filled = 1;  
    for (int i = 0; i < 3; i++) {  
       for (int j = 0; j < 3; j++) {  
          if (board[i][j] == '0') {  
            all_filled = 0;  
            break;  
        }  
       }  
	  }
    if (all_filled) {  
        printf("nobody win...\n");
	}  
    
    //判断是否胜利
	if (is_win(board, computer_mark)) {  
        printf("is we win!!\n");
    }  
    
    t_f = (((int)t_r)*3 + ((int)t_l) +1);
    
	//返回机械臂姿态
	return t_f;
}  

