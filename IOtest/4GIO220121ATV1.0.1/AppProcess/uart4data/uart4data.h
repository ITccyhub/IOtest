#ifndef _UART4DATA_H
#define _UART4DATA_H
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
#include "uart1data.h"
#include "dtu.h"
#include "iap.h"	
#include "ConfigModuleNoBlock.h"
#include "mqtt.h"

extern uint8_t Updata_Flag;           //�ϴ���־ 

void DTU_Work(void);             //4Gģ�鹤������
void MqttConnect(void);          //������MQTT�ص�
void MqttDisConnect(void);       //MQTT�Ͽ����ӻص�
void subscribedCb(int pdata);    //���ĳɹ��ص�
void failsubscribedCb(int pdata);//����ʧ�ܻص�
void PublishedCb(void);          //�ɹ�������Ϣ�ص�
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//���յ����ݻص�
void Usart4Data(void);


#endif
