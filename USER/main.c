/******************** (C) COPYRIGHT 2014 POWSOS Team **************************
 * 文件名  ：main.c
 * 描述    :     
 * 实验平台：STM32F103RBT6
 * 库版本  ：ST3.5.0
 * 作者    :  
 * 版本    ：V2.0
 * 日期    ：2014.8.23
 * 修订历史：V2.0
******************************************************************************/

#include "stm32f10x.h"
#include "iic.h"
#include "timer.h"
#include "usart.h"
#include "mpu6050.h"
#include "filter.h"
#include "calculate.h"
#include "gpio.h"
#include "time_test.h"
#include "hc_sr04.h"
#include "delay.h"
#include <stdio.h>
#include <math.h>


//#define Debug  

#define GX_OFFSET 0x01
#define AX_OFFSET 0x01
#define AY_OFFSET 0x01
#define AZ_OFFSET 0x01


#define duoji_offset  120

extern u8  duoji_flag;
extern u8 duoji_cnt;
extern u16 time;
extern u8 duoji_pwm;

u8 hcsr04_test_flag = 0;
u8 receive_data;
u8 flg_get_senor_data;
u8 out[35]  ={0x5f, 0x60, 0};
u8 Duoji_direction = 1;  /*1,前方,2:左边   3:右边,舵机*/
u8 Move_direcetion = 1;  /*1静止  2,前方, 3,后退 4:左边   5:右边 小车运行方向*/
u16 distance =0 ;
int  pulsewidth;
float angle, angle_dot, f_angle, f_angle_dot;
s16 temp; 
s16 gx, gy, gz, ax ,ay, az, temperature;

#define FILTER_COUNT  16
s16 gx_buf[FILTER_COUNT], ax_buf[FILTER_COUNT], ay_buf[FILTER_COUNT],az_buf[FILTER_COUNT];
/******************************************************************************/
void delay(u32 count)
{
  for(; count != 0; count--);
}
/*************************************************

名称：void acc_filter(void)
功能：加速度计数据滤波
输入参数：据滤波后的数据
输出参数：无
返回值：  无
**************************************************/
void acc_filter(void)
{
  u8 i;
  s32 ax_sum = 0, ay_sum = 0, az_sum = 0; 

  for(i = 1 ; i < FILTER_COUNT; i++)
  {
    ax_buf[i - 1] = ax_buf[i];
	ay_buf[i - 1] = ay_buf[i];
	az_buf[i - 1] = az_buf[i];
  }

  ax_buf[FILTER_COUNT - 1] = ax;
  ay_buf[FILTER_COUNT - 1] = ay;
  az_buf[FILTER_COUNT - 1] = az;

  for(i = 0 ; i < FILTER_COUNT; i++)
  {
    ax_sum += ax_buf[i];
	ay_sum += ay_buf[i];
	az_sum += az_buf[i];
  }

  ax = (s16)(ax_sum / FILTER_COUNT);
  ay = (s16)(ay_sum / FILTER_COUNT);
  az = (s16)(az_sum / FILTER_COUNT);
}

/* I/O口模拟输出PWM控制舵机,50hz */
void servopulse(int myangle)//定义一个脉冲函数
{ 
	 // EA = 0;
	//  pulsewidth = (((myangle+duoji_offset)*25)+500)/1000;;// 舵机中心值可能会偏，修改20,做调整
	pulsewidth =myangle;

}

/*****************************************************************************/
int main(void)
{
	SystemInit();
	delay_init(72);
	gpio_init();	
 	delay(0x80000);
  usart_init();	 				 
  iic_init();
  timer_init();
  TIM1_Configuration();
  HCSR04_Init();
  motor_init();
	mpu6050_init();
	STOP_TIME;
	duoji_flag =1;
	duoji_cnt = 0;
	servopulse(3);/*上电将舵机放至中间*/
 	 while (1)
 	{
   	 if(flg_get_senor_data)
    	{
      	flg_get_senor_data = 0;
      	mpu6050_get_data(&gx, &gy, &gz, &ax, &ay , &az, &temperature);
	  	acc_filter();	

	  	gx-=  GX_OFFSET;
	  	ax -= AX_OFFSET; 
	  	ay -=	AY_OFFSET;
	  	az -= AZ_OFFSET;

	    angle_dot = gx * GYRO_SCALE;  //+-2000  0.060975 °/LSB   //陀螺仪
      	angle = atan(ay / sqrt(ax * ax + az * az ));
	     
     	angle = angle * 57.295780;    //180/pi


		 kalman_filter(angle, angle_dot, &f_angle, &f_angle_dot);//     加速度计计算的角度, 陀螺仪角速度 , 融合后的角度, 融合后的角速度
	  	 receive_parameter(receive_data);

	  	 pid(f_angle, f_angle_dot);

#ifdef  Debug
   		   	temp = (s16)(f_angle * 100);
   	  
				 out[2] = (u8)(gx >> 8);
		    out[3] = (u8)(gx);
				 out[4] = (u8)(gy >> 8);
				 out[5] = (u8)(gy);
				 out[6] = (u8)(gz >> 8);
				 out[7] = (u8)(gz);
				 out[8] = (u8)(ax >> 8);
				 out[9] = (u8)(ax);
				 out[10] = (u8)(ay >> 8);
				 out[11] = (u8)(ay);
				 out[12] = (u8)(az >> 8);
				 out[13] = (u8)(az);
			   out[14] = (u8)(temp >> 8);
				 out[15] = (u8)(temp);

				USART_SendStringData(USART1,&out[0],16);

#endif
	  
			
   	 }  //end if		
	
		
    if(hcsr04_test_flag)
    {
		 hcsr04_test_flag=0;		 
		 measure_distance(receive_data);
					 
		 switch(Duoji_direction)
		 {
			case Duoji_Front:	USART_printf(USART2,"Front_distance: %d cm\r\n",distance);break;
			case Duoji_Left:	USART_printf(USART2,"Left_distance: %d cm\r\n",distance);break;
			case Duoji_Right:	USART_printf(USART2,"Right_distance: %d cm\r\n",distance);break;
		 }
		 
		 
	}
	#if  0
	/*若前方距离小于100,并且小车行驶方向是往前的，此时小车需要停止*/	
	if((distance<50)&&(Move_direcetion==Move_front))
	{				
		receive_data ='s';
	}
		
	#endif

  }  
	
}

/*****************END OF FILE************************************************************/
