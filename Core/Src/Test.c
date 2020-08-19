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
uint8_t Enable_DHCP = OFF;
uint8_t MAC[6] = {0x00, 0x08, 0xDC, 0x47, 0x47, 0x54};//MAC Address
uint8_t IpDevice[4] = {192,168,0,8};//IpDevice Address
uint8_t GateWay[4] = {192,168,0,1};//Gateway Address
uint8_t SubNet[4] = {255,255,255,0};//SubnetMask Address

uint8_t Domain_name[] = "www.google.com";
uint8_t Domain_IP[4]  = {0, };

uint8_t IpTrace[4];//IpTrace Address
uint16_t PortTrace;


CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def;

uint16_t PortServer = 8888;
uint8_t IpServer[4] = {192,168,0,3}; //mi PC
uint8_t DNS_Server_1[4] = {8,8,8,8};//1st DNS server
uint8_t DNS_Server_2[4] = {168, 126, 63, 1};//Secondary server


/*About Sockets*/
uint16_t any_port = 1001;
uint16_t io_mode = 0;
uint16_t is_sending = 0;
uint16_t remained_size[MAX_SOCK_NUM] = {0,0,};
uint8_t  pack_info[MAX_SOCK_NUM] = {0,};
//TX MEM SIZE- SOCKET 0-7:4KB
//RX MEM SIZE- SOCKET 0-7:4KB
uint8_t txsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};
uint8_t rxsize[MAX_SOCK_NUM] = {4,4,4,4,4,4,4,4};
/*********/

/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*##############################################################################################################*/
/*############################################ TCP CLIENT ########################################################*/

uint8_t ch_status[MAX_SOCK_NUM] = { 0, }; /** 0:close, 1:ready, 2:connected */
uint8_t TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications


uint8_t TcpClientConn_SendStream(TcpClient *p, uint8_t * data_buf, 	uint16_t len)
{
	uint8_t buf[1024] = { 0 };

	Uint32_ToBytes(len, buf);

	memcpy(&buf[4], data_buf, len);

	if (getSn_SR(p->socket) == SOCK_ESTABLISHED)
	{
		send(p->socket, (uint8_t*) buf, len + 4, (bool)0);
		return 1;
	}

	return 0;
}

uint8_t eepromBuf[100]={0};

void Client_Receiver_EventHandler(uint8_t *data, int len){

	int fixedLen=0;

	T("Recibi: %s, len:%d",(char *)data,len);

	T("Guardar en eeprom");

	fixedLen = len/4;
	if(len%4) fixedLen++;

	EE_Writes(0,fixedLen,(uint32_t *)data);  // data[0,0,0,0,0,0,0,0]  (uint32_t*)data[0000,0000]

	EE_Reads(0,fixedLen,(uint32_t *)eepromBuf);

	T("data leida de memoria: %s",(char *)eepromBuf);

	uint8_t size = strlen((char *)eepromBuf);
	if(size)
		TcpClientConn_SendStream(&devtcc, eepromBuf, size);
	else return;

}
void Client_onConnection_EventHandler(void){
	T("Me conecte al servidor de Renato: %d.%d.%d.%d",devtcc.dest_Ip[0],devtcc.dest_Ip[1],devtcc.dest_Ip[2],devtcc.dest_Ip[3]);
}


void TcpClientConn_Events(TcpClient *p, uint16_t port)
{
	uint16_t RSR_len;
	uint16_t received_len;
	uint8_t * data_buf = TX_BUF;

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
				 T("TCP Client %d Started. port: %d", p->socket, port);
				 ch_status[p->socket] = 1;
			}
			if(!socket(p->socket, Sn_MR_TCP, any_port++, 0x00))    /* init o reinit el socket */
			{
				 T("\a%d : Fail to create socket.",p->socket);
				 ch_status[p->socket] = 0;
			}else
				T("socket %d ready!",p->socket);
		break;

		case SOCK_INIT:     /* if a socket is initiated */
			if(Timer_Elapsed(&p->request_Timer) )
			{  /* For TCP client's connection request delay : 3 sec */
				connect(p->socket, p->dest_Ip, p->dest_Port); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
				T("status: %d - Connect socket %d ", getSn_SR(p->socket), p->socket);
				Timer_Start(&p->request_Timer);
			}
		break;

		default:break;
	 }
}


