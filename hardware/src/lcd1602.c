/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	lcd1602.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		LCD1602��ʼ������ʾ
	*
	*	�޸ļ�¼��	V1.1��������EN�Žӿ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "lcd1602.h"
#include "delay.h"

//C��
#include <stdarg.h>





//���ݡ��������
#define RS_H	GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define RS_L	GPIO_ResetBits(GPIOC, GPIO_Pin_6)

//��д����
#define RW_H	GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define RW_L	GPIO_ResetBits(GPIOA, GPIO_Pin_11)

//ʹ�ܿ���
#define EN_H	GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define EN_L	GPIO_ResetBits(GPIOB, GPIO_Pin_4)







/*
************************************************************
*	�������ƣ�	Lcd1602_SendByte
*
*	�������ܣ�	��LCD1602дһ���ֽ�
*
*	��ڲ�����	byte����Ҫд�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Lcd1602_SendByte(unsigned char byte)
{
	
	unsigned short value = 0;
	
	value = GPIO_ReadOutputData(GPIOB);					//��ȡGPIOB������
	value &= ~(0x001F << 5);							//���bit5~8
	value |= ((unsigned short)byte & 0x001F) << 5;		//��Ҫд�������ȡ��5λ������5λ
	GPIO_Write(GPIOB, value);							//д��GPIOB
	
	value = GPIO_ReadOutputData(GPIOC);					//��ȡGPIOC������
	value &= ~(0x0007 << 0);							//���bit0~2
	value |= ((unsigned short)byte & 0x00E0) >> 5;		//��Ҫд�������ȡ��3λ������5λ
	GPIO_Write(GPIOC, value);							//д��GPIOC
	
	DelayUs(10);

}

/*
************************************************************
*	�������ƣ�	Lcd1602_WriteCom
*
*	�������ܣ�	��LCD1602д����
*
*	��ڲ�����	byte����Ҫд�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Lcd1602_WriteCom(unsigned char byte)
{

	RS_L;						//RS���ͣ�����ģʽ
	RW_L;						//RW���ͣ�дģʽ
	
	Lcd1602_SendByte(byte);		//����һ���ֽ�
	
	EN_H;
	DelayUs(20);
	EN_L;
	DelayUs(5);

}

/*
************************************************************
*	�������ƣ�	Lcd1602_WriteCom_Busy
*
*	�������ܣ�	��LCD1602д����
*
*	��ڲ�����	byte����Ҫд�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Lcd1602_WriteCom_Busy(unsigned char byte)
{
	
	DelayXms(10);

	RS_L;
	RW_L;
	
	Lcd1602_SendByte(byte);
	
	EN_H;
	DelayUs(20);
	EN_L;
	DelayUs(5);

}

/*
************************************************************
*	�������ƣ�	Lcd1602_WriteData
*
*	�������ܣ�	��LCD1602дһ������
*
*	��ڲ�����	byte����Ҫд�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Lcd1602_WriteData(unsigned char byte)
{

	RS_H;						//RS���ߣ�����ģʽ
	RW_L;						//RW���ͣ�дģʽ
	
	Lcd1602_SendByte(byte);		//����һ���ֽ�

	EN_H;
	DelayUs(20);
	EN_L;
	DelayUs(5);

}

/*
************************************************************
*	�������ƣ�	Lcd1602_Init
*
*	�������ܣ�	LCD1602��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		RW-PA11		RS-PC6		EN-PC3
*				DATA0~4-PB5~9		DATA5~7-PC0~2
************************************************************
*/
void Lcd1602_Init(void)
{

	GPIO_InitTypeDef gpioInitStrcut;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);				//��ֹJTAG����
	
	gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStrcut);
	
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6;
	GPIO_Init(GPIOC, &gpioInitStrcut);
	
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &gpioInitStrcut);
	
	DelayXms(15);
    Lcd1602_WriteCom(0x38);
    DelayXms(5);
    Lcd1602_WriteCom(0x38);
    DelayXms(5);
    Lcd1602_WriteCom(0x38);
    Lcd1602_WriteCom_Busy(0x38);
    Lcd1602_WriteCom_Busy(0x08);
    Lcd1602_WriteCom_Busy(0x01);
    Lcd1602_WriteCom_Busy(0x06);
    Lcd1602_WriteCom_Busy(0x0c);
	
    EN_L;

}

/*
************************************************************
*	�������ƣ�	Lcd1602_Clear
*
*	�������ܣ�	LCD1602���ָ����
*
*	��ڲ�����	pos��ָ������
*
*	���ز�����	��
*
*	˵����		0x80-��һ��		0xC0-�ڶ���		0xFF-����
************************************************************
*/
void Lcd1602_Clear(unsigned char pos)
{

	switch(pos)
	{
		case 0x80:
			
			Lcd1602_DisString(0x80, "                ");
		
		break;
		
		case 0xC0:
			
			Lcd1602_DisString(0xC0, "                ");
		
		break;
		
		case 0xFF:
			
			Lcd1602_WriteCom_Busy(0x01);
		
		break;
	}

}

/*
************************************************************
*	�������ƣ�	Lcd1602_DisString
*
*	�������ܣ�	����LCD1602��ʾ������
*
*	��ڲ�����	pos��Ҫ��ʾ����
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Lcd1602_DisString(unsigned char pos, char *fmt,...)
{

	unsigned char LcdPrintfBuf[33];
	unsigned char count = 0;
	unsigned char remain = 0;					//һ��ʣ��ռ�
	va_list ap;
	unsigned char *pStr = LcdPrintfBuf;
	
	va_start(ap,fmt);
	vsprintf((char *)LcdPrintfBuf, fmt, ap);
	va_end(ap);
	
	remain = 0x8f - pos;						//������bug����ǰֻ��д16�Σ����Ҫ���ݿ�ʼ��λ��������
	
	Lcd1602_WriteCom_Busy(pos);
	
	while(*pStr != 0)
	{
		Lcd1602_WriteData(*pStr++);
		
		if(++count > remain && pos <= 0x8f)
		{
			count = 0;
			Lcd1602_WriteCom_Busy(0xC0);
			DelayXms(1);
		}
	}

}
