/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_device.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-03-02
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		�����豸Ӧ�ò�
	*
	*	�޸ļ�¼��	V1.1��1.ƽ̨IP��PORTͨ����������ķ�ʽȷ��������˲�ͬЭ�������豸������ͨ�õ����⡣
	*					  2.ȡ�����ֶ��������磬�ϵ�ȴ�wifiģ���Զ����ӣ������ɹ���ʹ��OneNET���ںŽ���������
	*					  3.NET_DEVICE_SendCmd����������mode���������Ƿ������������ķ���ֵ��
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f10x.h"	//��Ƭ��ͷ�ļ�

#include "net_device.h"	//�����豸Ӧ�ò�
#include "net_io.h"		//�����豸����IO��

//Ӳ������
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {0, 0, 0, 0, 0}; //





//==========================================================
//	�������ƣ�	NET_DEVICE_IO_Init
//
//	�������ܣ�	��ʼ�������豸IO��
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��ʼ�������豸�Ŀ������š������շ����ܵ�
//==========================================================
void NET_DEVICE_IO_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266��λ����
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-��λ
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	NET_IO_Init();											//�����豸����IO���ʼ��
	
	netDeviceInfo.reboot = 0;

}

#if(NET_DEVICE_TRANS == 1)
//==========================================================
//	�������ƣ�	ESP8266_QuitTrans
//
//	�������ܣ�	�˳�͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��������������+����Ȼ��ر�͸��ģʽ
//==========================================================
void ESP8266_QuitTrans(void)
{

	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';
	RTOS_TimeDly(3); 					//���ڴ�����֡ʱ��(10ms)
	
	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';        
	RTOS_TimeDly(3); 					//���ڴ�����֡ʱ��(10ms)
	
	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';        
	RTOS_TimeDly(20);					//�ȴ�100ms
	
	NET_DEVICE_SendCmd("AT+CIPMODE=0\r\n", "OK", 1); //�ر�͸��ģʽ

}

//==========================================================
//	�������ƣ�	ESP8266_EnterTrans
//
//	�������ܣ�	����͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_EnterTrans(void)
{
	
	NET_DEVICE_SendCmd("AT+CIPMUX=0\r\n", "OK", 1);	//������ģʽ

	NET_DEVICE_SendCmd("AT+CIPMODE=1\r\n", "OK", 1);//ʹ��͸��
	
	NET_DEVICE_SendCmd("AT+CIPSEND\r\n", ">", 1);	//��������
	
	RTOS_TimeDly(20);								//�ȴ�100ms

}

#endif

