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
		case SOCK_CLOSED:strcat(mt, "Cerrado");break;/**< closed */
		case SOCK_INIT:strcat(mt, "Init & Desconectado");break;/**< init State */
		case SOCK_LISTEN:strcat(mt, "Listen");break;/**< listen State */
		case SOCK_SYNSENT:strcat(mt, "Syn Sent");break;/**< connection State */
		case SOCK_SYNRECV:strcat(mt, "Syn Recv");break;/**< connection State */
		case SOCK_ESTABLISHED:strcat(mt, "Conectado");break;/**< success to connect */
		case SOCK_FIN_WAIT:strcat(mt, "Fin de espera");break;/**< closing State */
		case SOCK_CLOSING:strcat(mt, "Cerrando");break;/**< closing State */
		case SOCK_TIME_WAIT:strcat(mt, "Time Wait");break;/**< closing State */
		case SOCK_CLOSE_WAIT:strcat(mt, "Close Wait");break;/**< closing State */
		case SOCK_LAST_ACK:strcat(mt, "Last Ack");break;/**< closing State */
		default:strcat(mt, "default");break;
	}

	return ret;
}


int8_t socket(SOCKET sn, uint8_t protocol, uint16_t port, uint8_t flag)
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
      close(sn);

      setSn_MR(sn ,(protocol | (flag & 0xF0)));
      //IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);

      if (port != 0) {
    	  setSn_PORT(sn, port);
      } else {
         local_port++; // if don't set the source port, set local_port number.
         setSn_PORT(sn, local_port);
      }

      setSn_CR(sn, Sn_CR_OPEN);
	  while(getSn_CR(sn));

	  io_mode |= ((flag & SF_IO_NONBLOCK) << sn);  ; // BLOCK MODE = 0
	  is_sending &= ~(1<<sn);
	  remained_size[sn] = 0;
	  pack_info[sn] = PACK_COMPLETED;
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;

}


int8_t close(SOCKET sn)
{
	CHECK_SOCKNUM();

	setSn_CR( sn ,Sn_CR_CLOSE);
	/* wait to process the command... */
	while( getSn_CR(sn ) );
	/* clear all interrupt of the socket. */
	setSn_IR(sn, 0xFF);
	/* all clear */
	io_mode &= ~(1<<sn);
	//
	is_sending &= ~(1<<sn);
	remained_size[sn] = 0;
	pack_info[sn] = 0;
	while(getSn_SR(sn) != SOCK_CLOSED);
	return SOCK_OK;

}


int8_t listen(SOCKET sn)
{
	CHECK_SOCKNUM();
	CHECK_SOCKMODE(Sn_MR_TCP);
	CHECK_SOCKINIT();

	setSn_CR(sn, Sn_CR_LISTEN);
	while( getSn_CR(sn));

	while(getSn_SR(sn) != SOCK_LISTEN)
	{
		 close(sn);
		 return SOCKERR_SOCKCLOSED;
	}
	return SOCK_OK;
}


int8_t connect(SOCKET sn, uint8_t * addr, uint16_t port)
{
	CHECK_SOCKNUM();
	CHECK_SOCKMODE(Sn_MR_TCP);
	CHECK_SOCKINIT();

    if
        (
            ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)
        )
    {
      return SOCKERR_IPINVALID;
    }
    else
    {
		// set destination IP
		setSn_DIPR(sn, addr);
		// set destination port
		setSn_DPORT(sn, port);

		setSn_CR(sn , Sn_CR_CONNECT);
		/* wait for completion */
		while ( getSn_CR(sn) ) ;

		while(getSn_SR(sn) != SOCK_ESTABLISHED)
		{
			if (getSn_IR(sn) & Sn_IR_TIMEOUT)
			{
				setSn_IR(sn, Sn_IR_TIMEOUT);
				return SOCKERR_TIMEOUT;
			}

			if (getSn_SR(sn) == SOCK_CLOSED)
			{
				return SOCKERR_SOCKCLOSED;
			}
		}
    }

    return SOCK_OK;
}

/**
@brief   This function used for disconnect the socket and parameter is "sn" which represent the socket number
@return  1 for success else 0.
*/
int8_t disconnect(SOCKET sn)
{
/*
   setSn_CR(sn,Sn_CR_DISCON);
   while( getSn_CR(sn) );
*/
	setSn_CR(sn,Sn_CR_DISCON);
	/* wait to process the command... */
	while(getSn_CR(sn));
	is_sending &= ~(1<<sn);
	if(io_mode & (1<<sn)) return SOCK_BUSY;
	while(getSn_SR(sn) != SOCK_CLOSED)
	{
	   if(getSn_IR(sn) & Sn_IR_TIMEOUT)
	   {
		  close(sn);
		  return SOCKERR_TIMEOUT;
	   }
	}
	return SOCK_OK;

}


int32_t send(uint8_t sn, uint8_t * buf, uint16_t len)
{
   uint8_t tmp=0;
   uint16_t freesize=0;

   //T("len : %d",len);
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
      if( (io_mode & (1<<sn)) && (len > freesize) ){T("SOCK_BUSY 1"); return SOCK_BUSY;}
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
	 if((io_mode & (1<<sn)) && (recvsize == 0))return SOCK_BUSY;
	 if(recvsize != 0) break;
	};

	recv_data_processing(sn, buf, len);
	setSn_CR(sn,Sn_CR_RECV);
	while(getSn_CR(sn));

	return (int32_t)len;
}

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

