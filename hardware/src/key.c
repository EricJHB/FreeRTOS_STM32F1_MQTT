/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	key.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		按键IO初始化，按键功能判断
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//按键头文件
#include "key.h"

//硬件驱动
#include "delay.h"




/*
************************************************************
*	函数名称：	Key_Init
*
*	函数功能：	按键IO初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		SW2-PD2		SW3-PC11	SW4-PC12	SW5-PC13	
*				按下为低电平		释放为高电平
************************************************************
*/
void Key_Init(void)
{

	GPIO_InitTypeDef gpioInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	gpioInitStructure.GPIO_Mode = GPIO_Mode_IPU;
	gpioInitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStructure);
	
	gpioInitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &gpioInitStructure);

}

/*
************************************************************
*	函数名称：	KeyScan
*
*	函数功能：	按键电平扫描
*
*	入口参数：	GPIOX：需要扫描的GPIO组	NUM：该GPIO组内的编号
*
*	返回参数：	IO电平状态
*
*	说明：		
************************************************************
*/
_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM)
{
	
	if(GPIOX == GPIOC)
	{
		if(!GPIO_ReadInputDataBit(GPIOC, NUM))	//按下  为低
		{
			return KEYDOWN;
		}
		else									//弹起  为高
		{
			return KEYUP;
		}
	}
	else if(GPIOX == GPIOD)
	{
		if(!GPIO_ReadInputDataBit(GPIOD, NUM))	//按下  为低
		{
			return KEYDOWN;
		}
		else									//弹起  为高
		{
			return KEYUP;
		}
	}
	
	return KEYUP;								//默认返回按键释放
	
}

/*
************************************************************
*	函数名称：	Keyboard
*
*	函数功能：	按键功能检测
*
*	入口参数：	GPIOX：需要扫描的GPIO组	NUM：该GPIO组内的编号
*
*	返回参数：	IO电平状态
*
*	说明：		分单击、双击、长安
************************************************************
*/
unsigned char Keyboard(void)
{
	
	static unsigned int keyBusyFlag = 0;									//按键处于非释放状态
	static unsigned char keyCount = 0;										//按键按下时间
	unsigned char timeOut = 15;												//判断双击动作所需要的延时间隔
	
	if(KeyScan(GPIOC, KEY0) == KEYDOWN && !(keyBusyFlag & (~(1 << 0))))		//如果按下 且其他按键未按下
	{
		keyBusyFlag |= 1 << 0;												//此按键处于忙状态
		
		if(++keyCount >= KEYDOWN_LONG_TIME)									//按下计时
			keyCount = KEYDOWN_LONG_TIME;									//达到长按时长则不变
		
		return KEYNONE;														//返回无动作状态
	}
	else if(KeyScan(GPIOC, KEY0) == KEYUP && keyBusyFlag & (1 << 0))		//如果释放 且 按键之前是按下过的
	{
		keyBusyFlag &= ~(1 << 0);											//此按键处于空闲状态
		
		if(keyCount == KEYDOWN_LONG_TIME)									//如果是长按
		{
			keyCount = 0;													//按下计时清零
			return KEY0DOWNLONG;											//返回长按动作
		}
		else
		{
			keyCount = 0;													//按下计时清零
			while(--timeOut)												//这里主要是等待约250ms，判断是否有第二次按下
			{
				RTOS_TimeDly(2);											//让此任务进入阻塞态，这不影响代码正常的运行
				
				if(KeyScan(GPIOC, KEY0) == KEYDOWN)							//有第二次按下，说明为双击
				{
					while(KeyScan(GPIOC, KEY0) == KEYDOWN)					//等待释放，无此句，双击后会跟一个单击动作
						RTOS_TimeDly(1);									//让此任务进入阻塞态，这不影响代码正常的运行
					
					return KEY0DOUBLE;										//返回双击动作
				}
				
			}
			return KEY0DOWN;												//以上判断均无效，则为单击动作
		}
	}
	/********************************************下同**********************************************/
	if(KeyScan(GPIOC, KEY1) == KEYDOWN && !(keyBusyFlag & (~(1 << 1))))		//如果按下 且其他按键未按下
	{
		keyBusyFlag |= 1 << 1;												//此按键处于忙状态
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOC, KEY1) == KEYUP && keyBusyFlag & (1 << 1))		//如果释放
	{
		keyBusyFlag &= ~(1 << 1);											//此按键处于空闲状态
		
		if(keyCount == KEYDOWN_LONG_TIME)
		{
			keyCount = 0;
			return KEY1DOWNLONG;
		}
		else
		{
			keyCount = 0;
			while(--timeOut)
			{
				RTOS_TimeDly(2);
				
				if(KeyScan(GPIOC, KEY1) == KEYDOWN)
				{
					while(KeyScan(GPIOC, KEY1) == KEYDOWN)
						RTOS_TimeDly(1);
					
					return KEY1DOUBLE;
				}
				
			}
			return KEY1DOWN;
		}
	}
	
	if(KeyScan(GPIOC, KEY2) == KEYDOWN && !(keyBusyFlag & (~(1 << 2))))		//如果按下 且其他按键未按下
	{
		keyBusyFlag |= 1 << 2;												//此按键处于忙状态
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOC, KEY2) == KEYUP && keyBusyFlag & (1 << 2))		//如果释放
	{
		keyBusyFlag &= ~(1 << 2);											//此按键处于空闲状态
		
		if(keyCount == KEYDOWN_LONG_TIME)
		{
			keyCount = 0;
			return KEY2DOWNLONG;
		}
		else
		{
			keyCount = 0;
			while(--timeOut)
			{
				RTOS_TimeDly(2);
				
				if(KeyScan(GPIOC, KEY2) == KEYDOWN)
				{
					while(KeyScan(GPIOC, KEY2) == KEYDOWN)
						RTOS_TimeDly(1);
					
					return KEY2DOUBLE;
				}
				
			}
			return KEY2DOWN;
		}
	}
	
	if(KeyScan(GPIOD, KEY3) == KEYDOWN && !(keyBusyFlag & (~(1 << 3))))		//如果按下 且其他按键未按下
	{
		keyBusyFlag |= 1 << 3;												//此按键处于忙状态
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOD, KEY3) == KEYUP && keyBusyFlag & (1 << 3))		//如果释放
	{
		keyBusyFlag &= ~(1 << 3);											//此按键处于空闲状态
		
		if(keyCount == KEYDOWN_LONG_TIME)
		{
			keyCount = 0;
			return KEY3DOWNLONG;
		}
		else
		{
			keyCount = 0;
			while(--timeOut)
			{
				RTOS_TimeDly(2);
				
				if(KeyScan(GPIOD, KEY3) == KEYDOWN)
				{
					while(KeyScan(GPIOD, KEY3) == KEYDOWN)
						RTOS_TimeDly(1);
					
					return KEY3DOUBLE;
				}
				
			}
			return KEY3DOWN;
		}
	}
	
	keyBusyFlag = 0;
	keyCount = 0;
	return KEYNONE;
	
}