//==========================================================
//	�������ƣ�	NET_DEVICE_Exist
//
//	�������ܣ�	�����豸���ڼ��
//
//	��ڲ�����	��
//
//	���ز�����	���ؽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Exist(void)
{

	unsigned char timeOut = 50;
	_Bool status = 1;
	unsigned char count = 0;
	
	NET_DEVICE_ClrData();
	
	while(timeOut--)												//�ȴ�
	{
		RTOS_TimeDly(20);											//����ȴ�
		
		
		if(!NET_DEVICE_SendCmd("AT+CIPSTATUS\r\n", "STATUS:", 1))//eric
		{
			status = 0;
			break;
		}
		if(KeyScan(GPIOC, KEY0) == KEYDOWN)							//������������ģʽ
			count++;
		else
			count = 0;
		
		if((NET_IO_WaitRecive() == REV_OK) || count >= 20)			//����յ�����
		{
			if((strstr((char *)netIOInfo.buf, "WIFI CONNECTED")) && (count == 0))
			{
				status = 0;
			}
			else if(strstr((char *)netIOInfo.buf, "WIFI DISCONNECT") || count >= 20)
			{
				count = 0;
#if(PHONE_AP_MODE == 0)
				NET_DEVICE_SendCmd("AT+CWSMARTSTART=2\r\n", "OK", 1);
				UsartPrintf(USART_DEBUG, "��ʹ��OneNET���ں�����SSID��PSWD\r\n");
				NET_DEVICE_ClrData();
				
				while(1)
				{
					Led4_Set(LED_ON);Led5_Set(LED_ON);Led6_Set(LED_ON);Led7_Set(LED_ON);
					
					if(strstr((char *)netIOInfo.buf, "SMART SUCCESS"))
					{
						UsartPrintf(USART_DEBUG, "�յ�:\r\n%s\r\n", strstr((char *)netIOInfo.buf, "SSID:"));
						NET_DEVICE_ClrData();
						status = 1;
						Led4_Set(LED_OFF);Led5_Set(LED_OFF);Led6_Set(LED_OFF);Led7_Set(LED_OFF);
						break;
					}
					
					RTOS_TimeDly(100);
					Led4_Set(LED_OFF);Led5_Set(LED_OFF);Led6_Set(LED_OFF);Led7_Set(LED_OFF);
					RTOS_TimeDly(100);
				}
				
				NET_DEVICE_Reset();															//��λһ�£���Ȼ�����Զ�����
#else
				UsartPrintf(USART_DEBUG, "STA Tips:	Link Wifi\r\n");
				
				while(NET_DEVICE_SendCmd("AT+CWJAP=\"ONENET\",\"IOT@Chinamobile123\"\r\n", "GOT IP", 1))
				{
					Led7_Set(LED_ON);
					RTOS_TimeDly(100);
					
					Led7_Set(LED_OFF);
					RTOS_TimeDly(100);
				}
				
				status = 0;
				
				break;
#endif
			}
			else
			{
					UsartPrintf(USART_DEBUG, "netIOInfo.buf:	%s\r\n",netIOInfo.buf);
				status = 1;
			}
			break;
		}
	}
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Init
//
//	�������ܣ�	�����豸��ʼ��
//
//	��ڲ�����	��
//
//	���ز�����	���س�ʼ�����
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Init(char *ip, char *port)
{
	
	unsigned char errCount = 0;
	_Bool status = 1;
	char cfgBuffer[32];

	switch(netDeviceInfo.initStep)
	{
		case 0:
			
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
			strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
			strcat(cfgBuffer, ip);
			strcat(cfgBuffer, "\",");
			strcat(cfgBuffer, port);
			strcat(cfgBuffer, "\r\n");
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
		
			while(NET_DEVICE_SendCmd(cfgBuffer, "CONNECT", 1))				//����ƽ̨��������CONNECT�������ʧ�ܻ����ѭ����
			{
				Led7_Set(LED_ON);
				RTOS_TimeDly(100);
				
				Led7_Set(LED_OFF);
				RTOS_TimeDly(100);
				
				if(++errCount >= 10)
				{
					UsartPrintf(USART_DEBUG, "PT info Error,Use APP -> 8266\r\n");
					status = 1;
					break;
				}
			}
			
			if(errCount != 10)
				netDeviceInfo.initStep++;
		
		break;
			
#if(NET_DEVICE_TRANS == 1)
			
		case 2:

			ESP8266_EnterTrans();											//����͸��ģʽ
			UsartPrintf(USART_DEBUG, "Tips:	EnterTrans\r\n");

			netDeviceInfo.initStep++;

		break;
			
#endif
		
		default:
			status = 0;
			UsartPrintf(USART_DEBUG, "Tips:	ESP8266 STA_Mode OK\r\n");
		break;
	}
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Reset
//
//	�������ܣ�	�����豸��λ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_Reset(void)
{
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();	//�˳�͸��ģʽ
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

	UsartPrintf(USART_DEBUG, "Tips:	ESP8266_Reset\r\n");
	
	NET_DEVICE_RST_ON;		//��λ
	RTOS_TimeDly(50);
	
	NET_DEVICE_RST_OFF;		//������λ
	RTOS_TimeDly(200);

}

//==========================================================
//	�������ƣ�	NET_DEVICE_ReLink
//
//	�������ܣ�	����ƽ̨
//
//	��ڲ�����	��
//
//	���ز�����	�������ӽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_ReLink(char *ip, char *port)
{
	
	_Bool status = 0;
	char cfgBuffer[32];
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif
	
	NET_DEVICE_SendCmd("AT+CIPCLOSE\r\n", "OK", 1);												//����ǰ�ȹر�һ��
	UsartPrintf(USART_DEBUG, "Tips:	CIPCLOSE\r\n");
	RTOS_TimeDly(100);																			//�ȴ�

	memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
	strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
	strcat(cfgBuffer, ip);
	strcat(cfgBuffer, "\",");
	strcat(cfgBuffer, port);
	strcat(cfgBuffer, "\r\n");
	UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);

	status = NET_DEVICE_SendCmd(cfgBuffer, "CONNECT", 1);										//��������
	
#if(NET_DEVICE_TRANS == 1)
		ESP8266_EnterTrans();
		UsartPrintf(USART_DEBUG, "Tips:	EnterTrans\r\n");
#endif
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_SendCmd
//
//	�������ܣ�	�������豸����һ��������ȴ���ȷ����Ӧ
//
//	��ڲ�����	cmd����Ҫ���͵�����
//				res����Ҫ��������Ӧ
//				mode��1-�������		0-�����(�ܻ�ȡ������Ϣ)
//
//	���ز�����	�������ӽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_SendCmd(char *cmd, char *res, _Bool mode)
{
	
	unsigned char timeOut = 200;
	
	NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));	//д��������豸
	
	while(timeOut--)												//�ȴ�
	{
		if(NET_IO_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)netIOInfo.buf, res) != NULL)	//����������ؼ���
			{
				if(mode)
					NET_IO_ClearRecive();							//��ջ���
				
				return 0;
			}
		}
		
		RTOS_TimeDly(2);											//����ȴ�
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_SendData
//
//	�������ܣ�	ʹ�����豸�������ݵ�ƽ̨
//
//	��ڲ�����	data����Ҫ���͵�����
//				len�����ݳ���
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	
#if(NET_DEVICE_TRANS == 1)
	NET_IO_Send(data, len);  						//�����豸������������
