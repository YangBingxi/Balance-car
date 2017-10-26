#ifndef TIME_TEST_H
#define TIME_TEST_H

#include "stm32f10x.h"
 
#define START_TIME  TIM_SetCounter(TIM1,0);TIM_Cmd(TIM1, ENABLE)
#define STOP_TIME  time =0;TIM_Cmd(TIM1, DISABLE); TIM_SetCounter(TIM1,0)


void TIM1_Configuration(void);

#endif	/* TIME_TEST_H */
