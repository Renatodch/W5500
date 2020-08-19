/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/	
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* copies uint16_t to the network buffer with network byte order. */
uint8_t * Uint16ToBytes(uint8_t * s, uint16_t i)
{
	*s++ = i >> 8;
	*s++ = i;
	return s;
}

/*******************************************************************************
* Function Name  : Bytes_To_Uint32
* Description    : Swap the bytes (Two by two) in the buffer.
* Input          : Pointer to buffer and the length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint16_t Bytes_ToUint16(uint8_t *bytes)
{
	uint16_t i;
	i = *bytes++ << 8;
	i = i + *bytes;
	return i;
}


/*******************************************************************************
* Function Name  : Bytes_To_Uint32
* Description    : Swap the bytes (Two by two) in the buffer.
* Input          : Pointer to buffer and the length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t Bytes_ToUint32(uint8_t *bytes)
{
			uint32_t uint  = bytes[3] | ( (uint32_t)bytes[2] << 8 ) | ( (uint32_t)bytes[1] << 16 ) | ( (uint32_t)bytes[0] << 24 );
			return uint;
}


/*******************************************************************************
* Function Name  : Bytes_Swap_EachTwoBytes
* Description    : Swap the bytes (Two by two) in the buffer.
* Input          : Pointer to buffer and the length.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bytes_Swap_EachTwoBytes(char *bytes, uint32_t length)
{
	char byte;
	int i;

	for (i = 0; i < (length/2); i++)
	{
		byte = bytes[i * 2];
		bytes[i * 2] = bytes[ (i * 2) + 1 ];
		bytes[ (i * 2) + 1 ] = byte;
	}
}

/*******************************************************************************
* Function Name  : Bytes_Print_ToHex
* Description    : Print bytes to hex.
* Input          : The bytes and length.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bytes_Print_ToHex(uint8_t *pBytes, uint32_t len)
{
	int i;
	char buf[1024];
	char s[12];

		strcpy(buf, "");

		for(i=0; i<len; i++)
		{
				strcpy(s, "");
				sprintf(s, "0x%02X  ", pBytes[i]);

				strcat(buf, s);
		}

		//T("%s", buf);
}


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



