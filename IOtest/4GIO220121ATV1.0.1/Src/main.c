
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
extern char Version_Num[30];//版本号
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void EEROM_Read(void)
{
   uint8_t temp=0;
   uint8_t databuf[30];
	 temp=AT24CXX_ReadOne(0);//避免每次开机都写AT24CXX	 
   if(temp!=0X55)	
	 {
	      AT24CXX_WriteOne(0,0X55);	
		    AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);     //0:关       1:4-20ma   2:0-5V 
		    AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);     //0:关       1:4-20ma   2:0-5V 
        AT24CXX_WriteOne(RS485_Adresss,Device485_type); //设备485类型(是否带智能微断)  0：不用  1：微端控制不带计量	
        AT24CXX_WriteOne(CNC_UpType_Adress,CNC_UpType); //0:不上报 1:变化上报 2:定时上报
		    AT24CXX_WriteOne(CNC_UpTime_Adress,CNC_UpTime); //1:单位分钟	
			  AT24CXX_WriteOne(CNC_YELLOW_Adress ,CNC_YELLOW);//0:常亮结束 1:闪烁结束  2:脉冲计数
        AT24CXX_WriteOne(CNC_GREEN_Adress ,CNC_GREEN);  //0:常亮结束 1:闪烁运行
        AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED);       //0:常亮故障 1:闪烁故障
		 
			  memcpy(databuf,&TIM3_CAPTURE_VAL1,4);           //TIM3_CAPTURE_VAL1计数值清零
			  AT24CXX_Write(TIM3_CAPTURE_VAL1_Adress,databuf,4);
		 
        memcpy(databuf,&AD1_PhyValue1,4);               //AD1物理量范围1
			  AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);	
		 
        memcpy(databuf,&AD1_PhyValue2,4);               //AD1物理量范围2
			  AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);		
		 
        memcpy(databuf,&AD2_PhyValue1,4);               //AD2物理量范围1
			  AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);	
				
        memcpy(databuf,&AD2_PhyValue2,4);               //AD2物理量范围2
			  AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);				 
		    				
				memcpy(databuf,&CNC_UpTime,2);                     //1:单位秒
			  AT24CXX_Write(CNC_UpTime_Adress,databuf,2);	
				
				AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);       //主站询问从机个数
				AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);   //主站询问首地址

        memcpy(databuf,&StartAddr,2);                       //起始地址
				AT24CXX_Write(StartAddr_Adress,databuf,2);		
				
        memcpy(databuf,&ValueOrLenth,2);                      //数据or长度写多个寄存器返回数据量的对比值
				AT24CXX_Write(ValueOrLenth_Adress,databuf,2);	
				
        AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);       //Modbus从机ID
				
        AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate);     //Modbus串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
		    AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength); //Modbus串口数据位  5-8
		    AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);     //Modbus串口停止位  1-2
		    AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);         //Modbus串口校验位  1:无校验 2:奇校验 3:偶校验
				
        memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //脉冲捕捉上升和下降沿之间时间单位ms
        AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);	
        SetServerIP((char*) IP);
			  SetServerPort((char*)Port);     
			  SetUserName((char*)MQTTUserName);   
				SetPassWord((char*)MQTTPassWord); 

        AD1_Type=AT24CXX_ReadOne(AD1_Type_Adress);         //0:关       1:4-20ma   2:0-5V 
		    AD2_Type=AT24CXX_ReadOne(AD2_Type_Adress);         //0:关       1:4-20ma   2:0-5V 
        Device485_type=AT24CXX_ReadOne(RS485_Adresss);     //设备485类型(是否带智能微断)  0：不用  1：微端控制不带计量	
        CNC_UpType=AT24CXX_ReadOne(CNC_UpType_Adress);     //0:不上报 1:变化上报 2:定时上报
	
       
		    CNC_YELLOW=AT24CXX_ReadOne(CNC_YELLOW_Adress);     //0:常亮结束 1:闪烁结束	
				CNC_GREEN=AT24CXX_ReadOne(CNC_GREEN_Adress);       //0:常亮结束 1:闪烁运行			
				CNC_RED=AT24CXX_ReadOne(CNC_RED_Adress);           //0:常亮结束 1:闪烁故障	
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
			     bsp_StartAutoTimer(1, CNC_UpTime*1000);	        /* 启动1个1000ms的自动重装的定时器 */		
           bsp_StartAutoTimer(7, CNC_UpTime*1000);	        /* 启动1个1000ms的自动重装的定时器定时上报计数 */						
			  }	
				
        Slave_Num=AT24CXX_ReadOne(Slave_Num_Adress);       //主站询问从机个数
        Master_ADDR=AT24CXX_ReadOne(Master_ADDR_Adress);   //主站询问首地址	
	      Slave_ADDR=AT24CXX_ReadOne(Slave_ADDR_Adress);     //Modbus从机ID
				
				AT24CXX_Read(StartAddr_Adress,databuf,2);          //起始地址
				memcpy(&StartAddr,databuf,2);	
				
				AT24CXX_Read(ValueOrLenth_Adress,databuf,2);      //数据or长度写多个寄存器返回数据量的对比值
				memcpy(&ValueOrLenth,databuf,2);					
	
        ModbusBaudRate=AT24CXX_ReadOne(ModbusBaudRate_Adress);     //Modbus串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
			  ModbusWordLength=AT24CXX_ReadOne(ModbusWordLength_Adress); //Modbus串口数据位  5-8	
				ModbusStopBits=AT24CXX_ReadOne(ModbusStopBits_Adress);     //Modbus串口停止位  1-2
				ModbusParity=AT24CXX_ReadOne(ModbusParity_Adress);         //Modbus串口校验位  1:无校验 2:奇校验 3:偶校验
				
				AT24CXX_Read(TIM3_Pulse_Threshold_Adress,databuf,2);      //脉冲捕捉上升和下降沿之间时间单位ms
				memcpy(&TIM3_Pulse_Threshold,databuf,2);
				GetServerIP((char*)IP);
				GetServerPort((char*) Port);			
				GetUserName((char*)MQTTUserName);
				GetPassWord((char*)MQTTPassWord);
			  printf("%s\r\n", "24cxx外部eerom_初始化!");	
	 }
	 else 
	 {
        AD1_Type=AT24CXX_ReadOne(AD1_Type_Adress);        //0:关       1:4-20ma   2:0-5V 
		    AD2_Type=AT24CXX_ReadOne(AD2_Type_Adress);        //0:关       1:4-20ma   2:0-5V 			
       	Device485_type=AT24CXX_ReadOne(RS485_Adresss);    //设备485类型(是否带智能微断)  0：不用  1：微端控制不带计量	
        CNC_UpType=AT24CXX_ReadOne(CNC_UpType_Adress);     //0:不上报 1:变化上报 2:定时上报

		    CNC_YELLOW=AT24CXX_ReadOne(CNC_YELLOW_Adress);     //0:常亮结束 1:闪烁结束	
				CNC_GREEN=AT24CXX_ReadOne(CNC_GREEN_Adress);       //0:常亮结束 1:闪烁运行			
				CNC_RED=AT24CXX_ReadOne(CNC_RED_Adress);           //0:常亮结束 1:闪烁故障	
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
			     bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器定时上报计数 */	
           bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器定时上报计数 */							
			  }
				
        Slave_Num=AT24CXX_ReadOne(Slave_Num_Adress);       //主站询问从机个数
        Master_ADDR=AT24CXX_ReadOne(Master_ADDR_Adress);   //主站询问首地址	
	      Slave_ADDR=AT24CXX_ReadOne(Slave_ADDR_Adress);     //Modbus从机ID
				
				AT24CXX_Read(StartAddr_Adress,databuf,2);          //起始地址
				memcpy(&StartAddr,databuf,2);	
				
				AT24CXX_Read(ValueOrLenth_Adress,databuf,2);      //数据or长度写多个寄存器返回数据量的对比值
				memcpy(&ValueOrLenth,databuf,2);	

        ModbusBaudRate=AT24CXX_ReadOne(ModbusBaudRate_Adress); //Modbus串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
			  ModbusWordLength=AT24CXX_ReadOne(ModbusWordLength_Adress); //Modbus串口数据位  5-8	
				ModbusStopBits=AT24CXX_ReadOne(ModbusStopBits_Adress);     //Modbus串口停止位  1-2
				ModbusParity=AT24CXX_ReadOne(ModbusParity_Adress);         //Modbus串口校验位  1:无校验 2:奇校验 3:偶校验	

				AT24CXX_Read(TIM3_Pulse_Threshold_Adress,databuf,2);      //脉冲捕捉上升和下降沿之间时间单位ms
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
	 //INTX_ENABLE();                     //开启中断	  
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
  my_mem_init(SRAMIN);			//初始化内部内存池

	if(AT24CXX_Check()==1)   //检测不到24c02
	{		
		  EEROM_Read();
	    printf("%s\r\n", "24cxx外部eerom_OK!");			
	}
	else
	{
	     printf("%s\r\n", "24cxx外部eerom_Fail!");	
	}
	
  MX_RTCBSP_Init();
	MX_USART3BSP_UART_Init();
	  /* 使能接收，进入中断回调函数 */
   HAL_UART_Receive_IT(&huart1,(uint8_t *)aRxBuffer, RXBUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	 HAL_UART_Receive_IT(&huart3,(uint8_t *)aRx3Buffer, RX3BUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	 HAL_UART_Receive_IT(&huart4,(uint8_t *)aRx4Buffer, RX4BUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
		//定时2启动
	 HAL_TIM_Base_Start_IT(&htim2);
	 HAL_TIM_Base_Start_IT(&htim3); 
	
	 if(CNC_YELLOW==2){		 
		 HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);   //开启TIM3的捕获通道2，并且开启捕获中断	
	 }
	 else
	 {
	   HAL_TIM_IC_Stop(&htim3,TIM_CHANNEL_2);   //关闭TIM3的捕获通道2，并且关闭捕获中断	
	 }
	bsp_StartAutoTimer(2, 6*1000);	    /* 启动1个1000ms的自动重装的定时器 本地定时显示*/	
  printf("\r\n\r\n*******************开始运行用户程序*******************\r\n");
	printf("硬件版本:%d\r\n",IAPGetHardwareVersion());
	printf("设备型号:%s\r\n",DeviceModel);
	printf("固件版本:%s\r\n", Version_Num);	
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
//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
uint16_t   TIM3CH2_CAPTURE_STA1=0;	    //输入捕获状态		   
uint16_t	 TIM3CH2_CAPTURE_VAL1=0;	    //输入捕获值(TIM2/TIM5是32位)
/**
  * 函数功能: 定时器输入捕获中断回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	
	if(htim->Instance == TIM3) {
		
		 if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)	
		 {
					if(TIM3CH2_CAPTURE_STA1&0X40)		//捕获到一个下降沿 		
					{	  			
						TIM3CH2_CAPTURE_STA1|=0X80;		//标记成功捕获到一次高电平脉宽							
						TIM3CH2_CAPTURE_VAL1=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2);//获取当前的捕获值.						
						TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2);                    //一定要先清除原来的设置！！
						TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);//配置TIM2通道1上升沿捕获					
					}
					else  								                     //还未开始,第一次捕获上升沿
					{
						TIM3CH2_CAPTURE_STA1=0;			             //清空
						TIM3CH2_CAPTURE_VAL1=0;
						TIM3CH2_CAPTURE_STA1|=0X40;		           //标记捕获到了上升沿	
							
						// 清除中断标志位
//						__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC2);						
//						__HAL_TIM_DISABLE(&htim3);              //关闭定时器2
						__HAL_TIM_SET_COUNTER(&htim3,0);
						TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2);   //一定要先清除原来的设置！！							
						TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);//定时器2通道1设置为下降沿捕获
	//							__HAL_TIM_ENABLE(&htim3);//使能定时器3
	//							 启动输入捕获并开启中断
						HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);   //开启TIM5的捕获通道1，并且开启捕获中断
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
		
			if((TIM3CH2_CAPTURE_STA1&0X80)==0)//还未成功捕获
			{
					if(TIM3CH2_CAPTURE_STA1&0X40)//已经捕获到高电平了
					{
						if((TIM3CH2_CAPTURE_STA1&0X3F)==0x3F)//高电平太长了
						{							
							//TIM3CH2_CAPTURE_STA1|=0X80;	     //标记捕获到了上升沿	
							//TIM3CH2_CAPTURE_VAL1=0XFFFF;
						}
						else 
						{
						   TIM3CH2_CAPTURE_STA1++;
							 //printf("TIM3CH2_CAPTURE_STA1:%d \r\n",TIM3CH2_CAPTURE_STA1);//打印总的高点平时间
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
