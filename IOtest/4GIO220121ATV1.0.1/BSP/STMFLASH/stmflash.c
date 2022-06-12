#include "stmflash.h"
							   
//////////////////////////////////////////////////////////////////////////////////
 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//地址增加2.
	}  
}


char WriteFlashHalfWord(uint32_t WriteAddress,uint16_t data)
{
	 HAL_StatusTypeDef FlashStatus = HAL_ERROR;
   HAL_FLASH_Unlock();	
		
	 FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddress,data);		

	 HAL_FLASH_Lock();		//上锁
	
	 return FlashStatus;
}
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain; //扇区内剩余地址(16位字计算)	   
 	uint16_t i;    
	uint32_t offaddr;   //去掉0X08000000后的地址
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	
	HAL_FLASH_Unlock();					//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)	//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)				//需要擦除
		{
			EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.PageAddress   = secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE;
      EraseInitStruct.NbPages       = 1;
      HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else 
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//等待上次操作完成
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 
		}
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		  pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain*2;	//写地址偏移(16位数据地址,需要*2)	   
		  NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	HAL_FLASH_Lock();		//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


/**
* @brief  擦除Flash  
* @param  EraseAddress  擦除的地址
* @param  PageCnt       擦除连续的几页(一页按照1024计算)
* @param  None
* @retval 4:成功
* @example 
* @waing  如果芯片的Flash是按照2048分页,擦除1页第二个参数应该填写2
**/
char FlashErasePage(uint32_t EraseAddress,uint16_t PageCnt)
{
	uint32_t i=0;
	uint32_t secpos;	   //扇区地址
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	HAL_StatusTypeDef status;
	if(EraseAddress<STM32_FLASH_BASE||(EraseAddress>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return 0;//非法地址
	
	secpos = EraseAddress-STM32_FLASH_BASE;//实际地址
	secpos = secpos/STM_SECTOR_SIZE;//扇区地址 
	printf("secpos = %d\r\n", secpos);
	if(STM_SECTOR_SIZE==2048){PageCnt=PageCnt/2;}		
	//擦除这个扇区
	/* Fill EraseInit structure*/
	 EraseInitStruct.TypeErase  = FLASH_TYPEERASE_PAGES;
		//EraseInitStruct.Banks = FLASH_BANK_1;
	 EraseInitStruct.PageAddress = secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE;
	 EraseInitStruct.NbPages = PageCnt;
	 HAL_FLASH_Unlock();					//解锁
	 status = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	 HAL_FLASH_Lock();//上锁
	 if(status != HAL_OK)
	 {
		 printf("erase fail, PageError = %d\r\n", SECTORError);
	 }	
	 printf("erase success\r\n");
	 return 4;
} 

//////////////////////////////////////////测试用///////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(uint32_t WriteAddr,uint16_t WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}
