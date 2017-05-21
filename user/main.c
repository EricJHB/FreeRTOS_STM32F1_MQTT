/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	main.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��ɵ�Ƭ����ʼ�������IC��ʼ��������Ĵ���������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//OS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "freertosAPI.h"

//Э��
#include "onenet.h"
#include "fault.h"
#include "mqtt_app.h"

//�����豸
#include "net_device.h"
#include "net_io.h"

//Ӳ������
#include "led.h"
#include "delay.h"
#include "key.h"
#include "lcd1602.h"
#include "usart.h"
#include "hwtimer.h"
#include "i2c.h"
#include "gy30.h"
#include "adxl345.h"
#include "sht20.h"
#include "iwdg.h"
#include "stmflash.h"
#include "at24c02.h"
#include "selfcheck.h"
#include "beep.h"
#include "oled.h"
#include "info.h"
#include "tcrt5000.h"

//C��
#include <string.h>






//���Ź�����
#define IWDG_TASK_PRIO		11
#define IWDG_STK_SIZE		64
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

//��������
#define USART_TASK_PRIO		10
#define USART_STK_SIZE		512
TaskHandle_t USART_Task_Handler;
void USART_Task(void *pvParameters);

//��������
#define HEART_TASK_PRIO		9
#define HEART_STK_SIZE		512
TaskHandle_t HEART_Task_Handler;
void HEART_Task(void *pvParameters);

//���ϴ�������
#define FAULT_TASK_PRIO		8 //
#define FAULT_STK_SIZE		256
TaskHandle_t FAULT_Task_Handler;
void FAULT_Task(void *pvParameters);

//����������
#define SENSOR_TASK_PRIO	7
#define SENSOR_STK_SIZE		256
TaskHandle_t SENSOR_Task_Handler;
void SENSOR_Task(void *pvParameters);

//���ݷ�������
#define SEND_TASK_PRIO		6
#define SEND_STK_SIZE		1024
TaskHandle_t SEND_Task_Handler;
void SEND_Task(void *pvParameters);

//��������
#define KEY_TASK_PRIO		5
#define KEY_STK_SIZE		128
TaskHandle_t KEY_Task_Handler;
void KEY_Task(void *pvParameters);

//�����ʼ������
#define NET_TASK_PRIO		4 //
#define NET_STK_SIZE		1024
TaskHandle_t NET_Task_Handler;
void NET_Task(void *pvParameters);

//���ݷ�������
#define DATA_TASK_PRIO		3 //
#define DATA_STK_SIZE		1024
TaskHandle_t DATA_Task_Handler;
void DATA_Task(void *pvParameters);

//��Ϣ��������
#define ALTER_TASK_PRIO		2 //
#define ALTER_STK_SIZE		128
TaskHandle_t ALTER_Task_Handler;
void ALTER_Task(void *pvParameters);



#define NET_TIME	60			//�趨ʱ��--��λ��
unsigned short timerCount = 0;	//ʱ�����--��λ��

TimerHandle_t t1_Thdl;

//������
DATA_STREAM dataStream[] = {
								{"Red_Led", &ledStatus.Led4Sta, TYPE_BOOL, 1},
								{"Green_Led", &ledStatus.Led5Sta, TYPE_BOOL, 1},
								{"Yellow_Led", &ledStatus.Led6Sta, TYPE_BOOL, 1},
								{"Blue_Led", &ledStatus.Led7Sta, TYPE_BOOL, 1},
								{"beep", &beepInfo.Beep_Status, TYPE_BOOL, 1},
								{"temperature", &sht20Info.tempreture, TYPE_FLOAT, 1},
								{"humidity", &sht20Info.humidity, TYPE_FLOAT, 1},
								{"Xg", &adxlInfo.incidence_Xf, TYPE_FLOAT, 1},
								{"Yg", &adxlInfo.incidence_Yf, TYPE_FLOAT, 1},
								{"Zg", &adxlInfo.incidence_Zf, TYPE_FLOAT, 1},
								{"errType", &faultTypeReport, TYPE_UCHAR, 1},
							};
unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);


/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ����Ƭ�������Լ�����豸
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);								//�жϿ�������������

	Delay_Init();																//systick��ʼ��
	
	Led_Init();																	//LED��ʼ��
	
	Key_Init();																	//������ʼ��
	
	Beep_Init();																//��������ʼ��
	
	TCRT5000_Init();															//TCRT5000��ʼ��
	
	IIC_Init();																	//���IIC���߳�ʼ��
	
	Lcd1602_Init();																//LCD1602��ʼ��
	
	Usart1_Init(115200); 														//��ʼ������   115200bps
	
	Lcd1602_DisString(0x80, "Check Power On");									//��ʾ���п������
	Check_PowerOn(); 															//�ϵ��Լ�
	Lcd1602_Clear(0x80);														//���һ����ʾ
	
	if(checkInfo.ADXL345_OK == DEV_OK) 											//�����⵽ADXL345���ʼ��
		ADXL345_Init();
	
	if(checkInfo.OLED_OK == DEV_OK)												//�����⵽OLED���ʼ��
	{
		OLED_Init();
	}

	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) 								//����ǿ��Ź���λ����ʾ
	{
		UsartPrintf(USART_DEBUG, "WARN:	IWDG Reboot\r\n");
		
		RCC_ClearFlag();														//������Ź���λ��־λ
		
		faultTypeReport = faultType = FAULT_REBOOT; 							//���Ϊ��������
		
		if(!Info_Check() && checkInfo.EEPROM_OK)								//���EEPROM������Ϣ
			Info_Read();
	}
	else
	{
		//�ȶ���ssid��pswd��devid��apikey
		if(!Info_Check() && checkInfo.EEPROM_OK)								//���EEPROM������Ϣ �� EEPROM����
		{
			//AT24C02_Clear(0, 255, 256);Iwdg_Feed();
			UsartPrintf(USART_DEBUG, "1.ssid_pswd in EEPROM\r\n");
			Info_Read();
		}
		else //û������
		{
			UsartPrintf(USART_DEBUG, "1.ssid_pswd in ROM\r\n");
		}
		
		UsartPrintf(USART_DEBUG, "2.DEVID: %s,     APIKEY: %s\r\n"
								, oneNetInfo.devID, oneNetInfo.apiKey);
	}
	
	//Iwdg_Init(4, 1250); 														//64��Ƶ��ÿ��625�Σ�����1250�Σ�2s
	
	UsartPrintf(USART_DEBUG, "3.Hardware init OK\r\n");							//��ʾ��ʼ�����

}

/*
************************************************************
*	�������ƣ�	OS_TimerCallBack
*
*	�������ܣ�	��ʱ�������״̬��־λ
*
*	��ڲ�����	�����ʱ�����
*
*	���ز�����	��
*
*	˵����		��ʱ�����񡣶�ʱ�������״̬�������������趨ʱ�����������ӣ������ƽ̨����
************************************************************
*/
void OS_TimerCallBack(TimerHandle_t xTimer)
{
	
	if(oneNetInfo.netWork == 0)											//�������Ͽ�
	{
		if(++timerCount >= NET_TIME) 									//�������Ͽ���ʱ
		{
			UsartPrintf(USART_DEBUG, "Tips:		Timer Check Err\r\n");
			
			checkInfo.NET_DEVICE_OK = 0;								//���豸δ����־
			
			NET_DEVICE_ReConfig(0);										//�豸��ʼ����������Ϊ��ʼ״̬
			
			oneNetInfo.netWork = 0;
		}
	}
	else
	{
		timerCount = 0;													//�������
	}

}

