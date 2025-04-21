#ifndef __USARTDATA_H__
#define __USARTDATA_H__


//////调试模式
//////D! R T0 M=E,E 111,123 O
////#define data_reset        RxBuffer[3]
////#define data_testnum      RxBuffer[6]
////#define data_motor1enable RxBuffer[10]	
////#define data_motor2enable RxBuffer[12]	
////#define data_M1_3         RxBuffer[14]
////#define data_M1_2 	      RxBuffer[15]	
////#define data_M1_1 	      RxBuffer[16]	
////#define data_M2_3 	      RxBuffer[18]	
////#define data_M2_2 	      RxBuffer[19]
////#define data_M2_1 	      RxBuffer[20]	

//////第1-3题
//////D! R T1 M=E,E 4:[5,1][2,5][8,8][9,9] O
////#define data_reset        RxBuffer[3]
////#define data_testnum      RxBuffer[6]
////#define data_motor1enable RxBuffer[10]	
////#define data_motor2enable RxBuffer[12]	
////#define data_amount       RxBuffer[14]
////#define data_chess1 	  RxBuffer[17]	
////#define data_place1 	  RxBuffer[19]	
////#define data_chess2 	  RxBuffer[22]	
////#define data_place2 	  RxBuffer[24]
////#define data_chess3 	  RxBuffer[27]	
////#define data_place3 	  RxBuffer[29]
////#define data_chess4 	  RxBuffer[32]	
////#define data_place4 	  RxBuffer[34]

//////第四五题：
//////黑先，在白失误的情况下赢
//////白先，黑保证不输
//////D! R T2 M=E,E S5 O
//////D! R T2 M=E,E R1 O
////#define data_reset        RxBuffer[3]
////#define data_testnum      RxBuffer[6]
////#define data_motor1enable RxBuffer[10]	
////#define data_motor2enable RxBuffer[12]	
////#define data_t4mode       RxBuffer[14]
////#define data_t4_start     'S'
////#define data_t4_response  'R'
////#define data_t4_place     RxBuffer[15]	


////extern u16 flag_rtos;
////extern u16	flag_rtos_key1 ;
////extern u16	flag_rtos_key2 ;

////extern u16  MotorGoal_chess[6];
////extern u16  MotorGoal_place[6];

////extern u16 Chess_MODE;





#endif
