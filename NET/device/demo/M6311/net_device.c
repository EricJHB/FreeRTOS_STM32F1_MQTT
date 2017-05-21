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
	*					  2.���ӻ�վ��λ���ܣ���net_device.h��ͨ����M6311_LOCATION��ȷʵ�Ƿ�ʹ�á�
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
#include "usart.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {0, 0, 7, 0, 0, 0};


GPS_INFO gps;


void M6311_Location(void)
{
	
	unsigned char initStep = 0;
	
	char *dataPtr, *ptr;
	
	char mcc[8]; 																				//�ƶ����Һ���
	char mnc[8];																				//�ƶ��������
	char lac[16];																				//λ��������
	char cell[16];																				//С����
	
	char sendBuf[128];

	while(initStep != 3)
	{
		switch(initStep)
		{
			case 0:
			
				while(NET_DEVICE_SendCmd("AT+IPSTART=\"TCP\",\"183.230.40.83\",80\r\n", "CONNECT OK", 1))	//��λ��ip
					RTOS_TimeDly(100);
				
				while(NET_DEVICE_SendCmd("AT+CCED=0,1\r\n", "OK", 0))							//���ҵ�
					RTOS_TimeDly(100);
				
				initStep += 2;
			
			break;
				
			case 1:
				
				while(NET_DEVICE_SendCmd("AT+CCED=0,2\r\n", "OK", 0))							//���ҵ�
					RTOS_TimeDly(100);
				
				initStep++;
			
			break;
				
			case 2:
				
				dataPtr = strstr((char *)netIOInfo.buf, "+CCED:");
	
				if(dataPtr != NULL)
				{
					dataPtr += 7;
					memset(mcc, 0, sizeof(mcc));
					ptr = mcc;
					while(*dataPtr >= '0' && *dataPtr <= '9')
						*ptr++ = *dataPtr++;
					
					dataPtr++;
					memset(mnc, 0, sizeof(mnc));
					ptr = mnc;
					while(*dataPtr >= '0' && *dataPtr <= '9')
						*ptr++ = *dataPtr++;
					
					dataPtr++;
					memset(lac, 0, sizeof(lac));
					ptr = lac;
					while(*dataPtr >= '0' && *dataPtr <= '9')
						*ptr++ = *dataPtr++;
					
					dataPtr++;
					memset(cell, 0, sizeof(cell));
					ptr = cell;
					while(*dataPtr >= '0' && *dataPtr <= '9')
						*ptr++ = *dataPtr++;
					
					UsartPrintf(USART_DEBUG, "mcc = %s, mcc = %s, lac = %s, cell = %s\r\n", mcc, mnc, lac, cell);
					
					memset(sendBuf, 0, sizeof(sendBuf));
					NET_DEVICE_ClrData();
					
					snprintf(sendBuf, sizeof(sendBuf), "GET http://api.lbs.heclouds.com/api/gsmlbs?mcc=%s&mnc=%s&cell=%s&lac=%s&apikey=B985C9CC0017EAD1EA11126F37CB1BA1\r\nHTTP/1.1\r\nHost:api.lbs.heclouds.com\r\n",
					
						mcc, mnc, cell, lac);
					
					NET_DEVICE_SendData((unsigned char *)sendBuf, strlen(sendBuf));
					
					dataPtr = (char *)NET_DEVICE_GetIPD(200);
					if(dataPtr != NULL)
					{
						if(strstr((char *)netIOInfo.buf, "Basestation"))							//δ���ֻ�վ
						{
							UsartPrintf(USART_DEBUG, "δ���ֻ�վ�������л��ٽ�λ��\r\n");
							
							NET_DEVICE_ClrData();
							
							initStep = 1;
							
							break;
						}
						
						if(strstr((char *)netIOInfo.buf, "Balance"))								//����
						{
							UsartPrintf(USART_DEBUG, "����\r\n");
							
							NET_DEVICE_ClrData();
							gps.flag = 0;
							
							return;
						}
						
						if(strstr((char *)netIOInfo.buf, "Unauthorized"))							//�Ƿ���apikey
						{
							UsartPrintf(USART_DEBUG, "�Ƿ���apikey\r\n");
							
							NET_DEVICE_ClrData();
							gps.flag = 0;
							
							return;
						}
						
						dataPtr = strstr(dataPtr, "\"lng\" :");
						dataPtr += 8;
						ptr = gps.lon;
						while((*dataPtr >= '0' && *dataPtr <= '9') || *dataPtr == '.')
							*ptr++ = *dataPtr++;
						
						dataPtr = strstr(dataPtr, "\"lat\" :");
						dataPtr += 8;
						ptr = gps.lat;
						while((*dataPtr >= '0' && *dataPtr <= '9') || *dataPtr == '.')
							*ptr++ = *dataPtr++;
						
						gps.flag = 1;
						
						UsartPrintf(USART_DEBUG, "lon = %s, lat = %s\r\n", gps.lon, gps.lat);
					}
				}
				else
					UsartPrintf(USART_DEBUG, "dataptr is null\r\n");
				
				NET_DEVICE_SendCmd("AT+IPCLOSE\r\n", "OK", 1);							//����ǰ�ȹر�һ��
				UsartPrintf(USART_DEBUG, "Tips:	CIPCLOSE\r\n");
				
				NET_DEVICE_ClrData();
				
				RTOS_TimeDly(100);														//�ȴ�
				
				initStep++;
			
			break;
		}
	}

}

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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

	//
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_1;			//GPIOA1-��λ
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	gpioInitStruct.GPIO_Pin = GPIO_Pin_4;			//GPIOC4-PowerKey
	GPIO_Init(GPIOC, &gpioInitStruct);
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IPD;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_7;			//GPIOA7-status
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	NET_DEVICE_PWRK_ON;
	
	NET_IO_Init();									//�����豸����IO���ʼ��
	
	netDeviceInfo.reboot = 0;
	
	while(!NET_DEVICE_STATUS)
		RTOS_TimeDly(2);

}

