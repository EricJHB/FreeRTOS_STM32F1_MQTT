/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	gy30.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		���մ�������ʼ������ȡ����ǿ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "gy30.h"
#include "i2c.h"
#include "delay.h"
#include "led.h"



GY30_INFO gy30Info;


/*
************************************************************
*	�������ƣ�	GY30_Init
*
*	�������ܣ�	GY30��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void GY30_Init(void)
{

	DelayUs(5);
	
	I2C_WriteByte(BH1750FVI_ADDR, BH1750_ON, (void *)0);			//power on
    DelayUs(2);
	
	I2C_WriteByte(BH1750FVI_ADDR, BH1750_RSET, (void *)0);			//clear
    DelayUs(40);
	
	I2C_WriteByte(BH1750FVI_ADDR, BH1750_Con_High_RM, (void *)0);	//����H�ֱ���ģʽ������120ms��֮���Զ��ϵ�ģʽ
    DelayUs(40);

}

/*
************************************************************
*	�������ƣ�	GY30_GetValue
*
*	�������ܣ�	��ȡ����ǿ��
*
*	��ڲ�����	��
*
*	���ز�����	��ȡ���
*
*	˵����		0-�ɹ�		1-ʧ��
				��λLX���տ�˹
************************************************************
*/
_Bool GY30_GetValue(void)
{
	
    unsigned char addr;
    unsigned char data[2];
    unsigned short result = 0;
    float result_lx = 0;
	
	addr = BH1750FVI_ADDR << 1;								//��ַ����
	
    IIC_Start();
	
    IIC_SendByte(addr);
	if(IIC_WaitAck(50))										//�ȴ�Ӧ��
		return 1;
	
	IIC_Start();

    IIC_SendByte(addr + 1);
	if(IIC_WaitAck(50))										//�ȴ�Ӧ��
		return 1;
	
	data[0] = IIC_RecvByte();
	IIC_Ack();
	data[1] = IIC_RecvByte();
	IIC_NAck();
	
	IIC_Stop();
	
    result = (unsigned short)((data[0] << 8) + data[1]);	//�ϳ����ݣ�����������
    result_lx = (float)result / 1.2;						//����1.2Ϊ����ǿ��
	
    gy30Info.lightVal = (unsigned short)result_lx;
	
	return 0;
	
}
