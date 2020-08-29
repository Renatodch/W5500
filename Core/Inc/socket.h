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

#define SF_IO_NONBLOCK           0x01              ///< Socket nonblock io mode. It used parameter in \ref socket().

/*
 * UDP & MACRAW Packet Infomation
 */
#define PACK_FIRST               0x80              ///< In Non-TCP packet, It indicates to start receiving a packet.
#define PACK_REMAINED            0x01              ///< In Non-TCP packet, It indicates to remain a packet to be received.
#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet.



#define CHECK_SOCKNUM()   \
   do{                    \
      if(sn > MAX_SOCK_NUM) return SOCKERR_SOCKNUM;   \
   }while(0);             \

#define CHECK_SOCKMODE(mode)  \
   do{                     \
      if((getSn_MR(sn) & 0x0F) != mode) return SOCKERR_SOCKMODE;  \
   }while(0);              \

#define CHECK_SOCKINIT()   \
   do{                     \
      if((getSn_SR(sn) != SOCK_INIT)) return SOCKERR_SOCKINIT; \
   }while(0);              \

#define CHECK_SOCKDATA()   \
   do{                     \
      if(len == 0) return SOCKERR_DATALEN;   \
   }while(0);              \

/*extern variables*/
extern uint16_t any_port;
extern uint16_t io_mode;
extern uint16_t is_sending;
extern uint16_t remained_size[MAX_SOCK_NUM];
extern uint8_t  pack_info[MAX_SOCK_NUM];

/*#######################################################################################################################################*/
/**
 * @ingroup WIZnet_socket_APIs
 * @brief Open a socket.
 * @details Initializes the socket with 'sn' passed as parameter and open.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param protocol Protocol type to operate such as TCP, UDP and MACRAW.
 * @param port Port number to be bined.
 * @param flag Socket flags as \ref SF_ETHER_OWN, \ref SF_IGMP_VER2, \ref SF_TCP_NODELAY, \ref SF_MULTI_ENABLE, \ref SF_IO_NONBLOCK and so on.\n
 *             Valid flags only in W5500 : @ref SF_BROAD_BLOCK, @ref SF_MULTI_BLOCK, @ref SF_IPv6_BLOCK, and @ref SF_UNI_BLOCK.
 * @sa Sn_MR
 *
 * @return @b Success : The socket number @b 'sn' passed as parameter\n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid socket number\n
 *                        @ref SOCKERR_SOCKMODE    - Not support socket mode as TCP, UDP, and so on. \n
 *                        @ref SOCKERR_SOCKFLAG    - Invaild socket flag.
 */
int8_t socket(SOCKET sn, uint8_t protocol, uint16_t port, uint8_t flag); // Inicializa y abre un socket en modo TCP o UDP
/**
 * @ingroup WIZnet_socket_APIs
 * @brief Close a socket.
 * @details It closes the socket  with @b'sn' passed as parameter.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 *
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    : @ref SOCKERR_SOCKNUM - Invalid socket number
 */
int8_t close(SOCKET sn); // Cierra el socket
/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to connect a server.
 * @details It requests connection to the server with destination IP address and port number passed as parameter.\n
 * @note It is valid only in TCP client mode.
 *       In block io mode, it does not return until connection is completed.
 *       In Non-block io mode, it return @ref SOCK_BUSY immediately.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param addr Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param port Destination port number.
 *
 * @return @b Success : @ref SOCK_OK \n
 * @b Fail    :\n @ref SOCKERR_SOCKNUM   - Invalid socket number\n
 *                @ref SOCKERR_SOCKMODE  - Invalid socket mode\n
 *                @ref SOCKERR_SOCKINIT  - Socket is not initialized\n
 *                @ref SOCKERR_IPINVALID - Wrong server IP address\n
 *                @ref SOCKERR_PORTZERO  - Server port zero\n
 *                @ref SOCKERR_TIMEOUT   - Timeout occurred during request connection\n
 *                @ref SOCK_BUSY         - In non-block io mode, it returned immediately\n
 */
int8_t connect(SOCKET sn, uint8_t * addr, uint16_t port); // Establece conexion TCP Activa

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to disconnect a connection socket.
 * @details It sends request message to disconnect the TCP socket 'sn' passed as parameter to the server or client.
 * @note It is valid only in TCP server or client mode. \n
 *       In block io mode, it does not return until disconnection is completed. \n
 *       In Non-block io mode, it return @ref SOCK_BUSY immediately. \n

 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return @b Success :   @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM  - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE - Invalid operation in the socket \n
 *                        @ref SOCKERR_TIMEOUT  - Timeout occurred \n
 *                        @ref SOCK_BUSY        - Socket is busy.
 */
int8_t disconnect(SOCKET sn); // Desconecta
/**
 * @ingroup WIZnet_socket_APIs
 * @brief Listen to a connection request from a client.
 * @details It is listening to a connection request from a client.
 * If connection request is accepted successfully, the connection is established. Socket sn is used in passive(server) mode.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKINIT   - Socket is not initialized \n
 *                        @ref SOCKERR_SOCKCLOSED - Socket closed unexpectedly.
 */
