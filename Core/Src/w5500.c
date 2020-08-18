/*
 * w5500.c
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */


#include "main.h"


void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	uint8_t cmd[4] = {0};

	cmd[0] = ((addrbsb & 0x00FF0000)>>16);// Address byte 1
	cmd[1] = ((addrbsb & 0x0000FF00)>> 8);// Address byte 2
	cmd[2] = ((addrbsb & 0x000000F8) + 4);// Data write command and Write data length 1
	cmd[3] = data;                    // Data write (write 1byte data)

	//__ASM volatile ("cpsid f" : : : "memory");

   cs_low();
   SPI1_Send_IT(cmd, sizeof(cmd));
   cs_high();

   //__ASM volatile ("cpsie f" : : : "memory");

}

uint8 IINCHIP_READ(uint32 addrbsb)
{
	uint8_t cmd[3] = {0};
	uint8_t buf = 0;

	cmd[0] = ((addrbsb & 0x00FF0000)>>16);// Address byte 1
	cmd[1] = ((addrbsb & 0x0000FF00)>> 8);// Address byte 2
	cmd[2] = (addrbsb & 0x000000F8);	 	 // Data read command and Read data length 1
	//cmd[3] = 0x00;
	//__ASM volatile ("cpsid f" : : : "memory");
	cs_low();
	SPI1_Send_IT(cmd, sizeof(cmd));
	SPI1_Receive_IT(&buf, sizeof(buf));

	//SPI1_SendReceive_IT(cmd, buf,  sizeof(buf));
	cs_high();
   //__ASM volatile ("cpsie f" : : : "memory");

   return buf;
}

uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint8_t cmd[3] = {0};

   if(len == 0) T("ERROR length = 0\r\n");


   cmd[0] = ((addrbsb & 0x00FF0000)>>16);// Address byte 1
   cmd[1] = ((addrbsb & 0x0000FF00)>> 8);// Address byte 2
   cmd[2] = ((addrbsb & 0x000000F8) + 4);    // Data write command and Write data length 1

   //__ASM volatile ("cpsid f" : : : "memory");
   cs_low();
   SPI1_Send_IT(cmd,sizeof(cmd));
   SPI1_Send_IT(buf,len);
   cs_high();
   //__ASM volatile ("cpsie f" : : : "memory");	// Interrupt TcpClient_Trace Routine Enable

   return len;
}

uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint8_t cmd[3]={0};

  if(len == 0) T("ERROR length = 0\r\n");

  cmd[0] = ( (addrbsb & 0x00FF0000)>>16);// Address byte 1
  cmd[1] = ( (addrbsb & 0x0000FF00)>> 8);// Address byte 2
  cmd[2] = ( (addrbsb & 0x000000F8));    // Data write command and Write data length 1

  //__ASM volatile ("cpsid f" : : : "memory");
  cs_low();

  SPI1_Send_IT(cmd,sizeof(cmd));
  SPI1_Receive_IT(buf, len);
  //SPI1_SendReceive_IT(cmd,buf,sizeof(cmd)+len);

  cs_high();
  //__ASM volatile ("cpsie f" : : : "memory");	   // Interrupt TcpClient_Trace Routine Enable

  return len;
}
/*###############################################################################################################*/





