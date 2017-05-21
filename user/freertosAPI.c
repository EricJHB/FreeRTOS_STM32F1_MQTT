/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	freertosAPI.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-01-24
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		FreeRTOS�ṩ��һЩ��ѯ�����ķ�װ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#include "freertosAPI.h"

#include "usart.h"






/*
************************************************************
*	�������ƣ�	FreeRTOS_GetFreeHeapSize
*
*	�������ܣ�	��ѯOS�ڴ�ʣ��
*
*	��ڲ�����	��
*
*	���ز�����	ʣ���ֽ�
*
*	˵����		
************************************************************
*/
size_t FreeRTOS_GetFreeHeapSize(void)
{

	size_t freeSize = 0;
	
	freeSize = xPortGetFreeHeapSize();
	UsartPrintf(USART_DEBUG, "freeSize = %dByte(s)\r\n", freeSize);			//ʣ���ڴ��С
	
	return freeSize;

}

/*
************************************************************
*	�������ƣ�	FreeRTOS_GetMinimumEverFreeHeapSize
*
*	�������ܣ�	���ϵ翪ʼ����ѯOS�ڴ�ʣ����Сֵ
*
*	��ڲ�����	��
*
*	���ز�����	ʣ���ֽ�
*
*	˵����		
************************************************************
*/
size_t FreeRTOS_GetMinimumEverFreeHeapSize(void)
{

	size_t freeSize = 0;
	
	freeSize = xPortGetMinimumEverFreeHeapSize();
	UsartPrintf(USART_DEBUG, "freeMiniSize = %d\r\n", freeSize);		//��ջ��ʷ��Сʣ���С
	
	return freeSize;

}

/*
************************************************************
*	�������ƣ�	FreeRTOS_GetNumberOfTasks
*
*	�������ܣ�	��ȡ��ǰ���ڵ�������
*
*	��ڲ�����	��
*
*	���ز�����	��������
*
*	˵����		
************************************************************
*/
UBaseType_t FreeRTOS_GetNumberOfTasks(void)
{
	
	UBaseType_t taskNum = uxTaskGetNumberOfTasks();
	
	UsartPrintf(USART_DEBUG, "Task Number = %d\r\n", taskNum);		//��������(��������+timer)

	return taskNum;

}

/*
************************************************************
*	�������ƣ�	FreeRTOS_GetTaskInfo
*
*	�������ܣ�	��ѯĳ������ľ�����Ϣ
*
*	��ڲ�����	pcNameToQuery��������
*
*	���ز�����	��
*
*	˵����		����ʹ�����������������������Ҫ����С��ջ
************************************************************
*/
void FreeRTOS_GetTaskInfo(const char *pcNameToQuery)
{
	
#if(INCLUDE_xTaskGetHandle == 1)
	
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	//��ʹ��		#define	INCLUDE_xTaskGetHandle					1
	TaskHandle = xTaskGetHandle(pcNameToQuery);												//������������ȡ������
	
	//��ȡLED0_Task��������Ϣ
	vTaskGetInfo((TaskHandle_t	)TaskHandle,												//������
				 (TaskStatus_t*	)&TaskStatus,												//������Ϣ�ṹ��
				 (BaseType_t	)pdTRUE,													//����ͳ�������ջ��ʷ��Сʣ���С
				 (eTaskState	)eInvalid);													//�����Լ���ȡ��������׳̬
	
	//ͨ�����ڴ�ӡ��ָ��������й���Ϣ
	UsartPrintf(USART_DEBUG, "������:                %s\r\n", TaskStatus.pcTaskName);
	UsartPrintf(USART_DEBUG, "������:              %d\r\n", (int)TaskStatus.xTaskNumber);
	UsartPrintf(USART_DEBUG, "����׳̬:              %d\r\n", TaskStatus.eCurrentState);
	UsartPrintf(USART_DEBUG, "����ǰ���ȼ�:        %d\r\n", (int)TaskStatus.uxCurrentPriority);
	UsartPrintf(USART_DEBUG, "��������ȼ�:          %d\r\n", (int)TaskStatus.uxBasePriority);
	UsartPrintf(USART_DEBUG, "�����ջ����ַ:        %#x\r\n", (int)TaskStatus.pxStackBase);
	UsartPrintf(USART_DEBUG, "�����ջ��ʷʣ����Сֵ:%d\r\n", TaskStatus.usStackHighWaterMark);
	
#else
	
	UsartPrintf(USART_DEBUG, "FreeRTOSConfig.h  Add \"#define	INCLUDE_xTaskGetHandle  1\"\r\n");
	
#endif

}

/*
************************************************************
*	�������ƣ�	FreeRTOS_GetAllTasksInfo
*
*	�������ܣ�	��ѯ��������ļ�����Ϣ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void FreeRTOS_GetAllTasksInfo(void)
{

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
	
	char InfoBuffer[512];
	
	vTaskList(InfoBuffer);								//��ȡ�����������Ϣ
	UsartPrintf(USART_DEBUG, "%s\r\n", InfoBuffer);
	
#else
	
	UsartPrintf(USART_DEBUG, "FreeRTOSConfig.h  Add \"#define	configUSE_TRACE_FACILITY  1 and configUSE_STATS_FORMATTING_FUNCTIONS > 0\"\r\n");
	
#endif

}