/*
************************************************************
*	�������ƣ�	main
*
*	�������ܣ�	��ɳ�ʼ�����񣬴���Ӧ������ִ��
*
*	��ڲ�����	��
*
*	���ز�����	0
*
*	˵����		
************************************************************
*/
int main(void)
{
	
	Hardware_Init();								//Ӳ����ʼ��
	
	//����Ӧ������
	
	xTaskCreate((TaskFunction_t)IWDG_Task, "IWDG", IWDG_STK_SIZE, NULL, IWDG_TASK_PRIO, (TaskHandle_t*)&IWDG_Task_Handler);
	
	xTaskCreate((TaskFunction_t)USART_Task, "USART", USART_STK_SIZE, NULL, USART_TASK_PRIO, (TaskHandle_t*)&USART_Task_Handler);
	
	xTaskCreate((TaskFunction_t)HEART_Task, "HEART", HEART_STK_SIZE, NULL, HEART_TASK_PRIO, (TaskHandle_t*)&HEART_Task_Handler);
	
	xTaskCreate((TaskFunction_t)FAULT_Task, "FAULT", FAULT_STK_SIZE, NULL, FAULT_TASK_PRIO, (TaskHandle_t*)&FAULT_Task_Handler);
	
	xTaskCreate((TaskFunction_t)SENSOR_Task, "SENSOR", SENSOR_STK_SIZE, NULL, SENSOR_TASK_PRIO, (TaskHandle_t*)&SENSOR_Task_Handler);
	
	xTaskCreate((TaskFunction_t)SEND_Task, "SEND", SEND_STK_SIZE, NULL, SEND_TASK_PRIO, (TaskHandle_t*)&SEND_Task_Handler);
	
	xTaskCreate((TaskFunction_t)KEY_Task, "KEY", KEY_STK_SIZE, NULL, KEY_TASK_PRIO, (TaskHandle_t*)&KEY_Task_Handler);
	
	xTaskCreate((TaskFunction_t)NET_Task, "NET", NET_STK_SIZE, NULL, NET_TASK_PRIO, (TaskHandle_t*)&NET_Task_Handler);
	
	xTaskCreate((TaskFunction_t)DATA_Task, "DATA", DATA_STK_SIZE, NULL, DATA_TASK_PRIO, (TaskHandle_t*)&DATA_Task_Handler);
	
	xTaskCreate((TaskFunction_t)ALTER_Task, "ALTER", ALTER_STK_SIZE, NULL, ALTER_TASK_PRIO, (TaskHandle_t*)&ALTER_Task_Handler);
	
	t1_Thdl = xTimerCreate("Timer1", 200, pdTRUE, (void *)1, (TimerCallbackFunction_t)OS_TimerCallBack);
	xTimerStart(t1_Thdl, 1);
	
	Lcd1602_Clear(0xff);							//����
	
	UsartPrintf(USART_DEBUG, "4.OSStart\r\n");		//��ʾ����ʼִ��
	
	vTaskStartScheduler();							//��ʼ�������
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	IWDG_Task
*
*	�������ܣ�	������Ź�
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���Ź�����
************************************************************
*/
void IWDG_Task(void *pdata)
{

	while(1)
	{
	
		Iwdg_Feed(); 		//ι��
		
		RTOS_TimeDly(50); 	//��������250ms
	
	}

}

/*
************************************************************
*	�������ƣ�	KEY_Task
*
*	�������ܣ�	ɨ�谴���Ƿ��£�����а��£����ж�Ӧ�Ĵ���
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		��������
************************************************************
*/
void KEY_Task(void *pdata)
{

	while(1)
	{
		
		switch(Keyboard())								//ɨ�谴��״̬
		{
			case KEY0DOWN:								//�����key0�����¼�
				
				if(ledStatus.Led4Sta == LED_OFF)
					Led4_Set(LED_ON);
				else
					Led4_Set(LED_OFF);
				
				oneNetInfo.sendData = 1;				//������ݷ���
			
			break;
			
			case KEY2DOWN:
				
				if(ledStatus.Led5Sta == LED_OFF)
					Led5_Set(LED_ON);
				else
					Led5_Set(LED_OFF);
			
				oneNetInfo.sendData = 1;
				
			break;
			
			case KEY3DOWN:
				
				if(ledStatus.Led6Sta == LED_OFF)
					Led6_Set(LED_ON);
				else
					Led6_Set(LED_OFF);
			
				oneNetInfo.sendData = 1;
				
			break;
			
			case KEY1DOWN:
				
				if(ledStatus.Led7Sta == LED_OFF)
					Led7_Set(LED_ON);
				else
					Led7_Set(LED_OFF);
				
				oneNetInfo.sendData = 1;
				
			break;
				
			case KEY0DOWNLONG:
				
			
			break;
			
			case KEY2DOWNLONG:
				
				timerCount = NET_TIME;
				checkInfo.NET_DEVICE_OK = 0;								//���豸δ����־
				NET_DEVICE_ReConfig(0);										//�豸��ʼ����������Ϊ��ʼ״̬
				oneNetInfo.netWork = 0;
			
			break;
			
			case KEY3DOUBLE:
				
				oneNetInfo.sendData = 3;				//������Ϣ
			
			break;
			
			case KEY3DOWNLONG:
				
				oneNetInfo.sendData = 4;				//ȡ������
			
			break;
			
			case KEY1DOWNLONG:
				
				oneNetInfo.sendData = 2;				//������Ϣ
			
			break;
			
			default:
			break;
		}
	
		RTOS_TimeDly(10); 								//��������50ms
	
	}

}

/*
************************************************************
*	�������ƣ�	HEART_Task
*
*	�������ܣ�	�������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		�������񡣷����������󲢵ȴ�������Ӧ�������趨ʱ����û����Ӧ������ƽ̨����
************************************************************
*/
void HEART_Task(void *pdata)
{

	while(1)
	{
		
		OneNet_HeartBeat();
		
		RTOS_TimeDly(14200);		//�������� 1min 11s
	
	}

}

/*
************************************************************
*	�������ƣ�	SEND_Task
*
*	�������ܣ�	�ϴ�����������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ݷ�������
************************************************************
*/
void SEND_Task(void *pdata)
{

	while(1)
	{
		
		OneNet_SendData(dataStreamLen);
		
		RTOS_TimeDly(12000);				//�������� 1min
		
	}

}

/*
************************************************************
*	�������ƣ�	USART_Task
*
*	�������ܣ�	����ƽ̨�·�������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ڽ�������������ģʽ��ʱ���ȴ�ƽ̨�·����������������
************************************************************
*/
void USART_Task(void *pdata)
{
	
	if(MqttSample_Init(ctx) < 0)
	{
		UsartPrintf(USART_DEBUG, "MqttSample_Init Failed\r\n");
		return;
	}

	while(1)
	{

		if(NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)
		{
			if(Mqtt_RecvPkt(ctx->mqttctx) == MQTTERR_NOERROR)
				NET_DEVICE_ClrData();
		}
		
		RTOS_TimeDly(2);														//��������10ms
	
	}

}

/*
************************************************************
*	�������ƣ�	SENSOR_Task
*
*	�������ܣ�	���������ݲɼ�����ʾ
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���������ݲɼ����񡣽�����Ӵ����������ݲɼ�����ȡ����ʾ
************************************************************
*/
void SENSOR_Task(void *pdata)
{
	
	OLED_ClearScreen();											//����
	
	//������ʾ
	OLED_DisChar16x16(0, 0, san);								//��ʾ������
	OLED_DisChar16x16(0, 16, zhou);								//��ʾ���ᡱ
	OLED_DisString6x8(1, 32, ":");								//��ʾ������
	
	OLED_DisChar16x16(2, 0, wen);								//��ʾ���¡�
	OLED_DisChar16x16(2, 16, shi);								//��ʾ��ʪ��
	OLED_DisChar16x16(2, 32, du);								//��ʾ���ȡ�
	OLED_DisString6x8(3, 48, ":");								//��ʾ������
	
	OLED_DisChar16x16(6, 0, zhuang);							//��ʾ��״��
	OLED_DisChar16x16(6, 16, tai);								//��ʾ��̬��
	OLED_DisString6x8(7, 32, ":");								//��ʾ������

	while(1)
	{
		
		if(checkInfo.ADXL345_OK == DEV_OK) 						//ֻ���豸����ʱ���Ż��ȡֵ����ʾ
		{
			ADXL345_GetValue();									//�ɼ�����������
			Lcd1602_DisString(0x80, "X%0.1f,Y%0.1f,Z%0.1f", adxlInfo.incidence_Xf, adxlInfo.incidence_Yf, adxlInfo.incidence_Zf);
			OLED_DisString6x8(1, 40, "X%0.1f,Y%0.1f,Z%0.1f", adxlInfo.incidence_Xf, adxlInfo.incidence_Yf, adxlInfo.incidence_Zf);
		}
		if(checkInfo.SHT20_OK == DEV_OK) 						//ֻ���豸����ʱ���Ż��ȡֵ����ʾ
		{
			SHT20_GetValue();									//�ɼ�����������
			Lcd1602_DisString(0xC0, "%0.1fC,%0.1f%%", sht20Info.tempreture, sht20Info.humidity);
			OLED_DisString6x8(3, 56, "%0.1fC,%0.1f%%", sht20Info.tempreture, sht20Info.humidity);
		}
		
		if(t5000Info.status == TCRT5000_ON)
		{
			TCRT5000_GetValue(5);
			if(t5000Info.voltag < 3500)
				Beep_Set(BEEP_ON);
			else
				Beep_Set(BEEP_OFF);
		}
		
		RTOS_TimeDly(100); 										//��������500ms
	
	}

}

/*
************************************************************
*	�������ƣ�	DATA_Task
*
*	�������ܣ�	ƽ̨�·���������ݷ���
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ݷ�����������ƽ̨�·�ָ���ķ���������͸��ģʽ��ʱ���յ�֮���������أ���͸��ģʽ��Ϊ��Ҫ����'>'���ţ�����ʹ������ķ�ʽ��������
************************************************************
*/
void DATA_Task(void *pdata)
{

	while(1)
	{
	
		switch(oneNetInfo.sendData)
		{
			case 1:
				
				oneNetInfo.sendData = OneNet_SendData(dataStreamLen);
			
			break;
			
			case 2:
				
				oneNetInfo.sendData = OneNet_Publish();
			
			break;
			
			case 3:
				
				oneNetInfo.sendData = OneNet_Subscribe();
			
			break;
			
			case 4:
				
				oneNetInfo.sendData = OneNet_UnSubscribe();
			
			break;
			
			default:
				break;
		}
		
		RTOS_TimeDly(10);					//��������50ms
	
	}

}

/*
************************************************************
*	�������ƣ�	FAULT_Task
*
*	�������ܣ�	����״̬������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ϴ������񡣵�������������豸����ʱ�����Ƕ�Ӧ��־λ��Ȼ���н��д���
************************************************************
*/
void FAULT_Task(void *pdata)
{

	while(1)
	{
		
		if(faultType != FAULT_NONE)									//��������־������
		{
			UsartPrintf(USART_DEBUG, "WARN:	Fault Process\r\n");
			Fault_Process();										//�����������
		}
		
		RTOS_TimeDly(10);											//��������50ms
	
	}

}

/*
************************************************************
*	�������ƣ�	NET_Task
*
*	�������ܣ�	�������ӡ�ƽ̨����
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���������������񡣻������������߼����������״̬������д�����״̬��Ȼ���������������
************************************************************
*/
void NET_Task(void *pdata)
{
	
	NET_DEVICE_IO_Init();													//�����豸IO��ʼ��
	NET_DEVICE_Reset();														//�����豸��λ
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);								//����Ϊ�����շ�ģʽ(����ESP8266Ҫ����AT�ķ��ػ���ƽ̨�·����ݵķ���)

	while(1)
	{
		
		if(!oneNetInfo.netWork && (checkInfo.NET_DEVICE_OK == DEV_OK))		//��û������ �� ����ģ���⵽ʱ
		{
			OLED_DisChar16x16(6, 48, lian);
			OLED_DisChar16x16(6, 64, jie);
			OLED_DisChar16x16(6, 80, zhong);
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//����Ϊ�����շ�ģʽ
			
			if(!NET_DEVICE_Init(oneNetInfo.ip, oneNetInfo.port))			//��ʼ�������豸������������
			{
				NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);					//����Ϊ�����շ�ģʽ
				
				OneNet_DevLink(oneNetInfo.devID, oneNetInfo.apiKey);		//����ƽ̨
				
				if(oneNetInfo.netWork)										//�������ɹ�
				{
					UsartPrintf(USART_DEBUG, "Tips:	NetWork OK\r\n");
					
					Beep_Set(BEEP_ON);										//�̽���ʾ�ɹ�
					RTOS_TimeDly(40);
					Beep_Set(BEEP_OFF);
					
					oneNetInfo.errCount = 0;
							
					OLED_DisChar16x16(6, 48, yi);
					OLED_DisChar16x16(6, 64, lian);
					OLED_DisChar16x16(6, 80, jie);
					
					OneNet_Subscribe();
				}
				else
				{
					UsartPrintf(USART_DEBUG, "Tips:	NetWork Fail\r\n");
					
					if(++oneNetInfo.errCount >= 5)								//��������趨�����󣬻�δ����ƽ̨
					{
						Beep_Set(BEEP_ON);										//������ʾʧ��
						RTOS_TimeDly(100);
						Beep_Set(BEEP_OFF);
						
						oneNetInfo.netWork = 0;
						faultType = faultTypeReport = FAULT_NODEVICE;			//���ΪӲ������
						
						OLED_DisChar16x16(6, 48, wei);
						OLED_DisChar16x16(6, 64, lian);
						OLED_DisChar16x16(6, 80, jie);
					}
				}
			}
		}
		
		if(checkInfo.NET_DEVICE_OK == DEV_ERR) 								//�������豸δ�����
		{
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//����Ϊ�����շ�ģʽ
			
			if(timerCount >= NET_TIME) 										//����������ӳ�ʱ
			{
				NET_DEVICE_Reset();											//��λ�����豸
				timerCount = 0;												//�������ӳ�ʱ����
				faultType = FAULT_NONE;										//��������־
			}
			
			if(!NET_DEVICE_Exist())											//�����豸���
			{
				UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
				checkInfo.NET_DEVICE_OK = DEV_OK;							//��⵽�����豸�����
				NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);					//����Ϊ�����շ�ģʽ
			}
			else
				UsartPrintf(USART_DEBUG, "NET Device :Error\r\n");
		}
		
		RTOS_TimeDly(5);													//��������25ms
	
	}

}

