/*
 * WebServer.h
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */

#ifndef INC_WEBSERVER_H_
#define INC_WEBSERVER_H_

#include "main.h"
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
	char 			ClaveUsuario[16];
	WEB_SERVER__STE State;
} WebServer;

void WebServer_ConfigurandoTarjeta(char *prms); //metodo invocado cuando se selecciona boton send en pagina web
void WebServer_ListenEventHandler(char * data, int len); //metodo asociado a tcp server, para iniciar sitio web
void WebServer_Init(void); //metodo para iniciar sitio web
void WebServer_WriteTable(char *prms); //metodo invocado cuando se selecciona boton Enviar Tabla en pagina web



#endif /* INC_WEBSERVER_H_ */
