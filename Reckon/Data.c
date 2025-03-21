
#include "Data.h"


//extern int KeyNum,Old_KeyNum;
//extern int Key_send_flag,Key_color_flag,Key_chess_place;
//extern int	Key_testnum,Key_place_flag,Key_state_flag,Key_Howtime;
//extern int Key_mode,modeLock;
//extern int testsendLock;
//extern int uart_lock;
//extern int DATA_key[4][4];

int KeyNum=0,Old_KeyNum=0;
int Key_send_flag=0,Key_color_flag=0,Key_chess_place=0;
int	Key_testnum=0,Key_place_flag=0,Key_state_flag,Key_Howtime;
int Key_mode=0,modeLock=0;
int testsendLock=0;
int uart_lock=0;
int DATA_key[4][4];
void show_OLED_keyitem(){
			switch(Key_mode){
			case 0:
				OLED_ShowString(1,1,"mode_0          ");
			  OLED_ShowNum(1, 10, modeLock, 1);	  
			  OLED_ShowString(3,1,"                ");
			  OLED_ShowString(4,1,"                ");
			  if(Old_KeyNum==6)
					OLED_ShowString(2,1,"send success");
				else
					OLED_ShowString(2,1,"                ");
				break;
			case 1:
				OLED_ShowString(1,1,"mode_1          ");
			  OLED_ShowNum(1, 10, modeLock, 1);
			  OLED_ShowNum(1, 14, Key_Howtime, 1);
			  OLED_ShowString(2,1,"color_flag:");
			  OLED_ShowNum(2, 13, Key_color_flag,1);
			  OLED_ShowString(3,1,"chess_place:");
			  OLED_ShowNum(3, 13, Key_chess_place,2);
			  OLED_ShowString(4,1,"                ");
				break;
			case 2:
				OLED_ShowString(1,1,"mode_2          ");
			  OLED_ShowNum(1, 10, modeLock, 1);
			  OLED_ShowNum(1, 14, Key_Howtime, 1);
			  OLED_ShowString(2,1,"0 or 45: ");
			  OLED_ShowNum(2, 13, Key_testnum,1);
			  OLED_ShowString(3,1,"place_flag: ");
			  OLED_ShowNum(3, 13, Key_place_flag,2);
			  OLED_ShowString(4,1,"                ");
				break;
			case 3:
			  OLED_ShowString(1,1,"mode_3          ");
			  OLED_ShowNum(1, 10, modeLock, 1);
			  OLED_ShowNum(1, 14, Key_Howtime, 1);
			  OLED_ShowString(2,1,"state_flag: ");
			  OLED_ShowNum(2, 13, Key_state_flag,1);
			  OLED_ShowString(3,1,"Key_Howtime:");
			  OLED_ShowNum(3, 13, Key_Howtime,2);
			
			  OLED_ShowNum(4, 5, Key_color_flag,1);
			  OLED_ShowNum(4, 7, Key_chess_place,2);
			  OLED_ShowNum(4, 10, Key_testnum,1);
			  OLED_ShowNum(4, 12, Key_place_flag,1);
				break;
			case 4:
				OLED_ShowString(1,1,"mode_4          ");
			  OLED_ShowNum(1, 10, modeLock, 1);
				OLED_ShowString(2,1,"send success!   ");
			  OLED_ShowString(3,1,"                ");
			  OLED_ShowString(4,1,"                ");
				break;
			case -1:
				Key_send_flag=0;
				Key_color_flag=0;
				Key_chess_place=0;
				Key_testnum=0;
				Key_place_flag=0;
				Key_state_flag=0;
				Key_Howtime=0;
				Key_mode=0;
				modeLock=0;
			break;
			case 5:
				Key_send_flag=0;
				Key_color_flag=0;
				Key_chess_place=0;
				Key_testnum=0;
				Key_place_flag=0;
				Key_state_flag=0;
				Key_Howtime=0;
				Key_mode=0;
				modeLock=0;
			break;
		}
	
}

