#ifndef _DELAY_H_
#define _DELAY_H_


#include "FreeRTOS.h"
#include "task.h"



#define RTOS_ENTER_CRITICAL()						taskENTER_CRITICAL()
#define RTOS_EXIT_CRITICAL()						taskEXIT_CRITICAL()

#define RTOS_EnterInt()								;
#define RTOS_ExitInt()								;

#define RTOS_TimeDly(ticks)							vTaskDelay(ticks)




void Delay_Init(void);

void DelayUs(unsigned short us);

void DelayXms(unsigned short ms);

void DelayMs(unsigned short ms);

#endif
