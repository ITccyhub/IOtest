#include "modbus.h"

const uint8_t  auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 

const uint8_t  auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
};

uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) 
{ 
	uint8_t  uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */ 
	uint8_t  uchCRCLo = 0xFF ; /* ��CRC �ֽڳ�ʼ�� */ 
	uint32_t uIndex ; /* CRCѭ���е����� */ 
	while (usDataLen--) /* ������Ϣ������ */ 
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; /* ����CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return ((uint16_t)uchCRCHi << 8 | uchCRCLo) ; 
}

uint8_t crc_detect(uint8_t *crc_buf)    //����1�ɹ���0ʧ�ܡ���CRCУ��Ϊ��׼MODBUSѯ�����ݣ�8�ֽڣ�ǰ6�������ݣ���2��CRCУ��
{
    uint8_t crc_temp[8], *p_buf = crc_buf;
    uint8_t i = 0, hi, lo;
    uint16_t crc;
    
    for(i = 0; i < 8; i++)
    {
        crc_temp[i] = *p_buf;
        p_buf++;    
    }    
    
    crc = crc16(crc_temp, 6);	
    hi = crc >> 8;	
    lo = crc & 0xff;  	
    
    if((hi == crc_temp[6]) && (lo == crc_temp[7]))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint16_t RS485_RX_CNT=0;		 //MODUBUS���ռ�����

//Modbus�����Ĵ���
uint8_t   Slave_Num=1;              //��վѯ�ʴӻ�����
uint8_t   Master_ADDR=1;            //��վѯ���׵�ַ
uint16_t  StartAddr=0;              //��ʼ��ַ
uint16_t  ValueOrLenth=10;          //����or����д����Ĵ��������������ĶԱ�ֵ
//Master�Ĵ����͵�Ƭ���Ĵ�����ӳ���ϵ
uint16_t   Master_InputIO[100];     //���뿪�����Ĵ���(����ʹ�õ���λ������) ע�⣺ ���ﴢ��ӻ����ص����ݡ�    ������������ֻ����0��1 ���� Master_InputIO[5]=0��Master_InputIO[8]=1��
uint16_t   Master_OutputIO[100];    //����������Ĵ���(����ʹ�õ���λ������)        ������ 05 15
uint16_t   Master_ReadReg[100];     //ֻ���Ĵ���----�洢�ӻ����ص�����              ������ 03
uint16_t   Master_WriteReg[50];     //д�Ĵ���-------���Ĵ����е������͸��ӻ�       ������ 06 16
uint16_t   RS485_Count=1;           //485��վ���ʱ�� 50ms��λ
uint8_t    TX_RX_SET=0;      //���ͣ����������л��� 0 ����ģʽ 1����ģʽ
uint8_t    ComErr=0Xff;      //0����ͨѶ����
                             //11����CRC����
			                       //22�����������
//Modbus�ӻ��Ĵ���
uint8_t  Slave_ADDR=1;
uint16_t SlaveStartAddr=0;          //��վ�������ݷ�����ʼ��ַ 
//Modbus�Ĵ����͵�Ƭ���Ĵ�����ӳ���ϵ
uint32_t Modbus_InputIO[100];  //���뿪�����Ĵ���ָ��(����ʹ�õ���λ������)
uint32_t Modbus_OutputIO[100]; //����������Ĵ���ָ��(����ʹ�õ���λ������)
uint16_t Modbus_HoldReg[100];  //���ּĴ���ָ��

/*********************EEPROM********************************************/
uint8_t ModbusBaudRate=8;      //Modbus���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:57600 8:115200
uint8_t ModbusWordLength=8;    //Modbus��������λ  8-9
uint8_t ModbusStopBits=1;      //Modbus����ֹͣλ  1-2
uint8_t ModbusParity=1;        //Modbus����У��λ  1:��У�� 2:��У�� 3:żУ��

float testData1=1021.1,testData2=1002,testData3=2303,testData4=8204;

void Modbus_RegMap(void)
{
	  static uint16_t Modbus_temp[4];
	    //���뿪�����Ĵ���ָ��ָ��
//		 Modbus_InputIO[0]=(uint32_t*)&PEin(4);//KEY0     //&PEin(4)��ȡPE4�ĵ�ַ��(vu32*)&PEin(4)��PE4��ַǿ��ת��Ϊuw32���͵ĵ�ַ��Modbus_InputIO[0]=(vu32*)&PEin(4); ��ת���õĵ�ַ�͸���ַָ��Modbus_InputIO[0]��
//		 Modbus_InputIO[1]=(uint32_t*)&PEin(3);//KEY1     //*M
//	   //����������Ĵ���ָ��ָ��
//		 Modbus_OutputIO[0]=(uint32_t*)&PBout(5);//LED0
//		 Modbus_OutputIO[1]=(uint32_t*)&PEout(5);//LED1
	
     Modbus_HoldReg[0]=(uint16_t)AD1_Type;//��������1	
	   memcpy(Modbus_temp,&AD1_PhyValue1,4);
	   Modbus_HoldReg[1]=Modbus_temp[1]; //��������1
	   Modbus_HoldReg[2]=Modbus_temp[0];//��������1
	
		 memcpy(Modbus_temp,&AD1_PhyValue2,4);
	   Modbus_HoldReg[3]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[4]=Modbus_temp[0];  //��������1
	
		 memcpy(Modbus_temp,&AD1_PhyValue2,4);
	   Modbus_HoldReg[5]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[6]=Modbus_temp[0];  //��������1
	
	   memcpy(Modbus_temp,&AD1_PhyValue,4);		
	   Modbus_HoldReg[7]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[8]=Modbus_temp[0];  //��������1
	
     Modbus_HoldReg[9]=(uint16_t)AD2_Type;//��������1	
	   memcpy(Modbus_temp,&AD2_PhyValue1,4);
	   Modbus_HoldReg[10]=Modbus_temp[1]; //��������1
	   Modbus_HoldReg[11]=Modbus_temp[0];//��������1
	
		 memcpy(Modbus_temp,&AD2_PhyValue2,4);
	   Modbus_HoldReg[12]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[13]=Modbus_temp[0];  //��������1
	
		 memcpy(Modbus_temp,&AD2_PhyValue2,4);
	   Modbus_HoldReg[14]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[15]=Modbus_temp[0];   //��������1
	
	   memcpy(Modbus_temp,&AD2_PhyValue,4);		
	   Modbus_HoldReg[16]=Modbus_temp[1];   //��������1
	   Modbus_HoldReg[17]=Modbus_temp[0];  //��������1
		 
		 Modbus_HoldReg[18]=(TIM3_CAPTURE_VAL1)>>16;//��������1
		 Modbus_HoldReg[19]=(TIM3_CAPTURE_VAL1)&0xffff;//��������1
		 Modbus_HoldReg[20]=(uint16_t)IN1;//��������1
		 Modbus_HoldReg[21]=(uint16_t)IN2;//��������1
		 Modbus_HoldReg[22]=(uint16_t)IN3;//��������1		 
}

//����·����Modbus��վ
#if EN_MODBUSMaster_RX   //����5ʹ��MODBUS��վ���ܽ���
//MODBUS��վ��������������ʹ���
//Modbus��վ������01������� ///////////////////////////////////////////////////////////��������֤OK
//�����������
void Master_01_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth )
{
	  uint16_t calCRC;
	  uint8_t RS485_TX_BUFF[20];
	  RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 01;    //modbus ָ����01
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
    calCRC=crc16(RS485_TX_BUFF,6);
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8);
    TX_RX_SET=1; //�������	
	  ComErr=0Xff;
}


