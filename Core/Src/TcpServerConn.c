/*
 * TcpServer.c
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */


#include "main.h"



/*########################################################################################################################################*/
/*################################################## TCP SERVER ##########################################################################*/

void ServerConn_Events(ServerConnection *p)
{

	uint16_t RSR_len;
	uint16_t received_len;
	uint8_t * data_buf = TX_BUF;
	uint8_t ip[4] = {0};
	uint16_t port;

	switch (getSn_SR(p->socket))
	{
		case SOCK_ESTABLISHED:              /* if connection is established */
			if(ch_status[p->socket]==1)
			{
				getSn_DIPR(p->socket, ip);
				port = getSn_DPORT(p->socket);

				 T("%d : Conectado", p->socket);
				 T("Peer IP : %d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3] );
				 T("Peer Port : %d", port) ;
				 ch_status[p->socket] = 2;
			}

			if ((RSR_len = getSn_RX_RSR(p->socket)) > 0)         /* check Rx data */
			{
				if (RSR_len > TX_RX_MAX_BUF_SIZE)
							RSR_len = TX_RX_MAX_BUF_SIZE;   	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
																	/* the data size to read is MAX_BUF_SIZE. */
				while((received_len = recv(p->socket, data_buf, RSR_len))<=0);         				/* read the received data */


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
				while(recv(p->socket, data_buf, RSR_len)<=0);     /* read the received data */
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
				T("%d : Fail to init socket.",p->socket);
				ch_status[p->socket] = 0;
			}

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
		while(send(p->socket, (uint8_t*) str, strlen(str))<=0);
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
