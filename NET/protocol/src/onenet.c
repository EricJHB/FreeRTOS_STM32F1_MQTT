/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-12-15
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��onenetƽ̨�����ݽ�����Э���
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "net_device.h"

//Э���ļ�
#include "onenet.h"
#include "fault.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "iwdg.h"
#include "hwtimer.h"
#include "selfcheck.h"

//ͼƬ�����ļ�
#include "image_2k.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



struct MqttSampleContext ctx[1];
char *topics[] = {"kylinV22", "pcTopic"};


ONETNET_INFO oneNetInfo = {"", "", "", "", "183.230.40.33", "6002", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];


/*
************************************************************
*	�������ƣ�	OneNet_DevLink
*
*	�������ܣ�	��onenet��������
*
*	��ڲ�����	devid�������豸��devid
*				auth_key�������豸��masterKey��apiKey
*
*	���ز�����	��
*
*	˵����		��onenetƽ̨�������ӣ��ɹ������oneNetInfo.netWork����״̬��־
************************************************************
*/
void OneNet_DevLink(const char* devid, const char* auth_key)
{
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , devid, auth_key);
	
	if(MqttSample_ReInit(ctx) < 0)
	{
		UsartPrintf(USART_DEBUG, "MqttSample_ReInit Failed\r\n");
		return;
	}
	
	if(MqttSample_Connect(ctx, oneNetInfo.proID, oneNetInfo.auif, oneNetInfo.devID, KEEP_ALIVE, C_SESSION) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	RTOS_TimeDly(100);
	
}

/*
************************************************************
*	�������ƣ�	OneNet_toString
*
*	�������ܣ�	����ֵתΪ�ַ���
*
*	��ڲ�����	dataStream��������
*				buf��ת����Ļ���
*				pos���������е��ĸ�����
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OneNet_toString(DATA_STREAM *dataStream, char *buf, unsigned short pos)
{
	
	memset(buf, 0, 48);

	switch((unsigned char)dataStream[pos].dataType)
	{
		case TYPE_BOOL:
			snprintf(buf, 48, "%d", *(_Bool *)dataStream[pos].data);
		break;
		
		case TYPE_CHAR:
			snprintf(buf, 48, "%d", *(char *)dataStream[pos].data);
		break;
		
		case TYPE_UCHAR:
			snprintf(buf, 48, "%d", *(unsigned char *)dataStream[pos].data);
		break;
		
		case TYPE_SHORT:
			snprintf(buf, 48, "%d", *(short *)dataStream[pos].data);
		break;
		
		case TYPE_USHORT:
			snprintf(buf, 48, "%d", *(unsigned short *)dataStream[pos].data);
		break;
		
		case TYPE_INT:
			snprintf(buf, 48, "%d", *(int *)dataStream[pos].data);
		break;
		
		case TYPE_UINT:
			snprintf(buf, 48, "%d", *(unsigned int *)dataStream[pos].data);
		break;
		
		case TYPE_LONG:
			snprintf(buf, 48, "%ld", *(long *)dataStream[pos].data);
		break;
		
		case TYPE_ULONG:
			snprintf(buf, 48, "%ld", *(unsigned long *)dataStream[pos].data);
		break;
			
		case TYPE_FLOAT:
			snprintf(buf, 48, "%f", *(float *)dataStream[pos].data);
		break;
		
		case TYPE_DOUBLE:
			snprintf(buf, 48, "%f", *(double *)dataStream[pos].data);
		break;
		
		case TYPE_GPS:
			snprintf(buf, 48, "{\"lon\":%s,\"lat\":%s}", (char *)dataStream[pos].data, (char *)dataStream[pos].data + 16);
		break;
	}

}

/*
************************************************************
*	�������ƣ�	OneNet_Load_DataStream
*
*	�������ܣ�	��������װ
*
*	��ڲ�����	type���������ݵĸ�ʽ
*				send_buf�����ͻ���ָ��
*				len�������������ĸ���
*
*	���ز�����	��
*
*	˵����		��װ��������ʽ
************************************************************
*/
void OneNet_Load_DataStream(char *send_buf, unsigned char len)
{
	
	unsigned char count = 0;
	char stream_buf[96];
	char data_buf[48];
	char *ptr = send_buf;

	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeFullJson\r\n");
		
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeFullJson\r\n");
		
	strncpy(send_buf, "{\"datastreams\":[", strlen("{\"datastreams\":["));
	for(; count < len; count++)
	{
		if(dataStream[count].flag) //���ʹ�ܷ��ͱ�־λ
		{
			memset(stream_buf, 0, sizeof(stream_buf));

			OneNet_toString(dataStream, data_buf, count);
			snprintf(stream_buf, sizeof(stream_buf), "{\"id\":\"%s\",\"datapoints\":[{\"value\":%s}]},", dataStream[count].name, data_buf);
			
			strncat(send_buf, stream_buf, strlen(stream_buf));
		}
	}
	
	while(*ptr != '\0')					//�ҵ�������
		ptr++;
	*(--ptr) = '\0';					//������','�滻Ϊ������
	
	strncat(send_buf, "]}", 2);

}

