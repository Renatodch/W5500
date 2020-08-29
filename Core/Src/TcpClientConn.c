/*
 * TcpClient.c
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */


#include "main.h"


/*################################################################################################################*/
/*############################################ TCP CLIENT ########################################################*/
uint8_t TcpClientConn_SendStream(TcpClient *p, uint8_t * data_buf, 	uint16_t len)
{
	uint8_t buf[1024] = { 0 };

	Uint32_ToBytes(len, buf);

	memcpy(&buf[4], data_buf, len);

	if (getSn_SR(p->socket) == SOCK_ESTABLISHED)
	{
		while(send(p->socket, buf, len + 4)<=0);
		return 1;
	}

	return 0;
}


void Client_Receiver_EventHandler(uint8_t *data, int len){
	uint8_t eepromBuf[100]={0};

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
	T("Me conecte al servidor: %d.%d.%d.%d",devtcc.dest_Ip[0],devtcc.dest_Ip[1],devtcc.dest_Ip[2],devtcc.dest_Ip[3]);
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

				 while((received_len = recv(p->socket, data_buf, RSR_len))<=0);         /* read the received data */

				 p->receiver_EventHandler(data_buf, received_len);         	/* sent the received data */

			}

		break;

		case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
			T("\r\n%d : CLOSE_WAIT", p->socket);
			if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)         /* check Rx data */
			{
				 if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
																																										/* the data size to read is MAX_BUF_SIZE. */
				 while(recv(p->socket, data_buf, RSR_len)<=0);         /* read the received data */
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
			}

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
