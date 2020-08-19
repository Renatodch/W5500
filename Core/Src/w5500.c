/*
 * w5500.c
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */


#include "main.h"

/*########################################################### SPI1 Interface ##################################################*/
void IINCHIP_WRITE( uint32_t addrbsb,  uint8_t data)
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

uint8_t IINCHIP_READ(uint32_t addrbsb)
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

uint16_t wiz_write_buf(uint32_t addrbsb,uint8_t* buf,uint16_t len)
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

uint16_t wiz_read_buf(uint32_t addrbsb, uint8_t* buf,uint16_t len)
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


/*##################################################### FUNCIONES GENERALES DEL CHIP##########################################################*/

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



/**********************************************
 * Shared Buffer Definition for LOOPBACK TEST *
 **********************************************/
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];


static int8_t checkPhyLink(void)
{
   return(getPHYCFGR() & 1); //0: Link off 1: Link on
}

void W5500_Init(void)
{
	uint8_t tmp_array[8]={0};
    uint8_t i;

    T("W5500_Reset();");
	W5500_Reset();
	T("W5500_ResetSoftware();");
	W5500_ResetSoftware();

    /* PHY link status check */
	while(!checkPhyLink());

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

    //set ping block mode
    /*Falta implementar*/

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


static uint8_t I_STATUS[MAX_SOCK_NUM]; //interrupt state?
static uint16_t SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16_t RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */

uint8_t getISR(uint8_t s)
{
  return I_STATUS[s];
}
void putISR(uint8_t s, uint8_t val)
{
   I_STATUS[s] = val;
}

uint16_t getIINCHIP_RxMAX(uint8_t s)
{
   return RSIZE[s];
}
uint16_t getIINCHIP_TxMAX(uint8_t s)
{
   return SSIZE[s];
}

/**
@brief  This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/
void iinchip_init(void)
{
  setMR( MR_RST );
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
void sysinit( uint8_t * tx_size, uint8_t * rx_size  )
{
	int i;
	int ssum,rsum;


	ssum = 0;
	rsum = 0;

	for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i]);
		IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i]);

		T("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
		T("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));

		SSIZE[i] = (int)(0);
		RSIZE[i] = (int)(0);

		if (ssum <= 16384)
		{
		  switch( tx_size[i] )
		  {
		  case 1:
			SSIZE[i] = (int)(1024);
			break;
		  case 2:
			SSIZE[i] = (int)(2048);
			break;
		  case 4:
			SSIZE[i] = (int)(4096);
			break;
		  case 8:
			SSIZE[i] = (int)(8192);
			break;
		  case 16:
			SSIZE[i] = (int)(16384);
		  break;
		  default :
			RSIZE[i] = (int)(2048);
			break;
		  }
		}

	   if (rsum <= 16384)
		{
			switch( rx_size[i] )
		  {
		  case 1:
			RSIZE[i] = (int)(1024);
			break;
		  case 2:
			RSIZE[i] = (int)(2048);
			break;
		  case 4:
			RSIZE[i] = (int)(4096);
			break;
		  case 8:
			RSIZE[i] = (int)(8192);
			break;
		  case 16:
			RSIZE[i] = (int)(16384);
			break;
		  default :
			RSIZE[i] = (int)(2048);
			break;
		  }
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

	  }
}


/**
@brief  This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)
*/
void clearIR(uint8_t mask)
{
  IINCHIP_WRITE(IR, ~mask | getIR() ); // must be setted 0x10.
}


/**************************************** SETTERS ******************************************/

//set gateway addr
void setGAR(uint8_t * addr ) /**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
{
    wiz_write_buf(GAR0, addr, 4);
}

//set subnet mask
void setSUBR(uint8_t * addr)
{
    wiz_write_buf(SUBR0, addr, 4);
}

//set mac addr
void setSHAR(uint8_t * addr)  /**< a pointer to a 6 -byte array responsible to set the MAC address. */
{
  wiz_write_buf(SHAR0, addr, 6);
}

//set ip addr
void setSIPR(uint8_t * addr)  /**< a pointer to a 4 -byte array responsible to set the Source IP address. */
{
    wiz_write_buf(SIPR0, addr, 4);
}

//set mode
void setMR(uint8_t val)
{
  IINCHIP_WRITE(MR,val);
}
/**
@brief  This function sets up Retransmission time.
*/
void setRTR(uint16_t timeout)
{
  IINCHIP_WRITE(RTR0,(uint8_t)((timeout & 0xff00) >> 8));
  IINCHIP_WRITE(RTR1,(uint8_t)(timeout & 0x00ff));
}

/**
@brief  This function set the number of Retransmission.
*/
void setRCR(uint8_t retry)
{
  IINCHIP_WRITE(RCR,retry);
}



/************************************ GETTERS ***********************************/
//get gateway addr
void getGAR(uint8_t * addr)
{
    wiz_read_buf(GAR0, addr, 4);
}
//get subnet mask
void getSUBR(uint8_t * addr)
{
    wiz_read_buf(SUBR0, addr, 4);
}
//get mac addr
void getSHAR(uint8_t * addr)
{
    wiz_read_buf(SHAR0, addr, 6);
}

//get ip
void getSIPR(uint8_t * addr)
{
    wiz_read_buf(SIPR0, addr, 4);
}
//get mode
uint8_t getMR( void )
{
   return IINCHIP_READ(MR);
}
//gets Interrupt register in common register.
uint8_t getIR( void )
{
   return IINCHIP_READ(IR);
}

uint8_t getPHYCFGR( void ){
	return IINCHIP_READ(PHYCFGR);
}

/*################################################################### Sockets interface #####################################################*/
/**
@brief  This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16_t Sn_MSSR)
{
  IINCHIP_WRITE( Sn_MSSR0(s), (uint8_t)((Sn_MSSR & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_MSSR1(s), (uint8_t)(Sn_MSSR & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8_t ttl)
{
   IINCHIP_WRITE( Sn_TTL(s) , ttl);
}
void setSn_CR(SOCKET sn, uint8_t cr){
		IINCHIP_WRITE(Sn_CR(sn), cr);
}
//dest ip
void setSn_DIPR(SOCKET sn, uint8_t * dipr){
	wiz_write_buf((uint32_t)Sn_DIPR(sn), dipr, 4);
}
//dest port
void setSn_DPORT(SOCKET sn, uint16_t dport) {
	IINCHIP_WRITE(Sn_DPORT(sn),   (uint8_t) (dport>>8));
	IINCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1), (uint8_t)  dport);
}
void setSn_IR(SOCKET sn, uint8_t ir){
	IINCHIP_WRITE(Sn_IR(sn), (ir & 0x1F));
}
void setSn_RX_RD(SOCKET sn, uint8_t rxrd) {
	IINCHIP_WRITE(Sn_RX_RD(sn),   (uint8_t)(rxrd>>8));
	IINCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1), (uint8_t) rxrd);
}
//mode
void setSn_MR(SOCKET sn, uint8_t mr){
	IINCHIP_WRITE(Sn_MR(sn),mr);
}
//port
void setSn_PORT(SOCKET sn, uint16_t port){
	IINCHIP_WRITE( Sn_PORT0(sn) ,(uint8_t)((port & 0xff00) >> 8));
	IINCHIP_WRITE( Sn_PORT1(sn) ,(uint8_t)(port & 0x00ff));
}

/*********** GETTERS******/
uint8_t getSn_TXBUF_SIZE(SOCKET sn){
	return IINCHIP_READ(Sn_TXBUF_SIZE(sn));
}
uint16_t getSn_TxMAX(SOCKET sn){
	return (getSn_TXBUF_SIZE(sn) <<10);
}

uint8_t getSn_CR(SOCKET sn){
	return IINCHIP_READ(Sn_CR(sn));
}

uint8_t getSn_RX_RD(SOCKET sn){
	return (IINCHIP_READ((Sn_RX_RD(sn)) << 8) + IINCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1)));
}
uint8_t getSn_MR(SOCKET sn){
	return IINCHIP_READ(Sn_MR(sn));
}
/*
void getSn_TxMAX(sn){
	(getSn_TXBUF_SIZE(sn) << 10);
}
*/

