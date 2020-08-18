/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/	
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Uint32_To_Buffer
* Description    : Array the integer in the buffer.
* Input          : Pointer to buffer and the integer.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Uint32_ToBytes(uint32_t value, uint8_t *srcBuf)   /*srcBuf[0] should be LSB Byte*/
{
		srcBuf[0] = (value >> 24) & 0xFF;  /*MSB*/
		srcBuf[1] = (value >> 16) & 0xFF;
		srcBuf[2] = (value >> 8) & 0xFF;
		srcBuf[3] = value & 0xFF;		   /*LSB*/
}

/*******************************************************************************
* Function Name  : Uint32_To_Buffer
* Description    : Array the integer in the buffer.
* Input          : Pointer to buffer and the integer.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t Uint32_Swap( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

/*******************************************************************************
* Function Name  : Uint32_ToBuffer
* Description    : Array the integer in the buffer.
* Input          : Pointer to buffer and the integer.
* Output         : None.
* Return         : None.
*******************************************************************************/
char UInt32_IsBetweenWindow(uint32_t newVal, uint32_t min, uint32_t max)
{
			//min = mem - min;
			if( newVal < min)
				return 0;
			
			//max = mem + max;
			if( newVal > max)
				return 0;		

			return 1;	
}

