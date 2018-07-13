#include "stm32f4xx.h"
#include "pwm.h"

//PA0与PA1分别作为两路pwm输出，分别控制左右摩擦轮

void PWM_Configuration(void)
{
    GPIO_InitTypeDef          gpio;
    TIM_TimeBaseInitTypeDef   tim;
    TIM_OCInitTypeDef         oc;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   //PCLK1=42MHz,TIM5 clk =84MHz

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&gpio);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0, GPIO_AF_TIM5);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5); 

    /* TIM5 */
	/*
    tim.TIM_Prescaler = ;
    tim.TIM_CounterMode = ;
    tim.TIM_Period = ;   
    tim.TIM_ClockDivision = ;
    TIM_TimeBaseInit(TIM5,&tim);
		
		
    oc.TIM_OCMode = ;
    oc.TIM_OutputState = ;
    oc.TIM_OutputNState = ;
    oc.TIM_Pulse = ;
    oc.TIM_OCPolarity = ;
    oc.TIM_OCNPolarity = ;
    oc.TIM_OCIdleState = ;
    oc.TIM_OCNIdleState = ;
    TIM_OC1Init(,&oc);
    TIM_OC2Init(,&oc);

    
    TIM_OC1PreloadConfig(TIM5,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM5,TIM_OCPreload_Enable);
           
    TIM_ARRPreloadConfig(TIM5,ENABLE);
		
    TIM_Cmd(TIM5,ENABLE);
		*/
}
