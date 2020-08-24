/*
 * DHCP_Client.c
 *
 *  Created on: 19 ago. 2020
 *      Author: Renato
 */

#ifndef INC_DHCP_CLIENT_H_
#define INC_DHCP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"

/* DHCP state machine. */
#define STATE_DHCP_INIT          0        ///< Initialize
#define STATE_DHCP_DISCOVER      1        ///< send DISCOVER and wait OFFER
#define STATE_DHCP_REQUEST       2        ///< send REQEUST and wait ACK or NACK
#define STATE_DHCP_LEASED        3        ///< ReceiveD ACK and IP leased
#define STATE_DHCP_REREQUEST     4        ///< send REQUEST for maintaining leased IP
#define STATE_DHCP_RELEASE       5        ///< No use
#define STATE_DHCP_STOP          6        ///< Stop processing DHCP

#define DHCP_FLAGSBROADCAST      0x8000   ///< The broadcast value of flags in @ref RIP_MSG
#define DHCP_FLAGSUNICAST        0x0000   ///< The unicast   value of flags in @ref RIP_MSG

/* DHCP message OP code */
#define DHCP_BOOTREQUEST         1        ///< Request Message used in op of @ref RIP_MSG
#define DHCP_BOOTREPLY           2        ///< Reply Message used i op of @ref RIP_MSG

/* DHCP message type */
#define DHCP_DISCOVER            1        ///< DISCOVER message in OPT of @ref RIP_MSG
#define DHCP_OFFER               2        ///< OFFER message in OPT of @ref RIP_MSG
#define DHCP_REQUEST             3        ///< REQUEST message in OPT of @ref RIP_MSG
#define DHCP_DECLINE             4        ///< DECLINE message in OPT of @ref RIP_MSG
#define DHCP_ACK                 5        ///< ACK message in OPT of @ref RIP_MSG
#define DHCP_NAK                 6        ///< NACK message in OPT of @ref RIP_MSG
#define DHCP_RELEASE             7        ///< RELEASE message in OPT of @ref RIP_MSG. No use
#define DHCP_INFORM              8        ///< INFORM message in OPT of @ref RIP_MSG. No use

#define DHCP_HTYPE10MB           1        ///< Used in type of @ref RIP_MSG
#define DHCP_HTYPE100MB          2        ///< Used in type of @ref RIP_MSG

#define DHCP_HLENETHERNET        6        ///< Used in hlen of @ref RIP_MSG
#define DHCP_HOPS                0        ///< Used in hops of @ref RIP_MSG
#define DHCP_SECS                0        ///< Used in secs of @ref RIP_MSG

#define INFINITE_LEASETIME       0xffffffff	///< Infinite lease time

#define OPT_SIZE                 312               /// Max OPT size of @ref RIP_MSG
#define RIP_MSG_SIZE             (236+OPT_SIZE)    /// Max size of @ref RIP_MSG




/* Retry to processing DHCP */
#define	MAX_DHCP_RETRY          2        ///< Maximum retry count
#define	DHCP_WAIT_TIME          10       ///< Wait Time 10s


/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT      	67	      ///< DHCP server port number
#define DHCP_CLIENT_PORT         68	      ///< DHCP client port number


#define MAGIC_COOKIE             0x63825363  ///< You should not modify it number.

#define DCHP_HOST_NAME           "WIZnet\0"
#define DATA_BUF_SIZE   		2048

/*
 * @brief DHCP message format
 */
typedef struct {
	uint8_t  op;            ///< @ref DHCP_BOOTREQUEST or @ref DHCP_BOOTREPLY
	uint8_t  htype;         ///< @ref DHCP_HTYPE10MB or @ref DHCP_HTYPE100MB
	uint8_t  hlen;          ///< @ref DHCP_HLENETHERNET
	uint8_t  hops;          ///< @ref DHCP_HOPS
	uint32_t xid;           ///< @ref DHCP_XID  This increase one every DHCP transaction.
	uint16_t secs;          ///< @ref DHCP_SECS
	uint16_t flags;         ///< @ref DHCP_FLAGSBROADCAST or @ref DHCP_FLAGSUNICAST
	uint8_t  ciaddr[4];     ///< @ref Request IP to DHCP sever
	uint8_t  yiaddr[4];     ///< @ref Offered IP from DHCP server
	uint8_t  siaddr[4];     ///< No use
	uint8_t  giaddr[4];     ///< No use
	uint8_t  chaddr[16];    ///< DHCP client 6bytes MAC address. Others is filled to zero
	uint8_t  sname[64];     ///< No use
	uint8_t  file[128];     ///< No use
	uint8_t  OPT[OPT_SIZE]; ///< Option
} RIP_MSG;

