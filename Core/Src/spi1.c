/*
 * spi1.c
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */

#include "main.h"

uint8_t *Tx;
uint8_t *Rx;


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){

}

/*
static void Allocate_Tx(uint8_t * p){
	free(p);
	Tx = (uint8_t *)calloc(1,size); //actualizamos espacio de memoria
	memmove(Tx,p,size);
}
static void Allocate_Rx(uint8_t * p){
	free(p);
	Rx = (uint8_t *)calloc(1,size); //actualizamos espacio de memoria
	memmove(Rx,p,size);
}
*/

/*******************************************************************************
* Function Name  : SPI1_SendDataIT
* Description    :
* Input          : pTx: puntero datos a enviar
* 				   size: cantidad de bytes a enviar y recibir
* 				   pRx: puntero datos a recibir
* Output         : None.
* Return         : 0: FAIL.
* 				   1: OK
*******************************************************************************/

void SPI1_SendReceive_IT(uint8_t * pTx,uint8_t * pRx, uint16_t size){
	while(hspi1.State != HAL_SPI_STATE_READY);
	HAL_SPI_TransmitReceive_IT(&hspi1, pTx, pRx, size);
	while(hspi1.State != HAL_SPI_STATE_READY);

}
/*******************************************************************************
* Function Name  : SPI1_SendData_IT
* Description    :
* Input          : pTx: puntero datos a enviar
* 				   size: cantidad de bytes a enviar
* Output         : None.
* Return         : 0: FAIL.
* 				   1: OK
*******************************************************************************/
void SPI1_Send_IT(uint8_t *pTx, uint16_t size){
	while(hspi1.State != HAL_SPI_STATE_READY);
	HAL_SPI_Transmit_IT(&hspi1, Tx, size);
	while(hspi1.State != HAL_SPI_STATE_READY);
}


/*******************************************************************************
* Function Name  : SPI1_SendData_IT
* Description    :
* Input          : pTx: puntero datos a enviar
* 				   size: cantidad de bytes a enviar
* Output         : None.
* Return         : 0: FAIL.
* 				   1: OK
*******************************************************************************/
void SPI1_Receive_IT(uint8_t * pRx, uint16_t size){

	while(hspi1.State != HAL_SPI_STATE_READY); //esperamos recibirlo
	HAL_SPI_Receive_IT(&hspi1, pRx, size);
	while(hspi1.State != HAL_SPI_STATE_READY); //esperamos recibirlo

}





