// modbus RTUͨ����վ
#ifndef _MODBUS_H_

#define _MODBUS_H_
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "uart1data.h"
#define HI(n)  ((n)>>8)
#define LOW(n) ((n)&0xff)

#define EN_MODBUSMaster_RX      1               //ʹ��MODBUS��վ���ܽ���
#define EN_MODBUSSlave_RX       1               //���ʹ��MODBUS��վ���ܽ���
#define RS485_RX_BUFF			USART3_RX_BUF		      //ʹ��UART����
#define RS485_TX_BUFF			USART3_TX_BUF		      //ʹ��UART���� 

extern uint16_t RS485_RX_CNT;		 //MODUBUS���ռ�����

//Modbus�ӻ��Ĵ���
extern uint8_t  Slave_ADDR;
extern uint16_t SlaveStartAddr;            //��վ�������ݷ�����ʼ��ַ
//Modbus�Ĵ����͵�Ƭ���Ĵ�����ӳ���ϵ
extern uint32_t Modbus_InputIO[100];  //���뿪�����Ĵ���ָ��(����ʹ�õ���λ������)
extern uint32_t Modbus_OutputIO[100]; //����������Ĵ���ָ��(����ʹ�õ���λ������)
extern uint16_t Modbus_HoldReg[100];  //���ּĴ���ָ��


																										 
//Modbus�����Ĵ���
extern uint8_t   Slave_Num;              //��վѯ�ʴӻ�����
extern uint8_t   Master_ADDR;            //��վѯ���׵�ַ
extern uint16_t  StartAddr;              //��ʼ��ַ
extern uint16_t  ValueOrLenth;           //����or����д����Ĵ��������������ĶԱ�ֵ
//Master�Ĵ����͵�Ƭ���Ĵ�����ӳ���ϵ
extern uint16_t   Master_InputIO[100];     //���뿪�����Ĵ���(����ʹ�õ���λ������) ע�⣺ ���ﴢ��ӻ����ص����ݡ�    ������������ֻ����0��1 ���� Master_InputIO[5]=0��Master_InputIO[8]=1��
extern uint16_t   Master_OutputIO[100];    //����������Ĵ���(����ʹ�õ���λ������)        ������ 05 15
extern uint16_t   Master_ReadReg[100];     //ֻ���Ĵ���----�洢�ӻ����ص�����              ������ 03
extern uint16_t   Master_WriteReg[50];     //д�Ĵ���-------���Ĵ����е������͸��ӻ�       ������ 06 16
extern uint16_t   RS485_Count;             //485��վ���ʱ�� 50ms��λ
extern uint8_t    TX_RX_SET;        //���ͣ����������л��� 0 ����ģʽ 1����ģʽ
extern uint8_t    ComErr;           //0����ͨѶ����
                                    //1����CRC����
			                              //2����������� 

/*********************EEPROM********************************************/
extern uint8_t ModbusBaudRate;      //Modbus���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
extern uint8_t ModbusWordLength;    //Modbus��������λ  5-8
extern uint8_t ModbusStopBits;      //Modbus����ֹͣλ  1-2
extern uint8_t ModbusParity;        //Modbus����У��λ  1:��У�� 2:��У�� 3:żУ��

uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) ;
uint8_t crc_detect(uint8_t *crc_buf);

//����3MODBUS��վ����
void Master_03_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth);
void Master_06_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value);
void Master_05_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value);
void Master_01_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth);
void MODBUSMaster_PROCESS(uint8_t ADDR);
//����3MODBUS��վ���ܽ��մ���������
void MODBUSSlave_PROCESS(UART_HandleTypeDef *huart);
void Modbus_RegMap(void);//���´�վ����
#endif
