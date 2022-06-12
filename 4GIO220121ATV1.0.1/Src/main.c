
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "malloc.h"	
#include "24cxx.h"
#include "uart1data.h"
#include "modbus.h"
#include "logicaldata.h"
#include "iap.h"	
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern char Version_Num[30];//�汾��
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void EEROM_Read(void)
{
   uint8_t temp=0;
   uint8_t databuf[30];
	 temp=AT24CXX_ReadOne(0);//����ÿ�ο�����дAT24CXX	 
   if(temp!=0X55)	
	 {
	      AT24CXX_WriteOne(0,0X55);	
		    AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);     //0:��       1:4-20ma   2:0-5V 
		    AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);     //0:��       1:4-20ma   2:0-5V 
        AT24CXX_WriteOne(RS485_Adresss,Device485_type); //�豸485����(�Ƿ������΢��)  0������  1��΢�˿��Ʋ�������	
        AT24CXX_WriteOne(CNC_UpType_Adress,CNC_UpType); //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�
		    AT24CXX_WriteOne(CNC_UpTime_Adress,CNC_UpTime); //1:��λ����	
			  AT24CXX_WriteOne(CNC_YELLOW_Adress ,CNC_YELLOW);//0:�������� 1:��˸����  2:�������
        AT24CXX_WriteOne(CNC_GREEN_Adress ,CNC_GREEN);  //0:�������� 1:��˸����
        AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED);       //0:�������� 1:��˸����
		 
			  memcpy(databuf,&TIM3_CAPTURE_VAL1,4);           //TIM3_CAPTURE_VAL1����ֵ����
			  AT24CXX_Write(TIM3_CAPTURE_VAL1_Adress,databuf,4);
		 
        memcpy(databuf,&AD1_PhyValue1,4);               //AD1��������Χ1
			  AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);	
		 
        memcpy(databuf,&AD1_PhyValue2,4);               //AD1��������Χ2
			  AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);		
		 
        memcpy(databuf,&AD2_PhyValue1,4);               //AD2��������Χ1
			  AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);	
				
        memcpy(databuf,&AD2_PhyValue2,4);               //AD2��������Χ2
			  AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);				 
		    				
				memcpy(databuf,&CNC_UpTime,2);                     //1:��λ��
			  AT24CXX_Write(CNC_UpTime_Adress,databuf,2);	
				
				AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);       //��վѯ�ʴӻ�����
				AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);   //��վѯ���׵�ַ

        memcpy(databuf,&StartAddr,2);                       //��ʼ��ַ
				AT24CXX_Write(StartAddr_Adress,databuf,2);		
				
        memcpy(databuf,&ValueOrLenth,2);                      //����or����д����Ĵ��������������ĶԱ�ֵ
				AT24CXX_Write(ValueOrLenth_Adress,databuf,2);	
				
        AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);       //Modbus�ӻ�ID
				
        AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate);     //Modbus���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
		    AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength); //Modbus��������λ  5-8
		    AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);     //Modbus����ֹͣλ  1-2
		    AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);         //Modbus����У��λ  1:��У�� 2:��У�� 3:żУ��
				
        memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //���岶׽�������½���֮��ʱ�䵥λms
        AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);	
        SetServerIP((char*) IP);
			  SetServerPort((char*)Port);     
			  SetUserName((char*)MQTTUserName);   
				SetPassWord((char*)MQTTPassWord); 

        AD1_Type=AT24CXX_ReadOne(AD1_Type_Adress);         //0:��       1:4-20ma   2:0-5V 
		    AD2_Type=AT24CXX_ReadOne(AD2_Type_Adress);         //0:��       1:4-20ma   2:0-5V 
        Device485_type=AT24CXX_ReadOne(RS485_Adresss);     //�豸485����(�Ƿ������΢��)  0������  1��΢�˿��Ʋ�������	
        CNC_UpType=AT24CXX_ReadOne(CNC_UpType_Adress);     //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�
	
       
		    CNC_YELLOW=AT24CXX_ReadOne(CNC_YELLOW_Adress);     //0:�������� 1:��˸����	
				CNC_GREEN=AT24CXX_ReadOne(CNC_GREEN_Adress);       //0:�������� 1:��˸����			
				CNC_RED=AT24CXX_ReadOne(CNC_RED_Adress);           //0:�������� 1:��˸����	
				AT24CXX_Read(TIM3_CAPTURE_VAL1_Adress,databuf,4);
				memcpy(&TIM3_CAPTURE_VAL1,databuf,4);
				
				AT24CXX_Read(AD1_PhyValue1_Adress,databuf,4);
				memcpy(&AD1_PhyValue1,databuf,4);				

				AT24CXX_Read(AD1_PhyValue2_Adress,databuf,4);
				memcpy(&AD1_PhyValue2,databuf,4);						

				AT24CXX_Read(AD2_PhyValue1_Adress,databuf,4);
				memcpy(&AD2_PhyValue1,databuf,4);				

				AT24CXX_Read(AD2_PhyValue2_Adress,databuf,4);
				memcpy(&AD2_PhyValue2,databuf,4);		
				
				AT24CXX_Read(CNC_UpTime_Adress,databuf,2);
				memcpy(&CNC_UpTime,databuf,2);					
	      if(CNC_UpType==2)
			  {
			     bsp_StartAutoTimer(1, CNC_UpTime*1000);	        /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */		
           bsp_StartAutoTimer(7, CNC_UpTime*1000);	        /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */						
			  }	
				
        Slave_Num=AT24CXX_ReadOne(Slave_Num_Adress);       //��վѯ�ʴӻ�����
        Master_ADDR=AT24CXX_ReadOne(Master_ADDR_Adress);   //��վѯ���׵�ַ	
	      Slave_ADDR=AT24CXX_ReadOne(Slave_ADDR_Adress);     //Modbus�ӻ�ID
				
				AT24CXX_Read(StartAddr_Adress,databuf,2);          //��ʼ��ַ
				memcpy(&StartAddr,databuf,2);	
				
				AT24CXX_Read(ValueOrLenth_Adress,databuf,2);      //����or����д����Ĵ��������������ĶԱ�ֵ
				memcpy(&ValueOrLenth,databuf,2);					
	
        ModbusBaudRate=AT24CXX_ReadOne(ModbusBaudRate_Adress);     //Modbus���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
			  ModbusWordLength=AT24CXX_ReadOne(ModbusWordLength_Adress); //Modbus��������λ  5-8	
				ModbusStopBits=AT24CXX_ReadOne(ModbusStopBits_Adress);     //Modbus����ֹͣλ  1-2
				ModbusParity=AT24CXX_ReadOne(ModbusParity_Adress);         //Modbus����У��λ  1:��У�� 2:��У�� 3:żУ��
				
				AT24CXX_Read(TIM3_Pulse_Threshold_Adress,databuf,2);      //���岶׽�������½���֮��ʱ�䵥λms
				memcpy(&TIM3_Pulse_Threshold,databuf,2);
				GetServerIP((char*)IP);
				GetServerPort((char*) Port);			
				GetUserName((char*)MQTTUserName);
				GetPassWord((char*)MQTTPassWord);
			  printf("%s\r\n", "24cxx�ⲿeerom_��ʼ��!");	
	 }
	 else 
	 {
        AD1_Type=AT24CXX_ReadOne(AD1_Type_Adress);        //0:��       1:4-20ma   2:0-5V 
		    AD2_Type=AT24CXX_ReadOne(AD2_Type_Adress);        //0:��       1:4-20ma   2:0-5V 			
       	Device485_type=AT24CXX_ReadOne(RS485_Adresss);    //�豸485����(�Ƿ������΢��)  0������  1��΢�˿��Ʋ�������	
        CNC_UpType=AT24CXX_ReadOne(CNC_UpType_Adress);     //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�

		    CNC_YELLOW=AT24CXX_ReadOne(CNC_YELLOW_Adress);     //0:�������� 1:��˸����	
				CNC_GREEN=AT24CXX_ReadOne(CNC_GREEN_Adress);       //0:�������� 1:��˸����			
				CNC_RED=AT24CXX_ReadOne(CNC_RED_Adress);           //0:�������� 1:��˸����	
				AT24CXX_Read(TIM3_CAPTURE_VAL1_Adress,databuf,4);
				memcpy(&TIM3_CAPTURE_VAL1,databuf,4);	

				AT24CXX_Read(AD1_PhyValue1_Adress,databuf,4);
				memcpy(&AD1_PhyValue1,databuf,4);				

				AT24CXX_Read(AD1_PhyValue2_Adress,databuf,4);
				memcpy(&AD1_PhyValue2,databuf,4);						

				AT24CXX_Read(AD2_PhyValue1_Adress,databuf,4);
				memcpy(&AD2_PhyValue1,databuf,4);				

				AT24CXX_Read(AD2_PhyValue2_Adress,databuf,4);
				memcpy(&AD2_PhyValue2,databuf,4);	

       	AT24CXX_Read(CNC_UpTime_Adress,databuf,2);
				memcpy(&CNC_UpTime,databuf,2);
        if(CNC_UpType==2)
			  {
			     bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */	
           bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */							
			  }
				
        Slave_Num=AT24CXX_ReadOne(Slave_Num_Adress);       //��վѯ�ʴӻ�����
        Master_ADDR=AT24CXX_ReadOne(Master_ADDR_Adress);   //��վѯ���׵�ַ	
	      Slave_ADDR=AT24CXX_ReadOne(Slave_ADDR_Adress);     //Modbus�ӻ�ID
				
				AT24CXX_Read(StartAddr_Adress,databuf,2);          //��ʼ��ַ
				memcpy(&StartAddr,databuf,2);	
				
				AT24CXX_Read(ValueOrLenth_Adress,databuf,2);      //����or����д����Ĵ��������������ĶԱ�ֵ
				memcpy(&ValueOrLenth,databuf,2);	

        ModbusBaudRate=AT24CXX_ReadOne(ModbusBaudRate_Adress); //Modbus���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
			  ModbusWordLength=AT24CXX_ReadOne(ModbusWordLength_Adress); //Modbus��������λ  5-8	
				ModbusStopBits=AT24CXX_ReadOne(ModbusStopBits_Adress);     //Modbus����ֹͣλ  1-2
				ModbusParity=AT24CXX_ReadOne(ModbusParity_Adress);         //Modbus����У��λ  1:��У�� 2:��У�� 3:żУ��	

				AT24CXX_Read(TIM3_Pulse_Threshold_Adress,databuf,2);      //���岶׽�������½���֮��ʱ�䵥λms
				memcpy(&TIM3_Pulse_Threshold,databuf,2);	
				GetServerIP((char*)IP);
				GetServerPort((char*) Port);
				GetUserName((char*)MQTTUserName);
				GetPassWord((char*)MQTTPassWord);
       			
	 }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	 //INTX_ENABLE();                     //�����ж�	  
   SCB->VTOR = FLASH_BASE | 0x10000;  /* Vector Table Relocation in Internal FLASH. */ 
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��

	if(AT24CXX_Check()==1)   //��ⲻ��24c02
	{		
		  EEROM_Read();
	    printf("%s\r\n", "24cxx�ⲿeerom_OK!");			
	}
	else
	{
	     printf("%s\r\n", "24cxx�ⲿeerom_Fail!");	
	}
	
  MX_RTCBSP_Init();
	MX_USART3BSP_UART_Init();
	  /* ʹ�ܽ��գ������жϻص����� */
   HAL_UART_Receive_IT(&huart1,(uint8_t *)aRxBuffer, RXBUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	 HAL_UART_Receive_IT(&huart3,(uint8_t *)aRx3Buffer, RX3BUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	 HAL_UART_Receive_IT(&huart4,(uint8_t *)aRx4Buffer, RX4BUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
		//��ʱ2����
	 HAL_TIM_Base_Start_IT(&htim2);
	 HAL_TIM_Base_Start_IT(&htim3); 
	
	 if(CNC_YELLOW==2){		 
		 HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);   //����TIM3�Ĳ���ͨ��2�����ҿ��������ж�	
	 }
	 else
	 {
	   HAL_TIM_IC_Stop(&htim3,TIM_CHANNEL_2);   //�ر�TIM3�Ĳ���ͨ��2�����ҹرղ����ж�	
	 }
	bsp_StartAutoTimer(2, 6*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� ���ض�ʱ��ʾ*/	
  printf("\r\n\r\n*******************��ʼ�����û�����*******************\r\n");
	printf("Ӳ���汾:%d\r\n",IAPGetHardwareVersion());
	printf("�豸�ͺ�:%s\r\n",DeviceModel);
	printf("�̼��汾:%s\r\n", Version_Num);	
  printf("ServerIP:%s\r\n", IP);		
  printf("ServerPort:%s\r\n",Port);	
  printf("UserName:%s\r\n", MQTTUserName);
  printf("PassWord:%s\r\n", MQTTPassWord);	 
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */
//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
uint16_t   TIM3CH2_CAPTURE_STA1=0;	    //���벶��״̬		   
uint16_t	 TIM3CH2_CAPTURE_VAL1=0;	    //���벶��ֵ(TIM2/TIM5��32λ)
/**
  * ��������: ��ʱ�����벶���жϻص�����
  * �������: htim����ʱ�����
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	
	if(htim->Instance == TIM3) {
		
		 if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)	
		 {
					if(TIM3CH2_CAPTURE_STA1&0X40)		//����һ���½��� 		
					{	  			
						TIM3CH2_CAPTURE_STA1|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����							
						TIM3CH2_CAPTURE_VAL1=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2);//��ȡ��ǰ�Ĳ���ֵ.						
						TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2);                    //һ��Ҫ�����ԭ�������ã���
						TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);//����TIM2ͨ��1�����ز���					
					}
					else  								                     //��δ��ʼ,��һ�β���������
					{
						TIM3CH2_CAPTURE_STA1=0;			             //���
						TIM3CH2_CAPTURE_VAL1=0;
						TIM3CH2_CAPTURE_STA1|=0X40;		           //��ǲ�����������	
							
						// ����жϱ�־λ
//						__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC2);						
//						__HAL_TIM_DISABLE(&htim3);              //�رն�ʱ��2
						__HAL_TIM_SET_COUNTER(&htim3,0);
						TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2);   //һ��Ҫ�����ԭ�������ã���							
						TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);//��ʱ��2ͨ��1����Ϊ�½��ز���
	//							__HAL_TIM_ENABLE(&htim3);//ʹ�ܶ�ʱ��3
	//							 �������벶�񲢿����ж�
						HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);   //����TIM5�Ĳ���ͨ��1�����ҿ��������ж�
					}		    
		 }			 
	 }	
	
	
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	static uint16_t count;
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM2) {
   	if(count++>=500)
		{
		   		HAL_GPIO_TogglePin(DOG_GPIO_Port,DOG_Pin);
		      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			    count=0;
		}

  }
  if (htim->Instance == TIM3) {
		
			if((TIM3CH2_CAPTURE_STA1&0X80)==0)//��δ�ɹ�����
			{
					if(TIM3CH2_CAPTURE_STA1&0X40)//�Ѿ����񵽸ߵ�ƽ��
					{
						if((TIM3CH2_CAPTURE_STA1&0X3F)==0x3F)//�ߵ�ƽ̫����
						{							
							//TIM3CH2_CAPTURE_STA1|=0X80;	     //��ǲ�����������	
							//TIM3CH2_CAPTURE_VAL1=0XFFFF;
						}
						else 
						{
						   TIM3CH2_CAPTURE_STA1++;
							 //printf("TIM3CH2_CAPTURE_STA1:%d \r\n",TIM3CH2_CAPTURE_STA1);//��ӡ�ܵĸߵ�ƽʱ��
						}
							
					}	 
			}   
  }

  
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
