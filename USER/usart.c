/******************** (C) COPYRIGHT 2014 POWSOS Team ***************************
 * 文件名  ：usart.c
 * 描述    : USART1(用于串口下载，调试) ，USART2(蓝牙)         
 * 实验平台：stm32f103RBT6
 * 硬件连接：------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
              PA2  - USART2 TX
              PA3  - USART2 RX	 
 * 库版本  ：ST3.5.0
**********************************************************************************/	
#include "misc.h"
#include <stdarg.h>
#include "stdio.h"
#include "usart.h"


/*
 * 函数名：USART_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void usart_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
	
	/* config USART1,USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) ,USART2 PA2 as alternate function push-pull */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/* Configure USART1 Rx (PA.10),USART2 PA3 as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
		/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
		/* USART1 GPIO config */
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);

}




//非中断方式
void USART_SendChar(USART_TypeDef* USARTx, uint8_t Data)
{
	
	USART_SendData(USARTx,(uint16_t)Data);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//????????
		
}
//非中断方式
void USART_SendString(USART_TypeDef* USARTx, uint16_t* String)
{
	while(*String)
	{
		USART_SendData(USARTx,(uint16_t)*String);
		String++;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//??????????
	}
}

void USART_SendStringwithEnter(USART_TypeDef* USARTx, uint8_t* String)
{
	while(*String)
	{
		USART_SendData(USARTx,(uint16_t)*String);
		String++;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//??????????
	}
	USART_SendChar(USARTx,0x0D);//????
	USART_SendChar(USARTx,0x0A);
}


void USART_SendStringWithData(USART_TypeDef* USARTx, uint8_t* String,uint8_t* dataIn)
{
	while(*String)
	{
		USART_SendData(USARTx,(uint16_t)*String);
		String++;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//??????????
	}
	while(*dataIn)
	{
		USART_SendData(USARTx,(uint16_t)*dataIn);
		dataIn++;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//??????????
	}
	USART_SendChar(USARTx,0x0D);//????
	USART_SendChar(USARTx,0x0A);
}

void USART_SendStringData(USART_TypeDef* USARTx, uint8_t* String,u8 NB_bytes)
{
	u8 i;
	
	for(i=0;i<NB_bytes;i++)
	{ 

		USART_SendData(USARTx,(uint16_t)*String);
		String++;
		
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//??????????
	}
}


/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
		/* 将Printf内容发往串口 */
	USART_SendData(USART1, (unsigned char) ch);
//	while (!(USART1->SR & USART_FLAG_TXE));
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
	return (ch);
}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	
	    /* Make the value positive. */
	    value *= -1;
	}
	
	for (i = 10000; i > 0; i /= 10)
	{
	    d = value / i;
	
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
	        value -= (d * i);
	        flag = 1;
	    }
	}
	
	/* Null terminate the string. */
	*ptr = 0;
	
	return string;

} /* NCL_Itoa */

/*
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //回车符
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //换行符
			USART_SendData(USARTx, 0x0a);	
			Data ++;
		break;
		
		default:
			Data ++;
		break;
	}			 
	}
	else if ( *Data == '%')
	{									  //
	switch ( *++Data )
	{				
		case 's':										  //字符串
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	
	case 'd':										//十进制
	d = va_arg(ap, int);
	itoa(d, buf, 10);
	for (s = buf; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
	Data++;
	break;
		 default:
				Data++;
		    break;
	}		 
	} /* end of else if */
	else USART_SendData(USARTx, *Data++);
	while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}

/******************* (C) COPYRIGHT 2014 POWSOS Team *****END OF FILE************/

