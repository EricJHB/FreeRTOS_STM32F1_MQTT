/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	key.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����IO��ʼ�������������ж�
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//����ͷ�ļ�
#include "key.h"

//Ӳ������
#include "delay.h"




/*
************************************************************
*	�������ƣ�	Key_Init
*
*	�������ܣ�	����IO��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		SW2-PD2		SW3-PC11	SW4-PC12	SW5-PC13	
*				����Ϊ�͵�ƽ		�ͷ�Ϊ�ߵ�ƽ
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
*	�������ƣ�	KeyScan
*
*	�������ܣ�	������ƽɨ��
*
*	��ڲ�����	GPIOX����Ҫɨ���GPIO��	NUM����GPIO���ڵı��
*
*	���ز�����	IO��ƽ״̬
*
*	˵����		
************************************************************
*/
_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM)
{
	
	if(GPIOX == GPIOC)
	{
		if(!GPIO_ReadInputDataBit(GPIOC, NUM))	//����  Ϊ��
		{
			return KEYDOWN;
		}
		else									//����  Ϊ��
		{
			return KEYUP;
		}
	}
	else if(GPIOX == GPIOD)
	{
		if(!GPIO_ReadInputDataBit(GPIOD, NUM))	//����  Ϊ��
		{
			return KEYDOWN;
		}
		else									//����  Ϊ��
		{
			return KEYUP;
		}
	}
	
	return KEYUP;								//Ĭ�Ϸ��ذ����ͷ�
	
}

/*
************************************************************
*	�������ƣ�	Keyboard
*
*	�������ܣ�	�������ܼ��
*
*	��ڲ�����	GPIOX����Ҫɨ���GPIO��	NUM����GPIO���ڵı��
*
*	���ز�����	IO��ƽ״̬
*
*	˵����		�ֵ�����˫��������
************************************************************
*/
unsigned char Keyboard(void)
{
	
	static unsigned int keyBusyFlag = 0;									//�������ڷ��ͷ�״̬
	static unsigned char keyCount = 0;										//��������ʱ��
	unsigned char timeOut = 15;												//�ж�˫����������Ҫ����ʱ���
	
	if(KeyScan(GPIOC, KEY0) == KEYDOWN && !(keyBusyFlag & (~(1 << 0))))		//������� ����������δ����
	{
		keyBusyFlag |= 1 << 0;												//�˰�������æ״̬
		
		if(++keyCount >= KEYDOWN_LONG_TIME)									//���¼�ʱ
			keyCount = KEYDOWN_LONG_TIME;									//�ﵽ����ʱ���򲻱�
		
		return KEYNONE;														//�����޶���״̬
	}
	else if(KeyScan(GPIOC, KEY0) == KEYUP && keyBusyFlag & (1 << 0))		//����ͷ� �� ����֮ǰ�ǰ��¹���
	{
		keyBusyFlag &= ~(1 << 0);											//�˰������ڿ���״̬
		
		if(keyCount == KEYDOWN_LONG_TIME)									//����ǳ���
		{
			keyCount = 0;													//���¼�ʱ����
			return KEY0DOWNLONG;											//���س�������
		}
		else
		{
			keyCount = 0;													//���¼�ʱ����
			while(--timeOut)												//������Ҫ�ǵȴ�Լ250ms���ж��Ƿ��еڶ��ΰ���
			{
				RTOS_TimeDly(2);											//�ô������������̬���ⲻӰ���������������
				
				if(KeyScan(GPIOC, KEY0) == KEYDOWN)							//�еڶ��ΰ��£�˵��Ϊ˫��
				{
					while(KeyScan(GPIOC, KEY0) == KEYDOWN)					//�ȴ��ͷţ��޴˾䣬˫������һ����������
						RTOS_TimeDly(1);									//�ô������������̬���ⲻӰ���������������
					
					return KEY0DOUBLE;										//����˫������
				}
				
			}
			return KEY0DOWN;												//�����жϾ���Ч����Ϊ��������
		}
	}
	/********************************************��ͬ**********************************************/
	if(KeyScan(GPIOC, KEY1) == KEYDOWN && !(keyBusyFlag & (~(1 << 1))))		//������� ����������δ����
	{
		keyBusyFlag |= 1 << 1;												//�˰�������æ״̬
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOC, KEY1) == KEYUP && keyBusyFlag & (1 << 1))		//����ͷ�
	{
		keyBusyFlag &= ~(1 << 1);											//�˰������ڿ���״̬
		
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
	
	if(KeyScan(GPIOC, KEY2) == KEYDOWN && !(keyBusyFlag & (~(1 << 2))))		//������� ����������δ����
	{
		keyBusyFlag |= 1 << 2;												//�˰�������æ״̬
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOC, KEY2) == KEYUP && keyBusyFlag & (1 << 2))		//����ͷ�
	{
		keyBusyFlag &= ~(1 << 2);											//�˰������ڿ���״̬
		
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
	
	if(KeyScan(GPIOD, KEY3) == KEYDOWN && !(keyBusyFlag & (~(1 << 3))))		//������� ����������δ����
	{
		keyBusyFlag |= 1 << 3;												//�˰�������æ״̬
		
		if(++keyCount >= KEYDOWN_LONG_TIME)
			keyCount = KEYDOWN_LONG_TIME;
		
		return KEYNONE;
	}
	else if(KeyScan(GPIOD, KEY3) == KEYUP && keyBusyFlag & (1 << 3))		//����ͷ�
	{
		keyBusyFlag &= ~(1 << 3);											//�˰������ڿ���״̬
		
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