void TcpClientConn_Init(TcpClient *p, char socket, uint8_t * ipDest, uint16_t portDest,
						Receiver_EventHandler receiver_EventHandler, 	OnConnection_EventHandler	onConnection_EventHandler)
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
}


/*########################################################################################################################################*/
/*################################################## TCP SERVER ##########################################################################*/

void ServerConn_Events(ServerConnection *p)
{

uint16_t RSR_len;
uint16_t received_len;
uint8_t * data_buf = TX_BUF;

 switch (getSn_SR(p->socket))
 {
	case SOCK_ESTABLISHED:              /* if connection is established */
		if(ch_status[p->socket]==1)
		{
			 T("%d : Conectado", p->socket);
			 T("Peer IP : %d.%d.%d.%d", IINCHIP_READ(Sn_DIPR0(p->socket)),  IINCHIP_READ(Sn_DIPR1(p->socket)), IINCHIP_READ(Sn_DIPR2(p->socket)), IINCHIP_READ(Sn_DIPR3(p->socket)) );
			 T("Peer Port : %d", ( (uint16_t) (IINCHIP_READ(Sn_DPORT0(p->socket)))<<8) + (uint16_t)IINCHIP_READ( Sn_DPORT1(p->socket)) ) ;
			 ch_status[p->socket] = 2;
		}

		if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)         /* check Rx data */
		{
			//T("RSR_len = %d", RSR_len);
			if (RSR_len > TX_RX_MAX_BUF_SIZE)
						RSR_len = TX_RX_MAX_BUF_SIZE;   	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
																/* the data size to read is MAX_BUF_SIZE. */
			received_len = recv(p->socket, data_buf, RSR_len);         				/* read the received data */
			//T("data_buf = \r\n%s", data_buf);
			//T("received_len = \r\n%d", received_len);
			p->listen_EventHandler((char*)data_buf, received_len);         										/* sent the received data */
			disconnect(p->socket);
			close(p->socket);
		}

		break;

	case SOCK_CLOSE_WAIT:    /* If the client request to close */
		T("%d : CLOSE_WAIT", p->socket);
		if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)     /* check Rx data */
		{
			if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */																													/* the data size to read is MAX_BUF_SIZE. */
			recv(p->socket, data_buf, RSR_len);     /* read the received data */
		}
		disconnect(p->socket);
		ch_status[p->socket] = 0;
		break;

	case SOCK_CLOSED:                                       /* if a socket is closed */
		if(!ch_status[p->socket])
		{
			T("ServerConnection %d Started. port: %d", p->socket, p->port);
			ch_status[p->socket] = 1;
		}
		if(!socket(p->socket,(Sn_MR_ND|Sn_MR_TCP), p->port,0x00))    /* reinitialize the socket */
		{
			T("%d : Fail to create socket.",p->socket);
			ch_status[p->socket] = 0;
		}
		else
			T("socket %d ready!",p->socket);
		break;

	case SOCK_INIT:   /* if a socket is initiated */
		listen(p->socket);
		T("status: %d - LISTEN socket %d ", getSn_SR(p->socket), p->socket);
	break;

		default:break;
 }

}
void ServerConn_SendLine(ServerConnection *p, const char * str)
{
	if (getSn_SR(p->socket) == SOCK_ESTABLISHED)
	{
		send(p->socket, (const uint8_t*) str, strlen(str), (bool)0);
	}
}

