#ifndef _HC_SR04__H
#define _HC_SR04__H

#include "stm32f10x.h"
//------------------------------------------------------------ÒÆÖ²ÐÞ¸ÄÇø-----------------------------------------------------------------------

#define HCSR04_PORT	GPIOB
#define HCSR04_CLK   RCC_APB2Periph_GPIOB
#define HCSR04_TRIG	 GPIO_Pin_14
#define HCSR04_ECHO	 GPIO_Pin_13

#define TRIG_High	GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define TRIG_Low	GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define ECHO_Reci	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)


#define Duoji_High	GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define Duoji_Low		GPIO_ResetBits(GPIOA,GPIO_Pin_11)


/*¶æ»ú*/
#define Duoji_Front 1
#define Duoji_Left  2
#define Duoji_Right	3
//---------------------------------------------------------------------------------------------------------------------------------------------
void HCSR04_Init(void);
void measure_distance(u8 cmd);
#endif



