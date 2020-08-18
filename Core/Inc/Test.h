/*
 * config.h
 *
 *  Created on: 14 ago. 2020
 *      Author: Renato
 */

#ifndef INC_TEST_H_
#define INC_TEST_H_

#include "main.h"
//=================================================




//Port A
#define LED1			GPIO_Pin_0	//
#define LED2			GPIO_Pin_1	//
#define PA2				GPIO_Pin_2	//
#define PA3				GPIO_Pin_3	//
#define WIZ_SCS			GPIO_Pin_4	// out
#define WIZ_SCLK		GPIO_Pin_5	// out
#define WIZ_MISO		GPIO_Pin_6	// in
#define WIZ_MOSI		GPIO_Pin_7	// out
#define IN3_PIN			GPIO_Pin_8	// in
#define USART1_TX		GPIO_Pin_9	// out
#define USART1_RX		GPIO_Pin_10	// in
#define PA11			GPIO_Pin_11	//
#define PA12			GPIO_Pin_12	//
#define PA13			GPIO_Pin_13	//
#define PA14			GPIO_Pin_14	// out
#define PA15			GPIO_Pin_15	// out

//Port B
#define SIRENA_OUT_1_PIN_B0	  GPIO_Pin_0	// OUT
#define IN4_PIN				  GPIO_Pin_6
#define IN2_PIN 			  GPIO_Pin_2	// in    HJM Chinet 03.00 Cambiado era 1
#define IN1_PIN				  GPIO_Pin_1	// in     HJM Chinet 03.00 Cambiado era 2
#define PB3					  GPIO_Pin_3	//
#define PB4					  GPIO_Pin_4	//
#define PB5					  GPIO_Pin_5	//
#define PB6					  GPIO_Pin_6	//
#define PB7					  GPIO_Pin_7	//
#define WIZ_RESET		      GPIO_Pin_8	// out
#define WIZ_PWDN		      GPIO_Pin_9	// out
#define PB10			  	  GPIO_Pin_10	//
#define PB11				  GPIO_Pin_11	//
#define PB12				  GPIO_Pin_12	//
#define PB13				  GPIO_Pin_13	//
#define PB14				  GPIO_Pin_14	//
#define PB15				  GPIO_Pin_15	//

// Port C
#define M485_ENABLE_RECV_PIN	GPIO_Pin_4	//
#define OUT2_PIN			    GPIO_Pin_5	// OUT
#define OUT3_PIN			    GPIO_Pin_7	// OUT
#define PC13					GPIO_Pin_13	//
#define PC14					GPIO_Pin_14	//
#define PC15					GPIO_Pin_15	//

// Port D

#define Wiz_En					GPIO_Pin_2


//=================================================
typedef struct _CONFIG_MSG
{
	uint8 Mac[6];
	uint8 IpDevice[4];
	uint8 Sub[4];
	uint8 Gw[4];
	uint8 DNS_ServerConnection_IP[4];
	uint8  DHCP;
}
CONFIG_MSG;


typedef struct _CONFIG_TYPE_DEF
{
	uint16 port;
	uint8 destip[4];
}CHCONFIG_TYPE_DEF;


#define SOCK_CONFIG		2	// UDP
#define SOCK_DNS		2	// UDP
#define SOCK_DHCP		3	// UDP

#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	30	// 30sec

#define ON	1
#define OFF	0

#define HIGH		1
#define LOW		0


// SRAM address range is 0x2000 0000 ~ 0x2000 4FFF (20KB)
#define TX_RX_MAX_BUF_SIZE	4096//2048
//#define TX_BUF	0x20004000
//#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)
extern uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
//extern uint8 RX_BUF[TX_RX_MAX_BUF_SIZE];


#define EEPROM_USUARIO						0

#define EEPROM_SERIE						900

#define EEPROM_EPOCH_TIME_ADDR				950		 /*NO Str*/

#define EEPROM_FLASH_KEY_ADDR				958		 /*NO Str*/
#define EEPROM_FLASH_READ_INDEX_ADDR		964    /*NO Str*/
#define EEPROM_FLASH_WRITE_INDEX_ADDR		972		 /*NO Str*/


#define EEPROM_OUT1							1032 					/*NO Str*/
#define EEPROM_TABLA						1064          //TABLA
#define EEPROM_EPOCH_TIME_TABLA				3400					//Fecha de tabla uploaded /*NO Str*/





/***************************************************************************************************/
/************************************  TCP CLIENTE *************************************************/
/***************************************************************************************************/

