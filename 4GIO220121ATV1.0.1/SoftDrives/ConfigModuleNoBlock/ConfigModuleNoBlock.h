#ifndef CONFIGMODULENOBLOCK_H_
#define CONFIGMODULENOBLOCK_H_
#include "stm32f1xx_hal.h"

//��������ʹ��
#define MainBufferLen 1024
extern uint32_t ConfigModuleNoBlockCnt;   //���ú�����ʱ����
extern char ConfigModuleNoBlockFlage;     //1-������ 0-δ������
extern int  ConfigModuleNoBlockCaseValue; //����ִ����һ��Case ���

//��������ʹ��
extern unsigned char MainBuffer[MainBufferLen];    //��������,ȫ��ͨ��
extern unsigned char MainBufferCopy[MainBufferLen];//��������,ȫ��ͨ��
extern uint32_t  MainLen;                          //ȫ��ͨ�ñ���
extern unsigned char *MainString;                  //ȫ��ͨ�ñ���


//����MQTT
extern unsigned char DomainName[55];     //����;���Ƚ�����������
extern unsigned char IP[55];             //IP��ַ

extern unsigned char Port[7] ;   //�˿ں�
extern unsigned char MQTTid[50] ;//ClientID
extern unsigned char MQTTUserName[20] ;//�û���
extern unsigned char MQTTPassWord[50] ;//����
extern unsigned char MQTTkeepAlive ;//������ʱ��

extern unsigned char MQTTPublishTopic[100];//�洢MQTT����������
extern unsigned char MQTTWillMessage[50];   //������Ϣ
extern unsigned char MQTTWillQos;          //��Ϣ�ȼ�
extern unsigned char MQTTWillRetained ;    //�Ƿ���Ҫ��������¼
extern unsigned char MQTTSubscribeTopic[100];//�洢MQTT���ĵ�����




extern void (*ConfigConnectDispose)(char *data,int len);//����һ������ָ�����,��������ģ�鷵�ص�����
void ConfigModuleNoBlock(void);
void FunctionParseCompare(char *data,int len);//�Ա��ж��Ƿ񷵻���Ҫ������
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue);


#endif

