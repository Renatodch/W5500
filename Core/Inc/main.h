/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"


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
#include "WebServer.h"
#include "TcpClientConn.h"
#include "TcpServerConn.h"

/********Includes de Referencia*************/
#include "eeprom.h"
#include "Pagina.h"
/*****************************************/

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
void T(char * format,...);

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define TX_RX_MAX_BUF_SIZE	4096//2048


extern SPI_HandleTypeDef hspi1;
extern IWDG_HandleTypeDef hiwdg;


extern DhcpConnection dhcpc;
extern DnsConnection dnsc;
extern TcpClient	devtcc;
extern ServerConnection websc;
extern WebServer webServer;

extern uint8_t TX_BUF[TX_RX_MAX_BUF_SIZE];
extern uint8_t IpServer[4]; //mi pc
extern uint16_t Port;
extern uint8_t ch_status[MAX_SOCK_NUM];

extern uint8_t DNS_Server_1[4];//1st DNS server
extern uint8_t DNS_Server_2[4];//Secondary server


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define W_int_Pin GPIO_PIN_0
#define W_int_GPIO_Port GPIOA
#define LED_L4_Pin GPIO_PIN_1
#define LED_L4_GPIO_Port GPIOA
#define Cs_Pin GPIO_PIN_4
#define Cs_GPIO_Port GPIOA
#define Out_02_Pin GPIO_PIN_5
#define Out_02_GPIO_Port GPIOC
#define Out_01_Pin GPIO_PIN_0
#define Out_01_GPIO_Port GPIOB
#define In_01_Pin GPIO_PIN_1
#define In_01_GPIO_Port GPIOB
#define In_02_Pin GPIO_PIN_2
#define In_02_GPIO_Port GPIOB
#define LED_CPU_Pin GPIO_PIN_6
#define LED_CPU_GPIO_Port GPIOC
#define Out_03_Pin GPIO_PIN_7
#define Out_03_GPIO_Port GPIOC
#define LED_L2_Pin GPIO_PIN_8
#define LED_L2_GPIO_Port GPIOC
#define LED_LAN_Pin GPIO_PIN_9
#define LED_LAN_GPIO_Port GPIOC
#define In_03_Pin GPIO_PIN_8
#define In_03_GPIO_Port GPIOA
#define nPwrDN_Pin GPIO_PIN_15
#define nPwrDN_GPIO_Port GPIOA
#define nRESET_Pin GPIO_PIN_12
#define nRESET_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_3
#define Buzzer_GPIO_Port GPIOB
#define LED_PLC_Pin GPIO_PIN_4
#define LED_PLC_GPIO_Port GPIOB
#define In_06_Pin GPIO_PIN_6
#define In_06_GPIO_Port GPIOB
#define LED_L1_Pin GPIO_PIN_9
#define LED_L1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
