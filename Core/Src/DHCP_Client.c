/*
 * DHCP_Client.c
 *
 *  Created on: 19 ago. 2020
 *      Author: Renato
 */

#include "main.h"


/* If you want to display debug & processing message, Define _DHCP_DEBUG_ in dhcp.h */


/*
 * @brief DHCP option and value (cf. RFC1533)
 */
enum
{
   padOption               = 0,
   subnetMask              = 1,
   timerOffset             = 2,
   routersOnSubnet         = 3,
   timeServer              = 4,
   nameServer              = 5,
   dns                     = 6,
   logServer               = 7,
   cookieServer            = 8,
   lprServer               = 9,
   impressServer           = 10,
   resourceLocationServer	= 11,
   hostName                = 12,
   bootFileSize            = 13,
   meritDumpFile           = 14,
   domainName              = 15,
   swapServer              = 16,
   rootPath                = 17,
   extentionsPath          = 18,
   IPforwarding            = 19,
   nonLocalSourceRouting   = 20,
   policyFilter            = 21,
   maxDgramReasmSize       = 22,
   defaultIPTTL            = 23,
   pathMTUagingTimeout     = 24,
   pathMTUplateauTable     = 25,
   ifMTU                   = 26,
   allSubnetsLocal         = 27,
   broadcastAddr           = 28,
   performMaskDiscovery    = 29,
   maskSupplier            = 30,
   performRouterDiscovery  = 31,
   routerSolicitationAddr  = 32,
   staticRoute             = 33,
   trailerEncapsulation    = 34,
   arpCacheTimeout         = 35,
   ethernetEncapsulation   = 36,
   tcpDefaultTTL           = 37,
   tcpKeepaliveInterval    = 38,
   tcpKeepaliveGarbage     = 39,
   nisDomainName           = 40,
   nisServers              = 41,
   ntpServers              = 42,
   vendorSpecificInfo      = 43,
   netBIOSnameServer       = 44,
   netBIOSdgramDistServer	= 45,
   netBIOSnodeType         = 46,
   netBIOSscope            = 47,
   xFontServer             = 48,
   xDisplayManager         = 49,
   dhcpRequestedIPaddr     = 50,
   dhcpIPaddrLeaseTime     = 51,
   dhcpOptionOverload      = 52,
   dhcpMessageType         = 53,
   dhcpServerIdentifier    = 54,
   dhcpParamRequest        = 55,
   dhcpMsg                 = 56,
   dhcpMaxMsgSize          = 57,
   dhcpT1value             = 58,
   dhcpT2value             = 59,
   dhcpClassIdentifier     = 60,
   dhcpClientIdentifier    = 61,
   endOption               = 255
};



uint8_t DHCP_SOCKET;                      // Socket number for DHCP

uint8_t DHCP_SIP[4];                      // DHCP Server IP address
uint8_t DHCP_REAL_SIP[4];                 // For extract my DHCP server in a few DHCP server

// Network information from DHCP Server
uint8_t OLD_allocated_ip[4]   = {0, };    // Previous IP address
uint8_t DHCP_allocated_ip[4]  = {0, };    // IP address from DHCP
uint8_t DHCP_allocated_gw[4]  = {0, };    // Gateway address from DHCP
uint8_t DHCP_allocated_sn[4]  = {0, };    // Subnet mask from DHCP
uint8_t DHCP_allocated_dns[4] = {0, };    // DNS address from DHCP


int8_t   dhcp_state        = STATE_DHCP_INIT;   // DHCP state
int8_t   dhcp_retry_count  = 0;

uint32_t dhcp_lease_time   			= INFINITE_LEASETIME;
volatile uint32_t dhcp_tick_1s      = 0;                 // unit 1 second
uint32_t dhcp_tick_next    			= DHCP_WAIT_TIME ;

uint32_t DHCP_XID;      // Any number

RIP_MSG* pDHCPMSG;      // Buffer pointer for DHCP processing

uint8_t HOST_NAME[] = DCHP_HOST_NAME;

uint8_t DHCP_CHADDR[6]; // DHCP Client MAC address.