//socket interrupt status
uint8_t getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}

//socket status
uint8_t getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief  get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16_t getSn_TX_FSR(SOCKET s)
{
  uint16_t val=0,val1=0;
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
uint16_t getSn_RX_RSR(SOCKET s)
{
  uint16_t val=0,val1=0;
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
void send_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
  uint16_t ptr = 0;
  uint32_t addrbsb = 0;

  if(len == 0)
  {
//    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ( Sn_TX_WR0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));

  addrbsb = (uint32_t)(ptr<<8) + (s<<5) + 0x10;
  wiz_write_buf(addrbsb, data, len);

  ptr += len;
  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8_t)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_TX_WR1(s),(uint8_t)(ptr & 0x00ff));
}


/**
@brief  This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
  uint16_t ptr = 0;
  uint32_t addrbsb = 0;

  if(len == 0)
  {
//    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ( Sn_RX_RD0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s) );

  addrbsb = (uint32_t)(ptr<<8) + (s<<5) + 0x18;
  wiz_read_buf(addrbsb, data, len);
  ptr += len;

  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
}

/*This function is for ignore received packets*/
void recv_data_ignore(uint8_t sn, uint16_t len)
{
   uint16_t ptr = 0;
   ptr = getSn_RX_RD(sn);
   ptr += len;
   setSn_RX_RD(sn,ptr);
}


