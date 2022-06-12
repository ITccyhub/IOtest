/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2022 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */

#define EN_USART1_RX 			  1		             //使能（1）/禁止（0）串口1接收
#define USART_REC_LEN  			200  	           //定义最大接收字节数 200
#define RXBUFFERSIZE        1                //缓存大小
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART_RX_STA;         		   //接收状态标记	
extern uint8_t aRxBuffer[RXBUFFERSIZE];      //HAL库USART接收Buffer

#define RS485_TX3_EN		HAL_GPIO_WritePin(DIR_485_GPIO_Port, DIR_485_Pin, GPIO_PIN_SET);	//485模式控制.0,接收;1,发送.
#define RS485_RX3_EN	  HAL_GPIO_WritePin(DIR_485_GPIO_Port, DIR_485_Pin, GPIO_PIN_RESET);	  //485模式控制.0,接收;1,发送.
#define EN_USART3_RX 			   1		               //使能（1）/禁止（0）串口1接收
#define USART3_REC_LEN  		200  	               //定义最大接收字节数 200
#define RX3BUFFERSIZE        1                   //缓存大小
extern uint8_t  USART3_RX_BUF[USART3_REC_LEN];   //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART3_RX_STA;         		       //接收状态标记	
extern uint8_t aRx3Buffer[RX3BUFFERSIZE];        //HAL库USART接收Buffer

#define EN_USART4_RX 			   1		               //使能（1）/禁止（0）串口1接收
#define USART4_REC_LEN  		1460 	               //定义最大接收字节数 200
#define RX4BUFFERSIZE        1                   //缓存大小
extern uint8_t  USART4_RX_BUF[USART4_REC_LEN];   //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART4_RX_STA;         		       //接收状态标记	
extern uint8_t aRx4Buffer[RX4BUFFERSIZE];        //HAL库USART接收Buffer
extern unsigned char usart4_read_buff_copy[USART4_REC_LEN];//从缓存拷贝数据使用
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void _Error_Handler(char *, int);

void MX_UART4_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void COM_Send(UART_HandleTypeDef *huart,uint8_t* pSendBuf,uint16_t len);
void MX_USART3BSP_UART_Init(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
