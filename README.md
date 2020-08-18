# W5500 - Lib - API HAL  

## Componentes 

### src  
W5500.c: Componente del dispositivo w5500   
socket.c: Componente de los sockets   
spi1.c: interface SPI1 + ISR  

### inc  
w5500.h  
socket.h    
spi1.h  
types.h  


## Ejemplos  
### Ejemplo

### Ejemplo con multiples sockets:  
-1er socket TCP_Client: echo response via eeprom
-2do socket TCP_Server: Http com, parsear datos enviados desde mi pc. Transferir file.txt  
-3er socket DNS_Client: ---  
-4to socket DHCP  

## Requerimientos  
1. Funcion para Inicializar: reset el W5500 (OK)
2. Funciones para configurar el W5500 (ip, puerto, ping, todo lo que este disponible [DNS client, DHCP]) (Falta)  
3. Funcion para servidor web para poder configurar la tarjeta. (Falta)
4. Funcion socket para enviar y recibir datos: leer un dato que llega por el socket y grabarlo en memoria, leer desde memoria y enviarlo por el socket. (OK)  
5. Manejo de multiples socket. (OK)    
6. Funcion para recibir un archivo .txt desde una PC-cliente. (Falta)       

## Descripción

### w5500 (Descripción en W5500.h)
-Funciones generales del chip  
-Funciones interface con spi1  
-Funciones interface con socket  

Comentarios:  
-ahora: void W5500_Init(); antes void Net_Init(void)  
-chicknet originial Net_Init() es llamado al inicio y otra vez luego de 10s. Por ahora solo lo llamo una vez y todo va OK    
-Falta metodo para configurar el ping block mode (bloquea respuesta a un ping request)  
-Types.h ya no existe. Se estandarizó al tipo de dato que maneja hal y algunas de definiciones en W5500.h  

### socket    

### spi1     







