
/**
  ******************************************************************************
  * @file    USART/Printf/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Time_test.h"
#include "HC_SR04.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#include "Time_test.h"
#include "HC_SR04.h"


extern u16 distance;
extern int  pulsewidth;


extern u8 flg_get_senor_data;
extern u8 receive_data;
extern u8 hcsr04_test_flag;

u8 succeed_flag = 0;
u16 time = 0;
u8 Error_flag =0 ;/*测量错误flag*/

u8 cnt = 0;
u8 cnt1 = 0;
u16 led_cnt =0;
u8  duoji_flag=0;
u8 duoji_cnt =0;
u8 duoji_pwm = 0;

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		led_cnt++;
		if(led_cnt==1000)
		{
			GPIOA->ODR ^= (1 << 7);
			led_cnt = 0;
		}
		cnt1++;
		if(cnt1==20)
		{
			cnt1 = 0;
			flg_get_senor_data = 1;
	 	 }

		cnt++;		
		if(cnt==200)/*100ms测一次*/
		{
			cnt=0;
			hcsr04_test_flag = 1;
		}

		if(duoji_flag)
		{
		duoji_pwm++;
		if(duoji_pwm<=pulsewidth)
		{
			Duoji_High;	
		}
		else if(duoji_pwm==41)
		{
			duoji_pwm=0;
			duoji_cnt++;/*20个脉冲就停止*/
			if(duoji_cnt==20)
			{		
					duoji_flag = 0;
					duoji_cnt=0;
      }
					
		}
		else 
		{
			Duoji_Low;
		}

		}
		
  }
}
void TIM1_UP_IRQHandler(void)  
{  

		Error_flag	= 1;	
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);  
  
}  
  


void EXTI15_10_IRQHandler(void)
{ 

	if(EXTI_GetITStatus(EXTI_Line13) != RESET) //
	{

  		START_TIME;//开启定时器
		
			while(ECHO_Reci&&(Error_flag!=1));//等待回波下降沿或者超出时间没收到回波就跳出
		
			if(Error_flag)
			{
			
			//distance=TIM_GetCounter(TIM1)*5*34/200.0;									 //?????àà?&&UltrasonicWave_Distance<150
				distance = 0;
			}
			else
			{	
				time=TIM_GetCounter(TIM1);
				distance = time/58;
				
			}	
			STOP_TIME; //关闭定时器
			Error_flag = 0;
		    EXTI->IMR&=~(1<<13);//??line11???? 
			EXTI_ClearITPendingBit(EXTI_Line13);     //??3y?D??±ê????
	}  
}

/*********************************************************/
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);

    /* Read one byte from the receive data register */
    receive_data = USART_ReceiveData(USART2);
		
  }	 	  
}
/***************************************************************/
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
