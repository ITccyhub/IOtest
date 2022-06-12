#include "24cxx.h"

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�

uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=I2C1_ReadData(I2C_OWN_ADDRESS,255);//����ÿ�ο�����дAT24CXX			   
	if(temp!=0X55)	
	{
		I2C1_WriteData(I2C_OWN_ADDRESS,255,0X55);
	  temp=I2C1_ReadData(I2C_OWN_ADDRESS,255);	  
		if(temp==0X55)
			return 1;
		else
			return 0;
	}
	else if(temp==0X55)
	{
	    return 1;		
	}										  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255

uint8_t AT24CXX_ReadOne(uint16_t ReadAddr)
{
  	uint8_t pBuffer;
		pBuffer=I2C1_ReadData(I2C_OWN_ADDRESS,ReadAddr);
	  return pBuffer;
}


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{		  
		*pBuffer++=I2C1_ReadData(I2C_OWN_ADDRESS,ReadAddr++);
		 NumToRead--;
	}
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255

void AT24CXX_WriteOne(uint16_t WriteAddr,uint8_t pBuffer)
{	
		I2C1_WriteData(I2C_OWN_ADDRESS,WriteAddr,pBuffer);
}


//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		I2C1_WriteData(I2C_OWN_ADDRESS,WriteAddr,*pBuffer);		
		WriteAddr++;
		pBuffer++;
	}
}
