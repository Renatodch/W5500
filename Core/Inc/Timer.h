/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
 extern "C" 
{
#endif

#include "main.h"
	
typedef struct
{
  	uint32_t 		startTime;  
  	uint32_t 		interval;  
	bool			enabled;
}Timer;

extern uint32_t st;

uint32_t Timer_GetRemainingTimeInSeconds(Timer *p);
uint32_t 		Timer_GetRemainingTime(Timer *p);
uint32_t  	Timer_GetTime(Timer *p);
void  			Timer_SetInterval(Timer *p, uint32_t interval);
void 				Timer_Start(Timer *p);
void 				Timer_Stop(Timer *p);
bool 		Timer_Elapsed(Timer *p);
void 				Timer_Init(Timer *p, uint32_t interval);

#ifdef __cplusplus
}
#endif

#endif 