#if(NET_DEVICE_TRANS == 1)
//==========================================================
//	�������ƣ�	M6311_QuitTrans
//
//	�������ܣ�	�˳�͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��������������+����Ȼ��ر�͸��ģʽ
//==========================================================
void M6311_QuitTrans(void)
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
	
	NET_DEVICE_SendCmd("AT+CMMODE=0\r\n","OK", 1);	//�ر�͸��ģʽ

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

	return NET_DEVICE_SendCmd("AT\r\n", "OK", 1);

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
	
	char cfgBuffer[32];
	
	switch(netDeviceInfo.initStep)
	{
		case 0:
			
			NET_IO_Send((unsigned char *)"AT+CMVERSION\r\n", strlen("AT+CMVERSION\r\n"));
			RTOS_TimeDly(30);
			UsartPrintf(USART_DEBUG, "\r\n**************************\r\n%s\r\n**************************\r\n", netIOInfo.buf);
			NET_DEVICE_ClrData();
			netDeviceInfo.initStep++;
		
		break;
		
		case 1:
			Led4_Set(LED_ON);
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+SSYS?\r\n");
			if(!NET_DEVICE_SendCmd("AT+SSYS?\r\n","OK", 1)) 					//�л�sim��   0-���ÿ�		1-���ÿ�	����ʹ�����ÿ�
				netDeviceInfo.initStep++;
		break;
		
		case 2:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+SIM1\r\n");
			if(!NET_DEVICE_SendCmd("AT+SIM1\r\n","OK", 1)) 						//������ÿ��Ƿ����		����+SIM1: EXSIT
				netDeviceInfo.initStep++;
		break;
		
		case 3:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CPIN?\r\n");
			if(!NET_DEVICE_SendCmd("AT+CPIN?\r\n", "+CPIN: READY", 1))			//ȷ��SIM��PIN�����������READY����ʾ�����ɹ�
				netDeviceInfo.initStep++;
		break;
		
		case 4: //�Զ��жϿ�����
		{
			char resBuf[5] = {0, 0, 0, 0, 0};
			char text[2] = {0, 0};
			
			strcpy(resBuf, "0,");
			sprintf(text, "%d", netDeviceInfo.cardType);
			strcat(resBuf, text);
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CREG?  %d\r\n",
										netDeviceInfo.cardType);
			if(!NET_DEVICE_SendCmd("AT+CREG?\r\n", resBuf, 1)) 				//ȷ�����������ɹ�,OK
				netDeviceInfo.initStep++;
			else 															//���ʧ�����ⷵ�ص�����
			{
				if(netIOInfo.buf[11] != 48)
					netDeviceInfo.cardType = netIOInfo.buf[11] - 48;
				
				NET_DEVICE_ClrData();
			}
		}
		break;
			
		case 5:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CSQ\r\n");
			if(!NET_DEVICE_SendCmd("AT+CSQ\r\n","OK", 1))					//��ѯ�ź�ǿ��,OK
				netDeviceInfo.initStep++;
		break;
			
		case 6:
				UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGREG?\r\n");		//�������ע��״̬
				if(!NET_DEVICE_SendCmd("AT+CGREG?\r\n","OK", 1))
					netDeviceInfo.initStep++;
		break;
		
		case 7:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGACT=1,1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CGACT=1,1\r\n","OK", 1)) 			//����
				netDeviceInfo.initStep++;
		break;
		
		case 8:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGATT=1\r\n");			//����GPRSҵ��
			if(!NET_DEVICE_SendCmd("AT+CGATT=1\r\n","OK", 1))
				netDeviceInfo.initStep++;
		break;
			
		case 9:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CMMUX=0\r\n");
			if(!NET_DEVICE_SendCmd("AT+CMMUX=0\r\n","OK", 1)) 				//����Ϊ�����ӣ���Ȼƽ̨IP��������
				netDeviceInfo.initStep++;
		break;
			
