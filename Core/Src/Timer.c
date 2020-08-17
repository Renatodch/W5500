#include "main.h"
#include "stdbool.h"

uint32_t st;

static uint32_t SystemTime_GetTime()
{
	return st;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t Timer_GetRemainingTimeInSeconds(Timer *p)
{
	uint32_t timeSpan = Timer_GetTime(p);	

	if(timeSpan == 0)
			return 0;	
	
	if(timeSpan < p->interval)
			return (p->interval - timeSpan) / 1000;

	return 0;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t Timer_GetRemainingTime(Timer *p)
{
	uint32_t timeSpan = Timer_GetTime(p);	

	if(timeSpan == 0)
			return 0;	
	
	if(timeSpan < p->interval)
			return p->interval - timeSpan;

	return 0;
}

/*******************************************************************************
* Function Name  : Timer_GetTime
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  	Timer_GetTime(Timer *p)
{
	uint32_t systemTime;
	uint32_t timeSpan;	

	if(p->enabled == false)
			return 0;
	
	if(p->enabled != true)
			return 0;		
	
	systemTime = SystemTime_GetTime();
	
	if(systemTime < p->startTime)
	{
			timeSpan = (0xFFFFFFFF - p->startTime) + systemTime;
	}
	else
	{
			timeSpan = systemTime - p->startTime;
	}
	
	return timeSpan;
}

/*******************************************************************************
* Function Name  : Timer_SetInterval
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
void  	Timer_SetInterval(Timer *p, uint32_t interval)
{
	p->interval = interval;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
void  	Timer_Start(Timer *p)
{
	p->startTime = SystemTime_GetTime();
	p->enabled = true;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
void  	Timer_Stop(Timer *p)
{
	p->enabled = false;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
bool Timer_Elapsed(Timer *p)
{
	uint32_t timeSpan = Timer_GetTime(p);	

	if(timeSpan == 0)
		return false;	

	if(timeSpan < p->interval)
		return false;

	Timer_Stop(p);

	return true;
}

/*******************************************************************************
* Function Name  : Timer
* Description    : 
* Input          : p : pointer to startTime struct
* Output         : None.
* Return         : None.
*******************************************************************************/
void  	Timer_Init(Timer *p, uint32_t interval)
{
	p->startTime = 0; 
	p->interval = interval;
	p->enabled = false;
}







