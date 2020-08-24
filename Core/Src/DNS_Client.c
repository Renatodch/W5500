/*
 * DNS_Client.c
 *
 *  Created on: 19 ago. 2020
 *      Author: Renato
 */

#include "main.h"

/*##############################################################################################################*/
/*############################################ DNS CLIENT ########################################################*/



/*
 *              CONVERT A DOMAIN NAME TO THE HUMAN-READABLE FORM
 *
 * Description : This function converts a compressed domain name to the human-readable form
 * Arguments   : msg        - is a pointer to the reply message
 *               compressed - is a pointer to the domain name in reply message.
 *               buf        - is a pointer to the buffer for the human-readable form name.
 *               len        - is the MAX. size of buffer.
 * Returns     : the length of compressed message
 */
int parse_name(uint8_t * msg, uint8_t * compressed, char * buf, int16_t len)
{
	uint16_t slen;		/* Length of current segment */
	uint8_t * cp;
	int clen = 0;		/* Total length of compressed name */
	int indirect = 0;	/* Set if indirection encountered */
	int nseg = 0;		/* Total number of segments in name */

	cp = compressed;

	for (;;)
	{
		slen = *cp++;	/* Length of this segment */

		if (!indirect) clen++;

		if ((slen & 0xc0) == 0xc0)
		{
			if (!indirect)
				clen++;
			indirect = 1;
			/* Follow indirection */
			cp = &msg[((slen & 0x3f)<<8) + *cp];
			slen = *cp++;
		}

		if (slen == 0)	/* zero length == all done */
			break;

		len -= slen + 1;

		if (len < 0) return -1;

		if (!indirect) clen += slen;

		while (slen-- != 0) *buf++ = (char)*cp++;
		*buf++ = '.';
		nseg++;
	}

	if (nseg == 0)
	{
		/* Root name; represent as single dot */
		*buf++ = '.';
		len--;
	}

	*buf++ = '\0';
	len--;

	return clen;	/* Length of compressed message */
}

/*
 *              PARSE QUESTION SECTION
 *
 * Description : This function parses the qeustion record of the reply message.
 * Arguments   : msg - is a pointer to the reply message
 *               cp  - is a pointer to the qeustion record.
 * Returns     : a pointer the to next record.
 */
uint8_t * dns_question(uint8_t * msg, uint8_t * cp)
{
	int len;
	char name[MAXCNAME];

	len = parse_name(msg, cp, name, MAXCNAME);


	if (len == -1) return 0;

	cp += len;
	cp += 2;		/* type */
	cp += 2;		/* class */

	return cp;
}
/*
 *              PARSE ANSER SECTION
 *
 * Description : This function parses the answer record of the reply message.
 * Arguments   : msg - is a pointer to the reply message
 *               cp  - is a pointer to the answer record.
 * Returns     : a pointer the to next record.
 */
uint8_t * dns_answer(uint8_t * msg, uint8_t * cp, uint8_t * ip_from_dns)
{
	int len, type;
	char name[MAXCNAME];

	len = parse_name(msg, cp, name, MAXCNAME);

	if (len == -1) return 0;

	cp += len;
	type = Bytes_ToUint16(cp);
	cp += 2;		/* type */
	cp += 2;		/* class */
	cp += 4;		/* ttl */
	cp += 2;		/* len */


	switch (type)
	{
		case TYPE_A:
			/* Just read the address directly into the structure */
			ip_from_dns[0] = *cp++;
			ip_from_dns[1] = *cp++;
			ip_from_dns[2] = *cp++;
			ip_from_dns[3] = *cp++;
			break;
		case TYPE_CNAME:
		case TYPE_MB:
		case TYPE_MG:
		case TYPE_MR:
		case TYPE_NS:
		case TYPE_PTR:
			/* These types all consist of a single domain name */
			/* convert it to ascii format */
			len = parse_name(msg, cp, name, MAXCNAME);
			if (len == -1) return 0;

			cp += len;
			break;
		case TYPE_HINFO:
			len = *cp++;
			cp += len;

			len = *cp++;
			cp += len;
			break;
		case TYPE_MX:
			cp += 2;
			/* Get domain name of exchanger */
			len = parse_name(msg, cp, name, MAXCNAME);
			if (len == -1) return 0;

			cp += len;
			break;
		case TYPE_SOA:
			/* Get domain name of name server */
			len = parse_name(msg, cp, name, MAXCNAME);
			if (len == -1) return 0;

			cp += len;

			/* Get domain name of responsible person */
			len = parse_name(msg, cp, name, MAXCNAME);
			if (len == -1) return 0;

			cp += len;

			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			break;
		case TYPE_TXT:
			/* Just stash */
			break;
		default:
			/* Ignore */
			break;
	}

	return cp;
}

