/*
 * TcpClient.h
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */

#ifndef INC_TCPCLIENTCONN_H_
#define INC_TCPCLIENTCONN_H_

#include "main.h"

/***************************************************************************************************/
/************************************  TCP CLIENTE *************************************************/
/***************************************************************************************************/

typedef void (*Receiver_EventHandler)(uint8_t *data, int len);
typedef void (*OnConnection_EventHandler)(void);

typedef struct
{
		Timer						request_Timer;
		uint8_t	 					dest_Ip[4];
		uint16_t  					dest_Port;
		uint8_t						socket;
		Receiver_EventHandler		receiver_EventHandler;
		OnConnection_EventHandler	onConnection_EventHandler;
}TcpClient;

void 	Client_Receiver_EventHandler(uint8_t *data, int len);
void 	Client_onConnection_EventHandler(void);
uint8_t TcpClientConn_SendStream(TcpClient *p, uint8_t * data_buf, 	uint16_t len);
void 	TcpClientConn_Events(TcpClient *p, uint16_t port);
void 	TcpClientConn_Init(TcpClient *p, char socket, uint8_t * ipDest, uint16_t portDest, Receiver_EventHandler rcvr, OnConnection_EventHandler con);


#endif /* INC_TCPCLIENTCONN_H_ */
