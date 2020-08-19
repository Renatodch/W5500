/*
 * socket.h
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */

#ifndef INC_SOCKET_H_
#define INC_SOCKET_H_

#include "main.h"
//#include "Types.h"

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

#define SOCK_ERROR            0
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)     ///< Socket is not initialized
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG           (SOCK_ERROR - 10)    ///< Invalid argrument.
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)    ///< Invalid IP address
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)    ///< Data length is zero or greater than buffer max size.
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.

#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.


/*
 * UDP & MACRAW Packet Infomation
 */
#define PACK_FIRST               0x80              ///< In Non-TCP packet, It indicates to start receiving a packet.
#define PACK_REMAINED            0x01              ///< In Non-TCP packet, It indicates to remain a packet to be received.
#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet.


uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag); // Opens a socket(TCP or UDP or IP_RAW mode)
void close(SOCKET s); // Close socket
uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port); // Establish TCP connection (Active connection)
void disconnect(SOCKET s); // disconnect the connection
uint8_t listen(SOCKET s);	// Establish TCP connection (Passive connection)
uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len, bool retry); // Send data (TCP)
uint16_t recv(SOCKET s, uint8_t * buf, uint16_t len);	// Receive data (TCP)
int32_t sendto(SOCKET sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);
int32_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t  *port); // Receive data (UDP/IP RAW)
char Socket_GetStatusToString(uint8_t socket, char *mt);
void Socket_Trace(char* tag, uint8_t s);

#endif /* INC_SOCKET_H_ */
