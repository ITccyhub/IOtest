// modbus RTU通信主站
#ifndef _MODBUS_H_

#define _MODBUS_H_
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "uart1data.h"
#define HI(n)  ((n)>>8)
#define LOW(n) ((n)&0xff)

#define EN_MODBUSMaster_RX      1               //使能MODBUS主站功能接收
#define EN_MODBUSSlave_RX       1               //如果使能MODBUS从站功能接收
#define RS485_RX_BUFF			USART3_RX_BUF		      //使用UART接收
#define RS485_TX_BUFF			USART3_TX_BUF		      //使用UART接收 

extern uint16_t RS485_RX_CNT;		 //MODUBUS接收计数器

//Modbus从机寄存器
extern uint8_t  Slave_ADDR;
extern uint16_t SlaveStartAddr;            //从站接收数据返回起始地址
//Modbus寄存器和单片机寄存器的映射关系
extern uint32_t Modbus_InputIO[100];  //输入开关量寄存器指针(这里使用的是位带操作)
extern uint32_t Modbus_OutputIO[100]; //输出开关量寄存器指针(这里使用的是位带操作)
extern uint16_t Modbus_HoldReg[100];  //保持寄存器指针


																										 
//Modbus主机寄存器
extern uint8_t   Slave_Num;              //主站询问从机个数
extern uint8_t   Master_ADDR;            //主站询问首地址
extern uint16_t  StartAddr;              //起始地址
extern uint16_t  ValueOrLenth;           //数据or长度写多个寄存器返回数据量的对比值
//Master寄存器和单片机寄存器的映射关系
extern uint16_t   Master_InputIO[100];     //输入开关量寄存器(这里使用的是位带操作) 注意： 这里储存从机返回的数据。    开关量的数据只能是0，1 例如 Master_InputIO[5]=0；Master_InputIO[8]=1；
extern uint16_t   Master_OutputIO[100];    //输出开关量寄存器(这里使用的是位带操作)        功能码 05 15
extern uint16_t   Master_ReadReg[100];     //只读寄存器----存储从机返回的数据              功能码 03
extern uint16_t   Master_WriteReg[50];     //写寄存器-------将寄存器中的数据送给从机       功能码 06 16
extern uint16_t   RS485_Count;             //485从站间隔时间 50ms单位
extern uint8_t    TX_RX_SET;        //发送，接受命令切换。 0 发送模式 1接受模式
extern uint8_t    ComErr;           //0代表通讯正常
                                    //1代表CRC错误
			                              //2代表功能码错误 

/*********************EEPROM********************************************/
extern uint8_t ModbusBaudRate;      //Modbus串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
extern uint8_t ModbusWordLength;    //Modbus串口数据位  5-8
extern uint8_t ModbusStopBits;      //Modbus串口停止位  1-2
extern uint8_t ModbusParity;        //Modbus串口校验位  1:无校验 2:奇校验 3:偶校验

uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) ;
uint8_t crc_detect(uint8_t *crc_buf);

//串口3MODBUS主站功能
void Master_03_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth);
void Master_06_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value);
void Master_05_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value);
void Master_01_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth);
void MODBUSMaster_PROCESS(uint8_t ADDR);
//串口3MODBUS从站功能接收处理返回数据
void MODBUSSlave_PROCESS(UART_HandleTypeDef *huart);
void Modbus_RegMap(void);//更新从站数据
#endif