/* make the common DHCP message */
void makeDHCPMSG(DhcpConnection * dhcpc)
{
	uint8_t  bk_mac[6];
	uint8_t* ptmp;
	uint8_t  i;
	getSHAR(bk_mac);
	dhcpc->pDHCPMSG->op      = DHCP_BOOTREQUEST;
	dhcpc->pDHCPMSG->htype   = DHCP_HTYPE10MB;
	dhcpc->pDHCPMSG->hlen    = DHCP_HLENETHERNET;
	dhcpc->pDHCPMSG->hops    = DHCP_HOPS;
	ptmp              = (uint8_t*)(&pDHCPMSG->xid);
	*(ptmp+0)         = (uint8_t)((dhcpc->xid & 0xFF000000) >> 24);
	*(ptmp+1)         = (uint8_t)((dhcpc->xid & 0x00FF0000) >> 16);
	*(ptmp+2)         = (uint8_t)((dhcpc->xid & 0x0000FF00) >>  8);
	*(ptmp+3)         = (uint8_t)((dhcpc->xid & 0x000000FF) >>  0);
	dhcpc->pDHCPMSG->secs    = DHCP_SECS;
	ptmp              = (uint8_t*)(&dhcpc->pDHCPMSG->flags);
	*(ptmp+0)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
	*(ptmp+1)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

	dhcpc->pDHCPMSG->ciaddr[0] = 0;
	dhcpc->pDHCPMSG->ciaddr[1] = 0;
	dhcpc->pDHCPMSG->ciaddr[2] = 0;
	dhcpc->pDHCPMSG->ciaddr[3] = 0;

	dhcpc->pDHCPMSG->yiaddr[0] = 0;
	dhcpc->pDHCPMSG->yiaddr[1] = 0;
	dhcpc->pDHCPMSG->yiaddr[2] = 0;
	dhcpc->pDHCPMSG->yiaddr[3] = 0;

	dhcpc->pDHCPMSG->siaddr[0] = 0;
	dhcpc->pDHCPMSG->siaddr[1] = 0;
	dhcpc->pDHCPMSG->siaddr[2] = 0;
	dhcpc->pDHCPMSG->siaddr[3] = 0;

	dhcpc->pDHCPMSG->giaddr[0] = 0;
	dhcpc->pDHCPMSG->giaddr[1] = 0;
	dhcpc->pDHCPMSG->giaddr[2] = 0;
	dhcpc->pDHCPMSG->giaddr[3] = 0;

	dhcpc->pDHCPMSG->chaddr[0] = dhcpc->chAddr[0];
	dhcpc->pDHCPMSG->chaddr[1] = dhcpc->chAddr[1];
	dhcpc->pDHCPMSG->chaddr[2] = dhcpc->chAddr[2];
	dhcpc->pDHCPMSG->chaddr[3] = dhcpc->chAddr[3];
	dhcpc->pDHCPMSG->chaddr[4] = dhcpc->chAddr[4];
	dhcpc->pDHCPMSG->chaddr[5] = dhcpc->chAddr[5];

	for (i = 6; i < 16; i++)  dhcpc->pDHCPMSG->chaddr[i] = 0;
	for (i = 0; i < 64; i++)  dhcpc->pDHCPMSG->sname[i]  = 0;
	for (i = 0; i < 128; i++) dhcpc->pDHCPMSG->file[i]   = 0;

	// MAGIC_COOKIE
	dhcpc->pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
	dhcpc->pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
	dhcpc->pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >>  8);
	dhcpc->pDHCPMSG->OPT[3] = (uint8_t) (MAGIC_COOKIE & 0x000000FF) >>  0;
}