/*
 *              PARSE THE DNS REPLY
 *
 * Description : This function parses the reply message from DNS server.
 * Arguments   : dhdr - is a pointer to the header for DNS message
 *               buf  - is a pointer to the reply message.
 *               len  - is the size of reply message.
 * Returns     : -1 - Domain name lenght is too big
 *                0 - Fail (Timout or parse error)
 *                1 - Success,
 */
int8_t parseDNSMSG(struct dhdr * pdhdr, uint8_t * pbuf, uint8_t * ip_from_dns)
{
	uint16_t tmp;
	uint16_t i;
	uint8_t * msg;
	uint8_t * cp;

	msg = pbuf;
	memset(pdhdr, 0, sizeof(*pdhdr));

	pdhdr->id = Bytes_ToUint16(&msg[0]);
	tmp = Bytes_ToUint16(&msg[2]);
	if (tmp & 0x8000) pdhdr->qr = 1;

	pdhdr->opcode = (tmp >> 11) & 0xf;

	if (tmp & 0x0400) pdhdr->aa = 1;
	if (tmp & 0x0200) pdhdr->tc = 1;
	if (tmp & 0x0100) pdhdr->rd = 1;
	if (tmp & 0x0080) pdhdr->ra = 1;

	pdhdr->rcode = tmp & 0xf;
	pdhdr->qdcount = Bytes_ToUint16(&msg[4]);
	pdhdr->ancount = Bytes_ToUint16(&msg[6]);
	pdhdr->nscount = Bytes_ToUint16(&msg[8]);
	pdhdr->arcount = Bytes_ToUint16(&msg[10]);


	/* Now parse the variable length sections */
	cp = &msg[12];

	/* Question section */
	for (i = 0; i < pdhdr->qdcount; i++)
	{
		cp = dns_question(msg, cp);

		//T("MAX_DOMAIN_NAME is too small, it should be redfine in dns.h");

		if(!cp) return -1;
	}

	/* Answer section */
	for (i = 0; i < pdhdr->ancount; i++)
	{
		cp = dns_answer(msg, cp, ip_from_dns);

		//T("MAX_DOMAIN_NAME is too small, it should be redfine in dns.h");

		if(!cp) return -1;
	}

	/* Name server (authority) section */
	for (i = 0; i < pdhdr->nscount; i++){;}

	/* Additional section */
	for (i = 0; i < pdhdr->arcount; i++){;}

	if(pdhdr->rcode == 0) return 1;		// No error
	else return 0;
}


/*
 *              MAKE DNS QUERY MESSAGE
 *
 * Description : This function makes DNS query message.
 * Arguments   : op   - Recursion desired
 *               name - is a pointer to the domain name.
 *               buf  - is a pointer to the buffer for DNS message.
 *               len  - is the MAX. size of buffer.
 * Returns     : the pointer to the DNS message.
 */
int16_t dns_makequery(uint16_t *msgId, uint16_t op, char * name, uint8_t * buf, uint16_t len)
{
	uint8_t *cp;
	char *cp1;
	char sname[MAXCNAME];
	char *dname;
	uint16_t p;
	uint16_t dlen;

	cp = buf;

	(*msgId)++;
	cp = Uint16ToBytes(cp, *msgId);
	p = (op << 11) | 0x0100;			/* Recursion desired */
	cp = Uint16ToBytes(cp, p);
	cp = Uint16ToBytes(cp, 1);
	cp = Uint16ToBytes(cp, 0);
	cp = Uint16ToBytes(cp, 0);
	cp = Uint16ToBytes(cp, 0);

	strcpy(sname, name);
	dname = sname;
	dlen = strlen(dname);
	for (;;)
	{
		/* Look for next dot */
		cp1 = strchr(dname, '.');

		if (cp1 != NULL) len = cp1 - dname;	/* More to come */
		else len = dlen;			/* Last component */

		*cp++ = len;				/* Write length of component */
		if (len == 0) break;

		/* Copy component up to (but not including) dot */
		strncpy((char *)cp, dname, len);
		cp += len;
		if (cp1 == NULL)
		{
			*cp++ = 0;			/* Last one; write null and finish */
			break;
		}
		dname += len+1;
		dlen -= len+1;
	}

	cp = Uint16ToBytes(cp, 0x0001);				/* type */
	cp = Uint16ToBytes(cp, 0x0001);				/* class */

	return ((int16_t)((uint32_t)(cp) - (uint32_t)(buf)));
}

