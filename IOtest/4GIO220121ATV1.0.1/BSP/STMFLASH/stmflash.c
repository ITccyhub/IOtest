#include "stmflash.h"
							   
//////////////////////////////////////////////////////////////////////////////////
 
//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//��ַ����2.
	}  
}


char WriteFlashHalfWord(uint32_t WriteAddress,uint16_t data)
{
	 HAL_StatusTypeDef FlashStatus = HAL_ERROR;
   HAL_FLASH_Unlock();	
		
	 FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddress,data);		

	 HAL_FLASH_Lock();		//����
	
	 return FlashStatus;
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //������ַ
	uint16_t secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	uint16_t secremain; //������ʣ���ַ(16λ�ּ���)	   
 	uint16_t i;    
	uint32_t offaddr;   //ȥ��0X08000000��ĵ�ַ
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	
	HAL_FLASH_Unlock();					//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)	//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)				//��Ҫ����
		{
			EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.PageAddress   = secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE;
      EraseInitStruct.NbPages       = 1;
      HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else 
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//�ȴ��ϴβ������
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 
		}
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		  pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain*2;	//д��ַƫ��(16λ���ݵ�ַ,��Ҫ*2)	   
		  NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	HAL_FLASH_Lock();		//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


/**
* @brief  ����Flash  
* @param  EraseAddress  �����ĵ�ַ
* @param  PageCnt       ���������ļ�ҳ(һҳ����1024����)
* @param  None
* @retval 4:�ɹ�
* @example 
* @waing  ���оƬ��Flash�ǰ���2048��ҳ,����1ҳ�ڶ�������Ӧ����д2
**/
char FlashErasePage(uint32_t EraseAddress,uint16_t PageCnt)
{
	uint32_t i=0;
	uint32_t secpos;	   //������ַ
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	HAL_StatusTypeDef status;
	if(EraseAddress<STM32_FLASH_BASE||(EraseAddress>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return 0;//�Ƿ���ַ
	
	secpos = EraseAddress-STM32_FLASH_BASE;//ʵ�ʵ�ַ
	secpos = secpos/STM_SECTOR_SIZE;//������ַ 
	printf("secpos = %d\r\n", secpos);
	if(STM_SECTOR_SIZE==2048){PageCnt=PageCnt/2;}		
	//�����������
	/* Fill EraseInit structure*/
	 EraseInitStruct.TypeErase  = FLASH_TYPEERASE_PAGES;
		//EraseInitStruct.Banks = FLASH_BANK_1;
	 EraseInitStruct.PageAddress = secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE;
	 EraseInitStruct.NbPages = PageCnt;
	 HAL_FLASH_Unlock();					//����
	 status = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	 HAL_FLASH_Lock();//����
	 if(status != HAL_OK)
	 {
		 printf("erase fail, PageError = %d\r\n", SECTORError);
	 }	
	 printf("erase success\r\n");
	 return 4;
} 

//////////////////////////////////////////������///////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(uint32_t WriteAddr,uint16_t WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}
