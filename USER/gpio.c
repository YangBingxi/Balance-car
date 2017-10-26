/******************** (C) COPYRIGHT 2014 POWSOS Team ***************************
 * 文件名  ：gpio.c
 * 描述    :      
 * 实验平台：stm32f103RBT6
 * 硬件连接: PA7---User_led
 * 库版本  ：ST3.5.0
 ******************************************************************************/
#include "stm32f10x.h"
#include "gpio.h"
void gpio_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOA,GPIO_Pin_7);  //blue led
}


