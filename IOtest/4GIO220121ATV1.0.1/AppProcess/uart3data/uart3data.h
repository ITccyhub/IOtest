#ifndef _UART3DATA_H
#define _UART3DATA_H

#include "usart.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>

struct ModbusPack{
	uint8_t Modbus_ReturnFlage;//������������
	uint8_t Modbus_MasterCnt;  //ѯ�ʴ���  
	uint8_t Modbus_Err;//
	uint16_t MobusTemp[60];	
};

typedef struct {
	
	char model[20];        //�ͺ�
	char serial_Number[20];//���к�
	float Vpv[12];//MPPT��ѹ
	float Ipv[12];//MPPT����
	float Istring[24];//�鴮����
  float Va;
	float Vb;
	float Vc;
	float Ia;
	float Ib;
	float Ic;
	float P;               //�й�����
	float Frequency;       //Ƶ��
	float Temper_Inside;   //�ڲ��¶�
	double E_Total;        //�ۼӷ�����
	double E_Day;          //���շ�����
	float Temper_Inside2;  //�ڲ��¶�2
	uint32_t Error_DSP;    //DSP����
	uint32_t Error_PV;     //�鴮����
	uint32_t Error_Temper; //�¶ȹ���
	uint32_t Error_Sub_DSP;//��DSP����	
	uint32_t H_Total;      //�ܷ���ʱ��
	uint16_t WorkMode;     //����ģʽ	
	uint8_t  Inverter_Fault;//�����ͨ�����	
}InverterHT;//�̵���HT�������������


typedef struct {
	
	char model[20];        //�ͺ�
	char serial_Number[20];//���к�	
  float Vpv[4];//MPPT��ѹ
	float Ipv[4];//MPPT����
	float Istring[20];//�鴮����
	uint16_t IsSta;   //1-16�鴮״̬
  float Va;
	float Vb;
	float Vc;
	float Ia;
	float Ib;
	float Ic;	
	float FrequencyA;      //Ƶ��
	float FrequencyB;      //Ƶ��
	float FrequencyC;      //Ƶ��	
	float P;               //�й�����
	uint16_t WorkMode;     //����ģʽ	
	float Temper_Inside;   //�ڲ��¶�
	uint32_t Error_Message;//����
	double E_Total;        //�ۼӷ�����
	uint32_t H_Total;      //�ܷ���ʱ��
	double E_Day;          //���շ�����
	uint8_t  Inverter_Fault;//�����ͨ�����
}InverterMT;//�̵���MT�������������

extern InverterHT GWHT;//�̵���HT��������
extern InverterMT GWMT;//�̵���MT��������

extern struct ModbusPack Modbus03Slave[3];

extern  uint32_t ModbusPollingCnt;              //���ú�����ʱ����,��ʱ���ڲ��ۼ�
void RS485Work(void);//ModbusЭ�鹤������

#endif