/* DNS CLIENT INIT */
void DNS_init(DnsConnection *dnsc, SOCKET s, char * domainName ,uint16_t msgId, uint8_t * ip1, uint8_t * ip2)
{
	for(int i = 0; i< 4; i++) dnsc->ip1[i] = ip1[i];
	for(int i = 0; i< 4; i++) dnsc->ip2[i] = ip2[i];
	strcpy(dnsc->domainName ,domainName);
	memset(dnsc->domainIp,0x00,sizeof(dnsc->domainIp));
	memset(dnsc->buf,0x00,sizeof(dnsc->buf));
	dnsc->socket = s;
	dnsc->msgId = msgId; //id del mensaje
	dnsc->retry = 0;
	Timer_Init(&dnsc->timeout,1000);
}

/* DNS CLIENT RUN */
int8_t DNS_run(DnsConnection *dnsc, uint8_t * ipServer)
{
	int8_t ret;
	struct dhdr dhp;
	uint8_t ip[4];
	uint16_t len, port;

   socket(dnsc->socket, Sn_MR_UDP, 0, 0); //Sn_MR_UDP

	T("> DNS Query to DNS Server : %d.%d.%d.%d", ipServer[0], ipServer[1], ipServer[2], ipServer[3]);

	len = dns_makequery(&dnsc->msgId, 0, dnsc->domainName, dnsc->buf, MAX_DNS_BUF_SIZE); //falta mejorar
	sendto(dnsc->socket, dnsc->buf, len, ipServer, IPPORT_DOMAIN);

	Timer_Start(&dnsc->timeout);

	while (1)
	{
		if ((len = getSn_RX_RSR(dnsc->socket)) > 0)
		{
			if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
			len = recvfrom(dnsc->socket, dnsc->buf, len, ip, &port);

			T("> Receive DNS message from %d.%d.%d.%d(%d). len = %d\r\n", ip[0], ip[1], ip[2], ip[3],port,len);

			ret = parseDNSMSG(&dhp, dnsc->buf, dnsc->domainIp);
			break;
		}
		// Check Timeout
		if (Timer_Elapsed(&dnsc->timeout))
		{
			if(dnsc->retry < 2){
				T("> DNS Timeout\r\n");
				sendto(dnsc->socket, dnsc->buf, len, ipServer, IPPORT_DOMAIN);
				dnsc->retry ++;
				Timer_Start(&dnsc->timeout);
			}
			else{
				T("> DNS Server is not responding : %d.%d.%d.%d\r\n", ipServer[0], ipServer[1], ipServer[2],ipServer[3]);
				dnsc->retry = 0;
				close(dnsc->socket);
				return 0; // timeout + retry occurred
			}
		}
	}
	close(dnsc->socket);
	// Return value
	// 0 > :  failed / 1 - success
	return ret;
}


void DnsClient_Lookup(DnsConnection * dnsc){
	int8_t ret;

	T("\r\n=== DNS Client Example ===============");
	T("> DNS 1st : %d.%d.%d.%d\r\n", dnsc->ip1[0], dnsc->ip1[1], dnsc->ip1[2], dnsc->ip1[3]);
	T("> DNS 2nd : %d.%d.%d.%d\r\n", dnsc->ip2[0], dnsc->ip2[1], dnsc->ip2[2], dnsc->ip2[3]);
	T("======================================");
	T("> Example Domain Name : %s", dnsc->domainName);

	/* DNS procssing */
	if ((ret = DNS_run(dnsc,dnsc->ip1)) > 0) // 1st DNS server
	{
	  T("> 1st DNS Response");
	}
	else if ((ret != -1) && ((ret = DNS_run(dnsc,dnsc->ip2))>0))  // retry to 2nd DNS server
	{
	  T("> 2nd DNS Response");
	}
	else if(ret == -1)
	{
	  T("> MAX_DOMAIN_NAME es muy pequenio");
	}
	else
	{
	  T("> DNS Failed :(");
	}

	if(ret > 0) //Success
	{
	  T("> Translated %s to %d.%d.%d.%d",dnsc->domainName,dnsc->domainIp[0],dnsc->domainIp[1],dnsc->domainIp[2],dnsc->domainIp[3]);
	}
}

