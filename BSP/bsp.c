#include "main.h"

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     
	PWM_Configuration();            		    
	TIM6_Configuration();
	Quad_Encoder_Configuration();
	CAN1_Configuration();	
	CAN2_Configuration();
	USART1_Configuration(100000);
	USART3_Configuration();
	USART6_Configuration();
	Led_Configuration();
	TIM6_Start();
	/*
	*≈‰÷√MPU6050µƒ÷–∂œ
	*/
	MPU6050_IntConfiguration();
	Encoder_Start();
}