//Modbus������02�������/////////////////////////////////////////////////////��������֤OK -----����������PE4 PE3 PE2 PA0 ��ʼ�������ſ���OK    KEY_Init();
//�����뿪����
void Master_02_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth)
{	
	  uint16_t calCRC;
	  uint8_t RS485_TX_BUFF[20];
	  RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 02;    //modbus ָ����02
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
    calCRC=crc16(RS485_TX_BUFF,6);
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8); 
    TX_RX_SET=1; //�������  
	  ComErr=0Xff;
}

//Modbus������03�������///////////////////////////////////////////////////////////////////////////////////////
//�����ּĴ���
void Master_03_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address,uint16_t lenth )
{ 
	  uint16_t calCRC;
	  uint8_t RS485_TX_BUFF[20];
    RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 03;    //modbus ָ����03
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
    calCRC=crc16(RS485_TX_BUFF,6);
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8); 
    TX_RX_SET=1; //�������	
	  ComErr=0Xff;
}

//Modbus������04�������/////////////////////////////////////////////////////////////////////////////////////// δʹ��
//������Ĵ���
void Master_04_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address,uint16_t lenth )
{ 
	  uint16_t calCRC;
	  uint8_t RS485_TX_BUFF[20];
    RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 04;    //modbus ָ����04
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
    calCRC=crc16(RS485_TX_BUFF,6);
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8);
    TX_RX_SET=1; //������� 
    ComErr=0Xff;	
}

