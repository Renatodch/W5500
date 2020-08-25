/*
 * socket.c
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */


#include "main.h"


static uint16_t local_port;
extern uint16_t sent_ptr;

/*About Sockets*/
uint16_t any_port = 1001;
uint16_t io_mode = 0;
uint16_t is_sending = 0;
uint16_t remained_size[MAX_SOCK_NUM] = {0,0,};
uint8_t  pack_info[MAX_SOCK_NUM] = {0,};

/**
@brief   This function used to receive the data in different mode
@return  pack_len: tamaño recibido
*/
int32_t recvfrom(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port)
{
	uint8_t  mr;
	uint8_t  head[8];
	uint16_t pack_len=0;

	switch((mr=getSn_MR(sn)) & 0x0F)
	{
	  case Sn_MR_UDP:
	  //case Sn_MR_IPRAW:
	  //case Sn_MR_MACRAW:
		 break;

	  default:
		 return SOCKERR_SOCKMODE;
	}

	//T("Enter sock remained...");
	if(remained_size[sn] == 0)
	{
		while(1)
		{
			pack_len = getSn_RX_RSR(sn);
			if(getSn_SR(sn) == SOCK_CLOSED) return SOCKERR_SOCKCLOSED;
			if( /*(sock_io_mode & (1<<sn)) &&*/ (pack_len == 0) ) return SOCK_BUSY;
			if(pack_len != 0) break;
		};
	}

	switch (mr & 0x07)
	{
	   case Sn_MR_UDP :
		 if(remained_size[sn] == 0)
		 {
			recv_data_processing(sn, head, 8);
			setSn_CR(sn,Sn_CR_RECV);
			while(getSn_CR(sn));
			// read peer's IP address, port number & packet length
			addr[0] = head[0];
			addr[1] = head[1];
			addr[2] = head[2];
			addr[3] = head[3];
			*port = head[4];
			*port = (*port << 8) + head[5];
			remained_size[sn] = head[6];
			remained_size[sn] = (remained_size[sn] << 8) + head[7];

			pack_info[sn] = PACK_FIRST;
		 }
			if(len < remained_size[sn]) pack_len = len;
			else pack_len = remained_size[sn];
			//A20150601 : For W5300
			len = pack_len;

			//
			// Need to packet length check (default 1472)
			//
			recv_data_processing(sn, buf, pack_len); // data copy.
			break;

		  default:
			 recv_data_ignore(sn, pack_len); // data copy.
			 remained_size[sn] = pack_len;
			 break;
	}
	setSn_CR(sn,Sn_CR_RECV);
	while(getSn_CR(sn)) ;

	remained_size[sn] -= pack_len;

	if(remained_size[sn] != 0)
	{
	   pack_info[sn] |= PACK_REMAINED;
	}
	else pack_info[sn] = PACK_COMPLETED;

   return (int32_t)pack_len;
}