typedef void (* dhcp_callback)(void);
typedef struct{
	uint8_t					ipDhcpFlag;
	int8_t					state;
	RIP_MSG* 				pDHCPMSG;
	uint32_t				xid;
	uint8_t					chAddr[6];
	uint8_t					ipServer[4];
	uint8_t					ipRealServer[4];
	uint8_t					allocatedSn[4];
	uint8_t					allocatedGw[4];
	uint8_t					allocatedDns[4];
	uint32_t				leaseTime;
	uint8_t					allocatedIp[4];
	uint8_t					oldAllocatedIp[4];
	uint16_t				msgId;
	uint16_t				clientPort;
	uint16_t				serverPort;
	SOCKET					socket;
	Timer					timeout;
	uint8_t					retry;
	uint8_t*				hostname;
	dhcp_callback			ip_assign;
	dhcp_callback			ip_update;
	dhcp_callback			ip_conflict;
}DhcpConnection;




/*
 * @brief
 * @details If you want to display debug & processing message, Define _DHCP_DEBUG_
 * @note    If defined, it depends on <stdio.h>
 */
//#define _DHCP_DEBUG_


/* Retry to processing DHCP */
#define	MAX_DHCP_RETRY          2        ///< Maximum retry count
#define	DHCP_WAIT_TIME          10       ///< Wait Time 10s


/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT      	67	      ///< DHCP server port number
#define DHCP_CLIENT_PORT         68	      ///< DHCP client port number


#define MAGIC_COOKIE             0x63825363  ///< You should not modify it number.


/*
 * @brief return value of @ref DHCP_run()
 */
enum
{
   DHCP_FAILED = 0,  ///< Processing Fail
   DHCP_RUNNING,     ///< Processing DHCP protocol
   DHCP_IP_ASSIGN,   ///< First Occupy IP from DHPC server      (if cbfunc == null, act as default default_ip_assign)
   DHCP_IP_CHANGED,  ///< Change IP address by new ip from DHCP (if cbfunc == null, act as default default_ip_update)
   DHCP_IP_LEASED,   ///< Stand by
   DHCP_STOPPED      ///< Stop processing DHCP protocol
};

/*
 * @brief DHCP client initialization (outside of the main loop)
 * @param s   - socket number
 * @param buf - buffer for processing DHCP message
 */
void DHCP_init(DhcpConnection *dhcpc, SOCKET s, dhcp_callback ip_assign, dhcp_callback ip_conflict);

void ip_assign_callback(void);
void ip_update_callback(void);
void ip_conflict_callback(void);

/*
 * @brief DHCP 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler
 */
void DHCP_time_handler(void);

/*
 * @brief Register call back function
 * @param ip_assign   - callback func when IP is assigned from DHCP server first
 * @param ip_update   - callback func when IP is changed
 * @param ip_conflict - callback func when the assigned IP is conflict with others.
 */
void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void));

/*
 * @brief DHCP client in the main loop
 * @return    The value is as the follow \n
 *            @ref DHCP_FAILED     \n
 *            @ref DHCP_RUNNING    \n
 *            @ref DHCP_IP_ASSIGN  \n
 *            @ref DHCP_IP_CHANGED \n
 * 			  @ref DHCP_IP_LEASED  \n
 *            @ref DHCP_STOPPED    \n
 *
 * @note This function is always called by you main task.
 */
uint8_t DHCP_run(DhcpConnection * dhcpc);

/*
 * @brief Stop DHCP processing
 * @note If you want to restart. call DHCP_init() and DHCP_run()
 */
void    DHCP_stop(DhcpConnection * dhcpc);

/* Get Network information assigned from DHCP server */
/*
 * @brief Get IP address
 * @param ip  - IP address to be returned
 */
void getIPfromDHCP(uint8_t* ip);
/*
 * @brief Get Gateway address
 * @param ip  - Gateway address to be returned
 */
void getGWfromDHCP(uint8_t* ip);
/*
 * @brief Get Subnet mask value
 * @param ip  - Subnet mask to be returned
 */
void getSNfromDHCP(uint8_t* ip);
/*
 * @brief Get DNS address
 * @param ip  - DNS address to be returned
 */
void getDNSfromDHCP(uint8_t* ip);

/*
 * @brief Get the leased time by DHCP sever
 * @return unit 1s
 */


void DhcpClient_Events(DhcpConnection *dhcpc);

/* send DISCOVER message to DHCP server */
void     send_DHCP_DISCOVER(DhcpConnection * dhcpc);

/* send REQEUST message to DHCP server */
void     send_DHCP_REQUEST(DhcpConnection * dhcpc);

/* send DECLINE message to DHCP server */
void     send_DHCP_DECLINE(DhcpConnection * dhcpc);

/* IP conflict check by sending ARP-request to leased IP and wait ARP-response. */
int8_t   check_DHCP_leasedIP(DhcpConnection * dhcpc);

/* check the timeout in DHCP process */
uint8_t  check_DHCP_timeout(DhcpConnection * dhcpc);

/* Initialize to timeout process.  */
void     reset_DHCP_timeout(void);

/* Parse message as OFFER and ACK and NACK from DHCP server.*/
int8_t   parseDHCPCMSG(void);

uint32_t getDHCPLeasetime(void);

#ifdef __cplusplus
}
#endif
#endif /* INC_DHCP_CLIENT_C_ */
