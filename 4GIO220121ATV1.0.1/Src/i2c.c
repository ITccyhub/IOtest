/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
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
#include "i2c.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#define EVAL_I2Cx_TIMEOUT_MAX                   3000
uint32_t I2cxTimeout = EVAL_I2Cx_TIMEOUT_MAX;
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/**
  * ��������: I2Cͨ�Ŵ�������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: һ����I2Cͨ�ų�ʱʱ���øú���
  */
static void I2C1_Error (void)
{
  /* ����ʼ��I2Cͨ������ */
  HAL_I2C_DeInit(&hi2c1);
  
  /* ���³�ʼ��I2Cͨ������*/
  MX_I2C1_Init();
  printf("I2Cͨ�ų�ʱ������ ��������I2C...\n");
}

/**
  * ��������: ͨ��I2Cд��һ��ֵ��ָ���Ĵ�����
  * �������: Addr��I2C�豸��ַ
  *           Reg��Ŀ��Ĵ���
  *           Value��ֵ
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void I2C1_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  HAL_Delay(10);
  /* ���I2Cͨ��״̬ */
  if(status != HAL_OK)
  {
    /* ����I2Cͨ�Ŵ������� */
    I2C1_Error();
  }
}

/**
  * ��������: ͨ��I2Cд��һ�����ݵ�ָ���Ĵ�����
  * �������: Addr��I2C�豸��ַ
  *           Reg��Ŀ��Ĵ���
  *           RegSize���Ĵ����ߴ�(8λ����16λ)
  *           pBuffer��������ָ��
  *           Length������������
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��ѭ�����������һ����ʱʱ��
  */
HAL_StatusTypeDef I2C1_WriteBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout); 
  HAL_Delay(10);
  /* ���I2Cͨ��״̬ */
  if(status != HAL_OK)
  {
    /* ����I2Cͨ�Ŵ������� */
      I2C1_Error();
  }        
  return status;
}


/**
  * ��������: ͨ��I2C��ȡһ��ָ���Ĵ�������
  * �������: Addr��I2C�豸��ַ
  *           Reg��Ŀ��Ĵ���
  * �� �� ֵ: uint8_t���Ĵ�������
  * ˵    ��: ��
  */
uint8_t I2C1_ReadData(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
  HAL_Delay(10); 
  /* ���I2Cͨ��״̬ */
  if(status != HAL_OK)
  {
    /* ����I2Cͨ�Ŵ������� */
     I2C1_Error();
  
  }
  return value;
}

/**
  * ��������: ͨ��I2C��ȡһ�μĴ������ݴ�ŵ�ָ���Ļ�������
  * �������: Addr��I2C�豸��ַ
  *           Reg��Ŀ��Ĵ���
  *           RegSize���Ĵ����ߴ�(8λ����16λ)
  *           pBuffer��������ָ��
  *           Length������������
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��
  */
HAL_StatusTypeDef I2C1_ReadBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout);
  
  /* ���I2Cͨ��״̬ */
  if(status != HAL_OK)
  {
    /* ����I2Cͨ�Ŵ������� */
    I2C1_Error();
  }        
  return status;
}

/**
  * ��������: ���I2C�豸�Ƿ���׼���ÿ���ͨ��״̬
  * �������: DevAddress��I2C�豸��ַ
  *           Trials�����Բ��Դ���
  * �� �� ֵ: HAL_StatusTypeDef���������
  * ˵    ��: ��
  */
HAL_StatusTypeDef I2C1_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, I2cxTimeout));
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
