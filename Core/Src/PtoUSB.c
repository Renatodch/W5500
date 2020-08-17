#include  "PtoUSB.h"


void Send_USB(const char * fmt, ...)
{
	char trace[256];
	uint16_t i;

	va_list args;

	va_start(args, fmt);

	vsnprintf(trace, 256, fmt, args);

	while (CDC_Transmit_FS((uint8_t*)trace, strlen(trace)) == USBD_BUSY)
	{
		if (i++ >= 100) break; 
		
	}
	va_end(args);
}
