/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	tcrt500.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-03-07
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����Թܾ���������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "adc.h"
#include "tcrt5000.h"



TCRT5000_INFO t5000Info;




/*
************************************************************
*	�������ƣ�	TCRT5000_Init
*
*	�������ܣ�	��ʼ�����ƽź�ADC1_ch13
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void TCRT5000_Init(void)
{

	GPIO_InitTypeDef gpioInitStrcut;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_5;
	gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStrcut);
	
	gpioInitStrcut.GPIO_Mode = GPIO_Mode_AIN;
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &gpioInitStrcut);
	
	ADC_ChInit(ADC1, 0);
	
	TCRT5000_Set(TCRT5000_ON);

}

/*
************************************************************
*	�������ƣ�	TCRT5000_GetValue
*
*	�������ܣ�	��ȡ������չܵ�ADֵ
*
*	��ڲ�����	times���ɼ�������ƽ��ֵ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void TCRT5000_GetValue(unsigned char times)
{

	t5000Info.voltag = ADC_GetValueTimes(ADC1, ADC_Channel_13, times);

}

/*
************************************************************
*	�������ƣ�	TCRT5000_Set
*
*	�������ܣ�	���ƽ�
*
*	��ڲ�����	status��TCRT5000_ON-�򿪾�����		TCRT5000_OFF���رվ�����
*
*	���ز�����	��
*
*	˵����		TCRT5000_OFF��ʱ���ѹ����ֵ����200
************************************************************
*/
void TCRT5000_Set(TCRT5000_ENUM status)
{

	GPIO_WriteBit(GPIOA, GPIO_Pin_5, status != TCRT5000_ON ? Bit_SET : Bit_RESET);
	
	t5000Info.status = status;

}
