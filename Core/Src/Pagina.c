/*
 * Pagina.c
 *
 *  Created on: 18 ago. 2020
 *      Author: Renato
 */


#include "main.h"


/*******************************************************************************
* Function Name  : Pages_Messages
* Description    : Page to send message.
* Input          : msg - message.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Pages_Messages(char *msg,char *msg1)
{
	if(String_Is_Empty(msg)) return;
	if(String_Is_Empty(msg1)) return;

	ServerConn_SendLine(&websc, "HTTP/1.1 200 OK\r\n");
	ServerConn_SendLine(&websc, "Content-Type: text/html\r\n");
	ServerConn_SendLine(&websc, "\r\n");
	ServerConn_SendLine(&websc, "<!DOCTYPE html>");
	ServerConn_SendLine(&websc, "<html>");
	ServerConn_SendLine(&websc, "<head>");

	ServerConn_SendLine(&websc, "<meta charset=\"utf-8\" />");
	ServerConn_SendLine(&websc, "<title>Configurar el dispositivo Galpon</title>");
	ServerConn_SendLine(&websc, "</head>");


	ServerConn_SendLine(&websc, "<body>");

	ServerConn_SendLine(&websc, "<h3>");
	ServerConn_SendLine(&websc, msg);
	ServerConn_SendLine(&websc, "</h3>");

	ServerConn_SendLine(&websc, "<p>");
	ServerConn_SendLine(&websc, msg1);
	ServerConn_SendLine(&websc, "</p>");

	ServerConn_SendLine(&websc, "</body>");
	ServerConn_SendLine(&websc, "</html>");
}

/*******************************************************************************
* Function Name  :  Pages_User
* Description    : Page to config the parameters user.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Pages_User(void)
{
	char str[256] = {0};
	char ip[20] = {0};


	T("\r\nSend Usuario Page.............................................");

	ServerConn_SendLine(&websc, "HTTP/1.1 200 OK\r\n");
	ServerConn_SendLine(&websc, "Content-Type: text/html\r\n");
	ServerConn_SendLine(&websc, "\r\n");
	ServerConn_SendLine(&websc, "<!DOCTYPE html>");
	ServerConn_SendLine(&websc, "<html>");

	ServerConn_SendLine(&websc, "<center>");
	ServerConn_SendLine(&websc, "<body vlink=black bgcolor='#99cc1f'>"); //hj
	ServerConn_SendLine(&websc, "<h1>Configuraci&#243n de Usuario</h1>"); //hj
	ServerConn_SendLine(&websc, "<form action='usuario'  method='post' >");
	ServerConn_SendLine(&websc, "<font size = -1> <table border=1>"); //hj

	sprintf(ip, "%d.%d.%d.%d", w5500_Network.IpDevice[0], w5500_Network.IpDevice[1], w5500_Network.IpDevice[2], w5500_Network.IpDevice[3]);
	sprintf(str, "<tr><td bgcolor=violet>IP del Dispositivo: <td><input size=20 name='IpDevice' Value= %s autofocus required><br> ", ip); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", w5500_Network.Gw[0], w5500_Network.Gw[1], w5500_Network.Gw[2], w5500_Network.Gw[3]);
	sprintf(str, "<tr><td bgcolor=silver>GateWay: <td><input size=20 type=text name='GateWay' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", w5500_Network.Sub[0], w5500_Network.Sub[1], w5500_Network.Sub[2], w5500_Network.Sub[3]);
	sprintf(str, "<tr><td bgcolor=silver>M&#225scara SubNet: <td><input size=20 type=text name='SubNet' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", IpServer[0], IpServer[1], IpServer[2], IpServer[3]);
	sprintf(str, "<tr><td bgcolor=silver>IP de Renato: <td><input size=20 type=text name='IpServer' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(str, "<tr><td bgcolor=silver>Puerto de Renato: <td><input size=20 type=text name='PortServer' Value=%d  required><br> ",Port); //hj
	ServerConn_SendLine(&websc, str);

	ServerConn_SendLine(&websc, "</center>"); //hj
	ServerConn_SendLine(&websc, "</table>"); //hj

	ServerConn_SendLine(&websc, "<p>");
	ServerConn_SendLine(&websc, "<input type='submit' value='Salvar' />");
	ServerConn_SendLine(&websc, "</p>");
	ServerConn_SendLine(&websc, "<p>");
	ServerConn_SendLine(&websc, "<input type='reset' value='Reset' />");
	ServerConn_SendLine(&websc, "</p>");
	ServerConn_SendLine(&websc, "</form>");

	ServerConn_SendLine(&websc, "<form action='usuario' enctype='multipart/form-data' method='post'>");     ////
	ServerConn_SendLine(&websc, "<input type='hidden' name='MAX_FILE_SIZE' value='4096' />");
	ServerConn_SendLine(&websc, "<input type =	'file' name=	'myfile'/>");
	ServerConn_SendLine(&websc, "<input type =	'submit' value =	'Enviar Tabla'/>");
	ServerConn_SendLine(&websc, "</form>");																																										////

	ServerConn_SendLine(&websc, "</body>");
	ServerConn_SendLine(&websc, "</html>");

}

void Pages_Servicio(void)
{
	char reg[80];

	//FlashRegistrosHead rh;

	memset(reg, 0, sizeof(reg));

	ServerConn_SendLine(&websc, "<!DOCTYPE html>");

	ServerConn_SendLine(&websc, "<html>");
	ServerConn_SendLine(&websc, "<head>");

	//ServerConn_SendLine(&websc, "<meta http-equiv='refresh' content='5'>");
	ServerConn_SendLine(&websc, "</head>");

	ServerConn_SendLine(&websc, "<body>");

	ServerConn_SendLine(&websc, "PAGES SERVICIO");



	ServerConn_SendLine(&websc, "</body>");
	ServerConn_SendLine(&websc, "</html>");
}
/*******************************************************************************
* Function Name  : Pages_Error_Clave
* Description    : Page to error of login
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Pages_Error_Clave(void)
{
		char reg[256];

		memset(reg, 0, sizeof(reg));

		ServerConn_SendLine(&websc, "HTTP/1.1 200 OK\r\n");
		ServerConn_SendLine(&websc, "Content-Type: text/html\r\n");
		ServerConn_SendLine(&websc, "\r\n");
		ServerConn_SendLine(&websc, "<!DOCTYPE html>");
		ServerConn_SendLine(&websc, "<html>");

		//Inicio de Prueba BOF
		ServerConn_SendLine(&websc, "<body>");

		ServerConn_SendLine(&websc, "<form action='clave' method='post' >");
		ServerConn_SendLine(&websc, "<table width='680' border='1' align='center' cellpadding='0' cellspacing='0' bordercolor='#336699'>");
		ServerConn_SendLine(&websc, "<tr>");
		ServerConn_SendLine(&websc, "<td valign='top'><table width='100%' border='0' align='center' cellpadding='0' cellspacing='0'>");
		ServerConn_SendLine(&websc, "<td height='48' valign='middle' bgcolor='#F3C47C' class='l' align='center'>");
		ServerConn_SendLine(&websc, "<strong><font color='#336699' size='3'><b>Interface ROTEM</b></font></strong>");
		ServerConn_SendLine(&websc, "</td>");
		ServerConn_SendLine(&websc, "</tr>");

		ServerConn_SendLine(&websc, "<tr bgcolor='ffffff'>");
		ServerConn_SendLine(&websc, "<td align=center bgcolor='ffffcc' height=69> <table width=500 border=0 cellspacing=5 cellpadding=0 align=center bgcolor='ffffcc'>");
		ServerConn_SendLine(&websc, "<tr>");
		ServerConn_SendLine(&websc, "<td width='32%'>");
		ServerConn_SendLine(&websc, "<div align=right><font color='#006699'><b>Clave</b></font></div>");
		ServerConn_SendLine(&websc, "</td>");
		ServerConn_SendLine(&websc, "<td width=68%>");
		ServerConn_SendLine(&websc, "<input type=password name='Clave'  required />"); ;
		ServerConn_SendLine(&websc, "</td>");
		ServerConn_SendLine(&websc, "</tr>");
		ServerConn_SendLine(&websc, "</table></td>");
		ServerConn_SendLine(&websc, "</tr>");
		ServerConn_SendLine(&websc, "<tr bgcolor='#FFFFFF'>");
		ServerConn_SendLine(&websc, "<td align=right> <div align=center>");
		ServerConn_SendLine(&websc, "<input type='submit' value='Accesar' />");
		ServerConn_SendLine(&websc, "</div><br>");
		ServerConn_SendLine(&websc, "</td>");
		ServerConn_SendLine(&websc, "</tr>");
		ServerConn_SendLine(&websc, "<tr>");

		//ServerConn_SendLine(&websc, reg);
		//ServerConn_SendLine(&websc, "<td bgcolor='#336699' align='center' class='l'><font size='1'> TECAVI - 2015</font></td>");
		ServerConn_SendLine(&websc, "</tr></table></td></tr></table>");

		ServerConn_SendLine(&websc, "</form>");

		ServerConn_SendLine(&websc, "<br><center>Clave error.</center><br>");

		//////Aqui dibujo los pollitos
		ServerConn_SendLine(&websc, "<canvas id='pollo' width='150' height='150'>");
		//////ServerConn_SendLine(&websc, "         Tu navegador no soporta canvas. Actualizalo");
		ServerConn_SendLine(&websc, "</canvas>");


		ServerConn_SendLine(&websc, "<script type='text/javascript'>");

		ServerConn_SendLine(&websc, "var c=document.getElementById('pollo');");
		ServerConn_SendLine(&websc, "var cxt=c.getContext('2d');");

		//Picos de los pollos
		ServerConn_SendLine(&websc, "cxt.moveTo(122,10);");
		ServerConn_SendLine(&websc, "cxt.lineTo(132,10);");
		ServerConn_SendLine(&websc, "cxt.lineTo(124,15);");
		ServerConn_SendLine(&websc, "cxt.moveTo(122,35);");
		ServerConn_SendLine(&websc, "cxt.lineTo(130,35);");
		ServerConn_SendLine(&websc, "cxt.lineTo(124,40);");
		ServerConn_SendLine(&websc, "cxt.lineWidth = 2;");
		ServerConn_SendLine(&websc, "cxt.stroke();");
		//Pollo Rojo cuerpo, cabeza
		ServerConn_SendLine(&websc, "cxt.fillStyle ='red';");
		ServerConn_SendLine(&websc, "cxt.beginPath();");
		ServerConn_SendLine(&websc, "cxt.arc(60,60,50,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.arc(105,20,20,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.closePath();");
		ServerConn_SendLine(&websc, "cxt.fill();");
		//Pollo Amarillo cuerpo, cabeza
		ServerConn_SendLine(&websc, "cxt.fillStyle ='yellow';");
		ServerConn_SendLine(&websc, "cxt.beginPath();");
		ServerConn_SendLine(&websc, "cxt.arc(70,70,40,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.arc(105,45,20,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.closePath();");
		ServerConn_SendLine(&websc, "cxt.fill();");
		//Ojos de los pollos 1.-ojo pollo amarillo 2.-ojo pollo rojo
		ServerConn_SendLine(&websc, "cxt.fillStyle ='black';");
		ServerConn_SendLine(&websc, "cxt.beginPath();");
		ServerConn_SendLine(&websc, "cxt.arc(110,35,2,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.arc(110,10,2,0,Math.PI*2,true);");
		ServerConn_SendLine(&websc, "cxt.closePath();");
		ServerConn_SendLine(&websc, "cxt.fill();");

		ServerConn_SendLine(&websc, "</script>");

		ServerConn_SendLine(&websc, "</body>");
		ServerConn_SendLine(&websc, "</html>");
		//Fin de Prueba EOF
}

void Pages_Claves(void)
{

	ServerConn_SendLine(&websc, "HTTP/1.1 200 OK\r\n");
	ServerConn_SendLine(&websc, "Content-Type: text/html\r\n");
	ServerConn_SendLine(&websc, "\r\n");
	ServerConn_SendLine(&websc, "<!DOCTYPE html>");
	ServerConn_SendLine(&websc, "<html>");

	ServerConn_SendLine(&websc, "<body>");

	ServerConn_SendLine(&websc, "<form action='clave' method='post' >");
	ServerConn_SendLine(&websc, "<table width='680' border='1' align='center' cellpadding='0' cellspacing='0' bordercolor='#336699'>");
	ServerConn_SendLine(&websc, "<tr>");
	ServerConn_SendLine(&websc, "<td valign='top'><table width='100%' border='0' align='center' cellpadding='0' cellspacing='0'>");
	ServerConn_SendLine(&websc, "<td height='68' valign='middle' bgcolor='#F3C47C' class='l' align='center'>");
	ServerConn_SendLine(&websc, "<strong><font color='#0354A4' size='5'><b>Interface ROTEM</b></font></br></br></strong>");
	//sprintf(reg,"<strong><font color='#336699' size='3'><b>Granja: %s   ---   Galp&#243n: %d</b> </font></strong>",Granjas[(app.granja_Id)],plc.galponNumero);
	//ServerConn_SendLine(&websc, reg);
//ServerConn_SendLine(&websc, "<strong><font color='#336699' size='3'><b>Granja: %X</b></font></strong>");
	ServerConn_SendLine(&websc, "</td>");
	ServerConn_SendLine(&websc, "</tr>");

	ServerConn_SendLine(&websc, "<tr bgcolor='ffffff'>");
	ServerConn_SendLine(&websc, "<td align=center bgcolor='ffffcc' height=69> <table width=500 border=0 cellspacing=5 cellpadding=0 align=center bgcolor='ffffcc'>");
	ServerConn_SendLine(&websc, "<tr>");
	ServerConn_SendLine(&websc, "<td width='32%'>");
	ServerConn_SendLine(&websc, "<div align=right><font color='#006699'><b>Clave</b></font></div>");
	ServerConn_SendLine(&websc, "</td>");
	ServerConn_SendLine(&websc, "<td width=68%>");
	ServerConn_SendLine(&websc, "<input type=password name='Clave'  required />"); ;
	ServerConn_SendLine(&websc, "</td>");
	ServerConn_SendLine(&websc, "</tr>");
	ServerConn_SendLine(&websc, "</table></td>");
	ServerConn_SendLine(&websc, "</tr>");
	ServerConn_SendLine(&websc, "<tr bgcolor='#FFFFFF'>");
	ServerConn_SendLine(&websc, "<td align=right> <div align=center>");
	ServerConn_SendLine(&websc, "<input type='submit' value='Accesar' />");
	ServerConn_SendLine(&websc, "</div><br>");
	ServerConn_SendLine(&websc, "</td>");
	ServerConn_SendLine(&websc, "</tr>");
	ServerConn_SendLine(&websc, "<tr>");

	//sprintf(reg, "<td bgcolor='#336699' align='center' class='l'><font color='#FBFCFC' size='3'>Serie: %s   --   Ver: %s  --   Compilaci&#243n: %s</font></td>", app.serie, get_version(), get_build_date_time()  );  //03.04
	//ServerConn_SendLine(&websc, reg);
	//ServerConn_SendLine(&websc, "<td bgcolor='#336699' align='center' class='l'><font size='1'> TECAVI - 2015</font></td>");
	ServerConn_SendLine(&websc, "</tr></table></td></tr></table>");

	ServerConn_SendLine(&websc, "</form>");


	//////Aqui dibujo los pollitos
	ServerConn_SendLine(&websc, "<canvas id='pollo' width='150' height='150'>");
	//////ServerConn_SendLine(&websc, "         Tu navegador no soporta canvas. Actualizalo");
	ServerConn_SendLine(&websc, "</canvas>");


	ServerConn_SendLine(&websc, "<script type='text/javascript'>");

	ServerConn_SendLine(&websc, "var c=document.getElementById('pollo');");
	ServerConn_SendLine(&websc, "var cxt=c.getContext('2d');");

	//Picos de los pollos
	ServerConn_SendLine(&websc, "cxt.moveTo(122,10);");
	ServerConn_SendLine(&websc, "cxt.lineTo(132,10);");
	ServerConn_SendLine(&websc, "cxt.lineTo(124,15);");
	ServerConn_SendLine(&websc, "cxt.moveTo(122,35);");
	ServerConn_SendLine(&websc, "cxt.lineTo(130,35);");
	ServerConn_SendLine(&websc, "cxt.lineTo(124,40);");
	ServerConn_SendLine(&websc, "cxt.lineWidth = 2;");
	ServerConn_SendLine(&websc, "cxt.stroke();");
	//Pollo Rojo cuerpo, cabeza
	ServerConn_SendLine(&websc, "cxt.fillStyle ='red';");
	ServerConn_SendLine(&websc, "cxt.beginPath();");
	ServerConn_SendLine(&websc, "cxt.arc(60,60,50,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.arc(105,20,20,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.closePath();");
	ServerConn_SendLine(&websc, "cxt.fill();");
	//Pollo Amarillo cuerpo, cabeza
	ServerConn_SendLine(&websc, "cxt.fillStyle ='yellow';");
	ServerConn_SendLine(&websc, "cxt.beginPath();");
	ServerConn_SendLine(&websc, "cxt.arc(70,70,40,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.arc(105,45,20,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.closePath();");
	ServerConn_SendLine(&websc, "cxt.fill();");
	//Ojos de los pollos 1.-ojo pollo amarillo 2.-ojo pollo rojo
	ServerConn_SendLine(&websc, "cxt.fillStyle ='black';");
	ServerConn_SendLine(&websc, "cxt.beginPath();");
	ServerConn_SendLine(&websc, "cxt.arc(110,35,2,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.arc(110,10,2,0,Math.PI*2,true);");
	ServerConn_SendLine(&websc, "cxt.closePath();");
	ServerConn_SendLine(&websc, "cxt.fill();");

	ServerConn_SendLine(&websc, "</script>");

	ServerConn_SendLine(&websc, "</body>");
	ServerConn_SendLine(&websc, "</html>");
	//Fin de Prueba EOF

}
