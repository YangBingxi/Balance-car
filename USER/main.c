/******************** (C) COPYRIGHT 2014 POWSOS Team **************************
 * �ļ���  ��main.c
 * ����    :     
 * ʵ��ƽ̨��STM32F103RBT6
 * ��汾  ��ST3.5.0
 * ����    :  
 * �汾    ��V2.0
 * ����    ��2014.8.23
 * �޶���ʷ��V2.0
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
u8 Duoji_direction = 1;  /*1,ǰ��,2:���   3:�ұ�,���*/
u8 Move_direcetion = 1;  /*1��ֹ  2,ǰ��, 3,���� 4:���   5:�ұ� С�����з���*/
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

���ƣ�void acc_filter(void)
���ܣ����ٶȼ������˲�
������������˲��������
�����������
����ֵ��  ��
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

/* I/O��ģ�����PWM���ƶ��,50hz */
void servopulse(int myangle)//����һ�����庯��
{ 
	 // EA = 0;
	//  pulsewidth = (((myangle+duoji_offset)*25)+500)/1000;;// �������ֵ���ܻ�ƫ���޸�20,������
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
	servopulse(3);/*�ϵ罫��������м�*/
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

	    angle_dot = gx * GYRO_SCALE;  //+-2000  0.060975 ��/LSB   //������
      	angle = atan(ay / sqrt(ax * ax + az * az ));
	     
     	angle = angle * 57.295780;    //180/pi


		 kalman_filter(angle, angle_dot, &f_angle, &f_angle_dot);//     ���ٶȼƼ���ĽǶ�, �����ǽ��ٶ� , �ںϺ�ĽǶ�, �ںϺ�Ľ��ٶ�
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
	/*��ǰ������С��100,����С����ʻ��������ǰ�ģ���ʱС����Ҫֹͣ*/	
	if((distance<50)&&(Move_direcetion==Move_front))
	{				
		receive_data ='s';
	}
		
	#endif

  }  
	
}

/*****************END OF FILE************************************************************/
