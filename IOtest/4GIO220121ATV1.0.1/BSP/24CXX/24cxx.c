#include "24cxx.h"

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功

uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=I2C1_ReadData(I2C_OWN_ADDRESS,255);//避免每次开机都写AT24CXX			   
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

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255

uint8_t AT24CXX_ReadOne(uint16_t ReadAddr)
{
  	uint8_t pBuffer;
		pBuffer=I2C1_ReadData(I2C_OWN_ADDRESS,ReadAddr);
	  return pBuffer;
}


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{		  
		*pBuffer++=I2C1_ReadData(I2C_OWN_ADDRESS,ReadAddr++);
		 NumToRead--;
	}
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255

void AT24CXX_WriteOne(uint16_t WriteAddr,uint8_t pBuffer)
{	
		I2C1_WriteData(I2C_OWN_ADDRESS,WriteAddr,pBuffer);
}


//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		I2C1_WriteData(I2C_OWN_ADDRESS,WriteAddr,*pBuffer);		
		WriteAddr++;
		pBuffer++;
	}
}
