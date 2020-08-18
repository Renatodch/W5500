/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UINT32_H
#define __UINT32_H


#include "main.h"

void Uint32_ToBytes(uint32_t value, uint8_t *srcBuf);
uint32_t Uint32_Swap( uint32_t val );
char UInt32_IsBetweenWindow(uint32_t newVal, uint32_t min, uint32_t max);

	
#endif 
