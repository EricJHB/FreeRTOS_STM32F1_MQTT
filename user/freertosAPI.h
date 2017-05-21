#ifndef _FREERTOSAPI_H_
#define _FREERTOSAPI_H_


//OSÍ·ÎÄ¼þ
#include "FreeRTOS.h"
#include "task.h"




size_t FreeRTOS_GetFreeHeapSize(void);

size_t FreeRTOS_GetMinimumEverFreeHeapSize(void);

UBaseType_t FreeRTOS_GetNumberOfTasks(void);

void FreeRTOS_GetTaskInfo(const char *pcNameToQuery);

void FreeRTOS_GetAllTasksInfo(void);


#endif
