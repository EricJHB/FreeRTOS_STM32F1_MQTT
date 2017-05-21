/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-12-15
	*
	*	版本： 		V1.0
	*
	*	说明： 		与onenet平台的数据交互，协议层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"
#include "fault.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "iwdg.h"
#include "hwtimer.h"
#include "selfcheck.h"

//图片数据文件
#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



struct MqttSampleContext ctx[1];
char *topics[] = {"kylinV22", "pcTopic"};


ONETNET_INFO oneNetInfo = {"", "", "", "", "183.230.40.33", "6002", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];


/*
************************************************************
*	函数名称：	OneNet_DevLink
*
*	函数功能：	与onenet创建连接
*
*	入口参数：	devid：创建设备的devid
*				auth_key：创建设备的masterKey或apiKey
*
*	返回参数：	无
*
*	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
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
*	函数名称：	OneNet_toString
*
*	函数功能：	将数值转为字符串
*
*	入口参数：	dataStream：数据流
*				buf：转换后的缓存
*				pos：数据流中的哪个数据
*
*	返回参数：	无
*
*	说明：		
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
*	函数名称：	OneNet_Load_DataStream
*
*	函数功能：	数据流封装
*
*	入口参数：	type：发送数据的格式
*				send_buf：发送缓存指针
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		封装数据流格式
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
		if(dataStream[count].flag) //如果使能发送标志位
		{
			memset(stream_buf, 0, sizeof(stream_buf));

			OneNet_toString(dataStream, data_buf, count);
			snprintf(stream_buf, sizeof(stream_buf), "{\"id\":\"%s\",\"datapoints\":[{\"value\":%s}]},", dataStream[count].name, data_buf);
			
			strncat(send_buf, stream_buf, strlen(stream_buf));
		}
	}
	
	while(*ptr != '\0')					//找到结束符
		ptr++;
	*(--ptr) = '\0';					//将最后的','替换为结束符
	
	strncat(send_buf, "]}", 2);

}

/*
************************************************************
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_SendData(unsigned char len)
{
	
	char send_buf[SEND_BUF_SIZE];

	if(!oneNetInfo.netWork)															//如果网络未连接
		return 1;
	
	memset(send_buf, 0, SEND_BUF_SIZE);
	
	OneNet_Load_DataStream(send_buf, len);											//加载数据流
	
	RTOS_ENTER_CRITICAL();
	if(MqttSample_Savedata(ctx, send_buf) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	else
		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
	RTOS_EXIT_CRITICAL();
	
	MqttBuffer_Reset(ctx->mqttbuf);
	
	NET_DEVICE_ClrData();

	faultTypeReport = FAULT_NONE;													//发送之后清除标记
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_Publish
*
*	函数功能：	发布消息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_Publish(void)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
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
*	函数名称：	OneNet_Subscribe
*
*	函数功能：	订阅消息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_Subscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Subscribe\r\n");

	if(MqttSample_Subscribe(ctx, topics, 2) == 0)									//可一次订阅多个
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_UnSubscribe
*
*	函数功能：	取消订阅
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_UnSubscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_UnSubscribe\r\n");

	if(MqttSample_Unsubscribe(ctx, topics, 2) == 0)									//可一次订阅多个
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_HeartBeat
*
*	函数功能：	心跳检测
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_HeartBeat(void)
{
	
	unsigned char sCount = 5;
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork)															//如果网络未连接
		return 1;
	
	oneNetInfo.heartBeat = 0;
	
	while(--sCount)																	//循环检测计数
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
			errType = CHECK_NO_ERR;													//标记无错误
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
	
	if(sCount == 0)																//超出
	{
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut\r\n");
		
		errType = NET_DEVICE_Check();											//网络设备状态检查
	}
	
	if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
		faultTypeReport = faultType = FAULT_EDP;								//标记为协议错误
	else if(errType == CHECK_NO_DEVICE)
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
	else
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	
	NET_DEVICE_ClrData();														//情况缓存
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_App
*
*	函数功能：	平台下发命令解析、处理
*
*	入口参数：	cmd：平台下发的命令
*
*	返回参数：	无
*
*	说明：		提取出命令，响应处理
************************************************************
*/
void OneNet_App(char *cmd)
{

	char *dataPtr;
	char numBuf[10];
	int num = 0;
	
	dataPtr = strstr((const char *)cmd, "}");			//搜索'}'

	if(dataPtr != NULL)									//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		if(strstr((char *)cmd, "redled"))				//搜索"redled"
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				Led4_Set(LED_ON);
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				Led4_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;					//标记数据反馈
		}
														//下同
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
	
	NET_DEVICE_ClrData();								//清空缓存

}
