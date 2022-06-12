#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f1xx_hal.h"	
#include "stm32f1xx_hal_flash_ex.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
extern void  FLASH_PageErase(uint32_t PageAddress);

#define STM32_FLASH_SIZE 	512 	 		      //��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 	1              	//ʹ��FLASHд��(0��������;1��ʹ��)
#define FLASH_WAITETIME  	50000          	//FLASH�ȴ���ʱʱ��

#define FLASH_IAP_SIZE 64                //BootLoader����FLASH��С(KB)
#define FLASH_UPDATE_SIZE 2              //�洢���������������FLASH��С(KB)
#define FLASH_USERDATA_SIZE 2            //�洢�û���������FLASH��С(KB)

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH����ʼ��ַ



//�û������С = (��FLASH���� - �û���������FLASH��С - ���������������FLASH��С - IAP����FLASH��С)/2
#define FLASH_USER_SIZE  ((STM32_FLASH_SIZE - FLASH_USERDATA_SIZE - FLASH_UPDATE_SIZE - FLASH_IAP_SIZE)/2)  //�û������С(KB)

//�û�����1���е�ַ
#define FLASH_APP1_ADDR  (STM32_FLASH_BASE+1024*FLASH_IAP_SIZE)
//�����û������ַ
#define FLASH_APP2_ADDR  (STM32_FLASH_BASE+1024*(FLASH_USER_SIZE+FLASH_IAP_SIZE))

//�洢����������ݵ�FLASH��ʼ��ַ
#define FLASH_UPDATE_ADDR    (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_USERDATA_SIZE - FLASH_UPDATE_SIZE)) 
//�洢�û����ݵ�FLASH��ʼ��ַ (FLASH���FLASH_USERDATA_SIZE(KB))
#define FLASH_USERDATE_ADDR  (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_USERDATA_SIZE))


#define FLASH_UPDATE_HARDWARE_VERSION (FLASH_UPDATE_ADDR+2)//Ӳ���汾
#define FLASH_UPDATE_RUNUSER_WHICH    (FLASH_UPDATE_ADDR+4)//�洢���е���һ�ݳ�����Ϣ 0:û�п������еĳ���;1:���еĵ�һ�ݳ���;2:���еĵڶ��ݳ���
#define FLASH_UPDATE_FLAG     (FLASH_UPDATE_ADDR+6) //�洢���±�־
#define FLASH_UPDATE_STATUS   (FLASH_UPDATE_ADDR+8) //�洢���µ�״̬
#define FLASH_UPDATE_FILESIZE (FLASH_UPDATE_ADDR+10)//�洢�̼��ļ���С 10,11 12,13,������Ҫ��14��ʼ
#define FLASH_UPDATE_URL_ADDR (FLASH_UPDATE_ADDR+22)   //����URL
#define FLASH_UPDATE_IP_ADDR  (FLASH_UPDATE_ADDR+1022) //����IP
#define FLASH_UPDATE_Port_ADDR (FLASH_UPDATE_ADDR+1072)//����Port
#define FLASH_UPDATE_UserName_ADDR (FLASH_UPDATE_ADDR+1080)//����UserName
#define FLASH_UPDATE_PassWord_ADDR (FLASH_UPDATE_ADDR+1100)//����PassWord

uint8_t STMFLASH_GetStatus(void);				          //���״̬
uint8_t STMFLASH_WaitDone(uint16_t time);				  //�ȴ���������
uint8_t STMFLASH_ErasePage(uint32_t paddr);			  //����ҳ
uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint16_t dat);//д�����
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		           //��������  
void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);						        //ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
char FlashErasePage(uint32_t EraseAddress,uint16_t PageCnt);
char WriteFlashHalfWord(uint32_t WriteAddress,uint16_t data);
//����д��
void Test_Write(uint32_t WriteAddr,uint16_t WriteData);								   
#endif