/*
************************************************************
*	�������ƣ�	OneNet_SendData
*
*	�������ܣ�	�ϴ����ݵ�ƽ̨
*
*	��ڲ�����	len�������������ĸ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_SendData(unsigned char len)
{
	
	char send_buf[SEND_BUF_SIZE];

	if(!oneNetInfo.netWork)															//�������δ����
		return 1;
	
	memset(send_buf, 0, SEND_BUF_SIZE);
	
	OneNet_Load_DataStream(send_buf, len);											//����������
	
	RTOS_ENTER_CRITICAL();
	if(MqttSample_Savedata(ctx, send_buf) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	else
		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
	RTOS_EXIT_CRITICAL();
	
	MqttBuffer_Reset(ctx->mqttbuf);
	
	NET_DEVICE_ClrData();

	faultTypeReport = FAULT_NONE;													//����֮��������
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_Publish
*
*	�������ܣ�	������Ϣ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_Publish(void)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
	
	if(MqttSample_Publish(ctx, "OneNet MQTT Publish Test") == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_Subscribe
*
*	�������ܣ�	������Ϣ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_Subscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Subscribe\r\n");

	if(MqttSample_Subscribe(ctx, topics, 2) == 0)									//��һ�ζ��Ķ��
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_UnSubscribe
*
*	�������ܣ�	ȡ������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_UnSubscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_UnSubscribe\r\n");

	if(MqttSample_Unsubscribe(ctx, topics, 2) == 0)									//��һ�ζ��Ķ��
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_HeartBeat
*
*	�������ܣ�	�������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_HeartBeat(void)
{
	
	unsigned char sCount = 5;
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork)															//�������δ����
		return 1;
	
	oneNetInfo.heartBeat = 0;
	
	while(--sCount)																	//ѭ��������
	{
		if(Mqtt_PackPingReqPkt(ctx->mqttbuf) == MQTTERR_NOERROR)
			Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
		else
			UsartPrintf(USART_DEBUG, "WARN:		Mqtt_PackPingReqPkt Failed\r\n");
		MqttBuffer_Reset(ctx->mqttbuf);
		
		//Mqtt_RecvPkt(ctx->mqttctx);
		RTOS_TimeDly(100);
		
		if(oneNetInfo.heartBeat == 1)
		{
			errType = CHECK_NO_ERR;													//����޴���
			MqttBuffer_Reset(ctx->mqttbuf);
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
			
			errType = NET_DEVICE_Check();
		}
	}
	
	if(sCount == 0)																//����
	{
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut\r\n");
		
		errType = NET_DEVICE_Check();											//�����豸״̬���
	}
	
	if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
		faultTypeReport = faultType = FAULT_EDP;								//���ΪЭ�����
	else if(errType == CHECK_NO_DEVICE)
		faultTypeReport = faultType = FAULT_NODEVICE;							//���Ϊ�豸����
	else
		faultTypeReport = faultType = FAULT_NONE;								//�޴���
	
	NET_DEVICE_ClrData();														//�������
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_App
*
*	�������ܣ�	ƽ̨�·��������������
*
*	��ڲ�����	cmd��ƽ̨�·�������
*
*	���ز�����	��
*
*	˵����		��ȡ�������Ӧ����
************************************************************
*/
void OneNet_App(char *cmd)
{

	char *dataPtr;
	char numBuf[10];
	int num = 0;
	
	dataPtr = strstr((const char *)cmd, "}");			//����'}'

	if(dataPtr != NULL)									//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
		if(strstr((char *)cmd, "redled"))				//����"redled"
		{
			if(num == 1)								//�����������Ϊ1������
			{
				Led4_Set(LED_ON);
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
				Led4_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;					//������ݷ���
		}
														//��ͬ
		else if(strstr((char *)cmd, "greenled"))
		{
			if(num == 1)
			{
				Led5_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led5_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "yellowled"))
		{
			if(num == 1)
			{
				Led6_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led6_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "blueled"))
		{
			if(num == 1)
			{
				Led7_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led7_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "beep"))
		{
			if(num == 1)
			{
				Beep_Set(BEEP_ON);
			}
			else if(num == 0)
			{
				Beep_Set(BEEP_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
	}
	
	NET_DEVICE_ClrData();								//��ջ���

}
