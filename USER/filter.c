
/******************** (C) COPYRIGHT 2014 POWSOS Team **************************
 * 文件名 :filter.c
 * 描述    :     
 * 实验平台：STM32F103RBT6
 * 库版本  ：ST3.5.
 * 功能: 陀螺仪数据与加速度计数据通过滤波算法融合
 * 说明：
*************************************************/
#include "stm32f10x.h"
#include "filter.h"
#include <stdio.h>
/************************************************************/
static float angle, angle_dot; 		
const float Q_angle = 0.002, Q_gyro = 0.002, R_angle = 0.5, dt = 0.01;			
static float P[2][2]={
				       { 1, 0 },
			           { 0, 1 }
			         };	
static float Pdot[4] = {0, 0, 0, 0};
const u8 C_0 = 1;
static float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
/*************************************************

名称：void kalman_filter(float angle_m, float gyro_m, float *angle_f, float *angle_dot_f)
功能：陀螺仪数据与加速度计数据通过滤波算法融合
输入参数：
  	float angle_m   加速度计计算的角度
	float gyro_m    陀螺仪角速度
	float *angle_f  融合后的角度
	float *angle_dot_f  融合后的角速度
输出参数：滤波后的角度及角速度
返回值：无
**************************************************/
void kalman_filter(float angle_m, float gyro_m, float *angle_f, float *angle_dot_f)			
{
  angle += (gyro_m - q_bias) * dt;
	
  Pdot[0]  =Q_angle - P[0][1] - P[1][0];
  Pdot[1] = -P[1][1];
  Pdot[2] = -P[1][1];
  Pdot[3] = Q_gyro;
	
  P[0][0] += Pdot[0] * dt;
  P[0][1] += Pdot[1] * dt;
  P[1][0] += Pdot[2] * dt;
  P[1][1] += Pdot[3] * dt;
	
  angle_err = angle_m - angle;
	
  PCt_0=C_0 * P[0][0];
  PCt_1=C_0 * P[1][0];
	
  E = R_angle + C_0 * PCt_0;
	
  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;
	
  t_0 = PCt_0;
  t_1 = C_0 * P[0][1];

  P[0][0] -= K_0 * t_0;
  P[0][1] -= K_0 * t_1;
  P[1][0] -= K_1 * t_0;
  P[1][1] -= K_1 * t_1;
		
  angle	+= K_0 * angle_err;
  q_bias += K_1 * angle_err;
  angle_dot = gyro_m - q_bias;

  *angle_f = angle;
  *angle_dot_f = angle_dot;
}
/***********************************************************************/
