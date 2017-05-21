#ifndef _KEY_H_
#define _KEY_H_


#include "stm32f10x.h"




#define KEY0			GPIO_Pin_11
#define KEY1			GPIO_Pin_13
#define KEY2			GPIO_Pin_12
#define KEY3			GPIO_Pin_2

#define KEYPORTNUM1		(GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4)
#define KEYPORTNUM2		GPIO_Pin_0


/*******************************************
			按键按下与弹起
*******************************************/
#define KEYDOWN			1
#define KEYUP			0

#define KEY0DOWN		0
#define KEY0UP			1
#define KEY0DOUBLE		2
#define KEY0DOWNLONG	100

#define KEY1DOWN		3
#define KEY1UP			4
#define KEY1DOUBLE		5
#define KEY1DOWNLONG	101

#define KEY2DOWN		6
#define KEY2UP			7
#define KEY2DOUBLE		8
#define KEY2DOWNLONG	102

#define KEY3DOWN		9
#define KEY3UP			10
#define KEY3DOUBLE		11
#define KEY3DOWNLONG	103

#define KEYNONE			255


/*******************************************
			按键计时
*******************************************/
#define KEYDOWN_LONG_TIME		20 //计算长按时长。目前keyboard函数每50ms调用一次。


extern void Key_Init(void);

_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM);

extern unsigned char Keyboard(void);

#endif
