#include "iap.h"					  

iapfun jump2app; 
uint16_t iapbuf[1024];

//设置栈顶地址
//addr:栈顶地址

__asm void MSR_MSP( uint32_t ulAddr ) 
{
    MSR MSP, r0 			                   //set Main Stack value
    BX r14
}

//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

/*复位芯片*/
void IAPResetMCU(void)
{
	__disable_fault_irq();   
	NVIC_SystemReset();//重启
}
char IAPUrlUserBinPath[IAPUrlPathLen];//固件程序下载地址
//BootLoader程序修改
#if (IAPProgramSelect==IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)//BootLoader  or  BootLoader+User
	//PCB硬件版本(0-65535)
	
	/*硬件版本号使用说明:(PCB硬件版本号并不是固件程序版本号,请不要混淆,PCB硬件版本号作用如下:)
	假设做的第一批PCB已经投放市场,然后第二批PCB修改了部分电路(不同的电路用户程序不一样).
	用户可以修改上面的硬件版为 1,然后下载BootLoader到单片机(第二批PCB的硬件版本就为1)
  在用户程序里面可以使用IAPGetHardwareVersion();获取BootLoader设置的这个版本号
	if(IAPGetHardwareVersion()==0)//第一批PCB硬件版本
	{	
		执行相应的程序
	}
	else if(IAPGetHardwareVersion()==1)//第二批PCB硬件版本
	{
		执行相应的程序
	}*/
	
  //产品型号(为每一个产品设置一个型号)(要和用户程序里面的型号保持一致)
	char DeviceModel[50] = "MTIO_COUNT_Air724";
	
	//固件程序下载地址
	//char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-29/610262d705632.bin";
  char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-28/6100d7b10fe9c.bin";
	//char IAPUrlUserBinPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-22/60f902f46fb34.bin";

	
	//如果Flash没有存储固件程序下载地址,则此URL作为默认固件程序下载地址
	//https://mnif.cn:443/ota/hardware/STM32ESP8266PP/
#endif

//用户程序修改
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect ==IAPBootLoaderAndUserProgram)
	//当前固件程序的版本号(字母/数字字符串,最长默认20个)
	
	//产品型号(为每一个产品设置一个型号)(设置为常量存储在偏移1024字节倍数的位置)
	char DeviceModel[50] __attribute__((at(FLASH_APP1_ADDR+1024*17))) = "MTIO_COUNT_Air724";
	/*在使用APP控制升级时,APP首先询问设备的型号和版本
		然后根据设备型号访问云端固件信息文件(info.txt文件具体信息看下面)
		如果设备固件程序版本号和云端的不一致,则把固件程序大小和下载地址发送给设备
		设备收到信息以后把固件程序大小和下载地址存储到Flash,设置更新标志,重启!
		BootLoader检测到有更新标志,提取固件程序大小和下载地址,执行升级操作.
	*/
	
	
	//获取云端固件信息文件的地址(设备主动访问升级时事先访问的文件,文件具体含义如下:)
  char IAPUrlUserInfoPath[IAPUrlPathLen]="http://106.110.173.79:8000/uploads/iot/files/2021-07-28/6100d7b10fe9c.bin";
	/*info.txt文件信息如下:(JSON格式)
	{"version":"0.0.1","size":15990,"url":"http://mnif.cn/ota/hardware/STM32Air724BK/user_crc.bin","info":"1.解决了部分BUG
2.优化了部分程序"}
	
	version:0.0.1 解释:云端固件程序版本
	size:15990    解释:云端固件程序大小(字节)
	url:http://mnif.cn/ota/hardware/STM32Air724BK/user_crc.bin 解释:云端固件程序下载地址
	info:1.解决了部分BUG 2.优化了部分程序 解释:本次固件更新的内容信息(用于APP控制升级时,APP的提示信息)
	
	单片机获取以上信息以后和自身的版本号做对比,如果版本号和云端的不一致
	把固件程序大小和下载地址存储到Flash,设置更新标志,重启!
	BootLoader检测到有更新标志,提取固件程序大小和下载地址,执行升级操作.
	*/
#endif

IAPStruct IAPStructValue;



