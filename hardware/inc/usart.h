#ifndef _USART_H_
#define _USART_H_


#include "stm32f10x.h"




typedef struct
{

	char alterBuf[150];
	unsigned char alterCount;

} ALTER_INFO;

extern ALTER_INFO alterInfo;


#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����



void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);


#endif
