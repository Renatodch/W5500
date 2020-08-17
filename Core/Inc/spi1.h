/*
 * spi1.h
 *
 *  Created on: 12 ago. 2020
 *      Author: Renato
 */

#ifndef INC_SPI1_H_
#define INC_SPI1_H_

#include "stm32f1xx_hal.h"

#define MAX_TRASMIT_SIZE 200
#define MAX_RECEIVE_SIZE 200
#define MAX_SIZE (MAX_TRASMIT_SIZE + MAX_RECEIVE_SIZE)

#define CS_pin			(uint32_t)(1<<4)																											//
#define CS_port			GPIOA																														//


#define cs_low()		(CS_port->BRR = CS_pin)
#define cs_high()		(CS_port->BSRR = CS_pin)

void SPI1_SendReceive_IT(uint8_t * pTx,uint8_t * pRx, uint16_t size); //tx+rx sizes
void SPI1_Send_IT(uint8_t *pTx, uint16_t size);
void SPI1_Receive_IT(uint8_t * pRx, uint16_t size);
#endif /* INC_SPI1_H_ */
