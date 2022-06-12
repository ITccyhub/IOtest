/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
//#define HAL_RTC_SetDate(a, b, c) HAL_OK
//#define HAL_RTC_SetTime(a, b, c) HAL_OK
#include "uart4data.h"
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

RTC_DateTypeDef datestructure;
RTC_TimeTypeDef timestructure;

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 23;
  sTime.Minutes = 1;
  sTime.Seconds = 1;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 21;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */
 
	 //__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //开启RTC时钟秒中断
	
  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */
 
  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void MX_RTCBSP_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

  /* USER CODE BEGIN RTC_Init 1 */
	__HAL_RCC_BKP_CLK_ENABLE();       //开启后备区域时钟
	__HAL_RCC_PWR_CLK_ENABLE();		    //开启电源时钟
  /* USER CODE END RTC_Init 1 */
  /**Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
     _Error_Handler(__FILE__, __LINE__);
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x5051)
	{
		
			/* USER CODE END Check_RTC_BKUP */

			/**Initialize RTC and set the Time and Date 
			*/
			sTime.Hours = 1;
			sTime.Minutes = 1;
			sTime.Seconds = 1;

			if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
			{
				 _Error_Handler(__FILE__, __LINE__);
			}
			DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
			DateToUpdate.Month = 1;
			DateToUpdate.Date = 1;
			DateToUpdate.Year = 21;

			if(HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
			{
				 _Error_Handler(__FILE__, __LINE__);
			}
			/* USER CODE BEGIN RTC_Init 2 */
			
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)DateToUpdate.Year);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)DateToUpdate.Month);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)DateToUpdate.Date);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)DateToUpdate.WeekDay);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//向指定的后备区域寄存器写入数据
      __HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //开启RTC时钟秒中断			
  }
	else
	{
		  DateToUpdate.Year    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
			DateToUpdate.Month   = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
			DateToUpdate.Date    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
			DateToUpdate.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
			HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
		  __HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //开启RTC时钟秒中断		
	}
	
  /* USER CODE END RTC_Init 2 */
  
}


void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
		if(hrtc->Instance == RTC)
		{			  
        HAL_RTC_GetTime(hrtc, &timestructure, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(hrtc, &datestructure, RTC_FORMAT_BIN);
       	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR2, (uint16_t)datestructure.Year);
				HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR3, (uint16_t)datestructure.Month);
				HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR4, (uint16_t)datestructure.Date);
				HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR5, (uint16_t)datestructure.WeekDay);	
        if((timestructure.Minutes%5==0)&&(timestructure.Seconds==0))//整点5分钟计时时间到
				{
				    Updata_Flag=1;				
				}
			
		} 
	    
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
