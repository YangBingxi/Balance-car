 /******************** (C) COPYRIGHT 2014 POWSOS Team **************************
  * �ļ���	��time_test.c
  * ����	:	  ��������ʱ������
  * ����	:  Powsos_Team
  * �汾	��V2.0
  * ����	��2014.8.23
  * �޶���ʷ��V2.0
 ******************************************************************************/


#include "Time_test.h"


/*
 * ��������TIM2_NVIC_Configuration
 * ����  ��TIM2�ж����ȼ�����
 * ����  ����
 * ���  ����	
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


/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ24ms,��������Զ��400cm��58us/cm*400=23200us,���Զ�ʱ24ms���������ʱ��˵����������*/
void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
    TIM_DeInit(TIM1);
    TIM_TimeBaseStructure.TIM_Period=24000;		 								/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				    /* ʱ��Ԥ��Ƶ�� 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* ������Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  	TIM1_NVIC_Configuration();

	  TIM_ClearFlag(TIM1, TIM_FLAG_Update);							    		/* �������жϱ�־ */
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM1, DISABLE);																		/* ����ʱ�� */
    
  
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
