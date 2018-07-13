#include "main.h"
#include "FIFO.h"
#include "mpu6050_driver.h"
/*-----USART3_TX-----PB10-----*/
/*-----USART3_RX-----PB11-----*/

FIFO_S_t* UART_TranFifo6;
//static unsigned char rx_buffer[256];
extern int receive[3];
//uint8_t receive_from_pi = 0;
//uint8_t rx_count=0;
//uint8_t remote_mode;
extern int aim[4];
extern int add[4];
extern int tmp[4];
//float a[3],w[3],angle[3],T,tmp_angle[3];
uint8_t Re_buf[11];
uint8_t counter;
//static unsigned char sign;
void USART6_Configuration(void)
{
    USART_InitTypeDef usart6;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

    GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); 

    gpio.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
    GPIO_Init(GPIOC,&gpio);
    //printf("1\n");
    usart6.USART_BaudRate =115200;          // speed 10byte/ms
    usart6.USART_WordLength = USART_WordLength_8b;
    usart6.USART_StopBits = USART_StopBits_1;
    usart6.USART_Parity = USART_Parity_No;
    usart6.USART_Mode = USART_Mode_Tx	| USART_Mode_Rx;
    usart6.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART6,&usart6);

    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART6,ENABLE);

    nvic.NVIC_IRQChannel = USART6_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&nvic);
    //printf("2\n");
    UART_TranFifo6 = FIFO_S_Create(100);  
    if(!UART_TranFifo6)
    {
       // while(1);  avoid while in program
	}
}

void UART6_Printint(uint32_t num)
{    
	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET){}
	if(num!=0){
		while(num){
			USART_SendData(USART6, num%10+'0');
			while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET){}
			num/=10;
		}
	}
	else{}
	USART_SendData(USART6,'0');
	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET){}
}

void UART6_PrintCh(uint8_t ch)
{    
    FIFO_S_Put(UART_TranFifo6, ch);
    USART_ITConfig(USART6,USART_IT_TXE, ENABLE);
}

void UART6_PutStr (const char *str)//?????
{
    while(*str)
    {
		USART_SendData(USART6, *str++); 
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET){}
    }
}

void UART6_PrintBlock(uint8_t* pdata, uint8_t len)
{
	uint8_t i = 0;
    for(i = 0; i < len; i++)
    {
        FIFO_S_Put(UART_TranFifo6, pdata[i]);
    }
    USART_ITConfig(USART6, USART_IT_TXE, ENABLE);  
}


int MPU6050_Readdone=0;   
float back_ax,back_ay,back_az,back_wax,back_way,back_waz,back_wx,back_wy,back_wz; 
float last_ax,last_ay;       
float Speed_x=0,Speed_y=0;
float Displacement_x=0,Displacement_y=0;
void USART6_IRQHandler(void)
{  
    if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
    {   
        if(!FIFO_S_IsEmpty(UART_TranFifo6))
        {
			uint16_t data = (uint16_t)FIFO_S_Get(UART_TranFifo6);
			USART_SendData(USART6, data);
        }
        else
        {
			USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
        }  
    }
	else if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET&&MPU6050_Readdone==0)   
    {
		uint8_t data_get;
		data_get=USART_ReceiveData(USART6);
		if(data_get==0x55&&counter==0)
		{
			Re_buf[counter]=data_get;
			counter++;
		}
		else
		{	 
			if(counter>0)
			{
				Re_buf[counter]=data_get;
				counter++;
				if(counter==11) 
				{
					counter=0;
					MPU6050_Readdone=1;
					//MPU6050_getDisplacement(&Displacement_x,&Displacement_y,&Speed_x,&Speed_y);//????
					last_ax = back_ax;
					last_ay = back_ay;
					MPU6050_getMotion(&back_ax,&back_ay,&back_az,&back_wax,&back_way,&back_waz,&back_wx,&back_wy,&back_wz);//?????
					//MPU6050_getSpeed(&Speed_x,&Speed_y,&back_ax,&back_ay,&last_ax,&last_ay);//????
				}
			}
		}
    }     			
}