#if(NET_DEVICE_TRANS == 1)
			
		case 10:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CMMODE=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CMMODE=1\r\n","OK", 1))					//����͸��
				netDeviceInfo.initStep++;
		break;
		
		case 11:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CMTCFG=1,1024,1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CMTCFG=1,1024,1\r\n","OK", 1)) 		//����͸������󳤶�2000�ֽڣ������100ms�����ó�hexģʽ
				netDeviceInfo.initStep++;
		break;
			
		case 12:
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
			strcpy(cfgBuffer, "AT+IPSTART=\"TCP\",\"");
			strcat(cfgBuffer, ip);
			strcat(cfgBuffer, "\",");
			strcat(cfgBuffer, port);
			strcat(cfgBuffer, "\r\n");
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
			
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
			if(!NET_DEVICE_SendCmd(cfgBuffer,"CONNECT", 1)) 				//����ƽ̨
			{
				Led4_Set(LED_OFF);
				netDeviceInfo.initStep++;
			}
		break;
			
#else
			
		case 10:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CMHEAD=1\r\n");
		
			if(!NET_DEVICE_SendCmd("AT+CMHEAD=1\r\n","OK", 1))				//��ʾIPͷ
			{
#if(M6311_LOCATION == 1)
				M6311_Location();
#endif
				netDeviceInfo.initStep++;
			}
		break;
		
		case 11:
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
			strcpy(cfgBuffer, "AT+IPSTART=\"TCP\",\"");
			strcat(cfgBuffer, ip);
			strcat(cfgBuffer, "\",");
			strcat(cfgBuffer, port);
			strcat(cfgBuffer, "\r\n");
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
			
			if(!NET_DEVICE_SendCmd(cfgBuffer,"CONNECT", 1)) 				//����ƽ̨
			{
				Led4_Set(LED_OFF);
				netDeviceInfo.initStep++;
			}
		break;
			
		case 12:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CMSTATE\r\n");
			if(!NET_DEVICE_SendCmd("AT+CMSTATE\r\n", "CONNECTED", 1))		//�������״̬
				netDeviceInfo.initStep++;
			else
			{
				NET_DEVICE_SendCmd("AT+IPCLOSE\r\n", "OK", 1);
				RTOS_TimeDly(20);
				NET_DEVICE_ReConfig(11);
			}
		break;
