/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_IO.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		�����豸����IO��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f10x.h"  //��Ƭ��ͷ�ļ�

#include "net_io.h"		//�����豸����IO

#include "delay.h"		//Ӳ������

#include <string.h>		//C��




NET_IO_INFO netIOInfo;




//==========================================================
//	�������ƣ�	NET_IO_Init
//
//	�������ܣ�	��ʼ�������豸IO������
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		�ײ�������շ�����
//==========================================================
void NET_IO_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = 115200;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;								//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;							//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;						//8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);													//ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);								//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
	
	NET_IO_ClearRecive();

}

//==========================================================
//	�������ƣ�	NET_IO_Send
//
//	�������ܣ�	��������
//
//	��ڲ�����	str����Ҫ���͵�����
//				len�����ݳ���
//
//	���ز�����	��
//
//	˵����		�ײ�����ݷ�������
//
//==========================================================
void NET_IO_Send(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)											//����һ֡����
	{
		USART_SendData(NET_IO, *str++);
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
	}

}

//==========================================================
//	�������ƣ�	NET_IO_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool NET_IO_WaitRecive(void)
{

	if(netIOInfo.dataLen == 0) 						//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//�����һ�ε�ֵ�������ͬ����˵���������
	{
		netIOInfo.dataLen = 0;						//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	NET_IO_ClearRecive
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));

}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	�����ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{
	
	RTOS_EnterInt();

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //��ֹ���ڱ�ˢ��
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
	
	RTOS_ExitInt();

}
