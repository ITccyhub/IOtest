#include "iap.h"					  

iapfun jump2app; 
uint16_t iapbuf[1024];

//����ջ����ַ
//addr:ջ����ַ

__asm void MSR_MSP( uint32_t ulAddr ) 
{
    MSR MSP, r0 			                   //set Main Stack value
    BX r14
}

//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

/*��λоƬ*/
void IAPResetMCU(void)
{
	__disable_fault_irq();   
	NVIC_SystemReset();//����
}
char IAPUrlUserBinPath[IAPUrlPathLen];//�̼��������ص�ַ
//BootLoader�����޸�
#if (IAPProgramSelect==IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)//BootLoader  or  BootLoader+User
	//PCBӲ���汾(0-65535)
	
	/*Ӳ���汾��ʹ��˵��:(PCBӲ���汾�Ų����ǹ̼�����汾��,�벻Ҫ����,PCBӲ���汾����������:)
	�������ĵ�һ��PCB�Ѿ�Ͷ���г�,Ȼ��ڶ���PCB�޸��˲��ֵ�·(��ͬ�ĵ�·�û�����һ��).
	�û������޸������Ӳ����Ϊ 1,Ȼ������BootLoader����Ƭ��(�ڶ���PCB��Ӳ���汾��Ϊ1)
  ���û������������ʹ��IAPGetHardwareVersion();��ȡBootLoader���õ�����汾��
	if(IAPGetHardwareVersion()==0)//��һ��PCBӲ���汾
	{	
		ִ����Ӧ�ĳ���
	}
	else if(IAPGetHardwareVersion()==1)//�ڶ���PCBӲ���汾
	{
		ִ����Ӧ�ĳ���
	}*/
	
  //��Ʒ�ͺ�(Ϊÿһ����Ʒ����һ���ͺ�)(Ҫ���û�����������ͺű���һ��)
	char DeviceModel[50] = "MTIO_COUNT_Air724";
	
	//�̼��������ص�ַ
	//char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-29/610262d705632.bin";
  char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-28/6100d7b10fe9c.bin";
	//char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-22/60f902f46fb34.bin";

	
	//���Flashû�д洢�̼��������ص�ַ,���URL��ΪĬ�Ϲ̼��������ص�ַ
	//https://mnif.cn:443/ota/hardware/STM32ESP8266PP/
#endif

//�û������޸�
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect ==IAPBootLoaderAndUserProgram)
	//��ǰ�̼�����İ汾��(��ĸ/�����ַ���,�Ĭ��20��)
	
	//��Ʒ�ͺ�(Ϊÿһ����Ʒ����һ���ͺ�)(����Ϊ�����洢��ƫ��1024�ֽڱ�����λ��)
	char DeviceModel[50] __attribute__((at(FLASH_APP1_ADDR+1024*17))) = "MTIO_COUNT_Air724";
	/*��ʹ��APP��������ʱ,APP����ѯ���豸���ͺźͰ汾
		Ȼ������豸�ͺŷ����ƶ˹̼���Ϣ�ļ�(info.txt�ļ�������Ϣ������)
		����豸�̼�����汾�ź��ƶ˵Ĳ�һ��,��ѹ̼������С�����ص�ַ���͸��豸
		�豸�յ���Ϣ�Ժ�ѹ̼������С�����ص�ַ�洢��Flash,���ø��±�־,����!
		BootLoader��⵽�и��±�־,��ȡ�̼������С�����ص�ַ,ִ����������.
	*/
	
	
	//��ȡ�ƶ˹̼���Ϣ�ļ��ĵ�ַ(�豸������������ʱ���ȷ��ʵ��ļ�,�ļ����庬������:)
  char IAPUrlUserInfoPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-28/6100d7b10fe9c.bin";
	/*info.txt�ļ���Ϣ����:(JSON��ʽ)
	{"version":"0.0.1","size":15990,"url":"http://mnif.cn/ota/hardware/STM32Air724BK/user_crc.bin","info":"1.����˲���BUG
2.�Ż��˲��ֳ���"}
	
	version:0.0.1 ����:�ƶ˹̼�����汾
	size:15990    ����:�ƶ˹̼������С(�ֽ�)
	url:http://mnif.cn/ota/hardware/STM32Air724BK/user_crc.bin ����:�ƶ˹̼��������ص�ַ
	info:1.����˲���BUG 2.�Ż��˲��ֳ��� ����:���ι̼����µ�������Ϣ(����APP��������ʱ,APP����ʾ��Ϣ)
	
	��Ƭ����ȡ������Ϣ�Ժ������İ汾�����Ա�,����汾�ź��ƶ˵Ĳ�һ��
	�ѹ̼������С�����ص�ַ�洢��Flash,���ø��±�־,����!
	BootLoader��⵽�и��±�־,��ȡ�̼������С�����ص�ַ,ִ����������.
	*/
