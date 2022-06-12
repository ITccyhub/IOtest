#ifndef __IAP_H__
#define __IAP_H__
#include "stm32f1xx_hal.h"	
#include "stmflash.h"
#include "cString.h"
#include "usart.h"
#include "ConfigModuleNoBlock.h"
#include "mqtt.h"
#include "uart4data.h"
//////////////////////////////////////////////////////////////////////////////////	
typedef  void (*iapfun)(void);				  //定义一个函数类型的参数.

#define FLASH_APP_ADDR		0x08010000  	//第一个应用程序起始地址(存放在FLASH)
											                  //保留0X08000000~0X0800FFFF的空间为IAP使用
																				
																				
/*选择IAP程序作为什么程序使用*/
#define IAPBootLoaderProgram        0  //作为BootLoader使用
#define IAPUserProgram              1  //作为用户程序使用
#define IAPBootLoaderAndUserProgram 2  // BootLoader + User

#define IAPProgramSelect IAPUserProgram /*选择IAP程序作为什么程序使用*/


#define UpdateStatus_None              0x00    //无
#define UpdateStatus_WriteAppOk        0x01    //上次成功写入了更新程序
#define UpdateStatus_UrlResolveErr     0x02    //解析URL错误
#define UpdateStatus_FlashEraseBackErr 0x03    //Flash 擦除失败(备份地址)
#define UpdateStatus_FlashBackErr      0x04    //备份失败


#define UpdateStatus_DataAddressError  0xE0    //Flash的高位地址不是0x08 或者 RAM的高位地址不是0x20
#define UpdateStatus_LoopListWriteErr  0xE1    //写缓存错误
#define UpdateStatus_DownloadTimeout   0xE2    //程序下载超时
#define UpdateStatus_MainTimeout       0xE3    //整个程序运行的时间
#define UpdateStatus_FlashWriteErr     0xE4    //Flash 写错误
#define UpdateStatus_FlashEraseUserErr 0xE5    //Flash 擦除失败(运行地址)
#define UpdateStatus_FlashRollBackErr  0xE6    //备份区拷贝到运行区失败

#define UpdateStatus_ReceiveDataLost   0xE7    //数据接不完整

#define UpdateStatus_ProgramInvalid    0xE8    //程序无效

#define UpdateStatus_CheckErr          0xED    //校验错误


#define UpdateStatus_Err               0xEF    //进入更新标记ERR
#define UpdateStatus_WriteAppOkFlage   0xFF    //如果获取的状态是 0x01 则写入该值


#define IAPCheckDataBuffLen 128   //校验的数据个数

#define IAPLoopListBuffLen  2048   //环形队列接收数据缓存大小

#define IAPReadDataIdleTimer 3000 //在该时间内不再接收数据认为接收完数据(ms)

#define IAPUpdateDownloadNumber 3 //尝试下载程序的次数(默认3,最大不得超过255)

#define IAPUrlPathLen 1000  //存储更新文件路径;不要超过1000,必须是偶数

