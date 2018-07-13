#ifndef __USART3_H__
#define __USART3_H__
#include "main.h"

void USART3_Configuration(void);
void USART3_PutStr(const char *);
void USART3_Printint(uint32_t);
void USART3_PrintCh(uint8_t ch);

#endif