void keyitem(){
		
		KeyNum = MatrixKey_GetValue();
		if(Old_KeyNum!=KeyNum && KeyNum!=0)
			Old_KeyNum = KeyNum;
		//OLED_ShowNum(1, 1, Old_KeyNum, 2);
		//
		
		//״̬1:
	  if(MatrixKey_GetValue()==2){
			
			if(Old_KeyNum==15 && modeLock==0){
				modeLock=1;
				Key_mode++;
			}
			if(Old_KeyNum==14 && modeLock==0){
				modeLock=1;
				Key_mode--;
			}
			if(Old_KeyNum==13){
				modeLock=0;
			}
			 
			
			 if(Old_KeyNum==16){
				 Key_send_flag=0;
				 Key_color_flag=0;
				 Key_chess_place=0;
				 Key_testnum=0;
				 Key_place_flag=0;
				 Key_state_flag=0;
				 Key_Howtime=0;
				 Key_mode=0;
				 modeLock=0;
				 testsendLock=0;
			 }
			 if(Key_mode==0){
				 	switch(Old_KeyNum){
						case 6: printf("Player ready!");break;
				  }						
			 }
			 
			 if(Key_mode==1){
				  switch(Old_KeyNum){
						case 12: Key_color_flag=1;break;
						case 8:  Key_color_flag=2;break;
						case 11: Key_chess_place=1;break;
						case 10: Key_chess_place=2;break;
						case 9:  Key_chess_place=3;break;
						case 7:  Key_chess_place=4;break;
						case 6:  Key_chess_place=5;break;
						case 5:  Key_chess_place=6;break;
						case 4:  Key_chess_place=7;break;
						case 3:  Key_chess_place=8;break;
						case 2:  Key_chess_place=9;break;
						case 1:  Key_chess_place=10;break;
				  }						
			 }
			 if(Key_mode==2){
				  switch(Old_KeyNum){
						case 12: Key_testnum=1;break;
						case 8:  Key_testnum=2;break;
						case 11: Key_place_flag=1;break;
						case 10: Key_place_flag=2;break;
						case 9:  Key_place_flag=3;break;
						case 7:  Key_place_flag=4;break;
						case 6:  Key_place_flag=5;break;
						case 5:  Key_place_flag=6;break;
						case 3:  Key_place_flag=7;break;
						case 2:  Key_place_flag=8;break;
						case 1:  Key_place_flag=9;break;
				  }	

			 }
			 if(Key_mode==3){
				 
					 switch(Old_KeyNum){
						case 12: Key_state_flag=1;break;
						case 8:  Key_state_flag=2;break;
						case 4:  Key_state_flag=3;break;
						case 6:  Key_Howtime=0;break;
						case 5:  Key_Howtime=1;break;
						case 2:  Key_Howtime=2;break;
						case 1:  Key_Howtime=3;break;
				  }						
					DATA_key[Key_Howtime][0]=Key_color_flag;
					DATA_key[Key_Howtime][1]=Key_chess_place;
					DATA_key[Key_Howtime][2]=Key_testnum;
					DATA_key[Key_Howtime][3]=Key_place_flag;
			 }
			 if(Key_mode==4){		
				 if(MatrixKey_GetValue()==1)
					 //printf("command:%1d %1d %1d %02d %1d %1d\n",1,1,Key_color_flag,Key_chess_place,Key_testnum,Key_place_flag);
					switch(Key_state_flag){
						case 0:
							Key_mode=3;
						case 1: 
							if(testsendLock < 4){
								testsendLock++;
								//printf("command:%1d %1d %1d %02d %1d %1d\n",1,1,Key_color_flag,Key_chess_place,Key_testnum,Key_place_flag);
							}
		 					break;
						case 2:  
							Key_mode=0;
						  break;
						case 3:  
							for(int j=0;j<4;j++){
								if(testsendLock < 4){
									testsendLock++;
									//printf("command:%1d %1d %1d %02d %1d %1d\n",4,j+1,DATA_key[j][0],DATA_key[j][1],DATA_key[j][2],DATA_key[j][3]);
								}
								
							}
						  break;
				  }
			 }
		 }
}