typedef struct {
	char* Str;  //通用变量
	long  Len;  //通用变量
	uint16_t   UpdateStatusValue; //获取更新的状态变量
	
	//Url相关
	char  Url[IAPUrlPathLen];//Url地址
	char  SSLEN;//1:SSL  0:非SSL
	char  IP[256];//服务器IP/域名
	uint16_t   Port;//端口号
	uint16_t  HardwareVersionCopy;//硬件版本:0-65534
	char  Path[IAPUrlPathLen];//Path(文件路径)
	
	/*处理HTTP数据*/
	char  HttpHeadCnt;
	char  HttpHeadOK;//接收到正常的http数据
	char  HttpHeadEndOK;//http的heap接收完成,后面发过来的是数据
	char  HttpDataStartFlage;//下次传进来的是消息体
	
	#if (IAPProgramSelect==IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)//BootLoader  or  BootLoader+User
	
		char  SendGetFileOrderFlag;//发送完获取文件指令标志;1-发送完获取指令
		uint16_t   SendGetFileOrderCnt; //超时回复检测;定时器1ms累加
		  
		uint32_t   ReadDataIdleCnt;   //空闲时间累加变量
		char  ReadDataEndFlag;   //接收完了程序;确认接收完程序以后设置该变量为1
		
		char  UpdateFlag;        //更新标志;1:有更新;0:无更新
		char  ConnectWebFlag;    //连接Web服务器标志;1-连接; 0-未连接
		char  FlashWriteErrFlag; //写入Flash出错标志;1:写入出错;0:无错误
		char  UpdateDownloadCnt; //下载程序的次数
		
		//校验
		char  CheckDataBuff[IAPCheckDataBuffLen+2];//校验数据
		uint32_t   CheckDataCnt;
		uint16_t   CheckValue;
		char  CheckValueErrFlag;    //1:crc校验错误  0:无错误
		uint32_t   FileSizeNow;//固件文件大小-当前接收的
		uint32_t   FileSizeSave;//固件文件大小-存储在flash的
		//Flash地址
		uint32_t   BackupAddress;      //备份程序的Flash地址
		uint32_t   UpdateAddressCnt;   //用于写Flash的时候地址累加
		uint32_t   RunAddress;         //运行的程序地址
		
		//备份/回滚变量
		uint32_t   UserDataSize; //用户程序大小
		uint32_t   UserRunAdddrCnt; //用户程序运行首地址
		uint32_t   UserBackAdddrCnt;//用户程序备份首地址
		uint16_t   ReadDate;
		//超时
		uint32_t   DownloadTimeout;    //程序下载超时累加变量
		uint32_t   MainTimeout;        //整体运行超时累加变量
		
		//人为操作更新
		uint32_t PeopleControlCnt;     //人为操作更新累加变量
		
		//环形队列
		rb_t  LoopList;
		char  LoopListBuff[IAPLoopListBuffLen];//接收程序数据缓存数组
		char  LoopListPutStart;             //控制是不是可以向环形队列写入数据
		char  LoopListWriteErr;             //环形队列写错误
		char  LoopListReadDate[2];          //从环形队列读取数据使用
		u16   LoopListU16Date;              //从环形队列读取数据,组合成16位数据写入Flash
		char  LoopListDataFlag;             //1-环形队列有数据 ; 0-无数据
		
		//标记文件是否有效;1:有效
		char ProgramEfficient;
	#endif
}IAPStruct;

extern IAPStruct IAPStructValue;
//固件程序下载地址
extern 	char IAPUrlUserBinPath[IAPUrlPathLen];
extern char DeviceModel[50];                    //型号



//获取更新的信息
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)
	void IAPInfoPathInit(void); //用户程序调用
	void IAPUpdateDispose(void);//处理更新
#endif

//设置服务器IP
void SetServerIP(char* ch);
//获取服务器IP
void GetServerIP(char* ch);
//设置服务器Port
void SetServerPort(char* ch);
//获取服务器Port
void GetServerPort(char* ch);
//设置MQTT客户端UserName
void SetUserName(char* ch);
//获取MQTT客户端UserName
void GetUserName(char* ch);
//设置MQTT客户端PassWord
void SetPassWord(char* ch);
//获取MQTT客户端PassWord
void GetPassWord(char* ch);

//设置URL
void IAPSetUpdateUrl(char* ch);
//获取URL
void IAPGetUpdateUrl(char* ch);
//解析Url
int IAPResolveUrl(char* ch);
//设置硬件版本
void IAPSetHardwareVersion(uint16_t data);
//获取硬件版本
uint16_t IAPGetHardwareVersion(void);

//设置程序文件大小
void IAPSetFileSize(uint32_t data);
//获取程序文件大小
uint32_t IAPGetFileSize(void);


void IAPResetMCU(void);                   /*复位芯片*/
void IAPSetUpdateFlage(void);             //设置更新标志
void IAPClearUpdateFlage(void);           //清除更新标志
uint16_t  IAPGetUpdateFlage(void);        //获取更新标志

void IAPSetUpdateStatus(char Status);     //设置设备更新状态
uint16_t  IAPGetUpdateStatus(void);       //获取设备更新状态


void INTX_DISABLE(void);
void INTX_ENABLE(void);
void iap_load_app(uint32_t appxaddr);			                                //执行flash里面的app程序
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//在指定地址开始,写入bin
#endif