/* SEND DHCP DISCOVER */
void send_DHCP_DISCOVER(DhcpConnection *dhcpc)
{
	uint16_t i;
	uint8_t ip[4];
	uint16_t k = 0;

   makeDHCPMSG(dhcpc);

   memset(dhcpc->ipServer,0x00,sizeof(dhcpc->ipServer));
   memset(dhcpc->ipRealServer,0x00,sizeof(dhcpc->ipRealServer));

   k = 4;     // because MAGIC_COOKIE already made by makeDHCPMSG()

	// Option Request Param
	dhcpc->pDHCPMSG->OPT[k++] = dhcpMessageType;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = DHCP_DISCOVER;

	// Client identifier
	dhcpc->pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	dhcpc->pDHCPMSG->OPT[k++] = 0x07;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[0];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[1];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[2];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[3];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[4];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[5];

	// host name
	dhcpc->pDHCPMSG->OPT[k++] = hostName;
	dhcpc->pDHCPMSG->OPT[k++] = 0;          // fill zero length of hostname
	for(i = 0 ; dhcpc->hostname[i] != 0; i++)
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->hostname[i];
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[3] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[3]);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[4] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[4]);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[5] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[5]);
	dhcpc->pDHCPMSG->OPT[k - (i+6+1)] = i+6; // length of hostname

	dhcpc->pDHCPMSG->OPT[k++] = dhcpParamRequest;
	dhcpc->pDHCPMSG->OPT[k++] = 0x06;	// length of request
	dhcpc->pDHCPMSG->OPT[k++] = subnetMask;
	dhcpc->pDHCPMSG->OPT[k++] = routersOnSubnet;
	dhcpc->pDHCPMSG->OPT[k++] = dns;
	dhcpc->pDHCPMSG->OPT[k++] = domainName;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpT1value;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpT2value;
	dhcpc->pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) dhcpc->pDHCPMSG->OPT[i] = 0;

	// send broadcasting packet
	ip[0] = 255;
	ip[1] = 255;
	ip[2] = 255;
	ip[3] = 255;

	T("> Send DHCP_DISCOVER\r\n");

	sendto(dhcpc->socket, (uint8_t *)dhcpc->pDHCPMSG, RIP_MSG_SIZE, ip, dhcpc->serverPort);
}

/* SEND DHCP REQUEST */
void send_DHCP_REQUEST(DhcpConnection * dhcpc)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;

   makeDHCPMSG(dhcpc);

   if(dhcpc->state == STATE_DHCP_LEASED || dhcpc->state == STATE_DHCP_REREQUEST)
   {
   	*((uint8_t*)(&dhcpc->pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
   	*((uint8_t*)(&dhcpc->pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
   	dhcpc->pDHCPMSG->ciaddr[0] = dhcpc->allocatedIp[0];
   	dhcpc->pDHCPMSG->ciaddr[1] = dhcpc->allocatedIp[1];
   	dhcpc->pDHCPMSG->ciaddr[2] = dhcpc->allocatedIp[2];
   	dhcpc->pDHCPMSG->ciaddr[3] = dhcpc->allocatedIp[3];
   	ip[0] = dhcpc->ipServer[0];
   	ip[1] = dhcpc->ipServer[1];
   	ip[2] = dhcpc->ipServer[2];
   	ip[3] = dhcpc->ipServer[3];
   }
   else
   {
   	ip[0] = 255;
   	ip[1] = 255;
   	ip[2] = 255;
   	ip[3] = 255;
   }

   k = 4;      // because MAGIC_COOKIE already made by makeDHCPMSG()

	// Option Request Param.
	dhcpc->pDHCPMSG->OPT[k++] = dhcpMessageType;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = DHCP_REQUEST;

	dhcpc->pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	dhcpc->pDHCPMSG->OPT[k++] = 0x07;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[0];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[1];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[2];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[3];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[4];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[5];

   if(ip[3] == 255)  // if(dchp_state == STATE_DHCP_LEASED || dchp_state == DHCP_REREQUEST_STATE)
   {
		dhcpc->pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
		dhcpc->pDHCPMSG->OPT[k++] = 0x04;
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[0];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[1];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[2];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[3];

		dhcpc->pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
		dhcpc->pDHCPMSG->OPT[k++] = 0x04;
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[0];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[1];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[2];
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[3];
	}

	// host name
	dhcpc->pDHCPMSG->OPT[k++] = hostName;
	dhcpc->pDHCPMSG->OPT[k++] = 0; // length of hostname
	for(i = 0 ; dhcpc->hostname[i] != 0; i++)
		dhcpc->pDHCPMSG->OPT[k++] = dhcpc->hostname[i];

	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[3] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[3]);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[4] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[4]);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[5] >> 4);
	dhcpc->pDHCPMSG->OPT[k++] = NibbleToHex(dhcpc->chAddr[5]);
	dhcpc->pDHCPMSG->OPT[k - (i+6+1)] = i+6; // length of hostname

	dhcpc->pDHCPMSG->OPT[k++] = dhcpParamRequest;
	dhcpc->pDHCPMSG->OPT[k++] = 0x08;
	dhcpc->pDHCPMSG->OPT[k++] = subnetMask;
	dhcpc->pDHCPMSG->OPT[k++] = routersOnSubnet;
	dhcpc->pDHCPMSG->OPT[k++] = dns;
	dhcpc->pDHCPMSG->OPT[k++] = domainName;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpT1value;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpT2value;
	dhcpc->pDHCPMSG->OPT[k++] = performRouterDiscovery;
	dhcpc->pDHCPMSG->OPT[k++] = staticRoute;
	dhcpc->pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) dhcpc->pDHCPMSG->OPT[i] = 0;


	T("> Send DHCP_REQUEST\r\n");

	sendto(dhcpc->socket, (uint8_t *)dhcpc->pDHCPMSG, RIP_MSG_SIZE, ip, dhcpc->serverPort);

}