//Modbus������05�������   ///////////////////////////////////////////////////////��������֤OK
//д������������� ���ֻ����0XFF00->1 ON; 0X0000->0 OFF;
void Master_05_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value)
{
	
		uint16_t i;
		uint16_t calCRC;
	  uint8_t RS485_TX_BUFF[20];
		if(value==1)	i=0xff00;	
		else          i=0x0000;
	  RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 05;   //modbus ָ����05
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(i);
    RS485_TX_BUFF[5] = LOW(i);
    calCRC=crc16(RS485_TX_BUFF,6);
	  RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8);	 
    TX_RX_SET=1; //�������	
	  ComErr=0Xff;
}

//Modbus������06�������   //////////////////////////////////////////////////////////////////////////////////
//д�������ּĴ���
void Master_06_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t value)
{
	  uint16_t calCRC;	
	  uint8_t RS485_TX_BUFF[20];
	  RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 06;   //modbus ָ����06
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(value);
    RS485_TX_BUFF[5] = LOW(value);
    calCRC=crc16(RS485_TX_BUFF,6);
	  RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,8);	
    TX_RX_SET=1; //�������	
	  ComErr=0Xff;
}
//Modbus������15�������   //////////////////////////////////////////////////////��������֤OK
//д������������
void Master_15_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth) 
{	
		  uint16_t calCRC;
	    uint8_t RS485_TX_BUFF[lenth/8+10];
	    uint16_t ByteNum;
      uint16_t i;
        if((start_address+lenth)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {

                ByteNum=lenth/8;//�ֽ���
                if(lenth%8) ByteNum+=1;//���λ���������������ֽ���+1
                for(i=0;i<lenth;i++)
                {
                         if(i%8==0) RS485_TX_BUFF[7+i/8]=0x00;
						             RS485_TX_BUFF[7+i/8]=RS485_TX_BUFF[7+i/8]>>1;
                         if(Master_OutputIO[i]) RS485_TX_BUFF[7+i/8]=RS485_TX_BUFF[7+i/8]|0x80;//��λ�ȷ���
                         if(i==lenth-1)//���͵����һ��λ��
                         {
                                if(lenth%8) RS485_TX_BUFF[7+i/8]>>=8-(lenth%8);//������һ���ֽڻ�����������ʣ��MSB���0
                         }
                }
				
			}	      
			RS485_TX_BUFF[0] = board_adr;
			RS485_TX_BUFF[1] = 15;   //modbus ָ����15
			RS485_TX_BUFF[2] = HI(start_address);  
			RS485_TX_BUFF[3] = LOW(start_address); 
			RS485_TX_BUFF[4] = HI(lenth);
			RS485_TX_BUFF[5] = LOW(lenth);	
			RS485_TX_BUFF[6] = ByteNum;
			calCRC=crc16(RS485_TX_BUFF,7+ByteNum);
			RS485_TX_BUFF[7+ByteNum]=(calCRC>>8)&0xFF; 
			RS485_TX_BUFF[8+ByteNum]=(calCRC)&0xFF;
			COM_Send(huart,RS485_TX_BUFF,9+ByteNum); 
			TX_RX_SET=1; //�������
			ComErr=0Xff;
}

//Modbus������16������� /////////////////////////////////////////////////////////////////////////////////////////////////����֤����OK
//д������ּĴ���
void Master_16_Slove(UART_HandleTypeDef *huart,uint8_t board_adr,uint16_t start_address, uint16_t lenth) ///
{
		  uint16_t calCRC;
	    uint8_t RS485_TX_BUFF[lenth*2+10];
	    uint8_t i;
			if((start_address+lenth)<100)//�Ĵ�����ַ+�����ڷ�Χ��
			{
							for(i=0;i<lenth;i++)
							{
										RS485_TX_BUFF[7+i*2]= Master_WriteReg[i]>>8;  //���ֽ���ǰ               
										RS485_TX_BUFF[8+i*2]= Master_WriteReg[i];    //���ֽ��ں�
							}
			}       
	  RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = 16;   //modbus ָ����16
    RS485_TX_BUFF[2] = HI(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HI(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
	  RS485_TX_BUFF[6] = 2*lenth;
    calCRC=crc16(RS485_TX_BUFF,7+2*lenth);
	  RS485_TX_BUFF[7+2*lenth]=(calCRC>>8)&0xFF; 
    RS485_TX_BUFF[8+2*lenth]=(calCRC)&0xFF;
	  COM_Send(huart,RS485_TX_BUFF,9+2*lenth); 
    TX_RX_SET=1; //�������	
    ComErr=0Xff;			
}

//MODBUS��վ����֮��������ݽ�������
//Modbus������01������� ///////////////////////////////////////////////////////////��������֤OK
//�����������
void Modbus_01_Master(void)   
{		
	    uint16_t i;
			if(ValueOrLenth<100)//�Ĵ�����ַ+�����ڷ�Χ��
			{
					for(i=0;i<ValueOrLenth;i++)
					{
								 Master_OutputIO[i]=RS485_RX_BUFF[3+i/8]&0x01;//��λ�ȷ���
								 RS485_RX_BUFF[3+i/8]>>=1;
					}
					
					ComErr=0;
      }
			else
			{
			    ComErr=1;
			}	
		TX_RX_SET=0; //�������		
}

//Modbus������02������� ///////////////////////////////////////////////////////////��������֤OK
//�����뿪����
void Modbus_02_Master(void)   
{		
	    uint16_t i;
			if(ValueOrLenth<100)//�Ĵ�����ַ+�����ڷ�Χ��
			{
					for(i=0;i<ValueOrLenth;i++)
					{
								 Master_InputIO[i]=RS485_RX_BUFF[3+i/8]&0x01;//��λ�ȷ���
								 RS485_RX_BUFF[3+i/8]>>=1;
					}
	
			   ComErr=0;
			}
		else
		{
				 ComErr=2;
		}
		TX_RX_SET=0; //�������
}



//Modbus������03�������///////////////////////////////////////////////////////////////////////////////////////����֤����OK
//�����ּĴ���
void Modbus_03_Master(void)
{
    uint8_t i;
		uint8_t RegNum;
		RegNum= RS485_RX_BUFF[2]/2;//��ȡ�ֽ��� 6
		if(RegNum<100)//�Ĵ�����ַ+�����ڷ�Χ��
		{
				for(i=0;i<RegNum;i++)
				{
							 Master_ReadReg[i]= RS485_RX_BUFF[3+i*2];                                 // ��8λ
							 Master_ReadReg[i]= RS485_RX_BUFF[4+i*2]+(Master_ReadReg[i]<<8);          // ��8λ+��8λ
				} 

				ComErr=0;
		}
		else
		{
				ComErr=3;
		}
		TX_RX_SET=0; //�������
}

//Modbus������05�������   ///////////////////////////////////////////////////////��������֤OK
//д�������������
void Modbus_05_Master(void)
{    
	   if((RS485_RX_BUFF[4]==0XFF&&RS485_RX_BUFF[5]==0X00)||(RS485_RX_BUFF[4]==0X00&&RS485_RX_BUFF[5]==0X00))
	   {
			    ComErr=0;			
	   }
	   else
	   {
			    ComErr=5;
	   }
	   TX_RX_SET=0; //�������	   
}

//Modbus������06�������   //////////////////////////////////////////////////////////////////////////////////����֤����OK
//д�������ּĴ���
void Modbus_06_Master(void)
{   
	  ComErr=0;
		
		TX_RX_SET=0; //�������

}
//Modbus������15�������   //////////////////////////////////////////////////////��������֤OK
//д������������
void Modbus_15_Master(void)
{
		uint16_t i;//���ݷ���У����
		i=(((uint16_t)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
		if(i==ValueOrLenth)
		{
		    ComErr=0;
		}
    else
		{
			  ComErr=15;
		}
		TX_RX_SET=0; //�������   
}

//Modbus������16������� /////////////////////////////////////////////////////////////////////////////////////////////////����֤����OK
//д������ּĴ���
void Modbus_16_Master(void)
{
		uint16_t i;//���ݷ���У����
		i= (((uint16_t)RS485_RX_BUFF[4])<<8)|((RS485_RX_BUFF[5]));//��ȡ�Ĵ�������
		if(i==ValueOrLenth)
		{
		    ComErr=0;
		}
    else
		{
		    ComErr=16;
		}
		TX_RX_SET=0; //�������           
}
void MODBUSMaster_PROCESS(uint8_t ADDR)
{
   	uint16_t recCRC;//��ȡԴ����CRCУ����
	  uint16_t calCRC;//����CRCУ���� 	  
    if(USART3_RX_STA&0x8000)
    {		
        bsp_StopTimer(8);	        //���յ����ݹرճ�ʱ		
        if(RS485_RX_BUFF[0]==ADDR)//
        {
            if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==03)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==06)||(RS485_RX_BUFF[1]==15)||(RS485_RX_BUFF[1]==16))//?????
            {               
                    calCRC=crc16(RS485_RX_BUFF,RS485_RX_CNT-2);//
									  recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((uint16_t)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//
									  if(calCRC==recCRC)//CRCУ����ȷ
                    {
                     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        switch(RS485_RX_BUFF[1])//
												{
												    case 1://
														{
																		Modbus_01_Master();															      
														        break;
														}
														
														case 2://
														{
																		Modbus_02_Master();
																		break;
														}
																		
														case 5://
														{
																		Modbus_05_Master();
																		break;
														}
																		
														case 15://
														{
																		Modbus_15_Master();
																		break;
														}
																		
														case 03: //
														{
																		Modbus_03_Master();													
																		break;
														}
																		
														case 06: //
														{
															      
																		Modbus_06_Master();
																		break;
														}
																		
														case 16: //
														{
																	  Modbus_16_Master();
																		break;
														}
														
														default:
					                          break;
                                                                                        
                        }
                      
                    }
                    else//CRCУ�����
										{
											  ComErr=11;											  											
										}        
                                                      
            }
					  else//���������
					  {
						    ComErr=22;	               						
					  }
					
        }//��ַ��ȷ
       memset(RS485_RX_BUFF,0,USART3_RX_STA&0X3FFF); 
       RS485_RX_CNT=0;				
       USART3_RX_STA=0;//���ռ�������				
    }    
}                   
#endif



#if EN_MODBUSSlave_RX   //����3ʹ��MODBUS��վ���ܽ���
//Modbus��վ���շ������ݸ�ʽ���(����·����Modbus��վ)
//Modbus������01������� ///////////////////////////////////////////////////////////��������֤OK
//�����������
void Modbus_01_Solve(UART_HandleTypeDef *huart)
{
        uint16_t ByteNum,RegNum,calCRC;
        uint16_t i;	      	
        RegNum=(((uint16_t)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
	      uint8_t RS485_TX_BUFF[RegNum+5];
        if((SlaveStartAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//�ֽ���
                if(RegNum%8) ByteNum+=1;//���λ���������������ֽ���+1
                RS485_TX_BUFF[2]=ByteNum;//����Ҫ��ȡ���ֽ���
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//��λ�ȷ���
                        RS485_TX_BUFF[3+i/8]|=((Modbus_OutputIO[SlaveStartAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//���͵����һ��λ��
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//������һ���ֽڻ�����������ʣ��MSB���0
                        }
                }
                calCRC=crc16(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
								COM_Send(huart,RS485_TX_BUFF,ByteNum+5);                
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                COM_Send(huart,RS485_TX_BUFF,3); 
        }
}

//Modbus������02�������/////////////////////////////////////////////////////��������֤OK -----����������PE4 PE3 PE2 PA0 ��ʼ�������ſ���OK    KEY_Init();
//�����뿪����
void Modbus_02_Solve(UART_HandleTypeDef *huart)
{
        uint16_t ByteNum,RegNum,calCRC;
        uint16_t i;
        RegNum=(((uint16_t)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
	      uint8_t RS485_TX_BUFF[RegNum+5];
        if((SlaveStartAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//�ֽ���
                if(RegNum%8) ByteNum+=1;//���λ���������������ֽ���+1
                RS485_TX_BUFF[2]=ByteNum;//����Ҫ��ȡ���ֽ���
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//��λ�ȷ���
                        RS485_TX_BUFF[3+i/8]|=((Modbus_InputIO[SlaveStartAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//���͵����һ��λ��
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//������һ���ֽڻ�����������ʣ��MSB���0
                        }
                }
                calCRC=crc16(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
								COM_Send(huart,RS485_TX_BUFF,ByteNum+5);                  
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��              
					      COM_Send(huart,RS485_TX_BUFF,3); 
        }
}

//Modbus������03�������///////////////////////////////////////////////////////////////////////////////////////����֤����OK
//�����ּĴ���
void Modbus_03_Solve(UART_HandleTypeDef *huart)
{
        uint16_t i,RegNum,calCRC;
        RegNum= (((uint16_t)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
	      uint8_t RS485_TX_BUFF[RegNum*2+5];
        if((SlaveStartAddr+RegNum)<1000)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RegNum*2;
                for(i=0;i<RegNum;i++)
                {
                        RS485_TX_BUFF[3+i*2]=(Modbus_HoldReg[SlaveStartAddr+i]>>8)&0xFF;//           /////////�ȷ��͸��ֽ�--�ڷ��͵��ֽ�
                        RS485_TX_BUFF[4+i*2]=(Modbus_HoldReg[SlaveStartAddr+i])&0xFF; //
                }
                calCRC=crc16(RS485_TX_BUFF,RegNum*2+3);
                RS485_TX_BUFF[RegNum*2+3]=(calCRC>>8)&0xFF;         //CRC�ߵ�λ������  // �ȸߺ��
                RS485_TX_BUFF[RegNum*2+4]=(calCRC)&0xFF;
								COM_Send(huart,RS485_TX_BUFF,RegNum*2+5);                
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                COM_Send(huart,RS485_TX_BUFF,3); 
        }
}


//Modbus������05�������   ///////////////////////////////////////////////////////��������֤OK
//д�������������
void Modbus_05_Solve(UART_HandleTypeDef *huart)
{
	      uint16_t calCRC;
	      uint8_t RS485_TX_BUFF[20];
        if(SlaveStartAddr<100)//�Ĵ�����ַ�ڷ�Χ��
        {
                if((RS485_RX_BUFF[4]==0xFF)||(RS485_RX_BUFF[5]==0xFF)) Modbus_OutputIO[SlaveStartAddr]=0x01;
                else Modbus_OutputIO[SlaveStartAddr]=0x00;
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                
                calCRC=crc16(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
					      COM_Send(huart,RS485_TX_BUFF,8);                
        }
        else//�Ĵ�����ַ������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                COM_Send(huart,RS485_TX_BUFF,3); 
        }
}



//Modbus������06�������   //////////////////////////////////////////////////////////////////////////////////����֤����OK
//д�������ּĴ���
void Modbus_06_Solve(UART_HandleTypeDef *huart)
{
	      uint16_t calCRC;
	      uint8_t RS485_TX_BUFF[20];
        Modbus_HoldReg[SlaveStartAddr]=RS485_RX_BUFF[4]<<8;//���ֽ���ǰ                    ////////�޸�Ϊ���ֽ���ǰ�����ֽ��ں�
        Modbus_HoldReg[SlaveStartAddr]|=((uint16_t)RS485_RX_BUFF[5]);//���ֽ��ں�
        
        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
        RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
        RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
        RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
        RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
        RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
        
        calCRC=crc16(RS485_TX_BUFF,6);
        RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
        RS485_TX_BUFF[7]=(calCRC)&0xFF;
	      COM_Send(huart,RS485_TX_BUFF,8);      
}

//Modbus������15�������   //////////////////////////////////////////////////////��������֤OK
//д������������
void Modbus_15_Solve(UART_HandleTypeDef *huart)
{
        uint16_t i,RegNum,calCRC;
	      uint8_t RS485_TX_BUFF[20];
        RegNum=(((uint16_t)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
        if((SlaveStartAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {        
                for(i=0;i<RegNum;i++)
                {
                        if(RS485_RX_BUFF[7+i/8]&0x01) Modbus_OutputIO[SlaveStartAddr+i]=0x01;
                        else Modbus_OutputIO[SlaveStartAddr+i]=0x00;
                        RS485_RX_BUFF[7+i/8]>>=1;//�ӵ�λ��ʼ
                }
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                calCRC=crc16(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
								COM_Send(huart,RS485_TX_BUFF,8);                
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                COM_Send(huart,RS485_TX_BUFF,3); 
        }
}


//Modbus������16������� /////////////////////////////////////////////////////////////////////////////////////////////////����֤����OK
//д������ּĴ���
void Modbus_16_Solve(UART_HandleTypeDef *huart)
{
        uint16_t i,RegNum,calCRC;
	      uint8_t RS485_TX_BUFF[20];
        RegNum= (((uint16_t)RS485_RX_BUFF[4])<<8)|((RS485_RX_BUFF[5]));//��ȡ�Ĵ�������
        if((SlaveStartAddr+RegNum)<1000)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                for(i=0;i<RegNum;i++)
                {
                        Modbus_HoldReg[SlaveStartAddr+i]=RS485_RX_BUFF[7+i*2]; //���ֽ���ǰ                 /////// ���ֽ���ǰ�����ֽ��ں�����
                        Modbus_HoldReg[SlaveStartAddr+i]|=((uint16_t)RS485_RX_BUFF[8+i*2])<<8; //���ֽ��ں�
                }
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                
                calCRC=crc16(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
								COM_Send(huart,RS485_TX_BUFF,8);    
                
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
					      COM_Send(huart,RS485_TX_BUFF,3);    
               
        }
}
//����3MODBUS��վ���ܽ��մ���������
void MODBUSSlave_PROCESS(UART_HandleTypeDef *huart)
{
   	uint16_t recCRC;//��ȡԴ����CRCУ����
	  uint16_t calCRC;//����CRCУ���� 
    uint8_t RS485_TX_BUFF[20];  
    if(USART3_RX_STA&0x8000)
    {			  
        if(USART3_RX_BUF[0]==Slave_ADDR)//
        {
            if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==03)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==06)||(RS485_RX_BUFF[1]==15)||(RS485_RX_BUFF[1]==16))//?????
            {               
                    calCRC=crc16(RS485_RX_BUFF,RS485_RX_CNT-2);//
									  recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((uint16_t)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//
									  if(calCRC==recCRC)//CRCУ����ȷ
                    {
                     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        switch(RS485_RX_BUFF[1])//
												{
												    case 1://
														{
																		Modbus_01_Solve(huart);													     
														        break;
														}
														
														case 2://
														{
																		Modbus_02_Solve(huart);															      														      
																		break;
														}
																		
														case 5://
														{
															      Modbus_05_Solve(huart);																																     
																		break;
														}
																		
														case 15://
														{
																		Modbus_15_Solve(huart);															      													      
																		break;
														}
																		
														case 03: //
														{																	
                                    Modbus_03_Solve(huart);															
																		break;
														}
																		
														case 06: //
														{																		
                                    Modbus_06_Solve(huart);															
																		break;
														}
																		
														case 16: //
														{																		
                                    Modbus_16_Solve(huart);															
																		break;
														}
														
														default:
					                          break;
                                                                                        
                        }
                      
                    }
                    else//CRCУ�����
										{
                        ComErr=1;	
                        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                        RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                       	RS485_TX_BUFF[2]=ComErr;
                        COM_Send(huart,RS485_TX_BUFF,3); 											
										}        
                                                      
            }
					  else//���������
					  {
						    ComErr=2;	
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
								RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
								RS485_TX_BUFF[2]=ComErr;
								COM_Send(huart,RS485_TX_BUFF,3); 							
					  }
					
        }//��ַ��ȷ
      memset(RS485_RX_BUFF,0,USART3_RX_STA&0X3FFF);            
      USART3_RX_STA=0;//���ռ�������
      RS485_RX_CNT=0;				
    }    
}                   
#endif
