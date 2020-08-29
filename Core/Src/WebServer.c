/*
 * WebServer.c
 *
 *  Created on: 23 ago. 2020
 *      Author: Renato
 */


#include "main.h"

/*##############################################################################################################*/
/*############################################ WEB ##############################################################*/

static void WebServer_GetParametersFromPOST(char *request)
{
	int len;
	char prms[4096] = {0};
	String_Between(request, "Content-Length:", "\r\n", prms);
	len = strlen( request ) - atoi( prms );
	strcpy( prms , request + len );
	strcpy(request, prms);
	T("Datos extraidos: %s", request);
}

static void WebServer_GetUri(char *request, char *uri)
{
	String_Between(request, " /", "HTTP", uri);
}

static void WebServer_GetMethod(char *request, char *method)
{
	String_Between(request, "", " ", method);
}

void WebServer_ConfigurandoTarjeta(char *prms)
{
	if ( String_Has(prms) )
	{
		Pages_Messages(prms,"");
	}
}

void WebServer_WriteTable(char *prms)
{
	char filename[64] = {0};
	char content[128] = {0};

	if ( String_Has(prms) )
	{
		String_Between(prms,"filename=\"","\"",filename);

		String_Between(prms,"Content-Type: text/plain","-",content);

		Pages_Messages(filename,content);
	}
}

static void WebServer_EnterToState(char *prms)
{
	switch(webServer.State)
	{
		case WebServer_Ste_Init  :
		break;

		case WebServer_Ste_Clave  :
				Pages_Claves();
		break;

		case WebServer_Ste_Usuario  :
				Pages_User();
		break;

		default: break;
	}
}

static void WebServer_ExitFromState(char *prms)
{
}

static void WebServer_Change_State(WEB_SERVER__STE ste, char *prms)
{
	WebServer_ExitFromState( prms );
	webServer.State = ste;
	WebServer_EnterToState( prms );
}

static void WebServer_Usuario_State(WEB_SERVER__EVT e, char *prms)
{
	switch(e)
	{
		case WEB_SERVER__EVT_IP  :
				WebServer_Change_State(WebServer_Ste_Clave, prms);
		break;

		case WEB_SERVER__EVT_POST_WRITE_EEPROM  :

				WebServer_ConfigurandoTarjeta(prms);

				WebServer_Change_State(WebServer_Ste_Init, prms);
		break;

		case WEB_SERVER__EVT_POST_WRITE_TABLE  :

				WebServer_WriteTable(prms);

				WebServer_Change_State(WebServer_Ste_Init, prms);
		break;
		default:break;
	}
}


static void WebServer_Clave_State(WEB_SERVER__EVT e, char *prms)
{
	switch(e)
	{
		case WEB_SERVER__EVT_IP  :
				WebServer_Change_State(WebServer_Ste_Clave, prms);
		break;

		case WEB_SERVER__EVT_CLAVE_USUARIO :
				WebServer_Change_State(WebServer_Ste_Usuario, prms);
		break;

		case WEB_SERVER__EVT_CLAVE_FACTORY  :
				WebServer_Change_State(WebServer_Ste_Factory, prms);
		break;
		default: break;
	}
}

static void WebServer_Fsm(WEB_SERVER__EVT e, char *prms)
{
	switch(webServer.State)
		{
		case WebServer_Ste_Init  :
				//WebServer_Init_State(e, prms);
		break;

		case WebServer_Ste_Clave  :
				WebServer_Clave_State(e, prms);
		break;

		case WebServer_Ste_Usuario  :
				WebServer_Usuario_State(e, prms);
		break;


		default:break;
	}
}

/*******************************************************************************
* Function Name  : WebServer_ProcessUris
* Description    : Verify the events of WebServer.
* Input          : prms - the parameters.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void WebServer_ProcessUris(char *prms)
{
	char claveValue[64] = {0};

	if( String_Is_Empty( prms ) )
		return;

	switch(webServer.State)
	{
		case WebServer_Ste_Clave  :          ////

			String_Between( prms, "Clave=", "", claveValue );

			String_Trim_Total( claveValue );

			if( String_Compare( claveValue, webServer.ClaveUsuario) )
			{
				T("WEB_SERVER__EVT_CLAVE_USUARIO");
				WebServer_Fsm( WEB_SERVER__EVT_CLAVE_USUARIO, "");
				break;
			}

			Pages_Error_Clave();
		break;


		case WebServer_Ste_Usuario  :
		case WebServer_Ste_Factory  :

			if (strstr( prms, "IpDevice") != NULL){

				WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_EEPROM, prms);
				return;
			}
			else{
				//TRACE_U("WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_TABLE, prms)");

				WebServer_Fsm( WEB_SERVER__EVT_POST_WRITE_TABLE, prms);
			}
		break;
		default: break;
	}
}


/*******************************************************************************
* Function Name  : WebServer_MethodGETHandler
* Description    : Handler the GET method.
* Input          : request - the request.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void WebServer_MethodGETEventHandler(char * request)
{
	char uri[64];

	WebServer_GetUri( request, uri );

	T("uri: %s",uri);

	String_Trim_Total( uri );

	if( String_Compare( uri, "home" ) )
	{
		WebServer_Change_State(WebServer_Ste_Clave,"");
		return;
	}

	if( String_Compare( uri, "tabla" ) )
	{
		//Pages_Servicio3();
		return;
	}

	//Pages_Messages("Comando no reconocido");

}

/*******************************************************************************
* Function Name  : WebServer_MethodPOSTHandler
* Description    : Handler the POST method.
* Input          : request - the request.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void WebServer_MethodPOSTEventHandler(char * request)
{
	char uri[512] = {0};

	WebServer_GetUri(request, uri);

	String_Trim_Total( uri );

	T("uri = %s",uri);

	if( String_Is_Empty( uri ) )
		return;

	switch(webServer.State)
	{
		case WebServer_Ste_Clave  :

			if( String_Compare( uri, "clave" ) )
			{
				WebServer_GetParametersFromPOST( request );
				WebServer_ProcessUris( request );
			}

		break;

		case WebServer_Ste_Usuario  :
		case WebServer_Ste_Factory  :

			if( String_Compare( uri, "usuario")  )
			{

					WebServer_GetParametersFromPOST( request );

					WebServer_ProcessUris( request );
			}
		break;
		default:break;
	}
}

void WebServer_ListenEventHandler(char * request, int len)
{
		char method[32] ={0};

		request[len] = 0;

		T("request = \r\n%s\r\n", request);

		WebServer_GetMethod(request, method);

		T("method: %s",method);

		if( String_Compare(method, "GET") )
		{
			WebServer_MethodGETEventHandler( request );
			return;
		}

		if( String_Compare(method, "POST") )
		{
			WebServer_MethodPOSTEventHandler( request );
			return;
		}

		Pages_Messages("Comando no reconocido","");

}

void WebServer_Init()
{
	strcpy(webServer.ClaveUsuario,"123456");
	webServer.State = WebServer_Ste_Init;
}


/*##################################################################################################*/
