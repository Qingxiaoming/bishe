# 围棋机器人控制系统

这是一个基于STM32F10x和FreeRTOS的自动化围棋机器人控制系统。该系统使用步进电机控制机械臂移动，电磁铁进行棋子抓取，并通过OLED显示界面和串口通信实现用户交互。

## 系统架构

系统基于FreeRTOS实现多任务并行处理：

- **DOG_Task**: 负责处理JSON数据通信(1s执行一次)
- **MOTOR_Task**: 控制步进电机运动(20ms执行一次)
- **Electro_Task**: 管理电磁铁控制和OLED界面(20ms执行一次)

## 串口通信协议

系统支持两种通信方式：

### 1. 标准串口数据包（USART1）

数据包格式：`D!` + 命令 + 参数 + `O`

- **包头**：`D!`
- **包尾**：`O`

#### 命令类型

##### 调试模式(T0)
```
D! R T0 M=E,E 111,123 O
```
- `R`: 复位指令
- `T0`: 测试模式0
- `M=E,E`: 电机1和电机2使能状态
- `111,123`: 电机1和电机2的位置目标

##### 棋子移动模式(T1)
```
D! R T1 M=E,E 4:[5,1][2,5][8,8][9,9] O
```
- `R`: 复位指令
- `T1`: 测试模式1
- `M=E,E`: 电机1和电机2使能状态
- `4`: 移动棋子数量
- `[5,1][2,5][8,8][9,9]`: 棋子位置和目标位置坐标

##### 对弈模式(T2)
```
D! R T2 M=E,E S5 O
D! R T2 M=E,E R1 O
```
- `R`: 复位指令
- `T2`: 测试模式2
- `M=E,E`: 电机1和电机2使能状态
- `S5`: 开始对弈，位置5
- `R1`: 响应对弈，位置1

### 2. JSON数据通信（供DOG_Task处理）

系统可以接收和解析JSON格式的数据：
```json
{
  "name": "命令名称",
  "age": 数值参数,
  "is_running": 运行状态
}
```

示例发送的JSON数据：
```json
{"params":{"temp":23.5,"o2":21}}
```

### 3. OLED界面控制（USART2）

通过USART2可以发送以下单字符命令控制OLED界面：

- `a`: 点击/确认
- `b`: 向上移动
- `c`: 向下移动
- `d`: 返回

## 电机控制状态机

每个电机有一个状态机控制其运动：

1. **状态0**: 初始化状态，记录当前时间
2. **状态1**: 延时后设置电机前进，进入状态2
3. **状态2**: 电机运动到goal_1位置，进入状态3
4. **状态3**: 延时后设置电机方向，进入状态4
5. **状态4**: 电机运动到goal_2位置，进入状态5
6. **状态5**: 延时后设置电机后退，进入状态6
7. **状态6**: 电机运动回到原点，进入状态7
8. **状态7**: 延时后完成一个完整移动周期，重置为状态0

## 工作模式

系统有三种主要工作模式：

1. **调试模式** (`Chess_debug`): 用于测试电机精确运动
2. **棋子移动模式** (`Chess_onlymove`): 单纯移动棋子
3. **对弈模式** (`Chess_Chess`): 完整实现象棋走子功能

## 硬件连接

### 串口通信
- **USART1**: 
  - TX: PA9
  - RX: PA10
  - 功能: 接收控制命令
  - 波特率: 9600bps

- **USART2**: 
  - TX: PA2
  - RX: PA3
  - 功能: 调试输出和OLED界面控制
  - 波特率: 9600bps
  
- **USART3**: 
  - TX: PB10
  - RX: PB11
  - 功能: 辅助通信接口
  - 波特率: 用户可配置

### OLED显示屏 (I2C协议)
- SCL: PB10
- SDA: PB11
- 从机地址: 0x78

### 步进电机控制
- **电机1 (行/Row方向电机)**
  - 使能引脚: PA2
  - 方向引脚: PA1
  - PWM输出: PA6 (TIM3_CH1)

- **电机2 (列/Column方向电机)**
  - 使能引脚: PA3
  - 方向引脚: PA4
  - PWM输出: PA7 (TIM3_CH2)

### 其他外设
- **Timer1**: 用于系统时基
- **Timer3**: 用于步进电机PWM控制

## 使用方法

1. 通过串口1发送控制命令
2. 通过串口2调试或控制OLED界面
3. 观察OLED显示界面获取系统状态信息 




## 需要实现功能
uart2接收openmv数据（json）：
2s一次的棋盘棋子位置（是坐标而非实际值，如九宫格为{0，0，0，2，2，1，0，1，0}，棋子为{0，1，1，1，1，2，0，0，2，0}，规定0为空1为黑2为白）
单片机收到数据后与nowchess做对比，如果监测到只与nowchess有规则内的不一样（目前是只有一处不一样，需要预留围棋规则的接口），则：1将新获取到的棋盘用uart1发送给日志单片机，格式为{update：{此时的棋盘}}，2：不再认为openmv后续的更新有效，3，将此棋盘放入数据处理函数，此函数的输入为棋盘类型，需求角色（需要输出黑/白子走法），棋盘本体，输出为需求角色走完后的棋盘和走的那一步，4，将处理函数的返回值存回nowchess，并输出给日志单片机，5，将走的那一步交给电机控制，根据需求修改Motor_Chess函数，实现电机移动到指定位置放棋的操作，6，判断是否已经（输/赢），在第三步之前也要判断一次，可以参考Reckon文件夹里废案的实现，7，将棋盘更新到oled显示出来，oled现有一个较完整的框架，我们的棋盘应该绘制在画图选项中并全程维持，8，当一切完成后，接受openmv的更新
上述是一步棋的走法，而当棋盘为空或已经有胜负时，在oled显示start或lost或win文字提醒
忽略目前已有的通信格式的实现，所有功能以上面描述的为准，上述功能还没有实现的需要你写出来，和目前项目有冲突的则更改目前项目，详细阅读项目并做更改。