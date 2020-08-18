/*
 * Pagina.c
 *
 *  Created on: 18 ago. 2020
 *      Author: Renato
 */


#include "main.h"
#include "Test.h"
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
	char state[5][20]={"Disabled","Disabled","Disabled","Disabled","Disabled"};
	char i;
	uint16_t j=0;

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
	ServerConn_SendLine(&websc, "		<font size = -1> <table border=1>"); //hj

	sprintf(ip, "%d.%d.%d.%d", IpDevice[0], IpDevice[1], IpDevice[2], IpDevice[3]);
	sprintf(str, "		<tr><td bgcolor=violet>IP del Dispositivo: <td><input size=20 name='IpDevice' Value= %s autofocus required><br> ", ip); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", GateWay[0], GateWay[1], GateWay[2], GateWay[3]);
	sprintf(str, "		<tr><td bgcolor=silver>GateWay: <td><input size=20 type=text name='GateWay' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	//sprintf(str, "		<td bgcolor=cyan>Cantidad Balanzas: <td><input size=20 type=text name='NumBal' Value=%d  required><br> ",bals.numbal); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", SubNet[0], SubNet[1], SubNet[2], SubNet[3]);
	sprintf(str, "		<tr><td bgcolor=silver>M&#225scara SubNet: <td><input size=20 type=text name='SubNet' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	//for(i = 0 ; i <bals.numbal ; i++)	strcpy(state[i],"Enabled");

	//sprintf(str, "		<td bgcolor=feldspar>Balanza 1: <td><input size=20 type=text name='B1' %s Value=%s	 required><br> ",state[0],bals.serieStr[0]); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(ip, "%d.%d.%d.%d", IpServer[0], IpServer[1], IpServer[2], IpServer[3]);
	sprintf(str, "		<tr><td bgcolor=silver>IP ServerConnection: <td><input size=20 type=text name='IpServer' Value=%s  required><br> ",ip); //hj
	ServerConn_SendLine(&websc, str);

	//sprintf(str, "		<td bgcolor=feldspar>Balanza 2: <td><input size=20 type=text name='B2' %s Value=%s	 required><br> ",state[1], bals.serieStr[1]); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(str, "		<tr><td bgcolor=silver>Puerto ServerConnection: <td><input size=20 type=text name='PortServer' Value=%d  required><br> ",PortServer); //hj
	ServerConn_SendLine(&websc, str);

	//sprintf(str, "		<td bgcolor=feldspar>Balanza 3: <td><input size=20 type=text name='B3' %s Value=%s	 required><br> ",state[2], bals.serieStr[2]); //hj
	ServerConn_SendLine(&websc, str);

	//		sprintf(str, "		<tr><td bgcolor=feldspar>ID Granja: <td><input size=20 type=text name='IdGranja' Value=%d  required><br> ",app.granja_Id ); //hj
	//ServerConn_SendLine(&websc, str);
	/******/
	//sprintf(str, "		<tr><td bgcolor=feldspar>ID Granja: <td><input size=20 type=text name='IdGranja' Value=%d  list='nomGranjas' required><br> ",app.granja_Id ); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(str, " <datalist id='nomGranjas'> ");
	ServerConn_SendLine(&websc, str);

	for ( j=0; j<26;j++)
	{
	//sprintf(str,"<option value='%d'>%s</option> ",j,Granjas[j]);
	ServerConn_SendLine(&websc, str);
	}

	sprintf(str,"</datalist>");
	ServerConn_SendLine(&websc, str);
	/*************/
	//sprintf(str, "		<td bgcolor=feldspar>Balanza 4: <td><input size=20 type=text name='B4' %s Value=%s	 required><br> ",state[3], bals.serieStr[3]); //hj
	ServerConn_SendLine(&websc, str);

	//sprintf(str, "		<tr><td bgcolor=feldspar>Intervalo Trans: <td><input size=20 type=text name='IntervaloTrans' Value=%d  required><br> ", app.transmition_Timeout ); //hj
	ServerConn_SendLine(&websc, str);

	sprintf(str, "		<tr><td bgcolor=cyan>Clave Usuario: <td><input size=20 type=text name='ClaveUsuario' Value=%s  required><br> ",webServer.ClaveUsuario  ); //hj
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

	ServerConn_SendLine(&websc, "<P align=left>");
	//sprintf(str, "<B> Versi&#243n : %s </B><br>", get_version() );
	ServerConn_SendLine(&websc, str);
	//sprintf(str, "Fecha Compilaci&#243n : %s<br>", 		get_build_date_time() );
	ServerConn_SendLine(&websc, str);
	//sprintf(str, "Serie : %s<br>", 		app.serie );
	ServerConn_SendLine(&websc, str);
	ServerConn_SendLine(&websc, "</P>");
	ServerConn_SendLine(&websc, "</form>");																																										////

	ServerConn_SendLine(&websc, "</body>");
	ServerConn_SendLine(&websc, "</html>");

}


void Pages_Claves(void)
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
	ServerConn_SendLine(&websc, "<td height='68' valign='middle' bgcolor='#F3C47C' class='l' align='center'>");
	ServerConn_SendLine(&websc, "<strong><font color='#0354A4' size='5'><b>Interface ROTEM</b></font></br></br></strong>");
	//sprintf(reg,"<strong><font color='#336699' size='3'><b>Granja: %s   ---   Galp&#243n: %d</b> </font></strong>",Granjas[(app.granja_Id)],plc.galponNumero);
	ServerConn_SendLine(&websc, reg);
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
	ServerConn_SendLine(&websc, reg);
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
