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
typedef  void (*iapfun)(void);				  //����һ���������͵Ĳ���.

#define FLASH_APP_ADDR		0x08010000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
											                  //����0X08000000~0X0800FFFF�Ŀռ�ΪIAPʹ��
																				
																				
/*ѡ��IAP������Ϊʲô����ʹ��*/
#define IAPBootLoaderProgram        0  //��ΪBootLoaderʹ��
#define IAPUserProgram              1  //��Ϊ�û�����ʹ��
#define IAPBootLoaderAndUserProgram 2  // BootLoader + User

#define IAPProgramSelect IAPUserProgram /*ѡ��IAP������Ϊʲô����ʹ��*/


#define UpdateStatus_None              0x00    //��
#define UpdateStatus_WriteAppOk        0x01    //�ϴγɹ�д���˸��³���
#define UpdateStatus_UrlResolveErr     0x02    //����URL����
#define UpdateStatus_FlashEraseBackErr 0x03    //Flash ����ʧ��(���ݵ�ַ)
#define UpdateStatus_FlashBackErr      0x04    //����ʧ��


#define UpdateStatus_DataAddressError  0xE0    //Flash�ĸ�λ��ַ����0x08 ���� RAM�ĸ�λ��ַ����0x20
#define UpdateStatus_LoopListWriteErr  0xE1    //д�������
#define UpdateStatus_DownloadTimeout   0xE2    //�������س�ʱ
#define UpdateStatus_MainTimeout       0xE3    //�����������е�ʱ��
#define UpdateStatus_FlashWriteErr     0xE4    //Flash д����
#define UpdateStatus_FlashEraseUserErr 0xE5    //Flash ����ʧ��(���е�ַ)
#define UpdateStatus_FlashRollBackErr  0xE6    //������������������ʧ��

#define UpdateStatus_ReceiveDataLost   0xE7    //���ݽӲ�����

#define UpdateStatus_ProgramInvalid    0xE8    //������Ч

#define UpdateStatus_CheckErr          0xED    //У�����


#define UpdateStatus_Err               0xEF    //������±��ERR
#define UpdateStatus_WriteAppOkFlage   0xFF    //�����ȡ��״̬�� 0x01 ��д���ֵ


#define IAPCheckDataBuffLen 128   //У������ݸ���

#define IAPLoopListBuffLen  2048   //���ζ��н������ݻ����С

#define IAPReadDataIdleTimer 3000 //�ڸ�ʱ���ڲ��ٽ���������Ϊ����������(ms)

#define IAPUpdateDownloadNumber 3 //�������س���Ĵ���(Ĭ��3,��󲻵ó���255)

#define IAPUrlPathLen 1000  //�洢�����ļ�·��;��Ҫ����1000,������ż��

