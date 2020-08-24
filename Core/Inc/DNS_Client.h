/*
 * DNS_Client.h
 *
 *  Created on: 19 ago. 2020
 *      Author: Renato
 */

#ifndef INC_DNS_CLIENT_H_
#define INC_DNS_CLIENT_H_

#include "main.h"

/***************************************************************************************************/
/************************************  DNS CLient *************************************************/
/***************************************************************************************************/

#define MAX_DOMAIN_NAME 	32 //16
#define	MAXCNAME	   		(MAX_DOMAIN_NAME + (MAX_DOMAIN_NAME>>1))	   /* Maximum amount of cname recursion */
#define	IPPORT_DOMAIN    	53///< DNS server port number
#define MAX_DNS_BUF_SIZE 	256
#define DNS_MSG_ID        	0x1122   ///< ID for DNS message. You can be modifyed it any number

#define	TYPE_A		1	   /* Host address */
#define	TYPE_NS		2	   /* Name server */
#define	TYPE_MD		3	   /* Mail destination (obsolete) */
#define	TYPE_MF		4	   /* Mail forwarder (obsolete) */
#define	TYPE_CNAME	5	   /* Canonical name */
#define	TYPE_SOA	   6	   /* Start of Authority */
#define	TYPE_MB		7	   /* Mailbox name (experimental) */
#define	TYPE_MG		8	   /* Mail group member (experimental) */
#define	TYPE_MR		9	   /* Mail rename name (experimental) */
#define	TYPE_NULL	10	   /* Null (experimental) */
#define	TYPE_WKS	   11	   /* Well-known sockets */
#define	TYPE_PTR	   12	   /* Pointer record */
#define	TYPE_HINFO	13	   /* Host information */
#define	TYPE_MINFO	14	   /* Mailbox information (experimental)*/
#define	TYPE_MX		15	   /* Mail exchanger */
#define	TYPE_TXT	   16	   /* Text strings */
#define	TYPE_ANY	   255	/* Matches any type */

/* Header for all domain messages */
struct dhdr
{
	uint16_t id;   /* Identification */
	uint8_t	qr;      /* Query/Response */
#define	QUERY    0
#define	RESPONSE 1
	uint8_t	opcode;
#define	IQUERY   1
	uint8_t	aa;      /* Authoratative answer */
	uint8_t	tc;      /* Truncation */
	uint8_t	rd;      /* Recursion desired */
	uint8_t	ra;      /* Recursion available */
	uint8_t	rcode;   /* Response code */
#define	NO_ERROR       0
#define	FORMAT_ERROR   1
#define	SERVER_FAIL    2
#define	NAME_ERROR     3
#define	NOT_IMPL       4
#define	REFUSED        5
	uint16_t qdcount;	/* Question count */
	uint16_t ancount;	/* Answer count */
	uint16_t nscount;	/* Authority (name server) count */
	uint16_t arcount;	/* Additional record count */
};
typedef struct
{
		char					domainName[MAX_DOMAIN_NAME];
		uint8_t					domainIp[4];
		uint8_t					ip2[4]; //DNS server 1
		uint8_t					ip1[4]; //DNS server 2
		uint16_t				msgId;
		uint8_t					buf[MAX_DNS_BUF_SIZE];
		uint16_t				port;
		uint8_t					socket;
		Timer					timeout;
		uint8_t					retry;
}DnsConnection;

void DNS_init(DnsConnection *dns, SOCKET s, char * domainName ,uint16_t id, uint8_t * ip1, uint8_t * ip2);
int8_t DNS_run(DnsConnection *dnsc, uint8_t * ipServer);
void DnsClient_Lookup(DnsConnection * dnsc);

#endif /* INC_DNS_CLIENT_H_ */
