 /******************** (C) COPYRIGHT 2014 POWSOS Team **************************
  * 文件名	：time_test.c
  * 描述	:	  超声波定时器配置
  * 作者	:  Powsos_Team
  * 版本	：V2.0
  * 日期	：2014.8.23
  * 修订历史：V2.0
 ******************************************************************************/


#include "Time_test.h"


/*
 * 函数名：TIM2_NVIC_Configuration
 * 描述  ：TIM2中断优先级配置
 * 输入  ：无
 * 输出  ：无	
 */
void TIM1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*TIM_Period--1000   TIM_Prescaler--71 -->中断周期为24ms,超声波最远测400cm，58us/cm*400=23200us,所以定时24ms，超过这个时间说明测量有误*/
void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
    TIM_DeInit(TIM1);
    TIM_TimeBaseStructure.TIM_Period=24000;		 								/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				    /* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  	TIM1_NVIC_Configuration();

	  TIM_ClearFlag(TIM1, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM1, DISABLE);																		/* 开启时钟 */
    
  
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