void ServerConnection_Init(ServerConnection *p, char socket, uint16_t port, Listen_EventHandler listen_EventHandler)
{
	T("ServerConnection_Init()... \r\n");
	p->socket = socket;
	p->port = port;
	ch_status[p->socket] = 0;
	p->listen_EventHandler = listen_EventHandler;
}


/*##############################################################################################################*/
/*############################################ WEB ##############################################################*/

void WebServer_GetParameters(char *request)
{
	char result[1024];
	String_Between(request, "GET /?", "HTTP", result);
	strcpy(request, result);
}

void WebServer_GetParametersFromPOST(char *request)
{
	int len;
	char prms[4096] = {0};
	String_Between(request, "Content-Length:", "\r\n", prms);
	len = strlen( request ) - atoi( prms );
	strcpy( prms , request + len );
	strcpy(request, prms);
	T("Datos extraidos: %s", request);
}

void WebServer_GetUri(char *request, char *uri)
{
	String_Between(request, " /", "HTTP", uri);
}

void WebServer_GetMethod(char *request, char *method)
{
	String_Between(request, "", " ", method);
}

void WebServer_GetUriFromGET(char *request, char *get)
{
	String_Between(request, "GET", "HTTP", get);
}

unsigned char WebServer_IsIp(char *request)
{
	char prms[512];

	WebServer_GetUriFromGET( request, prms );

	if ( String_Has( prms ) )
	{
			String_Trim_Total(prms);

			if(strcmp(prms, "/") == 0)
			{
					return 1;
			}
	}
	return 0;
}

/*******************************************************************************
* Function Name  : WebServer_WriteEeprom_Usuario
* Description    : Write eeprom usuario.
* Input          : prms - parameters.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_WriteEeprom_Usuario(char *prms)
{
	char formatParams[1024] = {0};
	//char eeprom[1024] = {0};

	if ( String_Has(prms) )
	{
		String_Remplace(formatParams, prms, "&", "\r\n");

	}
}

void WebServer_WriteTable(char *prms)
{
	/*
	char *start = NULL;
	char *end = NULL;
	char name[64] = {0};
	char filename[2048] = {0};
	int len = 0;
*/
	T("void WebServer_WriteTable(char *prms)");

	/*
	if ( String_Has(prms) )
	{

		String_Between(prms,"filename=\"","\"",filename);

		strcat(name,"\"");
		strcat(name,filename);
		strcat(name,"\"");
		TRACE_U(name);

		start = strstr(prms,"0=");
		end = strstr(start,"-");
		len = end - start;


		strlcpy(filename,start,len+1);
		strcat(filename,name);

		TRACE_U("TABLA:\r\n%s",filename);
		if(String_ToEepromIsEqual(EEPROM_TABLA,filename) )
		{
			WebServer_BuzzerToWriteEeprom();
			Eeprom_WriteUint32( EEPROM_EPOCH_TIME_TABLA,app.epochTime);
			Pages_Servicio3();											//Muestra Tabla
			App_InitVariables();											//Para que el Rotem vuelva a leer el dia, actualice los rangos.

		}
		else
		{
			Pages_Messages("Table Failed...");
		}
	}
	*/
}



void WebServer_WriteEeprom_Factory(char *inParams)
{
	/*
	char eepromWrite[550];
	char formatParams[550];
	char value[16];
*/
		if ( String_Has(inParams) )
		{

		}
}

/*******************************************************************************
* Function Name  : WebServer_EnterToState
* Description    : Enter to state.
* Input          : prms - parameters.
* Output         : None.
* Return         : None.
*******************************************************************************/
void 	WebServer_EnterToState(char *prms)
{
	switch(webServer.State)
	{
		case WebServer_Ste_Init  :
		break;

		case WebServer_Ste_Clave  :
				Pages_Claves();
		break;

		case WebServer_Ste_Usuario  :
				Pages_User();
		break;

		case WebServer_Ste_Factory  :
				//Pages_Factory();
		break;
		default: break;
	}
}

