/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2021 STMicroelectronics International N.V. 
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "iwdg.h"
#include "bsp_tm7705.h"
#include "tim.h"
#include "uart1data.h"
#include "uart3data.h"
#include "uart4data.h"
#include "logicaldata.h"
#include "mqtt.h"
#include "ConfigModuleNoBlock.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId uart1Handle;
osThreadId uart3Handle;
osThreadId dataprocessHandle;
osThreadId uart4Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint16_t adc1, adc2; 
//互斥信号量句柄
SemaphoreHandle_t MutexSemaphore;	//互斥信号量
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
	//FreeRTOS时间统计所用的节拍计数器
  static unsigned long long FreeRTOSRunTimeTicks;
	FreeRTOSRunTimeTicks++;
  return FreeRTOSRunTimeTicks;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
 	//创建互斥信号量
	MutexSemaphore=xSemaphoreCreateMutex();      
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of uart1 */
  osThreadDef(uart1, StartTask02, osPriorityNormal, 0, 256);
  uart1Handle = osThreadCreate(osThread(uart1), NULL);

  /* definition and creation of uart3 */
  osThreadDef(uart3, StartTask03, osPriorityAboveNormal, 0, 256);
  uart3Handle = osThreadCreate(osThread(uart3), NULL);

  /* definition and creation of dataprocess */
  osThreadDef(dataprocess, StartTask04, osPriorityLow, 0, 128);
  dataprocessHandle = osThreadCreate(osThread(dataprocess), NULL);

  /* definition and creation of uart4 */
  osThreadDef(uart4, StartTask05, osPriorityBelowNormal, 0, 512);
  uart4Handle = osThreadCreate(osThread(uart4), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */

	bsp_StartAutoTimer(0,200);	/* 启动1个100ms的自动重装的定时器 */	
	bsp_InitTM7705();			      /* 初始化配置TM7705 */
	TM7705_CalibSelf(1);		    /* 自校准。执行时间较长，约180ms */		
	adc1 = TM7705_ReadAdc(1);		
	TM7705_CalibSelf(2);		    /* 自校准。执行时间较长，约180ms */
	adc2 = TM7705_ReadAdc(2);	

  /* Infinite loop */
  for(;;)
  {
		 xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取互斥信号量
		
		 if(bsp_CheckTimer(0))
		 {

		 	  adc1 = TM7705_ReadAdc(1);	/* 执行时间 80ms */		
	      adc2 = TM7705_ReadAdc(2);	/* 执行时间 80ms */	
			 	
		 }		 
	   xSemaphoreGive(MutexSemaphore);					//释放信号量
		 //xPortGetFreeHeapSize();
		 //uxTaskGetStackHighWaterMark(NULL);
		 //printf("ADTask%d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
		 
     osDelay(50);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the uart1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		Config_Param();//串口1配置打印数据
    osDelay(50);
		//printf("uart1Task%d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the uart3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
    RS485Work();// 		
    osDelay(50);
		//printf("uart3Task%d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the dataprocess thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
			/*喂狗，如果不喂狗，系统则会复位，LD1则会灭一次，如果在1s
			时间内准时喂狗的话，则LED1会常亮 */
     HAL_IWDG_Refresh(&hiwdg);   //喂狗
		 Io_Collect();//IO引脚状态采集
		 Count_Work();//计数逻辑判断
		 Count_Save();//计数保存实时数据 
		 AD_CAL();    // 计算物理量		
     //printf("dataprocessTask%d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));		
     osDelay(50);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the uart4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
//	char RunTimeInfo[400];		//保存任务运行时间信息
	mqtt_init(&mymqtt);
	mqtt_connect_reg(&mymqtt,MqttConnect);      //注册连接回调函数
	mqtt_disconnect_reg(&mymqtt,MqttDisConnect);//注册断开连接回调函数
	mqtt_received_reg(&mymqtt,MqttReceive);     //注册接收数据回调函数
  for(;;)
  {		
		ConfigModuleNoBlock();      //配置模块连接MQTT 		
		if(ConfigModuleNoBlockFlage)//(模块连接了MQTT)
		{
			mqtt_send_function(&mymqtt);//提取发送缓存的MQTT协议
			mqtt_keep_alive(&mymqtt);   //处理发送心跳包
		}
		Usart4Data();
		DTU_Work();//4G工作状态
		//printf("uart4Task%d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
//		vTaskGetRunTimeStats(RunTimeInfo);		//获取任务运行时间信息
//	  printf("任务名\t\t运行时间\t运行所占百分比\r\n");
//		printf("%s\r\n",RunTimeInfo);
    osDelay(50);
  }
  /* USER CODE END StartTask05 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
