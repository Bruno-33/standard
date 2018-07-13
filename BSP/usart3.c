#include "usart3.h"

/*-----USART3_TX-----PB10-----*/
/*-----USART3_RX-----PB11-----*/

FIFO_S_t* UART_TranFifo;


void USART3_Configuration(void)
{
    USART_InitTypeDef usart3;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;
		
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); 
		
    gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);
/*
    usart3.USART_BaudRate = ;         
    usart3.USART_WordLength = USART_WordLength_8b;
    usart3.USART_StopBits = ;
    usart3.USART_Parity = ;
    usart3.USART_Mode = USART_Mode_Tx	| USART_Mode_Rx;
    usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(, &usart3);

    USART_ITConfig(, USART_IT_RXNE, ENABLE);
    USART_Cmd(, ENABLE);
*/
    nvic.NVIC_IRQChannel = USART3_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&nvic);

    UART_TranFifo = FIFO_S_Create(100);  
    if(!UART_TranFifo)
    {
       // while(1);  avoid while in program
		}
}


void USART3_Printint(uint32_t num)
{    
	  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
	  if(num!=0){
				while(num){
						USART_SendData(USART3, num%10+'0');
						while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
						num/=10;
				}
		}
		else
				USART_SendData(USART3,'0');
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
}


void USART3_PrintCh(uint8_t ch)
{    
    FIFO_S_Put(UART_TranFifo, ch);
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}


void USART3_PutStr (const char *str)
{
    while(*str)
    {
			  USART_SendData(USART3, *str++); 
			  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
    }
}


void USART3_PrintBlock(uint8_t* pdata, uint8_t len)
{
		uint8_t i = 0;
    for(i = 0; i < len; i++)
    {
        FIFO_S_Put(UART_TranFifo, pdata[i]);
    }
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  //发送寄存器空中断
}


void USART3_IRQHandler(void)
{  
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {   
        if(!FIFO_S_IsEmpty(UART_TranFifo))
        {
						uint16_t data = (uint16_t)FIFO_S_Get(UART_TranFifo);
						USART_SendData(USART3, data);
        }
        else
        {
						USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }  
    }
	else if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)    //接收中断
    {
			uint8_t data_get;
			data_get = USART_ReceiveData(USART3);
			/*todo send the data we get ---data_get*/
			/*USART_SendData(, );*/
    }
}


void USART3_PrintCh_16(uint16_t ch)
{
		USART3_PrintCh((uint8_t)(ch >> 8 & 0xFF));
		USART3_PrintCh((uint8_t)(ch & 0xFF));
}


void USART3_Oscilloscope(float ch1, float ch2, float ch3, float ch4)
{
		USART3_PrintCh_16(0xFFFF);
		
		USART3_PrintCh_16((uint16_t)(ch1 * 10 + 32768));
		USART3_PrintCh_16((uint16_t)(ch2 * 10 + 32768));
		USART3_PrintCh_16((uint16_t)(ch3 * 10 + 32768));
		USART3_PrintCh_16((uint16_t)(ch4 * 10 + 32768));
}


int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
    USART_SendData(USART3, (uint8_t)ch);
    return ch;
}
