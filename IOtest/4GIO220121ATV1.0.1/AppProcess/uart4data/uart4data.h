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

extern uint8_t Updata_Flag;           //上传标志 

void DTU_Work(void);             //4G模块工作步骤
void MqttConnect(void);          //连接上MQTT回调
void MqttDisConnect(void);       //MQTT断开连接回调
void subscribedCb(int pdata);    //订阅成功回调
void failsubscribedCb(int pdata);//订阅失败回调
void PublishedCb(void);          //成功发布消息回调
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//接收到数据回调
void Usart4Data(void);


#endif
