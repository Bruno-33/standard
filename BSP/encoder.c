#include "main.h"

//encoder.INPUT_A---PA9(TIM1_CH1)
//encoder.INPUT_B---PA10(TIM1_CH2)

void Quad_Encoder_Configuration(void)
{
	/*
    GPIO_InitTypeDef gpio;
		
    RCC_AHB1PeriphClockCmd(, ENABLE);
    RCC_APB1PeriphClockCmd(, ENABLE);

	  gpio.GPIO_Pin =  ;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(,&gpio);
	
		GPIO_PinAFConfig(,,);
		GPIO_PinAFConfig(,,);
    TIM_EncoderInterfaceConfig(, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
    TIM_Cmd(,ENABLE);	 
	*/
}

void Encoder_Start(void)
{
    TIM3->CNT = 0x7fff;
}

static char Encoder_Dir = 0;
  
int32_t GetQuadEncoderDiff(void)
{
    int32_t cnt = 0;    
    cnt = (TIM3->CNT)-0x7fff;
    TIM3->CNT = 0x7fff;    
    if(Encoder_Dir == 1)
	{
		return cnt;	
	}
	else
	{
		return -cnt;            
	}
}
