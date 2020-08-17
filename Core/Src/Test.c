/*
 * Net.c
 *
 *  Created on: 14 ago. 2020
 *      Author: Renato
 */


/* Includes ------------------------------------------------------------------*/


#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Configuration Network Information of W5500
uint8 Enable_DHCP = OFF;
uint8 MAC[6] = {0x00, 0x08, 0xDC, 0x47, 0x47, 0x54};//MAC Address


uint8 IpDevice[4] = {192,168,0,6};//IpDevice Address
uint8 GateWay[4] = {192,168,0,1};//Gateway Address
uint8 SubNet[4] = {255,255,255,0};//SubnetMask Address

uint8 IpTrace[4];//IpTrace Address
uint16 PortTrace;

uint16 any_port = 1000;

CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def;

uint16 PortServer = 8888;
uint8 IpServer[4] = {192,168,0,13};

//TX MEM SIZE- SOCKET 0-7:4KB
//RX MEM SIZE- SOCKET 0-7:4KB
uint8 txsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};//{2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};//{2,2,2,2,2,2,2,2};
/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Net_Init
* Description    : Initilaiza the net.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Net_Init(void)
{
    uint8 i;

    W5500_Init();  //rdch

    // MAC ADDRESS
    for (i = 0 ; i < 6; i++) Config_Msg.Mac[i] = MAC[i];
    // Local IpDevice ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.IpDevice[i] = IpDevice[i];
    // GateWay ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.Gw[i] = GateWay[i];
    // Subnet Mask ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.Sub[i] = SubNet[i];

    setSHAR(Config_Msg.Mac);
    setSUBR(Config_Msg.Sub);
    setGAR(Config_Msg.Gw);
    setSIPR(Config_Msg.IpDevice);

    // Set DHCP
    Config_Msg.DHCP = Enable_DHCP;

    //Destination IpDevice address for TCP Client
    Chconfig_Type_Def.destip[0] = IpServer[0]; Chconfig_Type_Def.destip[1] = IpServer[1];
    Chconfig_Type_Def.destip[2] = IpServer[2]; Chconfig_Type_Def.destip[3] = IpServer[3];
    Chconfig_Type_Def.port = PortServer;

    //Set PTR and RCR register
    setRTR(2000);
    setRCR(5);

    //Init. TX & RX Memory size
    sysinit(txsize, rxsize);

//    T("\r\n----------------------------------------- \r\n");
//    T("W5500E01-M3                       \r\n");
//    T("Network Configuration Information \r\n");
//    T("----------------------------------------- ");

//    getSHAR(tmp_array);
//    T("\r\nMAC : %.2X.%.2X.%.2X.%.2X.%.2X.%.2X", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3],tmp_array[4],tmp_array[5]);

//    getSIPR (tmp_array);
//    T("\r\nIP : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

//    getSUBR(tmp_array);
//    T("\r\nSN : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

//    getGAR(tmp_array);
//    T("\r\nGW : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);
}


/*******************************************************************************
* Function Name  : Net_IpStrToArray
* Description    : Conviert string ip to byte array
* Input          : ipStr, the string ip and the array.
* Output         : None.
* Return         : None.
*******************************************************************************/
/*
void Net_IpStrToArray(char *ipStr, uint8_t *array)
{
	char *token;
	char delim[2];
	int i = 0;

	strcpy(delim, ".");

   token = strtok(ipStr, delim);

   while( token != NULL )
   {
			array[i] = atoi(token);
      token = strtok(NULL, delim);
			i++;
   }
}
*/

/*******************************************************************************
* Function Name  : Net_ArrayToIpStr
* Description    : Conviert the byte array to string ip
* Input          : array and the string ip.
* Output         : None.
* Return         : None.
*******************************************************************************/
/*
void Net_ArrayToIpStr(char *ipStr, uint8_t *array)
{
	strcpy(ipStr, "");
	sprintf(ipStr, "%d.%d.%d.%d", array[0], array[1], array[2], array[3]);
}
*/


/********************************** Web ************************************************/






/**************************************************************************************/


// NO HAL REQUIRED
void debug_uart(char * format,...){
	char msg[64] = {0};
	char *p = msg;
	va_list args;
	va_start(args,format);

	vsnprintf(msg, sizeof(msg), format, args);

	while(*p){
		USART1->DR = (uint16_t)*p++;
	}
	va_end(args);
}