void W5500_Reset(void){
	HAL_Delay(100);
	HAL_GPIO_WritePin(nPwrDN_GPIO_Port, nPwrDN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(nPwrDN_GPIO_Port, nPwrDN_Pin, GPIO_PIN_SET);
	HAL_Delay(3500);
}

void W5500_ResetSoftware(void){
	unsigned char mr;
	mr = getMR();
	//T("mr: %d",mr);
	setMR(mr | MR_RST);
	HAL_Delay(100);
	//mr = getMR();
	//T("mr: %d",mr);
}

void W5500_Init(void)
{
	uint8_t tmp_array[8]={0};
    uint8 i;

    W5500_Init();

    T("W5500_Reset();");
	W5500_Reset();
	T("W5500_ResetSoftware();");
	W5500_ResetSoftware();

    // MAC ADDRESS
    for (i = 0 ; i < 6; i++) Config_Msg.Mac[i] = MAC[i];
    // Local IpDevice ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.IpDevice[i] = IpDevice[i];
    // GateWay ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.Gw[i] = GateWay[i];
    // Subnet Mask ADDRESS
    for (i = 0 ; i < 4; i++) Config_Msg.Sub[i] = SubNet[i];

    //set mac
    setSHAR(Config_Msg.Mac);
    //set subnet mask
    setSUBR(Config_Msg.Sub);
    //set gateway
    setGAR(Config_Msg.Gw);
    //set ip
    setSIPR(Config_Msg.IpDevice);

    //set DHCP
    Config_Msg.DHCP = Enable_DHCP;

    //Destination Ip address for TCP Client
    for (i = 0 ; i < 4; i++) Chconfig_Type_Def.destip[i] = IpServer[i];
    Chconfig_Type_Def.port = PortServer;

    //set retry time-value: retransmission timeout = 200 ms
    setRTR(2000);
    //set retry count: 5
    setRCR(5);

    //Inicializa tamaño memoria Tx & Rx
    sysinit(txsize, rxsize);

    T("\r\n----------------------------------------- \r\n");
    T("W5500E01-M3                       \r\n");
    T("Network Configuration Information \r\n");
    T("----------------------------------------- ");

    getSHAR(tmp_array);
    T("\r\nMAC : %.2X.%.2X.%.2X.%.2X.%.2X.%.2X", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3],tmp_array[4],tmp_array[5]);

    memset(tmp_array,0x00,sizeof(tmp_array));
    getSIPR (tmp_array);
    T("\r\nIP : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

    memset(tmp_array,0x00,sizeof(tmp_array));
    getSUBR(tmp_array);
    T("\r\nSN : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

    getGAR(tmp_array);
    T("\r\nGW : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);
}


#ifdef __DEF_IINCHIP_PPP__
   #include "md5.h"
#endif

static uint8 I_STATUS[MAX_SOCK_NUM];
static uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */

uint8 getISR(uint8 s)
{
  return I_STATUS[s];
}
void putISR(uint8 s, uint8 val)
{
   I_STATUS[s] = val;
}

uint16 getIINCHIP_RxMAX(uint8 s)
{
   return RSIZE[s];
}
uint16 getIINCHIP_TxMAX(uint8 s)
{
   return SSIZE[s];
}



/**
@brief  This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/
void iinchip_init(void)
{
  setMR( MR_RST );
#ifdef __DEF_IINCHIP_DBG__
  printf("MR value is %02x \r\n",IINCHIP_READ_COMMON(MR));
#endif
}

/**
@brief  This function set the transmit & receive buffer size as per the channels is used
Note for TMSR and RMSR bits are as follows\n
bit 1-0 : memory size of channel #0 \n
bit 3-2 : memory size of channel #1 \n
bit 5-4 : memory size of channel #2 \n
bit 7-6 : memory size of channel #3 \n
bit 9-8 : memory size of channel #4 \n
bit 11-10 : memory size of channel #5 \n
bit 12-12 : memory size of channel #6 \n
bit 15-14 : memory size of channel #7 \n
Maximum memory size for Tx, Rx in the W5500 is 16K Bytes,\n
In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
and to data transmission and receiption from non-allocated channel may cause some problems.\n
If the 16KBytes memory is already  assigned to centain channel, \n
other 3 channels couldn't be used, for there's no available memory.\n
If two 4KBytes memory are assigned to two each channels, \n
other 2 channels couldn't be used, for there's no available memory.\n
*/
void sysinit( uint8 * tx_size, uint8 * rx_size  )
{
  int16 i;
  int16 ssum,rsum;
#ifdef __DEF_IINCHIP_DBG__
  printf("sysinit()\r\n");
#endif
  ssum = 0;
  rsum = 0;

  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
  {
          IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i]);
          IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i]);

#ifdef __DEF_IINCHIP_DBG__
         printf("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
         printf("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
#endif
    SSIZE[i] = (int16)(0);
    RSIZE[i] = (int16)(0);


    if (ssum <= 16384)
    {
         switch( tx_size[i] )
      {
      case 1:
        SSIZE[i] = (int16)(1024);
        break;
      case 2:
        SSIZE[i] = (int16)(2048);
        break;
      case 4:
        SSIZE[i] = (int16)(4096);
        break;
      case 8:
        SSIZE[i] = (int16)(8192);
        break;
      case 16:
        SSIZE[i] = (int16)(16384);
      break;
      default :
        RSIZE[i] = (int16)(2048);
        break;
      }
    }

   if (rsum <= 16384)
    {
         switch( rx_size[i] )
      {
      case 1:
        RSIZE[i] = (int16)(1024);
        break;
      case 2:
        RSIZE[i] = (int16)(2048);
        break;
      case 4:
        RSIZE[i] = (int16)(4096);
        break;
      case 8:
        RSIZE[i] = (int16)(8192);
        break;
      case 16:
        RSIZE[i] = (int16)(16384);
        break;
      default :
        RSIZE[i] = (int16)(2048);
        break;
      }
    }
    ssum += SSIZE[i];
    rsum += RSIZE[i];

  }
}


/*###################################### SETTERS ############################################*/

//set gateway addr
void setGAR(uint8 * addr ) /**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
{
    wiz_write_buf(GAR0, addr, 4);
}

//set subnet mask
void setSUBR(uint8 * addr)
{
    wiz_write_buf(SUBR0, addr, 4);
}

//set mac addr
void setSHAR(uint8 * addr)  /**< a pointer to a 6 -byte array responsible to set the MAC address. */

{
  wiz_write_buf(SHAR0, addr, 6);
}

//set ip addr
void setSIPR(uint8 * addr)  /**< a pointer to a 4 -byte array responsible to set the Source IP address. */
{
    wiz_write_buf(SIPR0, addr, 4);
}

//set mode
void setMR(uint8 val)
{
  IINCHIP_WRITE(MR,val);
}

/************************************ GETTERS ***********************************/
//get gateway addr
void getGAR(uint8 * addr)
{
    wiz_read_buf(GAR0, addr, 4);
}
//get subnet mask
void getSUBR(uint8 * addr)
{
    wiz_read_buf(SUBR0, addr, 4);
}
//get mac addr
void getSHAR(uint8 * addr)
{
    wiz_read_buf(SHAR0, addr, 6);
}

//get ip
void getSIPR(uint8 * addr)
{
    wiz_read_buf(SIPR0, addr, 4);
}
//get mode
uint8 getMR( void )
{
   return IINCHIP_READ(MR);
}
//gets Interrupt register in common register.
uint8 getIR( void )
{
   return IINCHIP_READ(IR);
}

/**
@brief  This function sets up Retransmission time.

If there is no response from the peer or delay in response then retransmission
will be there as per RTR (Retry Time-value Register)setting
*/
void setRTR(uint16 timeout)
{
  IINCHIP_WRITE(RTR0,(uint8)((timeout & 0xff00) >> 8));
  IINCHIP_WRITE(RTR1,(uint8)(timeout & 0x00ff));
}

/**
@brief  This function set the number of Retransmission.

If there is no response from the peer or delay in response then recorded time
as per RTR & RCR register setting then time out will occur.
*/
void setRCR(uint8 retry)
{
  IINCHIP_WRITE(RCR,retry);
}

/**
@brief  This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void clearIR(uint8 mask)
{
  IINCHIP_WRITE(IR, ~mask | getIR() ); // must be setted 0x10.
}

/**
@brief  This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16 Sn_MSSR)
{
  IINCHIP_WRITE( Sn_MSSR0(s), (uint8)((Sn_MSSR & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_MSSR1(s), (uint8)(Sn_MSSR & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8 ttl)
{
   IINCHIP_WRITE( Sn_TTL(s) , ttl);
}



//socket interrupt status
uint8 getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}

//socket status
uint8 getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief  get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_TX_FSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
      if (val1 != 0)
    {
        val = IINCHIP_READ(Sn_TX_FSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
    }
  } while (val != val1);
   return val;
}


/**
@brief   get socket RX recv buf size

This gives size of received data in receive buffer.
*/
uint16 getSn_RX_RSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_RX_RSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
    if(val1 != 0)
    {
        val = IINCHIP_READ(Sn_RX_RSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
    }
  } while (val != val1);
   return val;
}


/**
@brief   This function is being called by send() and sendto() function also.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr = 0;
  uint32 addrbsb = 0;

  if(len == 0)
  {
//    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }


  ptr = IINCHIP_READ( Sn_TX_WR0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
  wiz_write_buf(addrbsb, data, len);

  ptr += len;
  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));
}


/**
@brief  This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr = 0;
  uint32 addrbsb = 0;

  if(len == 0)
  {
//    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ( Sn_RX_RD0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s) );

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
  wiz_read_buf(addrbsb, data, len);
  ptr += len;

  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
}




