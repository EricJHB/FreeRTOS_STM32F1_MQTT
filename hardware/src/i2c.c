/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	i2c.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		���IIC����IO��ʼ������д����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/
//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "i2c.h"
#include "delay.h"
#include "usart.h"




IIC_INFO iicInfo;



/*
************************************************************
*	�������ƣ�	IIC_SpeedCtl
*
*	�������ܣ�	���IIC�ٶȿ���
*
*	��ڲ�����	speed����ʱ����
*
*	���ز�����	��
*
*	˵����		��λ��΢��
************************************************************
*/
void IIC_SpeedCtl(unsigned short speed)
{

	iicInfo.speed = speed;

}

/*
************************************************************
*	�������ƣ�	IIC_Init
*
*	�������ܣ�	���IIC����IO��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		ʹ�ÿ�©��ʽ���������Բ����л�IO�ڵ������������
************************************************************
*/
void IIC_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			//��©����������ȥ�л�������뷽��
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStruct);
	
	IIC_SpeedCtl(5);
	
	SDA_H;													//����SDA�ߣ����ڿ���״̬
	SCL_H;													//����SCL�ߣ����ڿ���״̬

}

/*
************************************************************
*	�������ƣ�	IIC_Start
*
*	�������ܣ�	���IIC��ʼ�ź�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void IIC_Start(void)
{
	
	SDA_H;						//����SDA��
	SCL_H;						//����SCL��
	DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���
	
	SDA_L;						//��SCL��Ϊ��ʱ��SDA��һ���½��ش���ʼ�ź�
	DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���
	SCL_L;						//ǯסSCL�ߣ��Ա㷢������

}

/*
************************************************************
*	�������ƣ�	IIC_Stop
*
*	�������ܣ�	���IICֹͣ�ź�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void IIC_Stop(void)
{

	SDA_L;						//����SDA��
	SCL_L;						//����SCL��
	DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���
	
	SCL_H;						//����SCL��
	SDA_H;						//����SDA�ߣ���SCL��Ϊ��ʱ��SDA��һ�������ش���ֹͣ�ź�
	DelayUs(iicInfo.speed);

}

/*
************************************************************
*	�������ƣ�	IIC_WaitAck
*
*	�������ܣ�	���IIC�ȴ�Ӧ��
*
*	��ڲ�����	timeOut����ʱʱ��
*
*	���ز�����	��
*
*	˵����		��λ��΢��
************************************************************
*/
_Bool IIC_WaitAck(unsigned int timeOut)
{
	
	
	SDA_H;DelayUs(iicInfo.speed);			//����SDA��
	SCL_H;DelayUs(iicInfo.speed);			//����SCL��
	
	while(SDA_R)							//�������SDA��Ϊ1����ȴ���Ӧ���ź�Ӧ��0
	{
		if(--timeOut)
		{
			UsartPrintf(USART1, "WaitAck TimeOut\r\n");

			IIC_Stop();						//��ʱδ�յ�Ӧ����ֹͣ����
			
			return IIC_Err;					//����ʧ��
		}
		
		DelayUs(iicInfo.speed);
	}
	
	SCL_L;									//����SCL�ߣ��Ա�����շ�����
	
	return IIC_OK;							//���سɹ�
	
}

/*
************************************************************
*	�������ƣ�	IIC_Ack
*
*	�������ܣ�	���IIC����һ��Ӧ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��SDA��Ϊ��ʱ��SCL��һ�������������һ��Ӧ���ź�
************************************************************
*/
void IIC_Ack(void)
{
	
	SCL_L;						//����SCL��
	SDA_L;						//����SDA��
	DelayUs(iicInfo.speed);
	SCL_H;						//����SCL��
	DelayUs(iicInfo.speed);
	SCL_L;						//����SCL��
	
}

/*
************************************************************
*	�������ƣ�	IIC_NAck
*
*	�������ܣ�	���IIC����һ�Ǹ�Ӧ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��SDA��Ϊ��ʱ��SCL��һ�������������һ����Ӧ���ź�
************************************************************
*/
void IIC_NAck(void)
{
	
	SCL_L;						//����SCL��
	SDA_H;						//����SDA��
	DelayUs(iicInfo.speed);
	SCL_H;						//����SCL��
	DelayUs(iicInfo.speed);
	SCL_L;						//����SCL��
	
}

/*
************************************************************
*	�������ƣ�	IIC_SendByte
*
*	�������ܣ�	���IIC����һ���ֽ�
*
*	��ڲ�����	byte����Ҫ���͵��ֽ�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void IIC_SendByte(unsigned char byte)
{

	unsigned char count = 0;
	
    SCL_L;							//����ʱ�ӿ�ʼ���ݴ���
	
    for(; count < 8; count++)		//ѭ��8�Σ�ÿ�η���һ��bit
    {
		if(byte & 0x80)				//�������λ
			SDA_H;
		else
			SDA_L;
		
		byte <<= 1;					//byte����1λ
		
		DelayUs(iicInfo.speed);
		SCL_H;
		DelayUs(iicInfo.speed);
		SCL_L;
    }

}

/*
************************************************************
*	�������ƣ�	IIC_RecvByte
*
*	�������ܣ�	���IIC����һ���ֽ�
*
*	��ڲ�����	��
*
*	���ز�����	���յ����ֽ�����
*
*	˵����		
************************************************************
*/
unsigned char IIC_RecvByte(void)
{
	
	unsigned char count = 0, receive = 0;
	
	SDA_H;							//����SDA�ߣ���©״̬�£����������Ա��ȡ����
	
    for(; count < 8; count++ )		//ѭ��8�Σ�ÿ�η���һ��bit
	{
		SCL_L;
		DelayUs(iicInfo.speed);
		SCL_H;
		
        receive <<= 1;				//����һλ
		
        if(SDA_R)					//���SDA��Ϊ1����receive����������ÿ���������Ƕ�bit0��+1��Ȼ����һ��ѭ����������һ��
			receive++;
		
		DelayUs(iicInfo.speed);
    }
	
    return receive;
	
}