#else
	char cmdBuf[32];

	RTOS_TimeDly(10);								//�ȴ�һ��
	
	NET_IO_ClearRecive();							//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	//if(!NET_DEVICE_SendCmd(cmdBuf, ">", 1))			//�յ���>��ʱ���Է�������
	NET_DEVICE_SendCmd(cmdBuf, ">", 1);
	{
		NET_IO_Send(data, len);  					//�����豸������������
	}
#endif

}

//==========================================================
//	�������ƣ�	NET_DEVICE_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *NET_DEVICE_GetIPD(unsigned short timeOut)
{

#if(NET_DEVICE_TRANS == 0)
	unsigned short byte = 0, count = 0;
	char sByte[5];
	char *ptrIPD;
#endif
	
	do
	{
		if(NET_IO_WaitRecive() == REV_OK)								//����������
		{
#if(NET_DEVICE_TRANS == 0)
			ptrIPD = strstr((char *)netIOInfo.buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strstr(ptrIPD, ",");ptrIPD++;					//�ҵ�','  ��Ȼ��ƫ�Ƶ���һ���ַ����������ݳ��ȵĵ�һ������
				
				while(*ptrIPD != ':')									//��','��':' ֮��Ķ������ݳ��ȵ�����
				{
					sByte[count++] = *ptrIPD++;
				}
				byte = (unsigned short)atoi(sByte);						//���ַ�תΪ��ֵ��ʽ
				
				ptrIPD++;												//��ʱptrIPDָ�뻹ָ��':'��������Ҫƫ�Ƶ��¸�����
				for(count = 0; count < byte; count++)					//��������
				{
					netIOInfo.buf[count] = *ptrIPD++;
				}
				memset(netIOInfo.buf + byte,
						0, sizeof(netIOInfo.buf) - byte);				//����ߵ��������
				netDeviceInfo.ipdBytes = byte;
				
				return netIOInfo.buf;									//������ɣ���������ָ��
			}
#else
			netDeviceInfo.ipdBytes = byte;
			return netIOInfo.buf;
#endif
		}
		
		RTOS_TimeDly(2);												//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	NET_DEVICE_ClrData
//
//	�������ܣ�	��������豸���ݽ��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_ClrData(void)
{

	NET_IO_ClearRecive();	//��ջ���

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Check
//
//	�������ܣ�	��������豸����״̬
//
//	��ڲ�����	��
//
//	���ز�����	����״̬
//
//	˵����		
//==========================================================
unsigned char NET_DEVICE_Check(void)
{
	
	unsigned char status = 0;
	unsigned char timeOut = 200;
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

	NET_IO_ClearRecive();												//��ջ���
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n",  14);				//����״̬���
	
	while(--timeOut)
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)netIOInfo.buf, "STATUS:2"))			//���IP
			{
				status = 2;
				UsartPrintf(USART_DEBUG, "ESP8266 Got IP\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:3"))	//��������
			{
				status = 0;
				UsartPrintf(USART_DEBUG, "ESP8266 Connect OK\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:4"))	//ʧȥ����
			{
				status = 1;
				UsartPrintf(USART_DEBUG, "ESP8266 Lost Connect\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:5"))	//�������
			{
				status = 3;
				UsartPrintf(USART_DEBUG, "ESP8266 Lost\r\n");			//�豸��ʧ
			}
			
			break;
		}
		
		RTOS_TimeDly(2);
	}
	
	if(timeOut == 0)
	{
		status = 3;
		UsartPrintf(USART_DEBUG, "ESP8266 TimeOut\r\n");
	}
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_ReConfig
//
//	�������ܣ�	�豸�����豸��ʼ���Ĳ���
//
//	��ڲ�����	����ֵ
//
//	���ز�����	��
//
//	˵����		�ú������õĲ����������豸��ʼ������õ�
//==========================================================
void NET_DEVICE_ReConfig(unsigned char step)
{

	netDeviceInfo.initStep = step;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Set_DataMode
//
//	�������ܣ�	�����豸�����շ�ģʽ
//
//	��ڲ�����	ģʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_Set_DataMode(unsigned char mode)
{

	netDeviceInfo.dataType = mode;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Get_DataMode
//
//	�������ܣ�	��ȡ�豸�����շ�ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	ģʽ
//
//	˵����		
//==========================================================
unsigned char NET_DEVICE_Get_DataMode(void)
{

	return netDeviceInfo.dataType;

}