/**
* @brief  用户程序
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect ==IAPBootLoaderAndUserProgram)

void IAPInfoPathInit(void)
{
	printf("解析云端固件信息的地址URL\r\n");
	IAPResolveUrl(IAPUrlUserBinPath);//解析URL
	printf("SSL连接 :%d\r\n",IAPStructValue.SSLEN);//SSL
	printf("IP地址  :%s\r\n",IAPStructValue.IP);//IP
	printf("端口号  :%d\r\n",IAPStructValue.Port);//Port
	printf("文件路径:%s\r\n\r\n",IAPStructValue.Path);//Path
}



/**
* @brief  获取,清除,打印更新状态
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPUpdateDispose(void)
{	
	IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//获取更新状态变量
	if(IAPStructValue.UpdateStatusValue!=0)//升级过程序
	{
		IAPSetUpdateStatus(UpdateStatus_None);//清零升级状态
		if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage)//是运行的新程序
		{
			printf("运行的新程序\r\n");
			MainLen = sprintf((char*)MainBuffer,"{\"UpdateStatus\":1}");
			mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息
			 
		}
		else
		{
			printf("更新错误:%x\r\n",IAPStructValue.UpdateStatusValue);
		  MainLen = sprintf((char*)MainBuffer,"{\"UpdateStatus\":%d}",IAPStructValue.UpdateStatusValue);
			mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息
		}		

		IAPStructValue.UpdateStatusValue=0;
	}			
}

#endif

//设置服务器IP
void SetServerIP(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_IP_ADDR,(uint16_t *)ch,50/2);
}

//获取服务器IP
void GetServerIP(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_IP_ADDR,(uint16_t *)ch,50/2);
}

//设置服务器Port
void SetServerPort(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_Port_ADDR,(uint16_t *)ch,8/2);
}

//获取服务器Port
void GetServerPort(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_Port_ADDR,(uint16_t *)ch,8/2);
}

//设置MQTT客户端UserName
void SetUserName(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_UserName_ADDR,(uint16_t *)ch,20/2);
}

//获取MQTT客户端UserName
void GetUserName(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_UserName_ADDR,(uint16_t *)ch,20/2);
}


//设置MQTT客户端PassWord
void SetPassWord(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_PassWord_ADDR,(uint16_t *)ch,50/2);
}

//获取MQTT客户端PassWord
void GetPassWord(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_PassWord_ADDR,(uint16_t *)ch,50/2);
}

//设置更新标志
void IAPSetUpdateFlage(void)
{
	uint16_t ReadDat = 1;
  STMFLASH_Write(FLASH_UPDATE_FLAG,&ReadDat,1);
}

//清除更新标志
void IAPClearUpdateFlage(void)
{
	uint16_t ReadDat = 0;
  STMFLASH_Write(FLASH_UPDATE_FLAG,&ReadDat,1);
}

//获取更新标志
uint16_t IAPGetUpdateFlage(void)
{
	uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_FLAG,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//设置URL
void IAPSetUpdateUrl(char* ch)
{
	STMFLASH_Write(FLASH_UPDATE_URL_ADDR,(uint16_t *)ch,IAPUrlPathLen/2);
}

//获取URL
void IAPGetUpdateUrl(char* ch)
{
	STMFLASH_Read(FLASH_UPDATE_URL_ADDR,(uint16_t *)ch,IAPUrlPathLen/2);
}

//解析Url;返回值:0 success
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
		if(IAPStructValue.Str != NULL){//带端口号
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
		else{//不带端口号
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

//设置设备更新状态
void IAPSetUpdateStatus(char Status)
{
	uint16_t ReadDat = Status;
  STMFLASH_Write(FLASH_UPDATE_STATUS,&ReadDat,1);
}

//获取设备更新状态
uint16_t IAPGetUpdateStatus(void)
{
  uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_STATUS,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//设置硬件版本
void IAPSetHardwareVersion(uint16_t data)
{
	uint16_t ReadDat = data;
  STMFLASH_Write(FLASH_UPDATE_HARDWARE_VERSION,&ReadDat,1);
}

//获取硬件版本
uint16_t IAPGetHardwareVersion(void)
{
  uint16_t ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_HARDWARE_VERSION,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//设置程序文件大小
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

//获取程序文件大小
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

//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
	uint16_t t;
	uint16_t i=0;
	uint16_t temp;
	uint32_t fwaddr=appxaddr;//当前写入的地址
	uint8_t *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(uint16_t)dfu[1]<<8;
		temp+=(uint16_t)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(uint32_t appxaddr)
{
	if(((*(uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		INTX_DISABLE();
		printf ( "开始执行 APP\n" ); 
		jump2app=(iapfun)*(uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(uint32_t*)appxaddr);					      //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)			
		jump2app();									                  //跳转到APP.
	}
}		 
