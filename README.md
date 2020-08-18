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

## Requerimientos
1. Funcion para Inicializar: reset el W5500  
2. Funciones para configurar el W5500 (ip, puerto, ping, todo lo que este disponible [DNS client, DHCP])  
3. Funcion para servidor web para poder configurar la tarjeta.
4. Funcion socket para enviar y recibir datos: leer un dato que llega por el socket y grabarlo en memoria, leer desde memoria y enviarlo por el socket.  
5. Manejo de multiples socket.    
6. Funcion para recibir un archivo .txt desde una PC-cliente.       
7. ServerWeb + sockets   
  
## Metodos

### w5500 (Descripción en W5500.h)
-Funciones generales del chip  
-Funciones interface con spi1  
-Funciones interface con socket  

Comentarios:  
-ahora: void W5500_Init(); antes void Net_Init(void)  
-chicknet originial Net_Init() es llamado al inicio y otra vez luego de 10s. Por ahora solo lo llamo una vez y todo va OK  

### socket    

### spi1     