#define 	SOCKET_1TCP			1																															//
#define 	SOCKET_2TCP			2																															//
#define 	SOCKET_3TCP			3																															//
#define 	SOCKET_4TCP			4																															//
#define 	SOCKET_5TCP			5																															//
#define 	SOCKET_6TCP			6																															//
#define 	SOCKET_7TCP			7																															//
#define 	SOCKET_8TCP			8


#define		SOCKET_ROTEM_NET 				SOCKET_2TCP
#define		SOCKET_TRACE					SOCKET_5TCP

//Tal para cual
typedef void (*Receiver_EventHandler)(uint8_t *data, int len);
typedef void (*OnConnection_EventHandler)(void);


extern void Client_Receiver_EventHandler(uint8_t *data, int len);
extern void Client_onConnection_EventHandler(void);

typedef struct
{
		Timer			request_Timer;
		uint8_t	 		dest_Ip[4];
		uint16_t  		dest_Port;
		uint8_t			socket;
		Receiver_EventHandler		receiver_EventHandler;
		OnConnection_EventHandler	onConnection_EventHandler;
}TcpClient;



uint8_t 		TcpClientConn_Get_State(TcpClient *p);
void 			TcpClientConn_Send_String(TcpClient *p, char * data_buf);
uint8_t 		TcpClientConn_SendStream(TcpClient *p, uint8_t * data_buf, 	uint16_t len);
void 			TcpClientConn_Events(TcpClient *p, uint16_t port);
void 			TcpClientConn_To_String(TcpClient *p, char *mt);
void 			TcpClientConn_Init(TcpClient *p, char socket, uint8 * ipDest, uint16_t portDest, Receiver_EventHandler receiver_EventHandler, 	OnConnection_EventHandler	onConnection_EventHandler);
/********************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************/

/***************************************************************************************************/
/************************************  TCP SERVER *************************************************/
/***************************************************************************************************/
typedef void (*Listen_EventHandler)(char *dat, int len);

typedef struct
{
		uint16_t  						port;
		uint8_t								socket;
		Listen_EventHandler		listen_EventHandler;
}ServerConnection;

void ServerConn_SendLine(ServerConnection *p, const char * str);
void ServerConn_SendString(ServerConnection *p, const char * data_buf);
void ServerConn_Events(ServerConnection *p);
void ServerConnection_ToString(ServerConnection *p, char *mt);
void ServerConnection_Init(ServerConnection *p, char socket, uint16_t port, Listen_EventHandler listen_EventHandler);


/********************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************/

/***************************************************************************************************/
/************************************  WEB *********************************************************/
/***************************************************************************************************/
typedef enum
{
	WEB_SERVER__EVT_IP,
	WEB_SERVER__EVT_CLAVE_FACTORY,
	WEB_SERVER__EVT_CLAVE_USUARIO,
	WEB_SERVER__EVT_POST_WRITE_EEPROM,
	WEB_SERVER__EVT_POST_WRITE_TABLE,
} WEB_SERVER__EVT;

typedef enum
{
	WebServer_Ste_Init,
	WebServer_Ste_Clave,
	WebServer_Ste_Factory,
	WebServer_Ste_Usuario,
	WebServer_Ste_Servicio3 			//Agregado el 22 de Junio de 2017
} WEB_SERVER__STE;

typedef struct
{
	Timer		close_Timer;
	char ClaveAdmin[16];
	char ClaveUsuario[16];
	WEB_SERVER__STE State;
} WebServer;




void WebServer_BuzzerToWriteEeprom(void);
void WebServer_ListenEventHandler(char * data, int len);
void WebServer_Events(void);
void WebServer_ToString(char *str);
void WebServer_Init(void);
void WebServer_WriteTable(char *prms);


//Pages
void page1(void);
void page2(void);


/********************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************/
//Debug
void T(char *format,...);

extern uint8 Enable_DHCP;

extern uint8 IpServer[4];
extern uint16 PortServer;
extern uint8 MAC[6];

extern uint8 IpDevice[4];//IpDevice Address
extern uint8 GateWay[4];//Gateway Address
extern uint8 SubNet[4];//SubnetMask Address

extern uint8 IpTrace[4];//IpTrace Address
extern uint16 PortTrace;

extern uint16 any_port;

extern CONFIG_MSG Config_Msg;
extern CHCONFIG_TYPE_DEF Chconfig_Type_Def;

extern uint16 PortServer;
extern uint8 IpServer[4] ;

//TX MEM SIZE- SOCKET 0-7:4KB
//RX MEM SIZE- SOCKET 0-7:4KB
extern uint8 txsize[MAX_SOCK_NUM];
extern uint8 rxsize[MAX_SOCK_NUM];
/* Extern variables ----------------------------------------------------------*/


#endif /* INC_TEST_H_ */