#endif
			
		default:
		break;
	}

	if(netDeviceInfo.initStep == 13)
		return 0;
	else
		return 1;

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
	M6311_QuitTrans();
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif
	
	UsartPrintf(USART_DEBUG, "Tips:	M6311_Reset\r\n");

	//��λģ��	�������λ�Ļ�������������ʱ��6311���޷���ʼ���ɹ���
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

#if(NET_DEVICE_TRANS == 1)
	return 1;
#else
	char cfgBuffer[32];
	
	NET_DEVICE_SendCmd("AT+IPCLOSE\r\n", "OK", 1);							//����ǰ�ȹر�һ��
	UsartPrintf(USART_DEBUG, "Tips:	CIPCLOSE\r\n");
	RTOS_TimeDly(100);														//�ȴ�
	
	memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
	strcpy(cfgBuffer, "AT+IPSTART=\"TCP\",\"");
	strcat(cfgBuffer, ip);
	strcat(cfgBuffer, "\",");
	strcat(cfgBuffer, port);
	strcat(cfgBuffer, "\r\n");
	UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
	
	UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
	if(!NET_DEVICE_SendCmd(cfgBuffer,"CONNECT", 1))							//����ƽ̨
		return 0;
	else
		return 1;
#endif

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
_Bool NET_DEVICE_SendCmd(char *cmd, char *res, _Bool mode) //
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
	NET_IO_Send(data, len);							//�����豸������������
#else
	char cmdBuf[30];
	
	RTOS_TimeDly(10);								//�ȴ�һ��

	NET_IO_ClearRecive();							//��ս��ջ���
	sprintf(cmdBuf, "AT+IPSEND=%d\r\n", len);		//��������
	//if(!NET_DEVICE_SendCmd(cmdBuf, ">", 1))		//�յ���>��ʱ���Է�������
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
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��M6311�ķ��ظ�ʽΪ	"<IPDATA: x>\r\nyyy"	x�������ݳ��ȣ�yyy����������
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
			ptrIPD = strstr((char *)netIOInfo.buf, "IPDATA:");			//������IPDATA��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strstr(ptrIPD, ":");ptrIPD++;					//�ҵ�':'  ��Ȼ��ƫ�Ƶ���һ���ַ����������ݳ��ȵĵ�һ������
				
				while(*ptrIPD != '\r')									//��':'��'\r' ֮��Ķ������ݳ��ȵ�����
				{
					sByte[count++] = *ptrIPD++;
				}
				byte = (unsigned short)atoi(sByte);						//���ַ�תΪ��ֵ��ʽ
				
				ptrIPD += 2;											//��ʱptrIPDָ�뻹ָ��':'��������Ҫƫ�Ƶ�����������
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

	NET_IO_ClearRecive();		//��ջ���

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
	
#if(NET_DEVICE_TRANS == 1)
	return 3;
#else
	
	unsigned char status = 0;
	
	NET_IO_ClearRecive();												//��ջ���
	
	if(NET_DEVICE_SendCmd("AT+SIM1\r\n", "OK", 1))						//���sim����ʧ
	{
		UsartPrintf(USART_DEBUG, "WARN:		No Sim Card\r\n");
		status = 5;
	}
	else
		UsartPrintf(USART_DEBUG, "Tips:		Sim Card\r\n");
	
	if(!NET_DEVICE_SendCmd("AT+CMSTATE\r\n", "CLOSED", 1))				//�������״̬
	{
		UsartPrintf(USART_DEBUG, "WARN:		CLOSED\r\n");
		status = 1;
	}
	else if(!NET_DEVICE_SendCmd("AT+CMSTATE\r\n", "CONNECTED", 1))		//�������״̬
	{
		UsartPrintf(USART_DEBUG, "WARN:		CONNECTED\r\n");
		status = 0;
	}
	
	return status;
	
#endif

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
