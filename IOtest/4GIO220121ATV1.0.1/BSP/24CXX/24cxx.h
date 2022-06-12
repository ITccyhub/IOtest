#ifndef _24CXX_H
#define _24CXX_H

#include "i2c.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  
//STM32F429������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C02

#define RS485_Adresss	           5 
#define AD1_PhyValue1_Adress	   6 
#define AD1_PhyValue2_Adress	   10 
#define AD2_PhyValue1_Adress	   14
#define AD2_PhyValue2_Adress	   18 
#define Slave_Num_Adress         22     //��վѯ�ʴӻ�����
#define Master_ADDR_Adress       23     //��վѯ���׵�ַ
#define StartAddr_Adress         24     //��ʼ��ַ
#define ValueOrLenth_Adress      26     //����or����д����Ĵ��������������ĶԱ�ֵ
#define Slave_ADDR_Adress        28     //��Ϊ�ӻ�ʱID��
#define ModbusBaudRate_Adress    29     //Modbus���ڲ�����
#define ModbusWordLength_Adress  30     //Modbus��������λ
#define ModbusStopBits_Adress    31     //Modbus����ֹͣλ
#define ModbusParity_Adress      32     //Modbus����У��λ
#define AD1_Type_Adress      33
#define AD2_Type_Adress      34
#define CNC_UpType_Adress    35  //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�
#define CNC_UpTime_Adress    36  //1:��λ��
#define CNC_YELLOW_Adress    38  //0:�������� 1:��˸����
#define CNC_GREEN_Adress     39  //0:�������� 1:��˸����
#define CNC_RED_Adress       40  //0:�������� 1:��˸����
#define TIM3_CAPTURE_VAL1_Adress    41
#define TIM3_Pulse_Threshold_Adress 45   //���岶׽�������½���֮��ʱ�䵥λms

#define OTA_Adress                      100

#define I2C_OWN_ADDRESS                         0xA0              // stm32����I2C��ַ

uint8_t AT24CXX_Check(void);  //�������
uint8_t AT24CXX_ReadOne(uint16_t ReadAddr);
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
void AT24CXX_WriteOne(uint16_t WriteAddr,uint8_t pBuffer);
#endif
