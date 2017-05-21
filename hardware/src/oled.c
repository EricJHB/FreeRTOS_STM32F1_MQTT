/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	oled.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		OLED��ʼ������ʾ����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "oled.h"
#include "i2c.h"
#include "delay.h"
#include "selfcheck.h"
#include "hwtimer.h"

//�ֿ�
#include "oled_zk.h"

//C��
#include <stdarg.h>







/*
************************************************************
*	�������ƣ�	OLED_SendByte
*
*	�������ܣ�	OLED����һ���ֽ�
*
*	��ڲ�����	byte����Ҫ���͵��ֽ�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_SendByte(unsigned char byte)
{

	unsigned char i = 0;
	
	for(; i < 8; i++)		
	{
		if(byte & 0x80)
			SDA_H;
		else
			SDA_L;
		
		DelayUs(iicInfo.speed);
		
		SCL_H;
		DelayUs(iicInfo.speed);
		SCL_L;
		
		byte <<= 1;
	}

}

/*
************************************************************
*	�������ƣ�	OLED_WriteData
*
*	�������ܣ�	OLEDд��һ������
*
*	��ڲ�����	byte����Ҫд�������
*
*	���ز�����	д����
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool OLED_WriteData(unsigned char byte)
{
	
	IIC_Start();
	
	OLED_SendByte(OLED_ADDRESS);
	if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	OLED_SendByte(0x40);	//write data
	if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	OLED_SendByte(byte);
	if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	IIC_Stop();
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OLED_WriteCom
*
*	�������ܣ�	OLEDд��һ������
*
*	��ڲ�����	cmd����Ҫд�������
*
*	���ز�����	д����
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool OLED_WriteCom(unsigned char cmd)
{
	
	IIC_Start();
	
	OLED_SendByte(OLED_ADDRESS);	//�豸��ַ
	if(IIC_WaitAck(5000))			//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	OLED_SendByte(0x00);
	if(IIC_WaitAck(5000))			//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	OLED_SendByte(cmd);
	if(IIC_WaitAck(5000))			//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	
	IIC_Stop();
	
	return 0;
	
}

/*
************************************************************
*	�������ƣ�	OLED_Init
*
*	�������ܣ�	OLED��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_Init(void)
{
#if 1
	OLED_WriteCom(0xAE); //�ر���ʾ
	OLED_WriteCom(0x20); //Set Memory Addressing Mode	
	OLED_WriteCom(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	OLED_WriteCom(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
	OLED_WriteCom(0xa1); //0xa0��X��������ʾ��0xa1��X�᾵����ʾ
	OLED_WriteCom(0xc8); //0xc0��Y��������ʾ��0xc8��Y�᾵����ʾ
	OLED_WriteCom(0x00); //�����е�ַ��4λ
	OLED_WriteCom(0x10); //�����е�ַ��4λ
	OLED_WriteCom(0x40); //������ʼ�ߵ�ַ
	OLED_WriteCom(0x81); //���öԱȶ�ֵ
	OLED_WriteCom(0x7f); //------
	OLED_WriteCom(0xa6); //0xa6,������ʾģʽ;0xa7��
	OLED_WriteCom(0xa8); //--set multiplex ratio(1 to 64)
	OLED_WriteCom(0x3F); //------
	OLED_WriteCom(0xa4); //0xa4,��ʾ����RAM�ĸı���ı�;0xa5,��ʾ���ݺ���RAM������
	OLED_WriteCom(0xd3); //������ʾƫ��
	OLED_WriteCom(0x00); //------
	OLED_WriteCom(0xd5); //�����ڲ���ʾʱ��Ƶ��
	OLED_WriteCom(0xf0); //------
	OLED_WriteCom(0xd9); //--set pre-charge period//
	OLED_WriteCom(0x22); //------
	OLED_WriteCom(0xda); //--set com pins hardware configuration//
	OLED_WriteCom(0x12); //------
	OLED_WriteCom(0xdb); //--set vcomh//
	OLED_WriteCom(0x20); //------
	OLED_WriteCom(0x8d); //--set DC-DC enable//
	OLED_WriteCom(0x14); //------
	OLED_WriteCom(0xaf); //����ʾ
#else
	OLED_WriteCom(0xAE);   //display off
	OLED_WriteCom(0x00);	//Set Memory Addressing Mode	
	OLED_WriteCom(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	OLED_WriteCom(0x40);	//Set Page Start Address for Page Addressing Mode,0-7
	OLED_WriteCom(0xb0);	//Set COM Output Scan Direction
	OLED_WriteCom(0x81);//---set low column address
	OLED_WriteCom(0xff);//---set high column address
	OLED_WriteCom(0xa1);//--set start line address
	OLED_WriteCom(0xa6);//--set contrast control register
	OLED_WriteCom(0xa8);
	OLED_WriteCom(0x3f);//--set segment re-map 0 to 127
	OLED_WriteCom(0xad);//--set normal display
	OLED_WriteCom(0x8b);//--set multiplex ratio(1 to 64)
	OLED_WriteCom(0x33);//
	OLED_WriteCom(0xc8);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_WriteCom(0xd3);//-set display offset
	OLED_WriteCom(0x00);//-not offset
	OLED_WriteCom(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WriteCom(0x80);//--set divide ratio
	OLED_WriteCom(0xd9);//--set pre-charge period
	OLED_WriteCom(0x1f); //
	OLED_WriteCom(0xda);//--set com pins hardware configuration
	OLED_WriteCom(0x12);
	OLED_WriteCom(0xdb);//--set vcomh
	OLED_WriteCom(0x40);//0x20,0.77xVcc
//	IIC_Write_Command(0x8d);//--set DC-DC enable
//	IIC_Write_Command(0x14);//
	OLED_WriteCom(0xaf);//--turn on oled panel
#endif

}

/*
************************************************************
*	�������ƣ�	OLED_Address
*
*	�������ܣ�	����OLED��ʾ��ַ
*
*	��ڲ�����	x���е�ַ
*				y���е�ַ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_Address(unsigned char x, unsigned char y)
{

	OLED_WriteCom(0xb0 + x);					//�����е�ַ
	DelayUs(iicInfo.speed);
	OLED_WriteCom(((y & 0xf0) >> 4) | 0x10);	//�����е�ַ�ĸ�4λ
	DelayUs(iicInfo.speed);
	OLED_WriteCom(y & 0x0f);					//�����е�ַ�ĵ�4λ
	DelayUs(iicInfo.speed);
	
}

/*
************************************************************
*	�������ƣ�	OLED_ClearScreen
*
*	�������ܣ�	OLEDȫ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_ClearScreen(void)
{
	
	unsigned char i = 0, j = 0;
	
	if(checkInfo.OLED_OK)
	{
		RTOS_ENTER_CRITICAL();
		
		IIC_SpeedCtl(1);
		
		for(; i < 8; i++)
		{
			OLED_WriteCom(0xb0 + i);
			OLED_WriteCom(0x10);
			OLED_WriteCom(0x00);
			
			for(j = 0; j < 132; j++)
			{
				OLED_WriteData(0x00);
			}
		}
		
		RTOS_EXIT_CRITICAL();
	}
	
}

/*
************************************************************
*	�������ƣ�	OLED_ClearAt
*
*	�������ܣ�	OLED���ָ����
*
*	��ڲ�����	x����Ҫ�������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_ClearAt(unsigned char x)
{

	unsigned char i = 0;
	
	if(checkInfo.OLED_OK)
	{
		RTOS_ENTER_CRITICAL();
		
		OLED_WriteCom(0xb0 + x);
		OLED_WriteCom(0x10);
		OLED_WriteCom(0x00);
			
		for(; i < 132; i++)
		{
			OLED_WriteData(0x00);
		}
		
		RTOS_EXIT_CRITICAL();
	}

}

/*
************************************************************
*	�������ƣ�	OLED_Dis12864_Pic
*
*	�������ܣ�	��ʾһ��128*64��ͼƬ
*
*	��ڲ�����	dp��ͼƬ����ָ��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OLED_Dis12864_Pic(const unsigned char *dp)
{
	
	unsigned char i = 0, j = 0;
	
	if(checkInfo.OLED_OK)
	{
		RTOS_ENTER_CRITICAL();
		
		for(; j < 8; j++)
		{
			OLED_Address(j, 0);
			
			for (i = 0; i < 128; i++)
			{	
				OLED_WriteData(*dp++); //д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			}
		}
		
		RTOS_EXIT_CRITICAL();
	}
	
}

/*
************************************************************
*	�������ƣ�	OLED_DisChar16x16
*
*	�������ܣ�	��ʾ16x16�ĵ�������
*
*	��ڲ�����	dp��ͼƬ����ָ��
*
*	���ز�����	��
*
*	˵����		��ʾ16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��
************************************************************
*/
void OLED_DisChar16x16(unsigned short x, unsigned short y, const unsigned char *dp)
{
	
	unsigned short i = 0, j = 0;
	
	if(checkInfo.OLED_OK)
	{
		IIC_SpeedCtl(20);					//i2c�ٶȿ���
		
		RTOS_ENTER_CRITICAL();
		
		for(j = 2; j > 0; j--)
		{
			OLED_Address(x, y);
			
			for (i = 0; i < 16; i++)
			{
				OLED_WriteData(*dp++);		//д���ݵ�OLED,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			}
			
			x++;
		}
		
		RTOS_EXIT_CRITICAL();
	}
	
}

/*
************************************************************
*	�������ƣ�	OLED_DisString6x8
*
*	�������ܣ�	��ʾ6x8�ĵ�������
*
*	��ڲ�����	x����ʾ��
*				y����ʾ��
*				fmt����������
*
*	���ز�����	��
*
*	˵����		����ʾ7��
************************************************************
*/
void OLED_DisString6x8(unsigned char x, unsigned char y, char *fmt, ...)
{

	unsigned char i = 0, ch = 0;
	unsigned char OledPrintfBuf[128];
	
	va_list ap;
	unsigned char *pStr = OledPrintfBuf;
	
	va_start(ap,fmt);
	vsnprintf((char *)OledPrintfBuf, sizeof(OledPrintfBuf), fmt, ap);
	va_end(ap);
	
	if(checkInfo.OLED_OK)
	{
		y += 2;
		IIC_SpeedCtl(20);							//i2c�ٶȿ���
		RTOS_ENTER_CRITICAL();
		
		while(*pStr != '\0')
		{
			ch = *pStr - 32;
			
			if(y > 126)
			{
				y = 2;
				x++;
			}
			
			OLED_Address(x, y);
			for(i = 0; i < 6; i++)
				OLED_WriteData(F6x8[ch][i]);
			
			y += 6;
			pStr++;
		}
		
		RTOS_EXIT_CRITICAL();
	}

}

/*
************************************************************
*	�������ƣ�	OLED_DisString8x16
*
*	�������ܣ�	��ʾ8x16�ĵ�������
*
*	��ڲ�����	x����ʾ��
*				y����ʾ��
*				fmt����������
*
*	���ز�����	��
*
*	˵����		����ʾ4��
************************************************************
*/
void OLED_DisString8x16(unsigned char x, unsigned char y, char *fmt, ...)
{

	unsigned char i = 0, ch = 0;
	unsigned char OledPrintfBuf[128];
	
	va_list ap;
	unsigned char *pStr = OledPrintfBuf;
	
	va_start(ap,fmt);
	vsnprintf((char *)OledPrintfBuf, sizeof(OledPrintfBuf), fmt, ap);
	va_end(ap);
	
	if(checkInfo.OLED_OK)
	{
		y += 2;
		IIC_SpeedCtl(20);							//i2c�ٶȿ���
		RTOS_ENTER_CRITICAL();
		
		while(*pStr != '\0')
		{
			ch = *pStr - 32;
			
			if(y > 128)
			{
				y = 2;
				x += 2;
			}
			
			OLED_Address(x, y);
			for(i = 0; i < 8; i++)
				OLED_WriteData(F8X16[(ch << 4) + i]);
			
			OLED_Address(x + 1, y);
			for(i = 0; i < 8; i++)
				OLED_WriteData(F8X16[(ch << 4) + i + 8]);
			
			y += 8;
			pStr++;
		}
		
		RTOS_EXIT_CRITICAL();
	}

}
