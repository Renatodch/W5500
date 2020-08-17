#include "TimerIRQ.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM2) //timer 1 segundo
	{
		HAL_GPIO_TogglePin(LED_CPU_GPIO_Port,LED_CPU_Pin);
	}
	
}