/* SEND DHCP DHCPDECLINE */
void send_DHCP_DECLINE(DhcpConnection * dhcpc)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;

	makeDHCPMSG(dhcpc);

   k = 4;      // because MAGIC_COOKIE already made by makeDHCPMSG()

	*((uint8_t*)(&dhcpc->pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
	*((uint8_t*)(&dhcpc->pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);

	// Option Request Param.
	dhcpc->pDHCPMSG->OPT[k++] = dhcpMessageType;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = DHCP_DECLINE;

	dhcpc->pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	dhcpc->pDHCPMSG->OPT[k++] = 0x07;
	dhcpc->pDHCPMSG->OPT[k++] = 0x01;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[0];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[1];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[2];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[3];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[4];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->chAddr[5];

	dhcpc->pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
	dhcpc->pDHCPMSG->OPT[k++] = 0x04;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[0];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[1];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[2];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->allocatedIp[3];

	dhcpc->pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
	dhcpc->pDHCPMSG->OPT[k++] = 0x04;
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[0];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[1];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[2];
	dhcpc->pDHCPMSG->OPT[k++] = dhcpc->ipServer[3];

	dhcpc->pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) dhcpc->pDHCPMSG->OPT[i] = 0;

	//send broadcasting packet
	ip[0] = 0xFF;
	ip[1] = 0xFF;
	ip[2] = 0xFF;
	ip[3] = 0xFF;

	T("\r\n> Send DHCP_DECLINE\r\n");

	sendto(dhcpc->socket, (uint8_t *)dhcpc->pDHCPMSG, RIP_MSG_SIZE, ip, dhcpc->serverPort);
}

/* PARSE REPLY pDHCPMSG */
int8_t parseDHCPMSG(DhcpConnection * dhcpc)
{
	uint8_t svr_addr[6];
	uint16_t  svr_port;
	uint16_t len;

	uint8_t * p;
	uint8_t * e;
	uint8_t type = 0;
	uint8_t opt_len;

	if((len = getSn_RX_RSR(dhcpc->socket)) > 0)
	{
		len = recvfrom(dhcpc->socket, (uint8_t *)dhcpc->pDHCPMSG, len, svr_addr, &svr_port);

		T("DHCP message : %d.%d.%d.%d(%d) %d received. \r\n",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port, len);
	}
	else{
		return 0; //aun no llega nada
	}

	if (svr_port == dhcpc->serverPort) {
      // compare mac address
		if ( (dhcpc->pDHCPMSG->chaddr[0] != dhcpc->chAddr[0]) || (dhcpc->pDHCPMSG->chaddr[1] != dhcpc->chAddr[1]) ||
		     (dhcpc->pDHCPMSG->chaddr[2] != dhcpc->chAddr[2]) || (dhcpc->pDHCPMSG->chaddr[3] != dhcpc->chAddr[3]) ||
		     (dhcpc->pDHCPMSG->chaddr[4] != dhcpc->chAddr[4]) || (dhcpc->pDHCPMSG->chaddr[5] != dhcpc->chAddr[5])   )
		{
			T("No My DHCP Message. This message is ignored.\r\n");
			return 0;
		}
        //compare DHCP server ip address
        if((dhcpc->ipServer[0]!=0) || (dhcpc->ipServer[1]!=0) || (dhcpc->ipServer[2]!=0) || (dhcpc->ipServer[3]!=0))
        {
            if( ((svr_addr[0]!=dhcpc->ipServer[0])|| (svr_addr[1]!=dhcpc->ipServer[1])|| (svr_addr[2]!=dhcpc->ipServer[2])|| (svr_addr[3]!=dhcpc->ipServer[3])) &&
                ((svr_addr[0]!=dhcpc->ipRealServer[0])|| (svr_addr[1]!=dhcpc->ipRealServer[1])|| (svr_addr[2]!=dhcpc->ipRealServer[2])|| (svr_addr[3]!=dhcpc->ipRealServer[3]))  )
            {
                T("Another DHCP sever send a response message. This is ignored.\r\n");
                return 0;
            }
        }
		p = (uint8_t *)(&dhcpc->pDHCPMSG->op);
		p = p + 240;      // 240 = sizeof(RIP_MSG) + MAGIC_COOKIE size in RIP_MSG.opt - sizeof(RIP_MSG.opt)
		e = p + (len - 240);

		while ( p < e ) {

			switch ( *p ) {

   			case endOption :
   			   p = e;   // for break while(p < e)
   				break;
            case padOption :
   				p++;
   				break;
   			case dhcpMessageType :
   				p++;
   				p++;
   				type = *p++;
   				break;
   			case subnetMask :
   				p++;
   				p++;
   				dhcpc->allocatedSn[0] = *p++;
   				dhcpc->allocatedSn[1] = *p++;
   				dhcpc->allocatedSn[2] = *p++;
   				dhcpc->allocatedSn[3] = *p++;
   				break;
   			case routersOnSubnet :
   				p++;
   				opt_len = *p++;
   				dhcpc->allocatedGw[0] = *p++;
   				dhcpc->allocatedGw[1] = *p++;
   				dhcpc->allocatedGw[2] = *p++;
   				dhcpc->allocatedGw[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dns :
   				p++;
   				opt_len = *p++;
   				dhcpc->allocatedDns[0] = *p++;
   				dhcpc->allocatedDns[1] = *p++;
   				dhcpc->allocatedDns[2] = *p++;
   				dhcpc->allocatedDns[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dhcpIPaddrLeaseTime :
   				p++;
   				opt_len = *p++;
   				dhcpc->leaseTime  = *p++;
   				dhcpc->leaseTime  = (dhcpc->leaseTime << 8) + *p++;
   				dhcpc->leaseTime  = (dhcpc->leaseTime << 8) + *p++;
   				dhcpc->leaseTime  = (dhcpc->leaseTime << 8) + *p++;
            #ifdef _DHCP_DEBUG_
   				dhcpc->leaseTime = 10;
 				#endif
   				break;
   			case dhcpServerIdentifier :
   				p++;
   				opt_len = *p++;
   				dhcpc->ipServer[0] = *p++;
   				dhcpc->ipServer[1] = *p++;
   				dhcpc->ipServer[2] = *p++;
   				dhcpc->ipServer[3] = *p++;
   				dhcpc->ipRealServer[0]=svr_addr[0];
   				dhcpc->ipRealServer[1]=svr_addr[1];
   				dhcpc->ipRealServer[2]=svr_addr[2];
   				dhcpc->ipRealServer[3]=svr_addr[3];
   				break;
   			default :
   				p++;
   				opt_len = *p++;
   				p += opt_len;
   				break;
			} // switch
		} // while
	} // if
	return	type;
}

uint8_t DHCP_run(DhcpConnection * dhcpc)
{
	uint8_t  type;
	uint8_t  ret = DHCP_RUNNING;

	if(dhcpc->state == STATE_DHCP_STOP) return DHCP_STOPPED;

	if(getSn_SR(dhcpc->socket) != SOCK_UDP)
	   socket(dhcpc->socket, Sn_MR_UDP, dhcpc->clientPort, 0x00);

	type = parseDHCPMSG(dhcpc);

	switch ( dhcpc->state )
	{
	   case STATE_DHCP_INIT     :
			dhcpc->allocatedIp[0] = 0;
			dhcpc->allocatedIp[1] = 0;
			dhcpc->allocatedIp[2] = 0;
			dhcpc->allocatedIp[3] = 0;
			send_DHCP_DISCOVER(dhcpc);
			dhcpc->state = STATE_DHCP_DISCOVER;
   		break;
		case STATE_DHCP_DISCOVER :
			if (type == DHCP_OFFER)
			{
				T("> Receive DHCP_OFFER\r\n");
				dhcpc->allocatedIp[0] = dhcpc->pDHCPMSG->yiaddr[0];
				dhcpc->allocatedIp[1] = dhcpc->pDHCPMSG->yiaddr[1];
				dhcpc->allocatedIp[2] = dhcpc->pDHCPMSG->yiaddr[2];
				dhcpc->allocatedIp[3] = dhcpc->pDHCPMSG->yiaddr[3];
				send_DHCP_REQUEST(dhcpc);
				dhcpc->state = STATE_DHCP_REQUEST;
			} else ret = check_DHCP_timeout(dhcpc);
         break;

		case STATE_DHCP_REQUEST :
			if (type == DHCP_ACK) {

				T("> Receive DHCP_ACK\r\n");

				if (check_DHCP_leasedIP(dhcpc)) {
					// Network info assignment from DHCP
					dhcpc->ip_assign();
					reset_DHCP_timeout();

					dhcpc->state = STATE_DHCP_LEASED;
				} else {
					// IP address conflict occurred
					reset_DHCP_timeout();
					dhcpc->ip_conflict();
					dhcpc->state = STATE_DHCP_INIT;
				}
			}else if (type == DHCP_NAK) {

				T("> Receive DHCP_NACK\r\n");
				reset_DHCP_timeout();
				dhcpc->state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout(dhcpc);
		break;

		case STATE_DHCP_LEASED :
		   ret = DHCP_IP_LEASED;
			if ((dhcp_lease_time != INFINITE_LEASETIME) && ((dhcp_lease_time/2) < dhcp_tick_1s)) {

 				T("> Maintains the IP address \r\n");
				type = 0;
				dhcpc->oldAllocatedIp[0] = dhcpc->allocatedIp[0];
				dhcpc->oldAllocatedIp[1] = dhcpc->allocatedIp[1];
				dhcpc->oldAllocatedIp[2] = dhcpc->allocatedIp[2];
				dhcpc->oldAllocatedIp[3] = dhcpc->allocatedIp[3];

				dhcpc->xid++;

				send_DHCP_REQUEST(dhcpc);

				reset_DHCP_timeout();

				dhcpc->state = STATE_DHCP_REREQUEST;
			}
		break;

		case STATE_DHCP_REREQUEST :
		   ret = DHCP_IP_LEASED;
			if (type == DHCP_ACK)
			{
				dhcpc->retry = 0;
				if (dhcpc->oldAllocatedIp[0] != dhcpc->allocatedIp[0] ||
					dhcpc->oldAllocatedIp[1] != dhcpc->allocatedIp[1] ||
					dhcpc->oldAllocatedIp[2] != dhcpc->allocatedIp[2] ||
					dhcpc->oldAllocatedIp[3] != dhcpc->allocatedIp[3])
				{
					ret = DHCP_IP_CHANGED;
					dhcpc->ip_update();

                  T(">IP changed.\r\n");
				}

				else T(">IP is continued.\r\n");

				reset_DHCP_timeout();
				dhcpc->state = STATE_DHCP_LEASED;
			} else if (type == DHCP_NAK) {

				T("> Receive DHCP_NACK, Failed to maintain ip\r\n");

				reset_DHCP_timeout();

				dhcpc->state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout(dhcpc);
	   	break;
		default :
   		break;
	}

	return ret;
}

void DHCP_stop(DhcpConnection * dhcpc)
{
   close(dhcpc->socket);
   dhcpc->state = STATE_DHCP_STOP;
}

uint8_t check_DHCP_timeout(DhcpConnection * dhcpc)
{
	uint8_t ret = DHCP_RUNNING;

	if (dhcp_retry_count < MAX_DHCP_RETRY) {
		if (dhcp_tick_next < dhcp_tick_1s) {

			switch ( dhcpc->state ) {
				case STATE_DHCP_DISCOVER :
//					printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
					send_DHCP_DISCOVER(dhcpc);
				break;

				case STATE_DHCP_REQUEST :
//					printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");

					send_DHCP_REQUEST(dhcpc);
				break;

				case STATE_DHCP_REREQUEST :
//					printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");

					send_DHCP_REQUEST(dhcpc);
				break;

				default :
				break;
			}

			dhcp_tick_1s = 0;
			dhcp_tick_next = dhcp_tick_1s + DHCP_WAIT_TIME;
			dhcp_retry_count++;
		}
	} else { // timeout occurred

		switch(dhcp_state) {
			case STATE_DHCP_DISCOVER:
				dhcp_state = STATE_DHCP_INIT;
				ret = DHCP_FAILED;
				break;
			case STATE_DHCP_REQUEST:
			case STATE_DHCP_REREQUEST:
				send_DHCP_DISCOVER(dhcpc);
				dhcp_state = STATE_DHCP_DISCOVER;
				break;
			default :
				break;
		}
		reset_DHCP_timeout();
	}
	return ret;
}

int8_t check_DHCP_leasedIP(DhcpConnection * dhcpc)
{
	uint8_t tmp;
	int32_t ret;

	//WIZchip RCR value changed for ARP Timeout count control
	tmp = getRCR();
	setRCR(0x03);

	// IP conflict detection : ARP request - ARP reply
	// Broadcasting ARP Request for check the IP conflict using UDP sendto() function
	ret = sendto(dhcpc->socket, (uint8_t *)"CHECK_IP_CONFLICT", 17, dhcpc->allocatedIp, 5000);

	// RCR value restore
	setRCR(tmp);

	if(ret == SOCKERR_TIMEOUT) {
		// UDP send Timeout occurred : allocated IP address is unique, DHCP Success

	T("\r\n> Check leased IP - OK\r\n");


		return 1;
	} else {
		// Received ARP reply or etc : IP address conflict occur, DHCP Failed
		send_DHCP_DECLINE(dhcpc);

		ret = dhcp_tick_1s;
		while((dhcp_tick_1s - ret) < 2) ;   // wait for 1s over; wait to complete to send DECLINE message;

		return 0;
	}
}


/* Reset the DHCP timeout count and retry count. */
void reset_DHCP_timeout(void)
{
	dhcp_tick_1s = 0;
	dhcp_tick_next = DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}

void DHCP_time_handler(void)
{
	dhcp_tick_1s++;
}

void getIPfromDHCP(uint8_t* ip)
{
	ip[0] = dhcpc.allocatedIp[0];
	ip[1] = dhcpc.allocatedIp[1];
	ip[2] = dhcpc.allocatedIp[2];
	ip[3] = dhcpc.allocatedIp[3];
}

void getGWfromDHCP(uint8_t* ip)
{
	ip[0] =dhcpc.allocatedGw[0];
	ip[1] =dhcpc.allocatedGw[1];
	ip[2] =dhcpc.allocatedGw[2];
	ip[3] =dhcpc.allocatedGw[3];
}

void getSNfromDHCP(uint8_t* ip)
{
   ip[0] = dhcpc.allocatedSn[0];
   ip[1] = dhcpc.allocatedSn[1];
   ip[2] = dhcpc.allocatedSn[2];
   ip[3] = dhcpc.allocatedSn[3];
}

void getDNSfromDHCP(uint8_t* ip)
{
   ip[0] = dhcpc.allocatedDns[0];
   ip[1] = dhcpc.allocatedDns[1];
   ip[2] = dhcpc.allocatedDns[2];
   ip[3] = dhcpc.allocatedDns[3];
}

uint32_t getDHCPLeasetime(void)
{
	return dhcp_lease_time;
}


/* The default handler of ip assign first */
void ip_assign_callback(void){
	uint8_t tmp_array[6] = {0};
	/*assign*/
   getIPfromDHCP(w5500_Network.IpDevice);
   getGWfromDHCP(w5500_Network.Gw);
   getSNfromDHCP(w5500_Network.Sub);
   getDNSfromDHCP(w5500_Network.DNS_1);

   setSIPR(w5500_Network.IpDevice);
   setSUBR(w5500_Network.Sub);
   setGAR(w5500_Network.Gw);

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

   T("DHCP LEASED TIME : %ld Sec.\r\n", getDHCPLeasetime());
}
void ip_update_callback(void)
{
	/* WIZchip Software Reset */
   setMR(MR_RST);
   getMR(); // for delay
   ip_assign_callback();
   setSHAR(dhcpc.chAddr);
}

void ip_conflict_callback(void){
	// WIZchip Software Reset
	setMR(MR_RST);
	getMR(); // for delay
	setSHAR(dhcpc.chAddr);
	T("CONFLICT IP from DHCP\r\n");
	while(1); // this example is halt.
}

uint8_t	buf[DATA_BUF_SIZE];

void DHCP_init(DhcpConnection *dhcpc, SOCKET s, dhcp_callback ip_assign, dhcp_callback ip_conflict)
{
	uint8_t zeroip[4] = {0,0,0,0};

	/*Mac should be established*/
	getSHAR(dhcpc->chAddr);

	dhcpc->socket = s; // SOCK_DHCP
	dhcpc->pDHCPMSG = (RIP_MSG*)buf;
	dhcpc->clientPort = DHCP_CLIENT_PORT;
	dhcpc->serverPort = DHCP_SERVER_PORT;
	dhcpc->ip_assign = ip_assign;
	dhcpc->ip_update = ip_assign;
	dhcpc->ip_conflict = ip_conflict;
	dhcpc->xid = 0x12345678;
	{
		dhcpc->xid += dhcpc->chAddr[3];
		dhcpc->xid += dhcpc->chAddr[4];
		dhcpc->xid += dhcpc->chAddr[5];
		dhcpc->xid += (dhcpc->chAddr[3] ^ dhcpc->chAddr[4] ^ dhcpc->chAddr[5]);
	}
	// WIZchip Netinfo Clear
	setSIPR(zeroip); //borra ip asignada
	setGAR(zeroip);	 //borra gateway

	dhcpc->retry = 0;
	//reset_DHCP_timeout();
	Timer_Init(&dhcpc->timeout, 5000);

	dhcpc->state = STATE_DHCP_INIT;
}

void DhcpClient_Events(DhcpConnection *dhcpc)
{
	dhcpc->ipDhcpFlag = DHCP_run(dhcpc);

	switch(dhcpc->ipDhcpFlag)
	{
		case DHCP_IP_ASSIGN:
			T("DHCP_IP_ASSIGN");


			break;
		case DHCP_IP_CHANGED:
			T("DHCP_IP_CHANGED");
			/* If this block empty, act with default_ip_assign & default_ip_update */
			//
			// This example calls my_ip_assign in the two case.
			//
			// Add to ...
			//
			dhcpc->ip_update();
			break;
		case DHCP_IP_LEASED:
			//T("DHCP_IP_LEASED entry");
			//
			// TO DO YOUR NETWORK APPs.
			//
			break;
		case DHCP_FAILED:

			/* ===== Example pseudo code =====  */
			// The below code can be replaced your code or omitted.
			// if omitted, retry to process DHCP
			dhcpc->retry++;
			if(dhcpc->retry > 3)
			{
				T(">> DHCP %d Failed\r\n", dhcpc->retry);
				dhcpc->retry = 0;
				DHCP_stop(dhcpc);      // if restart, recall DHCP_init()
				//network_init();   // apply the default static network and print out netinfo to serial
			}
			break;
		default:
			break;
	}
}