/*******************************************************************************
* Function Name  : WebServer_ExitFromState
* Description    : Exit from state.
* Input          : prms - parameters.
* Output         : None.
* Return         : None.
*******************************************************************************/
void 	WebServer_ExitFromState(char *prms)
{
	switch(webServer.State)
	{
		case WebServer_Ste_Init  :
		break;

		case WebServer_Ste_Clave  :
		break;

		case WebServer_Ste_Usuario  :
		break;

		case WebServer_Ste_Factory  :
		break;
		default: break;

	}
}


void 	WebServer_Change_State(WEB_SERVER__STE ste, char *prms)
{
	WebServer_ExitFromState( prms );
	webServer.State = ste;
	WebServer_EnterToState( prms );
}

void WebServer_Usuario_State(WEB_SERVER__EVT e, char *prms)
{
	switch(e)
	{
			case WEB_SERVER__EVT_IP  :
					WebServer_Change_State(WebServer_Ste_Clave, prms);
			break;

			case WEB_SERVER__EVT_POST_WRITE_EEPROM  :

					WebServer_WriteEeprom_Usuario(prms);

					WebServer_Change_State(WebServer_Ste_Init, prms);
			break;

			case WEB_SERVER__EVT_POST_WRITE_TABLE  :

					WebServer_WriteTable(prms);

					WebServer_Change_State(WebServer_Ste_Init, prms);
			break;
			default:break;
	}
}


void WebServer_Admin_State(WEB_SERVER__EVT e, char *prms)
{
	switch(e)
	{
		case WEB_SERVER__EVT_IP  :
				WebServer_Change_State(WebServer_Ste_Clave, prms);
		break;

		case WEB_SERVER__EVT_POST_WRITE_EEPROM  :

				WebServer_WriteEeprom_Factory(prms);

				WebServer_Change_State(WebServer_Ste_Init, prms);
		break;
		default: break;
	}
}


void WebServer_Clave_State(WEB_SERVER__EVT e, char *prms)
{
	switch(e)
	{
			case WEB_SERVER__EVT_IP  :
					WebServer_Change_State(WebServer_Ste_Clave, prms);
			break;

			case WEB_SERVER__EVT_CLAVE_USUARIO :
					WebServer_Change_State(WebServer_Ste_Usuario, prms);
			break;

			case WEB_SERVER__EVT_CLAVE_FACTORY  :
					WebServer_Change_State(WebServer_Ste_Factory, prms);
			break;
			default: break;
	}
}

void WebServer_Init_State(WEB_SERVER__EVT e, char *prms)
{
}


