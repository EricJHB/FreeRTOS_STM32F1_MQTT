/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	sht20.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��ʪ�ȶ�ȡ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "sht20.h"
#include "i2c.h"
#include "delay.h"


const int16_t POLYNOMIAL = 0x131;

SHT20_INFO sht20Info;


/*
************************************************************
*	�������ƣ�	SHT20_reset
*
*	�������ܣ�	SHT20��λ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void SHT20_reset(void)
{
	
    I2C_WriteByte(SHT20_ADDRESS, SHT20_SOFT_RESET, (void *)0);
	
}

/*
************************************************************
*	�������ƣ�	SHT20_read_user_reg
*
*	�������ܣ�	SHT20��ȡ�û��Ĵ���
*
*	��ڲ�����	��
*
*	���ز�����	��ȡ�����û��Ĵ�����ֵ
*
*	˵����		
************************************************************
*/
unsigned char  SHT20_read_user_reg(void)
{
	
    unsigned char val = 0;
	
    I2C_ReadByte(SHT20_ADDRESS, SHT20_READ_REG, &val);
	
    return val;
	
}

/*
************************************************************
*	�������ƣ�	SHT2x_CheckCrc
*
*	�������ܣ�	���������ȷ��
*
*	��ڲ�����	data����ȡ��������
*				nbrOfBytes����ҪУ�������
*				checksum����ȡ����У�Ա���ֵ
*
*	���ز�����	У����
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
char SHT2x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{
	
    char crc = 0;
    char bit = 0;
    char byteCtr = 0;
	
    //calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
	
    if(crc != checksum)
		return 1;
    else
		return 0;
	
}

/*
************************************************************
*	�������ƣ�	SHT2x_CalcTemperatureC
*
*	�������ܣ�	�¶ȼ���
*
*	��ڲ�����	u16sT����ȡ�����¶�ԭʼ����
*
*	���ز�����	�������¶�����
*
*	˵����		
************************************************************
*/
float SHT2x_CalcTemperatureC(unsigned short u16sT)
{
	
    float temperatureC = 0;            // variable for result

    u16sT &= ~0x0003;           // clear bits [1..0] (status bits)
    //-- calculate temperature [�C] --
    temperatureC = -46.85 + 175.72 / 65536 * (float)u16sT; //T= -46.85 + 175.72 * ST/2^16
	
    return temperatureC;
	
}

/*
************************************************************
*	�������ƣ�	SHT2x_CalcRH
*
*	�������ܣ�	ʪ�ȼ���
*
*	��ڲ�����	u16sRH����ȡ����ʪ��ԭʼ����
*
*	���ز�����	������ʪ������
*
*	˵����		
************************************************************
*/
float SHT2x_CalcRH(unsigned short u16sRH)
{
	
    float humidityRH = 0;              // variable for result
	
    u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    //humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
    humidityRH = ((float)u16sRH * 0.00190735) - 6;
	
    return humidityRH;
	
}

/*
************************************************************
*	�������ƣ�	SHT2x_MeasureHM
*
*	�������ܣ�	������ʪ��
*
*	��ڲ�����	cmd�������¶Ȼ���ʪ��
*				pMeasurand����Ϊ���򱣴�Ϊushortֵ���˵�ַ
*
*	���ز�����	�������
*
*	˵����		
************************************************************
*/
float SHT2x_MeasureHM(unsigned char cmd, unsigned short *pMeasurand)
{
	
    char  checksum = 0;  //checksum
    char  data[2];    //data array for checksum verification
	unsigned char addr = 0;
    unsigned short tmp = 0;
    float t = 0;
	
    addr = SHT20_ADDRESS << 1;
	
	IIC_Start();
	
	IIC_SendByte(addr);
	if(IIC_WaitAck(50000)) //�ȴ�Ӧ��
		return 0.0;
	
	IIC_SendByte(cmd);
	if(IIC_WaitAck(50000)) //�ȴ�Ӧ��
		return 0.0;
	
	IIC_Start();
	
	IIC_SendByte(addr + 1);
	while(IIC_WaitAck(50000)) //�ȴ�Ӧ��
	{
		IIC_Start();
		IIC_SendByte(addr + 1);
	}
	
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	RTOS_TimeDly(15);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	
	data[0] = IIC_RecvByte();
	IIC_Ack();
	data[1] = IIC_RecvByte();
	IIC_Ack();
	
	checksum = IIC_RecvByte();
	IIC_NAck();
	
	IIC_Stop();
	
	SHT2x_CheckCrc(data, 2, checksum);
    tmp = (data[0] << 8) + data[1];
    if(cmd == SHT20_Measurement_T_HM)
    {
        t = SHT2x_CalcTemperatureC(tmp);
    }
    else
    {
        t = SHT2x_CalcRH(tmp);
    }
	
    if(pMeasurand)
    {
        *pMeasurand = (unsigned short)t;
    }
	
    return t;
	
}

/*
************************************************************
*	�������ƣ�	SHT20_GetValue
*
*	�������ܣ�	��ȡ��ʪ������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʪ�Ƚ��������SHT20�ṹ����
************************************************************
*/
void SHT20_GetValue(void)
{
	
	unsigned char val = 0;
	
	IIC_SpeedCtl(5);
	
	SHT20_read_user_reg();
	DelayUs(100);
	
	sht20Info.tempreture = SHT2x_MeasureHM(SHT20_Measurement_T_HM, (void *)0);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	RTOS_TimeDly(15);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	
	sht20Info.humidity = SHT2x_MeasureHM(SHT20_Measurement_RH_HM, (void *)0);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	RTOS_TimeDly(6);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	
	SHT20_read_user_reg();
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	RTOS_TimeDly(6);
	//ʹ����������״̬���ﵽ��ʱ��Ŀ��
	
	I2C_WriteByte(SHT20_ADDRESS, SHT20_WRITE_REG, &val);
	DelayUs(100);
	
	SHT20_read_user_reg();
	DelayUs(100);
	
	SHT20_reset();
	DelayUs(100);

}
