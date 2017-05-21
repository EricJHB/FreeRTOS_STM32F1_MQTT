/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	AT24C02.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-01-19
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		EEPROM
	*
	*	�޸ļ�¼��	V1.1���޸���ʱ����д������bug��
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "at24c02.h"
#include "i2c.h"
#include "delay.h"
#include "hwtimer.h"




/*
************************************************************
*	�������ƣ�	AT24C02_WriteByte
*
*	�������ܣ�	дһ���ֽڵ�EEPROM
*
*	��ڲ�����	regAddr���Ĵ�����ַ
*				byte����Ҫд�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void AT24C02_WriteByte(unsigned char regAddr, unsigned char byte)
{

	I2C_WriteByte(AT24C02_ADDRESS, regAddr, &byte);

}

/*
************************************************************
*	�������ƣ�	AT24C02_WriteBytes
*
*	�������ܣ�	д����ֽڵ�EEPROM
*
*	��ڲ�����	regAddr���Ĵ�����ַ
*				byte����Ҫд������ݻ�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void AT24C02_WriteBytes(unsigned char regAddr, unsigned char *byte, unsigned char len)
{
	
	unsigned char count = 0;
	
	RTOS_ENTER_CRITICAL();									//д��������ʱ�������ٽ�Σ����ô�ϳ���
	
	for(; count < len;)
	{
		if(I2C_WriteByte(AT24C02_ADDRESS, regAddr, byte) == IIC_Err)
		{
			DelayXms(5);
			continue;
		}
		
		regAddr++;											//��ַ����
		byte++;												//ƫ�Ƶ��¸�����
		count++;
		
		DelayXms(5);										//��Ҫ��ʱ��������һ�����ˣ�����ʮ���ֽ�ʱ��1ms����ʱ��ʱ�޷���֤ȫ���ֽ���ȷд��
	}
	
	RTOS_EXIT_CRITICAL();

}

/*
************************************************************
*	�������ƣ�	AT24C02_ReadByte
*
*	�������ܣ�	��EEPROM��һ���ֽ�
*
*	��ڲ�����	regAddr���Ĵ�����ַ
*				byte����Ҫ��ȡ�����ݵĻ����ַ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void AT24C02_ReadByte(unsigned char regAddr, unsigned char *byte)
{

	I2C_ReadByte(AT24C02_ADDRESS, regAddr, byte);

}

/*
************************************************************
*	�������ƣ�	AT24C02_ReadBytes
*
*	�������ܣ�	��EEPROM������ֽ�
*
*	��ڲ�����	regAddr���Ĵ�����ַ
*				byte����Ҫд������ݻ�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void AT24C02_ReadBytes(unsigned char regAddr, unsigned char *byte, unsigned char len)
{
	
	RTOS_ENTER_CRITICAL();									//��ȡ�������ʱ�������ٽ�Σ����ô�ϳ���

	I2C_ReadBytes(AT24C02_ADDRESS, regAddr, byte, len);
	
	RTOS_EXIT_CRITICAL();

}

/*
************************************************************
*	�������ƣ�	AT24C02_Clear
*
*	�������ܣ�	ָ����ַ��ʼд������ͬ����
*
*	��ڲ�����	startAddr���Ĵ�����ʼ��ַ
*				byte����Ҫд�������
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		д��ָ��λ��ָ�����ȵ�ָ������
************************************************************
*/
_Bool AT24C02_Clear(unsigned char startAddr, unsigned char byte, unsigned short len)
{
	
	unsigned short count = 0;

	if(startAddr + len > 256)									//��������
		return 1;
	
	RTOS_ENTER_CRITICAL();										//д��������ʱ�������ٽ�Σ����ô�ϳ���
	
	for(; count < len;)
	{
		if(I2C_WriteByte(AT24C02_ADDRESS, startAddr, &byte) == IIC_Err)
		{
			DelayXms(5);
			continue;
		}
		
		startAddr++;											//��ַ����
		count++;
		
		DelayXms(5);											//��Ҫ��ʱ��������һ�����ˣ�����ʮ���ֽ�ʱ��1ms����ʱ��ʱ�޷���֤ȫ���ֽ���ȷд��
	}
	
	RTOS_EXIT_CRITICAL();
	
	return 0;

}
