#ifndef _UART1DATA_H
#define _UART1DATA_H

#include "usart.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "24cxx.h"
#include "rtc.h"
#include "tim.h"
#include "cJSON.h"
#include "malloc.h"
#include "modbus.h"
#include "uart3data.h"
#include "logicaldata.h"
#include "iap.h"
extern uint8_t GREEN_Value;          //�̵�ֵ
extern uint8_t RED_Value;            //���ֵ

extern uint8_t IN1;            //IN1ֵ
extern uint8_t IN2;            //IN2ֵ
extern uint8_t IN3;            //IN3ֵ

extern uint8_t DTU_Heart;       //DTU����״̬
extern uint16_t adc1, adc2; 
extern float AD1_Value;
extern float AD2_Value;
extern float AD1_PhyValue;     //������ֵ
extern float AD2_PhyValue;     //������ֵ

/*****************************��EEROM����*************************/

extern uint8_t Device485_type;  //�豸485����  0:����  1:MODBUS��վ 2:����Э�� 
extern uint8_t CNC_UpType;      //0:���ϱ�     1:�仯�ϱ� 2:��ʱ�ϱ�
extern uint16_t CNC_UpTime;     //1:��λ��
extern uint8_t CNC_YELLOW;      //0:��������   1:��˸����  2:�������
extern uint8_t CNC_GREEN;       //0:��������   1:��˸����
extern uint8_t CNC_RED;         //0:��������   1:��˸����
extern uint8_t AD1_Type;        //0:4-20ma   1:0-5V 
extern uint8_t AD2_Type;        //0:4-20ma   1:0-5V 
extern float AD1_PhyValue1;     //��������Χ1
extern float AD1_PhyValue2;     //��������Χ2
extern float AD2_PhyValue1;     //��������Χ1
extern float AD2_PhyValue2;     //��������Χ2
extern uint32_t TIM3_CAPTURE_VAL1;     //��ʱ��3ͨ��1����


extern uint32_t FileSize;       //OTA�ļ���С
extern uint8_t  UpdateFlage;    //������־
void Config_Param(void);//���ò���

#endif
