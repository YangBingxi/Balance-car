/***********************************************

 * 文件名  ：time.c
 * 描述    :     
 * 作者    :  Powsos_Team
 * 版本    ：V2.0
 * 日期    ：2014.8.23
 * 修订历史：V2.0
******************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "timer.h"
/*************************************************

名称：timer_init(void)
功能：timer3外设初始化（中断 定时时间）
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void timer_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;    // 0.1ms
  TIM_TimeBaseStructure.TIM_Prescaler = 5 - 1;  // (Period + 1) * (Prescaler + 1) / 72M = 1ms
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

}
/***************************END OF FILE**********************************************************************/