typedef struct {
	char* Str;  //ͨ�ñ���
	long  Len;  //ͨ�ñ���
	uint16_t   UpdateStatusValue; //��ȡ���µ�״̬����
	
	//Url���
	char  Url[IAPUrlPathLen];//Url��ַ
	char  SSLEN;//1:SSL  0:��SSL
	char  IP[256];//������IP/����
	uint16_t   Port;//�˿ں�
	uint16_t  HardwareVersionCopy;//Ӳ���汾:0-65534
	char  Path[IAPUrlPathLen];//Path(�ļ�·��)
	
	/*����HTTP����*/
	char  HttpHeadCnt;
	char  HttpHeadOK;//���յ�������http����
	char  HttpHeadEndOK;//http��heap�������,���淢������������
	char  HttpDataStartFlage;//�´δ�����������Ϣ��
	
	#if (IAPProgramSelect==IAPBootLoaderProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)//BootLoader  or  BootLoader+User
	
		char  SendGetFileOrderFlag;//�������ȡ�ļ�ָ���־;1-�������ȡָ��
		uint16_t   SendGetFileOrderCnt; //��ʱ�ظ����;��ʱ��1ms�ۼ�
		  
		uint32_t   ReadDataIdleCnt;   //����ʱ���ۼӱ���
		char  ReadDataEndFlag;   //�������˳���;ȷ�Ͻ���������Ժ����øñ���Ϊ1
		
		char  UpdateFlag;        //���±�־;1:�и���;0:�޸���
		char  ConnectWebFlag;    //����Web��������־;1-����; 0-δ����
		char  FlashWriteErrFlag; //д��Flash�����־;1:д�����;0:�޴���
		char  UpdateDownloadCnt; //���س���Ĵ���
		
		//У��
		char  CheckDataBuff[IAPCheckDataBuffLen+2];//У������
		uint32_t   CheckDataCnt;
		uint16_t   CheckValue;
		char  CheckValueErrFlag;    //1:crcУ�����  0:�޴���
		uint32_t   FileSizeNow;//�̼��ļ���С-��ǰ���յ�
		uint32_t   FileSizeSave;//�̼��ļ���С-�洢��flash��
		//Flash��ַ
		uint32_t   BackupAddress;      //���ݳ����Flash��ַ
		uint32_t   UpdateAddressCnt;   //����дFlash��ʱ���ַ�ۼ�
		uint32_t   RunAddress;         //���еĳ����ַ
		
		//����/�ع�����
		uint32_t   UserDataSize; //�û������С
		uint32_t   UserRunAdddrCnt; //�û����������׵�ַ
		uint32_t   UserBackAdddrCnt;//�û����򱸷��׵�ַ
		uint16_t   ReadDate;
		//��ʱ
		uint32_t   DownloadTimeout;    //�������س�ʱ�ۼӱ���
		uint32_t   MainTimeout;        //�������г�ʱ�ۼӱ���
		
		//��Ϊ��������
		uint32_t PeopleControlCnt;     //��Ϊ���������ۼӱ���
		
		//���ζ���
		rb_t  LoopList;
		char  LoopListBuff[IAPLoopListBuffLen];//���ճ������ݻ�������
		char  LoopListPutStart;             //�����ǲ��ǿ������ζ���д������
		char  LoopListWriteErr;             //���ζ���д����
		char  LoopListReadDate[2];          //�ӻ��ζ��ж�ȡ����ʹ��
		u16   LoopListU16Date;              //�ӻ��ζ��ж�ȡ����,��ϳ�16λ����д��Flash
		char  LoopListDataFlag;             //1-���ζ��������� ; 0-������
		
		//����ļ��Ƿ���Ч;1:��Ч
		char ProgramEfficient;
	#endif
}IAPStruct;

extern IAPStruct IAPStructValue;
//�̼��������ص�ַ
extern 	char IAPUrlUserBinPath[IAPUrlPathLen];
extern char DeviceModel[50];                    //�ͺ�



//��ȡ���µ���Ϣ
#if (IAPProgramSelect==IAPUserProgram || IAPProgramSelect == IAPBootLoaderAndUserProgram)
	void IAPInfoPathInit(void); //�û��������
	void IAPUpdateDispose(void);//�������
#endif

//���÷�����IP
void SetServerIP(char* ch);
//��ȡ������IP
void GetServerIP(char* ch);
//���÷�����Port
void SetServerPort(char* ch);
//��ȡ������Port
void GetServerPort(char* ch);
//����MQTT�ͻ���UserName
void SetUserName(char* ch);
//��ȡMQTT�ͻ���UserName
void GetUserName(char* ch);
//����MQTT�ͻ���PassWord
void SetPassWord(char* ch);
//��ȡMQTT�ͻ���PassWord
void GetPassWord(char* ch);

//����URL
void IAPSetUpdateUrl(char* ch);
//��ȡURL
void IAPGetUpdateUrl(char* ch);
//����Url
int IAPResolveUrl(char* ch);
//����Ӳ���汾
void IAPSetHardwareVersion(uint16_t data);
//��ȡӲ���汾
uint16_t IAPGetHardwareVersion(void);

//���ó����ļ���С
void IAPSetFileSize(uint32_t data);
//��ȡ�����ļ���С
uint32_t IAPGetFileSize(void);


void IAPResetMCU(void);                   /*��λоƬ*/
void IAPSetUpdateFlage(void);             //���ø��±�־
void IAPClearUpdateFlage(void);           //������±�־
uint16_t  IAPGetUpdateFlage(void);        //��ȡ���±�־

void IAPSetUpdateStatus(char Status);     //�����豸����״̬
uint16_t  IAPGetUpdateStatus(void);       //��ȡ�豸����״̬


void INTX_DISABLE(void);
void INTX_ENABLE(void);
void iap_load_app(uint32_t appxaddr);			                                //ִ��flash�����app����
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin
#endif