/*
************************************************************
*	�������ƣ�	I2C_WriteByte
*
*	�������ܣ�	���IICдһ������
*
*	��ڲ�����	slaveAddr���ӻ���ַ
*				regAddr���Ĵ�����ַ
*				byte����Ҫд�������
*
*	���ز�����	0-д��ɹ�	1-д��ʧ��
*
*	˵����		*byte�ǻ���д�����ݵı����ĵ�ַ����Ϊ��Щ�Ĵ���ֻ��Ҫ�����¼Ĵ�����������Ҫд��ֵ
************************************************************
*/
_Bool I2C_WriteByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *byte)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д
	
	IIC_Start();				//��ʼ�ź�
	
	IIC_SendByte(addr);			//�����豸��ַ(д)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//���ͼĴ�����ַ
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	if(byte)
	{
		IIC_SendByte(*byte);	//��������
		if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
			return IIC_Err;
	}
	
	IIC_Stop();					//ֹͣ�ź�
	
	return IIC_OK;

}

/*
************************************************************
*	�������ƣ�	I2C_ReadByte
*
*	�������ܣ�	���IIC��ȡһ���ֽ�
*
*	��ڲ�����	slaveAddr���ӻ���ַ
*				regAddr���Ĵ�����ַ
*				val����Ҫ��ȡ�����ݻ���
*
*	���ز�����	0-�ɹ�		1-ʧ��
*
*	˵����		val��һ����������ĵ�ַ
************************************************************
*/
_Bool I2C_ReadByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *val)
{

	unsigned char addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д
	
	IIC_Start();				//��ʼ�ź�
	
	IIC_SendByte(addr);			//�����豸��ַ(д)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//���ͼĴ�����ַ
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_Start();				//�����ź�
	
	IIC_SendByte(addr + 1);		//�����豸��ַ(��)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	*val = IIC_RecvByte();		//����
	IIC_NAck();					//����һ����Ӧ���źţ������ȡ����
	
	IIC_Stop();					//ֹͣ�ź�
	
	return IIC_OK;

}

/*
************************************************************
*	�������ƣ�	I2C_WriteBytes
*
*	�������ܣ�	���IICд�������
*
*	��ڲ�����	slaveAddr���ӻ���ַ
*				regAddr���Ĵ�����ַ
*				buf����Ҫд������ݻ���
*				num�����ݳ���
*
*	���ز�����	0-д��ɹ�	1-д��ʧ��
*
*	˵����		*buf��һ�������ָ��һ����������ָ��
************************************************************
*/
_Bool I2C_WriteBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д
	
	IIC_Start();				//��ʼ�ź�
	
	IIC_SendByte(addr);			//�����豸��ַ(д)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//���ͼĴ�����ַ
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	while(num--)				//ѭ��д������
	{
		IIC_SendByte(*buf);		//��������
		if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
			return IIC_Err;
		
		buf++;					//����ָ��ƫ�Ƶ���һ��
		
		DelayUs(10);
	}
	
	IIC_Stop();					//ֹͣ�ź�
	
	return IIC_OK;

}

/*
************************************************************
*	�������ƣ�	I2C_ReadBytes
*
*	�������ܣ�	���IIC���������
*
*	��ڲ�����	slaveAddr���ӻ���ַ
*				regAddr���Ĵ�����ַ
*				buf����Ҫ��ȡ�����ݻ���
*				num�����ݳ���
*
*	���ز�����	0-д��ɹ�	1-д��ʧ��
*
*	˵����		*buf��һ�������ָ��һ����������ָ��
************************************************************
*/
_Bool I2C_ReadBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned short addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д
	
	IIC_Start();				//��ʼ�ź�
	
	IIC_SendByte(addr);			//�����豸��ַ(д)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//���ͼĴ�����ַ
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	IIC_Start();				//�����ź�
	
	IIC_SendByte(addr + 1);		//�����豸��ַ(��)
	if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
		return IIC_Err;
	
	while(num--)
	{
		*buf = IIC_RecvByte();
		buf++;					//ƫ�Ƶ���һ�����ݴ洢��ַ
		
		if(num == 0)
        {
           IIC_NAck();			//���һ��������Ҫ��NOACK
        }
        else
        {
          IIC_Ack();			//��ӦACK
		}
	}
	
	IIC_Stop();
	
	return IIC_OK;

}
