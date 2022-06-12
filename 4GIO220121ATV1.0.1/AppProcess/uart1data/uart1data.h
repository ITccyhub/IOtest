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
extern uint8_t GREEN_Value;          //绿灯值
extern uint8_t RED_Value;            //红灯值

extern uint8_t IN1;            //IN1值
extern uint8_t IN2;            //IN2值
extern uint8_t IN3;            //IN3值

extern uint8_t DTU_Heart;       //DTU心跳状态
extern uint16_t adc1, adc2; 
extern float AD1_Value;
extern float AD2_Value;
extern float AD1_PhyValue;     //物理量值
extern float AD2_PhyValue;     //物理量值

/*****************************存EEROM参数*************************/

extern uint8_t Device485_type;  //设备485类型  0:不用  1:MODBUS主站 2:自由协议 
extern uint8_t CNC_UpType;      //0:不上报     1:变化上报 2:定时上报
extern uint16_t CNC_UpTime;     //1:单位秒
extern uint8_t CNC_YELLOW;      //0:常亮结束   1:闪烁结束  2:脉冲计数
extern uint8_t CNC_GREEN;       //0:常亮运行   1:闪烁运行
extern uint8_t CNC_RED;         //0:常亮故障   1:闪烁故障
extern uint8_t AD1_Type;        //0:4-20ma   1:0-5V 
extern uint8_t AD2_Type;        //0:4-20ma   1:0-5V 
extern float AD1_PhyValue1;     //物理量范围1
extern float AD1_PhyValue2;     //物理量范围2
extern float AD2_PhyValue1;     //物理量范围1
extern float AD2_PhyValue2;     //物理量范围2
extern uint32_t TIM3_CAPTURE_VAL1;     //定时器3通道1计数


extern uint32_t FileSize;       //OTA文件大小
extern uint8_t  UpdateFlage;    //升级标志
void Config_Param(void);//配置参数

#endif
