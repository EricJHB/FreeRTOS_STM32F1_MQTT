/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	freertosAPI.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-01-24
	*
	*	版本： 		V1.0
	*
	*	说明： 		FreeRTOS提供的一些查询函数的封装
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "freertosAPI.h"

#include "usart.h"






/*
************************************************************
*	函数名称：	FreeRTOS_GetFreeHeapSize
*
*	函数功能：	查询OS内存剩余
*
*	入口参数：	无
*
*	返回参数：	剩余字节
*
*	说明：		
************************************************************
*/
size_t FreeRTOS_GetFreeHeapSize(void)
{

	size_t freeSize = 0;
	
	freeSize = xPortGetFreeHeapSize();
	UsartPrintf(USART_DEBUG, "freeSize = %dByte(s)\r\n", freeSize);			//剩余内存大小
	
	return freeSize;

}

/*
************************************************************
*	函数名称：	FreeRTOS_GetMinimumEverFreeHeapSize
*
*	函数功能：	从上电开始，查询OS内存剩余最小值
*
*	入口参数：	无
*
*	返回参数：	剩余字节
*
*	说明：		
************************************************************
*/
size_t FreeRTOS_GetMinimumEverFreeHeapSize(void)
{

	size_t freeSize = 0;
	
	freeSize = xPortGetMinimumEverFreeHeapSize();
	UsartPrintf(USART_DEBUG, "freeMiniSize = %d\r\n", freeSize);		//堆栈历史最小剩余大小
	
	return freeSize;

}

/*
************************************************************
*	函数名称：	FreeRTOS_GetNumberOfTasks
*
*	函数功能：	获取当前存在的任务数
*
*	入口参数：	无
*
*	返回参数：	任务数量
*
*	说明：		
************************************************************
*/
UBaseType_t FreeRTOS_GetNumberOfTasks(void)
{
	
	UBaseType_t taskNum = uxTaskGetNumberOfTasks();
	
	UsartPrintf(USART_DEBUG, "Task Number = %d\r\n", taskNum);		//任务数量(空闲任务+timer)

	return taskNum;

}

/*
************************************************************
*	函数名称：	FreeRTOS_GetTaskInfo
*
*	函数功能：	查询某个任务的具体信息
*
*	入口参数：	pcNameToQuery：任务名
*
*	返回参数：	无
*
*	说明：		可以使用这个函数来调试任务所需要的最小堆栈
************************************************************
*/
void FreeRTOS_GetTaskInfo(const char *pcNameToQuery)
{
	
#if(INCLUDE_xTaskGetHandle == 1)
	
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	//需使能		#define	INCLUDE_xTaskGetHandle					1
	TaskHandle = xTaskGetHandle(pcNameToQuery);												//根据任务名获取任务句柄
	
	//获取LED0_Task的任务信息
	vTaskGetInfo((TaskHandle_t	)TaskHandle,												//任务句柄
				 (TaskStatus_t*	)&TaskStatus,												//任务信息结构体
				 (BaseType_t	)pdTRUE,													//允许统计任务堆栈历史最小剩余大小
				 (eTaskState	)eInvalid);													//函数自己获取任务运行壮态
	
	//通过串口打印出指定任务的有关信息
	UsartPrintf(USART_DEBUG, "任务名:                %s\r\n", TaskStatus.pcTaskName);
	UsartPrintf(USART_DEBUG, "任务编号:              %d\r\n", (int)TaskStatus.xTaskNumber);
	UsartPrintf(USART_DEBUG, "任务壮态:              %d\r\n", TaskStatus.eCurrentState);
	UsartPrintf(USART_DEBUG, "任务当前优先级:        %d\r\n", (int)TaskStatus.uxCurrentPriority);
	UsartPrintf(USART_DEBUG, "任务基优先级:          %d\r\n", (int)TaskStatus.uxBasePriority);
	UsartPrintf(USART_DEBUG, "任务堆栈基地址:        %#x\r\n", (int)TaskStatus.pxStackBase);
	UsartPrintf(USART_DEBUG, "任务堆栈历史剩余最小值:%d\r\n", TaskStatus.usStackHighWaterMark);
	
#else
	
	UsartPrintf(USART_DEBUG, "FreeRTOSConfig.h  Add \"#define	INCLUDE_xTaskGetHandle  1\"\r\n");
	
#endif

}

/*
************************************************************
*	函数名称：	FreeRTOS_GetAllTasksInfo
*
*	函数功能：	查询所有任务的简略信息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void FreeRTOS_GetAllTasksInfo(void)
{

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
	
	char InfoBuffer[512];
	
	vTaskList(InfoBuffer);								//获取所有任务的信息
	UsartPrintf(USART_DEBUG, "%s\r\n", InfoBuffer);
	
#else
	
	UsartPrintf(USART_DEBUG, "FreeRTOSConfig.h  Add \"#define	configUSE_TRACE_FACILITY  1 and configUSE_STATS_FORMATTING_FUNCTIONS > 0\"\r\n");
	
#endif

}
