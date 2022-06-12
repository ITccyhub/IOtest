#ifndef _UART3DATA_H
#define _UART3DATA_H

#include "usart.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>

struct ModbusPack{
	uint8_t Modbus_ReturnFlage;//返回数据正常
	uint8_t Modbus_MasterCnt;  //询问次数  
	uint8_t Modbus_Err;//
	uint16_t MobusTemp[60];	
};

typedef struct {
	
	char model[20];        //型号
	char serial_Number[20];//序列号
	float Vpv[12];//MPPT电压
	float Ipv[12];//MPPT电流
	float Istring[24];//组串电流
  float Va;
	float Vb;
	float Vc;
	float Ia;
	float Ib;
	float Ic;
	float P;               //有功功率
	float Frequency;       //频率
	float Temper_Inside;   //内部温度
	double E_Total;        //累加发电量
	double E_Day;          //当日发电量
	float Temper_Inside2;  //内部温度2
	uint32_t Error_DSP;    //DSP故障
	uint32_t Error_PV;     //组串故障
	uint32_t Error_Temper; //温度故障
	uint32_t Error_Sub_DSP;//副DSP故障	
	uint32_t H_Total;      //总发电时间
	uint16_t WorkMode;     //工作模式	
	uint8_t  Inverter_Fault;//逆变器通许故障	
}InverterHT;//固德威HT逆变器数据类型


typedef struct {
	
	char model[20];        //型号
	char serial_Number[20];//序列号	
  float Vpv[4];//MPPT电压
	float Ipv[4];//MPPT电流
	float Istring[20];//组串电流
	uint16_t IsSta;   //1-16组串状态
  float Va;
	float Vb;
	float Vc;
	float Ia;
	float Ib;
	float Ic;	
	float FrequencyA;      //频率
	float FrequencyB;      //频率
	float FrequencyC;      //频率	
	float P;               //有功功率
	uint16_t WorkMode;     //工作模式	
	float Temper_Inside;   //内部温度
	uint32_t Error_Message;//故障
	double E_Total;        //累加发电量
	uint32_t H_Total;      //总发电时间
	double E_Day;          //当日发电量
	uint8_t  Inverter_Fault;//逆变器通许故障
}InverterMT;//固德威MT逆变器数据类型

extern InverterHT GWHT;//固德威HT数据类型
extern InverterMT GWMT;//固德威MT数据类型

extern struct ModbusPack Modbus03Slave[3];

extern  uint32_t ModbusPollingCnt;              //配置函数延时变量,定时器内部累加
void RS485Work(void);//Modbus协议工作步骤

#endif
