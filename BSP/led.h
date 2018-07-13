#ifndef __LED_H__
#define __LED_H__

void Led_Configuration(void);

#define BLUE_LED_ON()      GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define BLUE_LED_OFF()     GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define BLUE_LED_TOGGLE()      GPIO_ToggleBits(GPIOB, GPIO_Pin_9)



#endif