/*******************************************************************************
* Function Name  : WebServer_Fsm
* Description    : Handle the events of WebServer.
* Input          : Event of plc.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_Fsm(WEB_SERVER__EVT e, char *prms)
{
	switch(webServer.State)
		{
		case WebServer_Ste_Init  :
				WebServer_Init_State(e, prms);
		break;

		case WebServer_Ste_Clave  :
				WebServer_Clave_State(e, prms);
		break;

		case WebServer_Ste_Usuario  :
				WebServer_Usuario_State(e, prms);
		break;

		case WebServer_Ste_Factory  :
				WebServer_Admin_State(e, prms);
		break;
		default:break;
	}
}

/*******************************************************************************
* Function Name  : WebServer_ProcessUris
* Description    : Verify the events of WebServer.
* Input          : prms - the parameters.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_ProcessUris(char *prms)
{
	char claveValue[64] = {0};

	if( String_Is_Empty( prms ) )
		return;

	switch(webServer.State)
	{
		case WebServer_Ste_Clave  :          ////

			String_Between( prms, "Clave=", "", claveValue );

			String_Trim_Total( claveValue );

			if( String_Compare( claveValue, webServer.ClaveUsuario) )
			{
				T("WEB_SERVER__EVT_CLAVE_USUARIO");
				WebServer_Fsm( WEB_SERVER__EVT_CLAVE_USUARIO, "");
				break;
			}

			if( String_Compare( claveValue, webServer.ClaveAdmin))
			{
				T("WEB_SERVER__EVT_CLAVE_FACTORY");
				WebServer_Fsm( WEB_SERVER__EVT_CLAVE_FACTORY, "");
				break;
			}

			//Pages_Error_Clave();
		break;


		case WebServer_Ste_Usuario  :
		case WebServer_Ste_Factory  :

			if (strstr( prms, "IpDevice") != NULL){

				WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_EEPROM, prms);
				return;
			}
			else{
				//TRACE_U("WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_TABLE, prms)");

				WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_TABLE, prms);
			}
		break;
		default: break;
	}
}


/*******************************************************************************
* Function Name  : WebServer_MethodGETHandler
* Description    : Handler the GET method.
* Input          : request - the request.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_MethodGETEventHandler(char * request)
{
	char uri[64];

	WebServer_GetUri( request, uri );

	T("uri: %s",uri);

	String_Trim_Total( uri );
	T("uri: %s",uri);

	if( String_Compare( uri, "home" ) )
	{
		T("home");
		WebServer_Change_State(WebServer_Ste_Clave,"");

		return;
	}

	if( String_Compare( uri, "servicio" ) )
	{
		T("web.uri servicio");
		//Pages_Servicio();
		return;
	}

	if( String_Compare( uri, "tabla" ) )
	{
				//Pages_Servicio3();
		return;
	}

	if( String_Compare( uri, "favicon.ico" ) )
	{
		T("web.uri favicon.ico");
		//Pages_Servicio();
		return;
	}

	if( String_Compare( uri, "settime" ) )
	{
		T("web.uri settime");
		//Pages_SetTime();
		return;
	}

	//Pages_Messages("Comando no reconocido");

}

/*******************************************************************************
* Function Name  : WebServer_MethodPOSTHandler
* Description    : Handler the POST method.
* Input          : request - the request.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WebServer_MethodPOSTEventHandler(char * request)
{
		char uri[512] = {0};	      //antes 512

		WebServer_GetUri(request, uri);

		String_Trim_Total( uri );

		T("uri = %s",uri);

		if( String_Is_Empty( uri ) )
			return;

		switch(webServer.State)
		{
			case WebServer_Ste_Clave  :

				if( String_Compare( uri, "clave" ) )
				{
					WebServer_GetParametersFromPOST( request );
					WebServer_ProcessUris( request );
				}

			break;

			case WebServer_Ste_Usuario  :
			case WebServer_Ste_Factory  :

				if( String_Compare( uri, "usuario")  )
				{
	//							TRACE_U("WebServer_MethodPOSTEventHandler: uri = Usuario");
	//							TRACE_U("1.......... %s\r\n",request);
						WebServer_GetParametersFromPOST( request );
	//							TRACE_U("2.......... %s\r\n",request);                                   ////
						WebServer_ProcessUris( request );
				}

				if( String_Compare( uri, "fabrica") )
				{
						WebServer_GetParametersFromPOST( request );
						WebServer_ProcessUris( request );
				}

			break;
			default:break;
		}
}

void WebServer_ListenEventHandler(char * request, int len)
{
		char method[32] ={0};

		request[len] = 0;

		T("request = \r\n%s\r\n", request);

		Timer_Start(&webServer.close_Timer);

		WebServer_GetMethod(request, method);

		T("method: %s",method);

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

		//Pages_Messages("Comando no reconocido");

}

void WebServer_Init()
{
	webServer.State = WebServer_Ste_Init;
	Timer_Init(&webServer.close_Timer, 60000);
}


/*##################################################################################################*/


// NO HAL REQUIRED
void T(char * format,...){
	char msg[1024] = {0};
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
