#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f1xx_hal.h"	
#include "stm32f1xx_hal_flash_ex.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
extern void  FLASH_PageErase(uint32_t PageAddress);

#define STM32_FLASH_SIZE 	512 	 		      //所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 	1              	//使能FLASH写入(0，不是能;1，使能)
#define FLASH_WAITETIME  	50000          	//FLASH等待超时时间

#define FLASH_IAP_SIZE 64                //BootLoader所用FLASH大小(KB)
#define FLASH_UPDATE_SIZE 2              //存储更新相关数据所用FLASH大小(KB)
#define FLASH_USERDATA_SIZE 2            //存储用户数据所用FLASH大小(KB)

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址



//用户程序大小 = (总FLASH容量 - 用户数据所用FLASH大小 - 更新相关数据所用FLASH大小 - IAP所用FLASH大小)/2
#define FLASH_USER_SIZE  ((STM32_FLASH_SIZE - FLASH_USERDATA_SIZE - FLASH_UPDATE_SIZE - FLASH_IAP_SIZE)/2)  //用户程序大小(KB)

//用户程序1运行地址
#define FLASH_APP1_ADDR  (STM32_FLASH_BASE+1024*FLASH_IAP_SIZE)
//备份用户程序地址
#define FLASH_APP2_ADDR  (STM32_FLASH_BASE+1024*(FLASH_USER_SIZE+FLASH_IAP_SIZE))

//存储更新相关数据的FLASH起始地址
#define FLASH_UPDATE_ADDR    (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_USERDATA_SIZE - FLASH_UPDATE_SIZE)) 
//存储用户数据的FLASH起始地址 (FLASH最后FLASH_USERDATA_SIZE(KB))
#define FLASH_USERDATE_ADDR  (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_USERDATA_SIZE))


#define FLASH_UPDATE_HARDWARE_VERSION (FLASH_UPDATE_ADDR+2)//硬件版本
#define FLASH_UPDATE_RUNUSER_WHICH    (FLASH_UPDATE_ADDR+4)//存储运行的哪一份程序信息 0:没有可以运行的程序;1:运行的第一份程序;2:运行的第二份程序
#define FLASH_UPDATE_FLAG     (FLASH_UPDATE_ADDR+6) //存储更新标志
#define FLASH_UPDATE_STATUS   (FLASH_UPDATE_ADDR+8) //存储更新的状态
#define FLASH_UPDATE_FILESIZE (FLASH_UPDATE_ADDR+10)//存储固件文件大小 10,11 12,13,后面需要从14开始
#define FLASH_UPDATE_URL_ADDR (FLASH_UPDATE_ADDR+22)   //更新URL
#define FLASH_UPDATE_IP_ADDR  (FLASH_UPDATE_ADDR+1022) //更新IP
#define FLASH_UPDATE_Port_ADDR (FLASH_UPDATE_ADDR+1072)//更新Port
#define FLASH_UPDATE_UserName_ADDR (FLASH_UPDATE_ADDR+1080)//更新UserName
#define FLASH_UPDATE_PassWord_ADDR (FLASH_UPDATE_ADDR+1100)//更新PassWord

uint8_t STMFLASH_GetStatus(void);				          //获得状态
uint8_t STMFLASH_WaitDone(uint16_t time);				  //等待操作结束
uint8_t STMFLASH_ErasePage(uint32_t paddr);			  //擦除页
uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint16_t dat);//写入半字
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		           //读出半字  
void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);						        //指定地址开始读取指定长度数据
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据
char FlashErasePage(uint32_t EraseAddress,uint16_t PageCnt);
char WriteFlashHalfWord(uint32_t WriteAddress,uint16_t data);
//测试写入
void Test_Write(uint32_t WriteAddr,uint16_t WriteData);								   
#endif
