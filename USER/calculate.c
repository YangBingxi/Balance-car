/******************** (C) COPYRIGHT 2014 POWSOS Team **************************
 * 文件名  ：calculate.c
 * 描述    :     pid
 * 作者    : 
 * 版本    ：V2.0
 * 日期    ：2014.8.23
 * 修订历史：V2.0
******************************************************************************/

#include "stm32f10x.h"
#include "calculate.h"
/************************************************************/

extern u8 Move_direcetion;
s16 speed_l = 0;
s16 speed_r = 0;
s16 speed_need = 0;
s16 turn_need_r = 0;
s16 turn_need_l = 0;
s16 speed_out = 0;
u8 stop = 0;
u8 previous_cmd;

float rout;               
float set_point= 0; 
float now_error;
float proportion = 300;     
float integral2 = 0.01;        
float derivative = 0;
float derivative2 = 1;
double position;
double speed;

#define FILTER_COUNT  20
#define MAX_SPEED  5500
s16 speed_buf[FILTER_COUNT]={0};
/*************************************************

名称：motor_init(void)
功能：相关管脚及timer外设初始化（中断 定时时间）
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void motor_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
  													
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_9|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //GPIO_ResetBits(GPIOC, GPIO_Pin_6);  //AT1	 50% current
	GPIO_SetBits(GPIOC, GPIO_Pin_6); 
  GPIO_SetBits(GPIOB, GPIO_Pin_0);    //AT2
 // GPIO_SetBits(GPIOB, GPIO_Pin_6);    //ST 
  //GPIO_ResetBits(GPIOB, GPIO_Pin_7);  //OE
  GPIO_SetBits(GPIOB, GPIO_Pin_15);    //FR	 left side

  //GPIO_ResetBits(GPIOA, GPIO_Pin_5);  //AT1  50% current
	GPIO_SetBits(GPIOA, GPIO_Pin_5); 
  GPIO_SetBits(GPIOA, GPIO_Pin_4);    //AT2
  //GPIO_SetBits(GPIOA, GPIO_Pin_7);    //ST
  //GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //OE
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //FR	 right side

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  //left setp
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //right setp 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;  // (Period + 1) * (Prescaler + 1) / 72M = 1ms  
  TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 3600;

  TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //PB9  left

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);           

  /* TIM2 enable counter */
  TIM_Cmd(TIM4, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;    
  TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 3600;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //PA1	right

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM2, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);  
}
/*************************************************

名称：void receive_parameter(u8 cmd)
功能：处理串口接收到的遥控数据
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void receive_parameter(u8 cmd)
{
  switch(cmd)
  {
    case 'f':  // forward
			Move_direcetion = Move_front;
	  if(previous_cmd == 'b')
	  {
	    integral2 = 0.01;        
        derivative2 = 1;		       
	    speed_need = 0;
        turn_need_r = 0;
        turn_need_l = 0;
		stop = 200;  
	  }
	  else 
	  {
	    if(stop == 0)
		{
		  integral2 = 0.05;        
      derivative2 = 1.5;		       
	    speed_need = -10000;
      turn_need_r = 0;
      turn_need_l = 0;
		}
		else 
		{
		  stop--;
		  integral2 += 0.0002;        
          derivative2 += 0.0025;		       
	      speed_need += -50;
		}
	  }
    break;
    case 'b': // back
			Move_direcetion = Move_back;
	  if(previous_cmd == 'f')
	  {
	    integral2 = 0.01;        
        derivative2 = 1;		       
	    speed_need = 0;
        turn_need_r = 0;
        turn_need_l = 0;
		stop = 200;  
	  }
	  else 
	  {
	    if(stop == 0)
		{
		  integral2 = 0.05;        
          derivative2 = 1.5;		       
	      speed_need = 10000;
          turn_need_r = 0;
          turn_need_l = 0;
		}
		else 
		{
		  stop--;
		  integral2 += 0.0002;        
          derivative2 += 0.0025;		       
	      speed_need += 50;
		}
	  }
    break;
	case 'l':  // turn left
		Move_direcetion = Move_left;
	  integral2 = 0.03;        
      derivative2 = 1.5;       
	  speed_need = 0;
      turn_need_r = 500;
      turn_need_l = -500;
	  stop = 0;
    break;
	case 'r':  // turn right
		Move_direcetion = Move_right;
	  integral2 = 0.03;        
      derivative2 = 1.5; 	       
	  speed_need = 0;
      turn_need_r = -500;
      turn_need_l = 500;
	  stop = 0;
    break;
	case 's':  // stop
			Move_direcetion = Move_stop;
	  integral2 = 0.01;
	  derivative2 = 1;
      speed_need = 0;
      turn_need_r = 0;
      turn_need_l = 0;
	  stop = 0;
    break;
  }
	
  previous_cmd = cmd;
}
/*************************************************

名称：void speed_filter(void)
功能：速度滤波
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void speed_filter(void)
{
  u8 i;
  s32 speed_sum = 0; 

  for(i = 1 ; i < FILTER_COUNT; i++)
  {
    speed_buf[i - 1] = speed_buf[i];
  }

  speed_buf[FILTER_COUNT - 1] = ((speed_l + speed_r) / 2);

  for(i = 0 ; i < FILTER_COUNT; i++)
  {
    speed_sum += speed_buf[i];
  }
   
  speed_out = (s16)(speed_sum / FILTER_COUNT);
}
/*************************************************

名称：void pid(float angle, float angle_dot)
功能：PID运算
输入参数：
   	 float angle     倾斜角度
	 float angle_dot 倾斜角速度
输出参数：无
返回值：  无
**************************************************/
void pid(float angle, float angle_dot)
{
  u32 temp;
  u16 sl, sr;

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  now_error = set_point - angle;

  speed_filter();

  speed *= 0.7;
  speed += speed_out * 0.3;
  position += speed;
  position -= speed_need;
  
  if(position < -60000) 
  {
    position = -60000;
  }
  else if(position > 60000) 
  {
    position =  60000;
  }
	
  rout = proportion * now_error + derivative * angle_dot - position * integral2 - derivative2 * speed;
  
  speed_l = -rout + turn_need_l;
  speed_r = -rout + turn_need_r;
	
  if(speed_l > MAX_SPEED)	
  {
    speed_l = MAX_SPEED;	
  }
  else if(speed_l < -MAX_SPEED)	
  {
	speed_l = -MAX_SPEED;
  }

  if(speed_r > MAX_SPEED)	
  {
    speed_r = MAX_SPEED;	
  }
  else if(speed_r < -MAX_SPEED)	
  {
	speed_r = -MAX_SPEED;
  }
  
  if(speed_l > 0)
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);  //left fr
		sl = speed_l;
  }
  else	
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
	sl = speed_l * (-1);
  }

  if(speed_r > 0)
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_1);  //right fr
	sr = speed_r;
  }
  else	
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	sr = speed_r * (-1);
  }
  
  temp = 1000000 / sl;
  if(temp > 65535)
  {
    sl = 65535;
  }
  else 
  {
    sl = (u16)temp;
  }
  	
  temp = 1000000 / sr;
  if(temp > 65535)
  {
    sr = 65535;
  }
  else 
  {
    sr = (u16)temp;
  }	

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = sl - 1;  // (Period + 1) * (Prescaler + 1) / 72M = 1ms  
  TIM_TimeBaseStructure.TIM_Prescaler = 30 - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = sl >> 1;

  TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //PB9  left

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = sr - 1;    
  TIM_TimeBaseStructure.TIM_Prescaler = 30  - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = sr >> 1;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //PA1	right  
}
/***************************END OF FILE**********************************************************************/
