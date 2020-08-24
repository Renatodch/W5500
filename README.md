# W5500 - Lib - API HAL  

## Componentes 

### src  
W5500.c: Componente del dispositivo w5500   
socket.c: Componente de los sockets   
spi1.c: interface SPI1 + ISR
TcpClientConn: Cliente TCP  
TcpServerConn: Servidor TCP    
DNS_Client: Cliente DNS UDP    
DHCPC_Client: Cliente DHCP UDP  
WebServer: Servidor Web FSM  
Pagina: Paginas Web  
  
### inc  
/********Includes del CLib *****************/  
#include "Timer.h"  
#include "Str.h"  
  
/**** Includes principales *************/  
#include "spi1.h"  
#include "w5500.h"  
#include "socket.h"  
#include "DNS_Client.h"  
#include "DHCP_Client.h"  
#include "Bytes.h" // fue modificado, es Parte del CLib.  
#include "Test.h"  
  
/********Includes de Referencia*************/  
#include "eeprom.h"  
#include "Pagina.h"  
/*****************************************/    

## Ejemplo con multiples sockets:
- W5500_Init: Configura el chip.
- 3er socket UDP DHCP_Client: Reestablece algunas características de la red  
- 1er socket TCP_Client: echo response via eeprom  
- 2do socket TCP_Server: Http WebServer, recibir datos desde mi pc. obtiene file.txt    
- 0zt socket UDP DNS_Client: Obtiene la direccion de un nombre de dominio. ej: www.google.com. Un solo método hace el trabajo (T.Max = 2s aprox). Sin máquina de estados   

## Requerimientos  
1. Funcion para Inicializar: reset el W5500 (OK)
2. Funciones para configurar el W5500 (ip, puerto, ping, todo lo que este disponible [DNS client, DHCP client]) (OK)  
3. Funcion para servidor web para poder configurar la tarjeta. (OK)
4. Funcion socket para enviar y recibir datos: leer un dato que llega por el socket y grabarlo en memoria, leer desde memoria y enviarlo por el socket. (OK)  
5. Manejo de multiples socket. (OK)    
6. Funcion para recibir un archivo .txt desde una PC-cliente. (OK)       

## Descripción de Componentes  

### DHCP_Client
- Funciones para manejar el DHCP_Client.

### DNS_Client
- Funciones para manejar el DNS_Client.

Comentarios:  
- CLib: Se agregaron 2 metodos a Bytes y Uint32 ya no existe. Bytes.c contiene lo de Uint32.c

### w5500
- Funciones generales del chip  
- Funciones interface con spi1  
- Funciones interface con socket  

Comentarios:  
- Creado objeto W5500_Network: caracteristicas de red del w5500    
- ahora: void W5500_Init(W5500_Network * w5500_Network); antes void Net_Init(void)  
- chicknet originial Net_Init() es llamado al inicio y otra vez luego de 10s. Por ahora solo lo llamo una vez y todo va OK      
- Falta metodo para configurar el ping block mode (bloquea respuesta a un ping request)  
- Types.h ya no existe. Se estandarizó al tipo de dato que maneja hal y algunas de definiciones en W5500.h    
- Agregado uint8_t checkPhyLink(void): Checkea estado de la capa fisica, cable ethernet conectado: 1 else 0  
- Se agregaron algunos macros para lectura de direcciones del w5500.  

### socket    
- Funciones para usar los sockets del chip  
Comentarios:  
- Se agregó función sendto: enviar datos con udp.  
- Se agregó dunción recvfrom: recibir datos con udp.    

### spi1     
- Funciones del spi1 + irq - Hal driver  
  
### TcpClientConn  
- Funciones para conxion tcp client.  

### TcpServerConn  
- Funciones para conexion tcp server.  
        
### WebServer   
- Funciones para servidor Web    
    
### Paginas
- Paginas Web  