#endif

IAPStruct IAPStructValue;



/**
* @brief  �û�����
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect ==IAPBootLoaderAndUserProgram)

void IAPInfoPathInit(void)
{
	printf("�����ƶ˹̼���Ϣ�ĵ�ַURL\r\n");
	IAPResolveUrl(IAPUrlUserBinPath);//����URL
	printf("SSL���� :%d\r\n",IAPStructValue.SSLEN);//SSL
	printf("IP��ַ  :%s\r\n",IAPStructValue.IP);//IP
	printf("�˿ں�  :%d\r\n",IAPStructValue.Port);//Port
	printf("�ļ�·��:%s\r\n\r\n",IAPStructValue.Path);//Path
}



/**
* @brief  ��ȡ,���,��ӡ����״̬
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPUpdateDispose(void)
{	
	IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//��ȡ����״̬����
	if(IAPStructValue.UpdateStatusValue!=0)//����������
	{
		IAPSetUpdateStatus(UpdateStatus_None);//��������״̬
		if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage)//�����е��³���
		{
			printf("���е��³���\r\n");
			MainLen = sprintf((char*)MainBuffer,"{\"UpdateStatus\":1}");
			mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ
			 
		}
		else
		{
			printf("���´���:%x\r\n",IAPStructValue.UpdateStatusValue);
		  MainLen = sprintf((char*)MainBuffer,"{\"UpdateStatus\":%d}",IAPStructValue.UpdateStatusValue);
			mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ
		}		

		IAPStructValue.UpdateStatusValue=0;
	}			
}

#endif

//���÷�����IP
void SetServerIP(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_IP_ADDR,(uint16_t *)ch,50/2);
}

//��ȡ������IP
void GetServerIP(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_IP_ADDR,(uint16_t *)ch,50/2);
}

//���÷�����Port
void SetServerPort(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_Port_ADDR,(uint16_t *)ch,8/2);
}

//��ȡ������Port
void GetServerPort(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_Port_ADDR,(uint16_t *)ch,8/2);
}

//����MQTT�ͻ���UserName
void SetUserName(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_UserName_ADDR,(uint16_t *)ch,20/2);
}

//��ȡMQTT�ͻ���UserName
void GetUserName(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_UserName_ADDR,(uint16_t *)ch,20/2);
}


//����MQTT�ͻ���PassWord
void SetPassWord(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_PassWord_ADDR,(uint16_t *)ch,50/2);
}

//��ȡMQTT�ͻ���PassWord
void GetPassWord(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_PassWord_ADDR,(uint16_t *)ch,50/2);
}

//���ø��±�־
void IAPSetUpdateFlage(void)
{
	uint16_t ReadDat = 1;
  STMFLASH_Write(FLASH_UPDATE_FLAG,&ReadDat,1);
}

//������±�־
void IAPClearUpdateFlage(void)
{
	uint16_t ReadDat = 0;
  STMFLASH_Write(FLASH_UPDATE_FLAG,&ReadDat,1);
}

//��ȡ���±�־
uint16_t IAPGetUpdateFlage(void)
{
	uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_FLAG,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//����URL
void IAPSetUpdateUrl(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_URL_ADDR,(uint16_t *)ch,IAPUrlPathLen/2);
}

//��ȡURL
void IAPGetUpdateUrl(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_URL_ADDR,(uint16_t *)ch,IAPUrlPathLen/2);
}

//����Url;����ֵ:0 success
//http://mnif.cn/ota/hardware/STM32ESP8266PP/
//https://mnif.cn/ota/hardware/STM32ESP8266PP/
//http://mnif.cn:80/ota/hardware/STM32ESP8266PP/
//https://mnif.cn:443/ota/hardware/STM32ESP8266PP/
int IAPResolveUrl(char* ch)
{
	if(ch != NULL && (strlen(ch)>5)){
		//http or https
		if(memcmp(ch,"https",5)==0){IAPStructValue.SSLEN = 1; IAPStructValue.Len = 5;}	
		else if(memcmp(ch,"http",4)==0){IAPStructValue.SSLEN = 0; IAPStructValue.Len = 4;}
		else{return -2;}
		
		//IP
		IAPStructValue.Str = StrBetwString(ch+IAPStructValue.Len,"://",":");
		if(IAPStructValue.Str != NULL){//���˿ں�
			memset(IAPStructValue.IP,0,sizeof(IAPStructValue.IP));
			memcpy(IAPStructValue.IP,IAPStructValue.Str,strlen(IAPStructValue.Str));
			IAPStructValue.Len = IAPStructValue.Len+3+strlen(IAPStructValue.IP);//14/15
			cStringRestore();
			
			//Port
			IAPStructValue.Str = StrBetwString(ch+IAPStructValue.Len,":","/");
			if(IAPStructValue.Str != NULL){
				if(strlen(IAPStructValue.Str)<6 && atoi(IAPStructValue.Str)!=0){
					IAPStructValue.Port = atoi(IAPStructValue.Str);
					IAPStructValue.Len = IAPStructValue.Len + 1 + strlen(IAPStructValue.Str);//18/19
					
					cStringRestore();
					//Path
					memcpy(IAPStructValue.Path,ch+IAPStructValue.Len,1000-IAPStructValue.Len);
				}
				else{
					return -4;
				}
			}
			else
			{
				cStringRestore();
				return -5;
			}
		}
		else{//�����˿ں�
			cStringRestore();
			if(IAPStructValue.SSLEN)
			{
				IAPStructValue.Port = 443;
			}
			else
			{
				IAPStructValue.Port = 80;
			}
			
			//IP
			IAPStructValue.Str = StrBetwString(ch+IAPStructValue.Len,"://","/");
			if(IAPStructValue.Str != NULL)
			{	
				memset(IAPStructValue.IP,0,sizeof(IAPStructValue.IP));
				memcpy(IAPStructValue.IP,IAPStructValue.Str,strlen(IAPStructValue.Str));
				IAPStructValue.Len = IAPStructValue.Len+3+strlen(IAPStructValue.IP);//14/15
			}
			else
			{
				return -6;
			}
			cStringRestore();
			//Path
			memcpy(IAPStructValue.Path,ch+IAPStructValue.Len,1000-IAPStructValue.Len);
		}
	}
	else{
		return -1;
	}
	return 0;
}

//�����豸����״̬
void IAPSetUpdateStatus(char Status)
{
	uint16_t ReadDat = Status;
  STMFLASH_Write(FLASH_UPDATE_STATUS,&ReadDat,1);
}

//��ȡ�豸����״̬
uint16_t IAPGetUpdateStatus(void)
{
  uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_STATUS,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//����Ӳ���汾
void IAPSetHardwareVersion(uint16_t data)
{
	uint16_t ReadDat = data;
  STMFLASH_Write(FLASH_UPDATE_HARDWARE_VERSION,&ReadDat,1);
}

//��ȡӲ���汾
uint16_t IAPGetHardwareVersion(void)
{
  uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_HARDWARE_VERSION,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//���ó����ļ���С
void IAPSetFileSize(uint32_t data)
{
	uint16_t ReadDat[2];
	uint32_t ReadData;
	ReadData = data;
	ReadData = ReadData>>16;
	ReadDat[0] = ReadData&0x0000FFFF;
	ReadDat[1] = data&0x0000FFFF;
	
	STMFLASH_Write(FLASH_UPDATE_FILESIZE,ReadDat,2);
}

//��ȡ�����ļ���С
uint32_t IAPGetFileSize(void)
{
	uint16_t ReadDat[2];
	uint32_t ReadData;
	STMFLASH_Read(FLASH_UPDATE_FILESIZE,ReadDat,2);
	
	ReadData = ReadDat[0];
	ReadData = ReadData<<16;
	ReadData = ReadData|ReadDat[1];
		
	return ReadData == 0xFFFFFFFF ? 0 : ReadData;
}

//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
	uint16_t t;
	uint16_t i=0;
	uint16_t temp;
	uint32_t fwaddr=appxaddr;//��ǰд��ĵ�ַ
	uint8_t *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(uint16_t)dfu[1]<<8;
		temp+=(uint16_t)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(uint32_t appxaddr)
{
	if(((*(uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		INTX_DISABLE();
		printf ( "��ʼִ�� APP\n" ); 
		jump2app=(iapfun)*(uint32_t*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(uint32_t*)appxaddr);					      //��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)			
		jump2app();									                  //��ת��APP.
	}
}		 
