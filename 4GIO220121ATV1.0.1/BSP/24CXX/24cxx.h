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
//STM32F429开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02

#define RS485_Adresss	           5 
#define AD1_PhyValue1_Adress	   6 
#define AD1_PhyValue2_Adress	   10 
#define AD2_PhyValue1_Adress	   14
#define AD2_PhyValue2_Adress	   18 
#define Slave_Num_Adress         22     //主站询问从机个数
#define Master_ADDR_Adress       23     //主站询问首地址
#define StartAddr_Adress         24     //起始地址
#define ValueOrLenth_Adress      26     //数据or长度写多个寄存器返回数据量的对比值
#define Slave_ADDR_Adress        28     //作为从机时ID号
#define ModbusBaudRate_Adress    29     //Modbus串口波特率
#define ModbusWordLength_Adress  30     //Modbus串口数据位
#define ModbusStopBits_Adress    31     //Modbus串口停止位
#define ModbusParity_Adress      32     //Modbus串口校验位
#define AD1_Type_Adress      33
#define AD2_Type_Adress      34
#define CNC_UpType_Adress    35  //0:不上报 1:变化上报 2:定时上报
#define CNC_UpTime_Adress    36  //1:单位秒
#define CNC_YELLOW_Adress    38  //0:常亮结束 1:闪烁结束
#define CNC_GREEN_Adress     39  //0:常亮运行 1:闪烁运行
#define CNC_RED_Adress       40  //0:常亮故障 1:闪烁故障
#define TIM3_CAPTURE_VAL1_Adress    41
#define TIM3_Pulse_Threshold_Adress 45   //脉冲捕捉上升和下降沿之间时间单位ms

#define OTA_Adress                      100

#define I2C_OWN_ADDRESS                         0xA0              // stm32本机I2C地址

uint8_t AT24CXX_Check(void);  //检查器件
uint8_t AT24CXX_ReadOne(uint16_t ReadAddr);
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
void AT24CXX_WriteOne(uint16_t WriteAddr,uint8_t pBuffer);
#endif
