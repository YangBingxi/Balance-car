#ifndef __usart_H
#define __usart_H

void usart_init(void);
void USART_SendStringData(USART_TypeDef* USARTx, uint8_t* String,u8 NB_bytes);
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
#endif
