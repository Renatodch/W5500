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


uint8 IpDevice[4] = {192,168,0,8};//IpDevice Address
uint8 GateWay[4] = {192,168,0,1};//Gateway Address
uint8 SubNet[4] = {255,255,255,0};//SubnetMask Address

uint8 IpTrace[4];//IpTrace Address
uint16 PortTrace;

uint16 any_port = 1001;

CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def;

uint16 PortServer = 8888;
uint8 IpServer[4] = {192,168,0,3}; //mi PC

//TX MEM SIZE- SOCKET 0-7:4KB
//RX MEM SIZE- SOCKET 0-7:4KB
uint8 txsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};//{2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};//{2,2,2,2,2,2,2,2};
/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



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


/*############################################ TCP CLIENT ########################################################*/
/*****************************************************************************************************************/
uint8 ch_status[MAX_SOCK_NUM] = { 0, }; /** 0:close, 1:ready, 2:connected */
uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications


uint8 TcpClientConn_SendStream(TcpClient *p, uint8 * data_buf, 	uint16 len)
{

	uint8_t buf[1024] = { 0 };

	Uint32_ToBytes(len, buf);

	memcpy(&buf[4], data_buf, len);

	if (getSn_SR(p->socket) == SOCK_ESTABLISHED)
	{
			send(p->socket, (uint8*) buf, len + 4, (bool)0);
			return 1;
	}

	return 0;
}

void Client_Receiver_EventHandler(uint8_t *data, int len){

	TcpClientConn_SendStream(&devtcc, data, len);
	T("Recibi: %s, len:%d",(char *)data,len);
}
void Client_onConnection_EventHandler(void){
	T("Me conecte al servidor de Renato: %d.%d.%d.%d",devtcc.dest_Ip[0],devtcc.dest_Ip[1],devtcc.dest_Ip[2],devtcc.dest_Ip[3]);
}

/*******************************************************************************
* Function Name  : TcpClientConn_Events
* Description    : Events of the tcp client.
* Input          : Pointer to tcp client.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TcpClientConn_Events(TcpClient *p, uint16_t port)
{
	uint16 RSR_len;
	uint16 received_len;
	uint8 * data_buf = TX_BUF;

	 switch (getSn_SR(p->socket))
	 {
	 	 case SOCK_ESTABLISHED:                 /* if connection is established */
			if(ch_status[p->socket]==1)
			{
				T("socket %d : Conectado",p->socket);
				if( p->onConnection_EventHandler )
							p->onConnection_EventHandler();
				ch_status[p->socket] = 2;
			}

			if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)         /* check Rx data */
			{
				 if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */

				 memset(TX_BUF,0x00,sizeof(TX_BUF));															/* the data size to read is MAX_BUF_SIZE. */
				 //received_len = recv(p->socket, data_buf, RSR_len);         /* read the received data */
				 //send(p->socket, data_buf, received_len, (bool)0);         /* sent the received data */
				 received_len = recv(p->socket, data_buf, RSR_len);         /* read the received data */
				 p->receiver_EventHandler(data_buf, received_len);         	/* sent the received data */

				 //disconnect(p->socket);
				 //close(p->socket);
			}

		break;

		case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
			T("\r\n%d : CLOSE_WAIT", p->socket);
			if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)         /* check Rx data */
			{
				 if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
																																										/* the data size to read is MAX_BUF_SIZE. */
				 recv(p->socket, data_buf, RSR_len);         /* read the received data */
			}
			disconnect(p->socket);
			ch_status[p->socket] = 0;
		break;

		case SOCK_CLOSED:/* Estado de inicio: socket cerrado*/
			if(!ch_status[p->socket])
			{
				 T("\r\n%d : Loop-Back TCP Client Started. port: %d", p->socket, port);
				 ch_status[p->socket] = 1;
			}
			if(!socket(p->socket, Sn_MR_TCP, any_port++, 0x00))    /* init o reinit el socket */
			{
				 T("\a%d : Fail to init socket.",p->socket);
				 ch_status[p->socket] = 0;
			}else
				T("socket %d init!",p->socket);
		break;

		case SOCK_INIT:     /* if a socket is initiated */
			if(Timer_Elapsed(&p->request_Timer) )
			{  /* For TCP client's connection request delay : 3 sec */
				connect(p->socket, p->dest_Ip, p->dest_Port); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
				Timer_Start(&p->request_Timer);
			}
		break;

		default:break;
	 }
}



/*******************************************************************************
* Function Name  : Tcp_Init
* Description    : Function of initialize.
* Input          : Pointer to tcp client.
* Input          : socket - num of the socket.
* Input          : ipDest - ip destination.
* Input          : portDest - port destination.
* Input          : receiver_EventHandler - function to call when received data.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TcpClientConn_Init(TcpClient *p, char socket, uint8 * ipDest, uint16_t portDest, Receiver_EventHandler receiver_EventHandler, 	OnConnection_EventHandler	onConnection_EventHandler)
{
		T("TcpClientConn_Init()... ");
		p->socket = socket;
		p->dest_Ip[0] = ipDest[0];
		p->dest_Ip[1] = ipDest[1];
		p->dest_Ip[2] = ipDest[2];
		p->dest_Ip[3] = ipDest[3];
		p->dest_Port = portDest;
		Timer_Init(&p->request_Timer, 3000);
		Timer_Start(&p->request_Timer);
		ch_status[p->socket] = 0;
		p->onConnection_EventHandler = onConnection_EventHandler;
		p->receiver_EventHandler = receiver_EventHandler;
		T("TcpClientConn_Init()<--- ");
}

/*############################################ TCP SERVER ########################################################*/
/*****************************************************************************************************************/
/*******************************************************************************
* Function Name  : ServerConnection_Init
* Description    : Function of initialize.
* Input          : p - Pointer to server.
* Input          : socket - Num. of socket.
* Input          : portLocal - The port local.
* Input          : listen_EventHandler - Function to handler the receiver.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ServerConnection_Init(ServerConnection *p, char socket, uint16_t port, Listen_EventHandler listen_EventHandler)
{
	T("ServerConnection_Init()... \r\n");
	p->socket = socket;
	p->port = port;
	ch_status[p->socket] = 0;
	p->listen_EventHandler = listen_EventHandler;
}



/*############################################ WEB ##############################################################*/
/*****************************************************************************************************************/
/*******************************************************************************
* Function Name  : WebServer_ListenEventHandler
* Description    : Handler the receiver data.
* Input          : datin - data receive.
* Input          : len - length.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_ListenEventHandler(char * request, int len)
{
		char method[32] ={0};

		request[len] = 0;

		T("\r\n request = \r\n%s\r\n", request);

		/*
		Timer_Start(&webServer.close_Timer);

		WebServer_GetMethod(request, method);

		if( String_Compare(method, "GET") )
		{
				WebServer_MethodGETEventHandler( request );
				return;
		}

		if( String_Compare(method, "POST") )
		{
				WebServer_MethodPOSTEventHandler( request );
				return;
		}

		Pages_Messages("Comando no reconocido");

*/
}




/**************************************************************************************/


// NO HAL REQUIRED
void T(char * format,...){
	char msg[64] = {0};
	char *p = msg;
	va_list args;
	va_start(args,format);

	vsnprintf(msg, sizeof(msg), format, args);
	strcat(msg,"\r\n");
	while(*p){

		USART1->DR = (uint32_t)*(p++);
		while(!(USART1->SR & USART_SR_TC));
	}
	va_end(args);
}