/*
************************************************************
*	�������ƣ�	ALTER_Task
*
*	�������ܣ�	ͨ�����ڸ���SSID��PSWD��DEVID��APIKEY
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ĺ�ᱣ�浽EEPROM��
************************************************************
*/
void ALTER_Task(void *pdata)
{
    
    unsigned char usart1Count = 0;

    while(1)
    {
    
        memset(alterInfo.alterBuf, 0, sizeof(alterInfo.alterBuf));
		alterInfo.alterCount = 0;usart1Count = 0;
        while((strlen(alterInfo.alterBuf) != usart1Count) || (usart1Count == 0))	//�ȴ��������
        {
            usart1Count = strlen(alterInfo.alterBuf);								//���㳤��
            RTOS_TimeDly(20);														//ÿ100ms���һ��
        }
				/*eric*/
				NET_IO_Send(alterInfo.alterBuf, strlen(alterInfo.alterBuf));	//д��������豸
				
        UsartPrintf(USART_DEBUG, "\r\nusart1Buf Len: %d, usart1Count = %d\r\n",
									strlen(alterInfo.alterBuf), usart1Count);
        
		if(checkInfo.EEPROM_OK == DEV_OK)											//���EEPROM����
		{
			if(Info_Alter(alterInfo.alterBuf))										//������Ϣ
			{
				Info_Read();
				
				if(oneNetInfo.netWork)
				{
					NET_DEVICE_ReLink(oneNetInfo.ip, oneNetInfo.port);				//����ƽ̨
					oneNetInfo.netWork = 0;											//����ƽ̨
				}
			}
		}
    
    }

}
