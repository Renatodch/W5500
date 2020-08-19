/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BYTES_H
#define __BYTES_H

#ifdef __cplusplus
 extern "C" 
{
#endif

#include "main.h"

uint8_t * Uint16ToBytes(uint8_t * s, uint16_t i);
uint16_t Bytes_ToUint16(uint8_t *bytes);
uint32_t Bytes_ToUint32(uint8_t *bytes);		
void Bytes_Swap_EachTwoBytes(char *bytes, uint32_t length);
void Bytes_Print_ToHex(uint8_t *pBytes, uint32_t len);
void Uint32_ToBytes(uint32_t value, uint8_t *srcBuf);
uint32_t Uint32_Swap( uint32_t val );
char UInt32_IsBetweenWindow(uint32_t newVal, uint32_t min, uint32_t max);

	
#ifdef __cplusplus
}
#endif

#endif 