int8_t listen(SOCKET sn);	// Establece conexion TCP Pasiva

/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Send data to the connected peer in TCP socket.
 * @details It is used to send outgoing data to the connected socket.
 * @note    It is valid only in TCP server or client mode. It can't send data greater than socket buffer size. \n
 *          In block io mode, It doesn't return until data send is completed - socket buffer size is greater than data. \n
 *          In non-block io mode, It return @ref SOCK_BUSY immediately when socket buffer is not enough. \n
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer buffer containing data to be sent.
 * @param len The byte length of data in buf.
 * @return	@b Success : The sent data size \n
 *          @b Fail    : \n @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                          @ref SOCKERR_TIMEOUT    - Timeout occurred \n
 *                          @ref SOCKERR_SOCKMODE 	- Invalid operation in the socket \n
 *                          @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                          @ref SOCKERR_DATALEN    - zero data length \n
 *                          @ref SOCK_BUSY          - Socket is busy.
 */
int32_t send(uint8_t sn, uint8_t * buf, uint16_t len);
/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Receive data from the connected peer.
 * @details It is used to read incoming data from the connected socket.\n
 *          It waits for data as much as the application wants to receive.
 * @note    It is valid only in TCP server or client mode. It can't receive data greater than socket buffer size. \n
 *          In block io mode, it doesn't return until data reception is completed - data is filled as <I>len</I> in socket buffer. \n
 *          In non-block io mode, it return @ref SOCK_BUSY immediately when <I>len</I> is greater than data size in socket buffer. \n
 *
 * @param sn  Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer buffer to read incoming data.
 * @param len The max data length of data in buf.
 * @return	@b Success : The real received data size \n
 *          @b Fail    :\n
 *                     @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                     @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                     @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                     @ref SOCKERR_DATALEN    - zero data length \n
 *                     @ref SOCK_BUSY          - Socket is busy.
 */
int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len);
/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Sends datagram to the peer with destination IP address and port number passed as parameter.
 * @details It sends datagram of UDP or MACRAW to the peer with destination IP address and port number passed as parameter.\n
 *          Even if the connectionless socket has been previously connected to a specific address,
 *          the address and port number parameters override the destination address for that particular datagram only.
 * @note    In block io mode, It doesn't return until data send is completed - socket buffer size is greater than <I>len</I>.
 *          In non-block io mode, It return @ref SOCK_BUSY immediately when socket buffer is not enough.
 *
 * @param sn    Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf   Pointer buffer to send outgoing data.
 * @param len   The byte length of data in buf.
 * @param addr  Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param port  Destination port number.
 *
 * @return @b Success : The sent data size \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE    - Invalid operation in the socket \n
 *                        @ref SOCKERR_SOCKSTATUS  - Invalid socket status for socket operation \n
 *                        @ref SOCKERR_DATALEN     - zero data length \n
 *                        @ref SOCKERR_IPINVALID   - Wrong server IP address\n
 *                        @ref SOCKERR_PORTZERO    - Server port zero\n
 *                        @ref SOCKERR_SOCKCLOSED  - Socket unexpectedly closed \n
 *                        @ref SOCKERR_TIMEOUT     - Timeout occurred \n
 *                        @ref SOCK_BUSY           - Socket is busy.
 */
int32_t sendto(SOCKET sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);// Envia datos en modo UDP

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Receive datagram of UDP or MACRAW
 * @details This function is an application I/F function which is used to receive the data in other then TCP mode. \n
 *          This function is used to receive UDP and MAC_RAW mode, and handle the header as well.
 *          This function can divide to received the packet data.
 *          On the MACRAW SOCKET, the addr and port parameters are ignored.
 * @note    In block io mode, it doesn't return until data reception is completed - data is filled as <I>len</I> in socket buffer
 *          In non-block io mode, it return @ref SOCK_BUSY immediately when <I>len</I> is greater than data size in socket buffer.
 *
 * @param sn   Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf  Pointer buffer to read incoming data.
 * @param len  The max data length of data in buf.
 *             When the received packet size <= len, receives data as packet sized.
 *             When others, receives data as len.
 * @param addr Pointer variable of destination IP address. It should be allocated 4 bytes.
 *             It is valid only when the first call recvfrom for receiving the packet.
 *             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO, &packinfo).
 * @param port Pointer variable of destination port number.
 *             It is valid only when the first call recvform for receiving the packet.
*             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO, &packinfo).
 *
 * @return	@b Success : This function return real received data size for success.\n
 *          @b Fail    : @ref SOCKERR_DATALEN    - zero data length \n
 *                       @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                       @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                       @ref SOCKBUSY           - Socket is busy.
 */
int32_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t  *port); // Recibe datos en modo UDP
char Socket_GetStatusToString(uint8_t socket, char *mt); //Obtiene el estado del socket en un mensaje
void Socket_Trace(char* tag, uint8_t sn); //Trace estado del socket

#endif /* INC_SOCKET_H_ */