/**
@brief   This function used to send the data in TCP mode
@return  len: tamaño a enviar
*/
int32_t sendto(SOCKET sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{
	uint8_t tmp = 0;
	uint16_t freesize = 0;
	uint32_t taddr;

	switch(getSn_MR(sn) & 0x0F)
	{
	  case Sn_MR_UDP:
	  //case Sn_MR_MACRAW:
	  //case Sn_MR_IPRAW:
		 break;
	  default:
		 return SOCKERR_SOCKMODE; //ERROR
	}

	taddr = ((uint32_t)addr[0]) & 0x000000FF;
	taddr = (taddr << 8) + ((uint32_t)addr[1] & 0x000000FF);
	taddr = (taddr << 8) + ((uint32_t)addr[2] & 0x000000FF);
	taddr = (taddr << 8) + ((uint32_t)addr[3] & 0x000000FF); //taddr = addr[0] addr[1] addr[2] addr[3]

	tmp = getSn_SR(sn);
	if((tmp != SOCK_MACRAW) && (tmp != SOCK_UDP) && (tmp != SOCK_IPRAW)) return SOCKERR_SOCKSTATUS;

	setSn_DIPR(sn,addr);
	setSn_DPORT(sn,port);

	freesize = getSn_TxMAX(sn);
	if (len > freesize) len = freesize; // check size not to exceed MAX size.

	while(1)
	{
	  freesize = getSn_TX_FSR(sn);
	  if(getSn_SR(sn) == SOCK_CLOSED) return SOCKERR_SOCKCLOSED;
	  if(len <= freesize) break;
	};

	send_data_processing(sn, buf, len);

	setSn_CR(sn,Sn_CR_SEND);
	/* wait to process the command... */
	while(getSn_CR(sn));

	while(1)
	{
	  tmp = getSn_IR(sn);
	  if(tmp & Sn_IR_SEND_OK)
	  {
		 setSn_IR(sn, Sn_IR_SEND_OK);
		 break;
	  }

	  else if(tmp & Sn_IR_TIMEOUT)
	  {
		 setSn_IR(sn, Sn_IR_TIMEOUT);

		 return SOCKERR_TIMEOUT;
	  }
	}

	return (int32_t)len;
}

void Socket_Trace(char* tag, uint8_t s)
{
	char str[32] = {0};
	Socket_GetStatusToString(s, str);
	uint8_t val = getSn_SR(s);
	T("%s socket: %s (%d)",tag,str,val);
}

char Socket_GetStatusToString(uint8_t socket, char *mt)
{
	uint8_t ret = 0;
	char str[64] ={0};

	//sprintf(str, "%d", socket);
	strcat(mt, str);

	switch (getSn_SR(socket))
	{
	case SOCK_CLOSED:							/**< closed */
		//ret = (uint8_t_t)STATUS_CLOSED;
		strcat(mt, "Cerrado");
		break;

	case SOCK_INIT:								/**< init State */
		//ret = STATUS_INIT;
		strcat(mt, "Init & Desconectado");
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
uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
   uint8_t ret;
   if (
        ((protocol&0x0F) == Sn_MR_TCP)    ||
        ((protocol&0x0F) == Sn_MR_UDP)    ||
        ((protocol&0x0F) == Sn_MR_IPRAW)  ||
        ((protocol&0x0F) == Sn_MR_MACRAW) ||
        ((protocol&0x0F) == Sn_MR_PPPOE)
      )
   {
      close(s);

      setSn_MR(s ,(protocol | flag));
      //IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);

      if (port != 0) {
    	  setSn_PORT(s, port);
      } else {
         local_port++; // if don't set the source port, set local_port number.
         setSn_PORT(s, local_port);
      }

      setSn_CR(s, Sn_CR_OPEN);
	  while(getSn_CR(s));

	  io_mode |= ((flag & SF_IO_NONBLOCK) << s);  ;
	  is_sending &= ~(1<<s);
	  remained_size[s] = 0;
	  pack_info[s] = PACK_COMPLETED;
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
	setSn_CR( s ,Sn_CR_CLOSE);
	/* wait to process the command... */
	while( getSn_CR(s ) );
	/* clear all interrupt of the socket. */
	setSn_IR(s, 0xFF);
	/* all clear */
	io_mode &= ~(1<<s);
	//
	is_sending &= ~(1<<s);
	remained_size[s] = 0;
	pack_info[s] = 0;
	while(getSn_SR(s) != SOCK_CLOSED);
}

/**
@brief   This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
@return  1 for success else 0.
*/
uint8_t listen(SOCKET s)
{
   uint8_t ret;
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
uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
{
    uint8_t ret;
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
						IINCHIP_WRITE( Sn_DPORT0(s), (uint8_t)((port & 0xff00) >> 8));
						IINCHIP_WRITE( Sn_DPORT1(s), (uint8_t)(port & 0x00ff));
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
int8_t disconnect(SOCKET s)
{
/*
   setSn_CR(s,Sn_CR_DISCON);
   while( getSn_CR(s) );
*/
	setSn_CR(s,Sn_CR_DISCON);
	/* wait to process the command... */
	while(getSn_CR(s));
	is_sending &= ~(1<<s);
	if(io_mode & (1<<s)) return SOCK_BUSY;
	while(getSn_SR(s) != SOCK_CLOSED)
	{
	   if(getSn_IR(s) & Sn_IR_TIMEOUT)
	   {
		  close(s);
		  return SOCKERR_TIMEOUT;
	   }
	}
	return SOCK_OK;

}

/**
@brief   This function used to send the data in TCP mode
@return  1 for success else 0.
*/
int32_t send(uint8_t sn, uint8_t * buf, uint16_t len, bool retry)
{
   uint8_t tmp=0;
   uint16_t freesize=0;

   tmp = getSn_SR(sn);
   if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) return SOCKERR_SOCKSTATUS;
   if( is_sending & (1<<sn) )
   {
      tmp = getSn_IR(sn);
      if(tmp & Sn_IR_SEND_OK)
      {
         setSn_IR(sn, Sn_IR_SEND_OK);

         is_sending &= ~(1<<sn);
      }
      else if(tmp & Sn_IR_TIMEOUT)
      {
         close(sn);
         return SOCKERR_TIMEOUT;
      }
      else return SOCK_BUSY;
   }
   freesize = getSn_TxMAX(sn);
   if (len > freesize) len = freesize; // check size not to exceed MAX size.
   while(1)
   {
      freesize = getSn_TX_FSR(sn);
      tmp = getSn_SR(sn);
      if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
      {
         close(sn);
         return SOCKERR_SOCKSTATUS;
      }
      if( (io_mode & (1<<sn)) && (len > freesize) ) return SOCK_BUSY;
      if(len <= freesize) break;
   }
   send_data_processing(sn, buf, len);

   setSn_CR(sn,Sn_CR_SEND);
   /* wait to process the command... */
   while(getSn_CR(sn));
   is_sending |= (1 << sn);
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}
/*
uint16_t send(SOCKET s, const uint8_t* buf, uint16_t len, bool retry)
{
  uint8_t status=0;
  uint16_t ret=0;
  uint16_t freesize=0;

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
  send_data_processing(s, (uint8_t *)buf, ret);
  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);


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
*/

/**
@brief   This function used to receive the data in TCP mode.
      It continues to wait for data as much as the application wants to receive.

@return  received data size for success else -1.
*/

uint16_t recv(SOCKET s, uint8_t * buf, uint16_t len)
{
   uint16_t ret=0;
   if ( len > 0 )
   {
      recv_data_processing(s, buf, len);
      setSn_CR(s ,Sn_CR_RECV);
      while( getSn_CR(s));

      ret = len;
   }
   return ret;
}


/*
int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len)
{
	uint8_t  tmp = 0;
	uint16_t recvsize = 0;

	recvsize = getSn_RxMAX(sn);
	if(recvsize < len) len = recvsize;

	while(1)
	{
	 recvsize = getSn_RX_RSR(sn);
	 tmp = getSn_SR(sn);
	 if (tmp != SOCK_ESTABLISHED)
	 {
		if(tmp == SOCK_CLOSE_WAIT)
		{
		   if(recvsize != 0) break;
		   else if(getSn_TX_FSR(sn) == getSn_TxMAX(sn))
		   {
			  close(sn);
			  return SOCKERR_SOCKSTATUS;
		   }
		}
		else
		{
		   close(sn);
		   return SOCKERR_SOCKSTATUS;
		}
	 }
	 if((io_mode & (1<<sn)) && (recvsize == 0)) return SOCK_BUSY;
	 if(recvsize != 0) break;
	};

	if(recvsize < len) len = recvsize;
	recv_data_processing(sn, buf, len);
	setSn_CR(sn,Sn_CR_RECV);
	while(getSn_CR(sn));

	return (int32_t)len;
}
*/

