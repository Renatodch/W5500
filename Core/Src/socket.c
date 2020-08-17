/*
 * socket.c
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */


#include "main.h"


static uint16 local_port;
extern uint16 sent_ptr;


int8 Socket_GetStatusToString(uint8 socket, char *mt)
{
	uint8_t ret = 0;
	char str[64] ={0};

	//sprintf(str, "%d", socket);
	strcat(mt, str);

	switch (getSn_SR(socket))
	{
	case SOCK_CLOSED:							/**< closed */
		//ret = (uint8_t)STATUS_CLOSED;
		strcat(mt, "Cerrado");
		break;

	case SOCK_INIT:								/**< init State */
		//ret = STATUS_INIT;
		strcat(mt, "Ini. Desconectado");
		break;

	case SOCK_LISTEN:							/**< listen State */
		//ret = STATUS_LISTEN;
		strcat(mt, "Listen");
		break;

	case SOCK_SYNSENT:						/**< connection State */
		//ret = STATUS_SYNSENT;
		strcat(mt, "Syn Sent");
		break;

	case SOCK_SYNRECV:						/**< connection State */
		//ret = STATUS_SYNRECV;
		strcat(mt, "Syn Recv");
		break;

	case SOCK_ESTABLISHED:				/**< success to connect */
		//ret = STATUS_ESTABLISHED;
		strcat(mt, "Conectado");
		break;

	case SOCK_FIN_WAIT:						/**< closing State */
		strcat(mt, "Fin de espera");
		//ret = STATUS_FIN_WAIT;
		break;

	case SOCK_CLOSING:						/**< closing State */
		strcat(mt, "Cerrando");
		//ret = STATUS_CLOSING;
		break;

	case SOCK_TIME_WAIT:					/**< closing State */
		//ret = STATUS_TIME_WAIT;
		strcat(mt, "Time Wait");
		break;

	case SOCK_CLOSE_WAIT:					/**< closing State */
		//ret = STATUS_CLOSE_WAIT;
		strcat(mt, "Close Wait");
		break;

	case SOCK_LAST_ACK:						/**< closing State */
		strcat(mt, "Last Ack");
		//ret = STATUS_LAST_ACK;
		break;

	default:
		strcat(mt, "default");
		break;
	}

	return ret;
}


/**
@brief   This Socket function initialize the channel in perticular mode, and set the port and wait for W5200 done it.
@return  1 for sucess else 0.
*/
uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag)
{
   uint8 ret;
   if (
        ((protocol&0x0F) == Sn_MR_TCP)    ||
        ((protocol&0x0F) == Sn_MR_UDP)    ||
        ((protocol&0x0F) == Sn_MR_IPRAW)  ||
        ((protocol&0x0F) == Sn_MR_MACRAW) ||
        ((protocol&0x0F) == Sn_MR_PPPOE)
      )
   {
      close(s);
      IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);
      if (port != 0) {
         IINCHIP_WRITE( Sn_PORT0(s) ,(uint8)((port & 0xff00) >> 8));
         IINCHIP_WRITE( Sn_PORT1(s) ,(uint8)(port & 0x00ff));
      } else {
         local_port++; // if don't set the source port, set local_port number.
         IINCHIP_WRITE(Sn_PORT0(s) ,(uint8)((local_port & 0xff00) >> 8));
         IINCHIP_WRITE(Sn_PORT1(s) ,(uint8)(local_port & 0x00ff));
      }
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_OPEN); // run sockinit Sn_CR

      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s)) )
         ;
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}


/**
@brief   This function close the socket and parameter is "s" which represent the socket number
*/
void close(SOCKET s)
{

   IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CLOSE);

   /* wait to process the command... */
   while( IINCHIP_READ(Sn_CR(s) ) )
      ;
   /* ------- */
        /* all clear */
   IINCHIP_WRITE( Sn_IR(s) , 0xFF);
}


/**
@brief   This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
@return  1 for success else 0.
*/
uint8 listen(SOCKET s)
{
   uint8 ret;
   if (IINCHIP_READ( Sn_SR(s) ) == SOCK_INIT)
   {
      IINCHIP_WRITE( Sn_CR(s) , Sn_CR_LISTEN);
      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s) ) )
         ;
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}

/**
@brief   This function established  the connection for the channel in Active (client) mode.
      This function waits for the untill the connection is established.

@return  1 for success else 0.
*/
uint8 connect(SOCKET s, uint8 * addr, uint16 port)
{
    uint8 ret;
    if
        (
            ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)
        )
    {
      ret = 0;
    }
    else
    {
						ret = 1;
						// set destination IP
						IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
						IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
						IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
						IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
						IINCHIP_WRITE( Sn_DPORT0(s), (uint8)((port & 0xff00) >> 8));
						IINCHIP_WRITE( Sn_DPORT1(s), (uint8)(port & 0x00ff));
						IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CONNECT);
						/* wait for completion */
						while ( IINCHIP_READ(Sn_CR(s) ) ) ;

					 while(getSn_SR(s) != SOCK_ESTABLISHED)
					 {
								if (getSn_IR(s) & Sn_IR_TIMEOUT)
								{
												//setSn_IR(s, Sn_IR_TIMEOUT);
												IINCHIP_WRITE(Sn_IR(s), (Sn_IR_TIMEOUT));  // clear TIMEOUT Interrupt

												return 0;
								}

								if (getSn_SR(s) == SOCK_CLOSED)
								{
											return 0;
								}
					}
    }

   return ret;
}




/**
@brief   This function used for disconnect the socket and parameter is "s" which represent the socket number
@return  1 for success else 0.
*/
void disconnect(SOCKET s)
{
   IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_DISCON);

   /* wait to process the command... */
   while( IINCHIP_READ(Sn_CR(s) ) );
   /* ------- */
}


/**
@brief   This function used to send the data in TCP mode
@return  1 for success else 0.
*/
uint16 send(SOCKET s, const uint8 * buf, uint16 len, bool retry)
{
  uint8 status=0;
  uint16 ret=0;
  uint16 freesize=0;

  if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
  else ret = len;

  // if freebuf is available, start.
  do
  {
    freesize = getSn_TX_FSR(s);
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
      ret = 0;
      break;
    }
  } while (freesize < ret);


  // copy data
  send_data_processing(s, (uint8 *)buf, ret);
  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);

  /* wait to process the command... */
  while( IINCHIP_READ(Sn_CR(s) ) );

  while ( (IINCHIP_READ(Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
  {
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
    {
//      printf("SEND_OK Problem!!\r\n");
      close(s);
      return 0;
    }
  }
  IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);

#ifdef __DEF_IINCHIP_INT__
   putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
   IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
#endif

   return ret;
}


/**
@brief   This function is an application I/F function which is used to receive the data in TCP mode.
      It continues to wait for data as much as the application wants to receive.

@return  received data size for success else -1.
*/
uint16 recv(SOCKET s, uint8 * buf, uint16 len)
{
   uint16 ret=0;
   if ( len > 0 )
   {
      recv_data_processing(s, buf, len);
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);
      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s) ));
      /* ------- */
      ret = len;
   }
   return ret;
}



