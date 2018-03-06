
#include "stm32f10x.h"
#include "HC_SR04.h"
#include "delay.h"
#include "Time_test.h"

u8 Last_direction =  1;/*上一舵机状态*/
extern u8 Duoji_direction;
extern u8  duoji_flag;
extern u8 duoji_cnt;
void servopulse(int myangle);//定义一个脉冲函数

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/********************************************************************************************************************************************
@f_name: void void HCSR04_Init(void)
@brief:	 超声波硬件模块初始化
@param:	 None
@return: None
*********************************************************************************************************************************************/
void HCSR04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
 	RCC_APB2PeriphClockCmd(HCSR04_CLK| RCC_APB2Periph_AFIO, ENABLE);
 	
	GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       //发送电平引脚
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);

	GPIO_InitStructure.GPIO_Pin =HCSR04_ECHO;		//返回电平引脚
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//
 	GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);

												
	/* config the NVIC(PB13) */
		NVIC_Configuration();

	/* EXTI line(PE5) mode config */
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13); 
  	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure); 
	
}
/********************************************************************************************************************************************
@f_name: void measure_distance(float *p)
@brief:	 超声波测距，均值5次，实际 测量过程发现测量静态的比较稳定，动态的不稳定，3米范围内比较稳定
@param:	 float *p:测距缓存变量
@return: None
*********************************************************************************************************************************************/
void measure_distance(u8 cmd)
{
	u8 i;
	//static u16 distance_data;	
    /*调整舵机方向*/
	switch(cmd)
	{
		case 'M': 
				Duoji_direction = Duoji_Front;
				if(Last_direction!=Duoji_direction)
			   {		
             duoji_flag=1;
             duoji_cnt=0;					 
	    			 servopulse(3);   	
            // pulsewidth=3;					 
				}
		
				break;
		case 'L': 			
				Duoji_direction = Duoji_Left;
				if(Last_direction!=Duoji_direction)
			  {
					  duoji_flag=1;
						duoji_cnt=0;						
						servopulse(4);  
                  //   pulsewidth	=4;	      
			 	}
				break;			
		case 'R': 
				Duoji_direction = Duoji_Right;				
				if(Last_direction!=Duoji_direction)
			   {		 
             duoji_flag=1;	
						duoji_cnt=0;	
	    			 servopulse(2);
                // pulsewidth =2;					 
				}
				break;
	}		

	Last_direction = Duoji_direction;/*把当前方向赋予上一方向，以便下一次测量进行判断*/
	TRIG_High;
	delay_us(20);
	TRIG_Low;
	EXTI->IMR|=1<<13;//打开外部中断

#if 0

	/*等待Echo回波引脚变回高电平*/
	while(!ECHO_Reci);

	succeed_flag = 0;

	START_TIME;/*打开定时器*/
	
	delay_us(50);
	if(succeed_flag)
	{
		distance_data = time;
    distance_data = distance_data*58;                    
			        			
    }                                        		                                       // X??=( 2*Y??)/344 ==?·X??=0.0058*Y?× ==?·à??×=?￠??/58    
		else
	{
       distance_data = 0;                    //
		   	
    }	
		STOP_TIME;
		
		return distance_data ;
		#endif
}

