
#include "UartData.h"  

////        static int t4_chess=0;        
////        //标志位复位键
////		if(data_reset == motorReset){
////			data_reset = 0;
////			flag_gowhere=0;
////            Chess_MODE = 0;
////			dogdog=0,nowtime1=0,nowtime2=0;
////			motor1.stepnum=0;motor2.stepnum=0;
////			motor1.goal_1=0;motor2.goal_1=0;
////			motor1.enable=0;motor2.enable=0;
////			motor1.mode=0;motor2.mode=0;
////            motor1.t4_enable=0;motor2.t4_enable=0;
////		
////            flag_rtos = 0;
////        }
////        //t4复位键
////		if(data_reset==motor_t4_Reset){
////			data_reset = 0;
////			flag_gowhere=0;
////            Chess_MODE = 0;
////			dogdog=0,nowtime1=0,nowtime2=0;
////			motor1.stepnum=0;motor2.stepnum=0;
////			motor1.goal_1=0;motor2.goal_1=0;
////			motor1.enable=0;motor2.enable=0;
////			motor1.mode=0;motor2.mode=0;
////            t4_chess = 0;
////            motor1.t4_enable=0;motor2.t4_enable=0;
////            for (int row = 0; row < 3; row++) {  
////                for (int col = 0; col < 3; col++) {  
////                   board[row][col] = '0' ; 
////                }  
////            }
////        }
////        //电机使能键
////		if(data_motor1enable == motorEnable){
////			motor1.enable=1;
////		}
////		if(data_motor2enable == motorEnable){
////			motor2.enable=1;
////		}
////		//分题号
////        switch(data_testnum){
////            case '0':{//调试模式
////                Chess_MODE = Chess_debug;
////                motor1.goal_1 = (data_M1_3-48)*100+(data_M1_2-48)*10+(data_M1_1-48);
////                motor2.goal_1 = (data_M2_3-48)*100+(data_M2_2-48)*10+(data_M2_1-48);
////                
////                break;
////            }
////            case '1':{//第1-3题

////                Chess_MODE = Chess_onlymove;
////                
////                MotorGoal_chess[0]=data_amount-48;
////                MotorGoal_place[0]=data_amount-48;

////                MotorGoal_chess[1]=data_chess1;
////                MotorGoal_place[1]=data_place1;
////                MotorGoal_chess[2]=data_chess2;
////                MotorGoal_place[2]=data_place2;
////                MotorGoal_chess[3]=data_chess3;
////                MotorGoal_place[3]=data_place3;
////                MotorGoal_chess[4]=data_chess4;
////                MotorGoal_place[4]=data_place4;
////                
//////                for(int j=1;j<=MotorGoal_chess[0];j++){
//////                    printf("%c ",MotorGoal_chess[j]);
//////                    printf("%c \n",MotorGoal_place[j]);
//////                }    
////                break;
////            }
////            case '2':{//人机对弈题,黑先
////                Chess_MODE = Chess_Chess;
////                
////                if(data_t4mode==data_t4_start){
////                    //只进行一轮
////                    MotorGoal_chess[0]=1;
////                    MotorGoal_place[0]=1;
////                    MotorGoal_chess[1]=t4_chess+48;
////                    MotorGoal_place[1]=data_t4_place;
////                    motor1.t4_enable = 1;
////                    motor2.t4_enable = 1;
////                    printf("go:%c %c\n",MotorGoal_chess[1],MotorGoal_place[1]);
////                    
////                    int t4_s_l = (data_t4_place-48-1)%3;
////                    int t4_s_r = (data_t4_place-48-1)/3;
////                    board[t4_s_r][t4_s_l]='2';
////                    print_board(board);
////                }
////                
////                if(data_t4mode==data_t4_response){
////                    int t4_r_num = reckon(board,data_t4_place-48);
////                    if(t4_r_num<=9 && t4_r_num>0){
////                        MotorGoal_chess[0]=1;
////                        MotorGoal_place[0]=1;
////                        t4_chess++;
////                        if(t4_chess==5)
////                            t4_chess=0;
////                        MotorGoal_chess[1]=t4_chess+48;
////                        MotorGoal_place[1]=t4_r_num+48;
////                        printf("go:%c %c\n",MotorGoal_chess[1],MotorGoal_place[1]);
////                        motor1.t4_enable = 1;
////                        motor2.t4_enable = 1;
////                        print_board(board);
////                    }
////                    else{
////                        switch(t4_r_num){
////                            case -1:
////                                printf("repeat! %d \n",t4_r_num);
////                                //print_board(board);
////                                break;
////                            case -2:
////                                printf("player win?\n");
////                                print_board(board);
////                                break;
////                            default:
////                                printf("error..? %d \n",t4_r_num);
////                                print_board(board);
////                        }
////                    }            
////                }
////                break;
////            }
////            case '3':{//人机对弈题,白先
////                break;
////            }
////            default:
////                break;
////        }
////        
