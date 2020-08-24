/*
 * TcpServer.h
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */

#ifndef INC_TCPSERVERCONN_H_
#define INC_TCPSERVERCONN_H_

#include "main.h"

/***************************************************************************************************/
/************************************  TCP SERVER *************************************************/
/***************************************************************************************************/
typedef void (*Listen_EventHandler)(char *dat, int len);

typedef struct
{
		uint16_t  				port;
		uint8_t					socket;
		Listen_EventHandler		listen_EventHandler;
}ServerConnection;

void ServerConn_SendLine(ServerConnection *p, const char * str);
void ServerConn_Events(ServerConnection *p);
void ServerConnection_Init(ServerConnection *p, char socket, uint16_t port, Listen_EventHandler listen_EventHandler);




#endif /* INC_TCPSERVERCONN_H_ */
