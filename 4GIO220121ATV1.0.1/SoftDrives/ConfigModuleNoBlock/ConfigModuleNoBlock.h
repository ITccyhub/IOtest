#ifndef CONFIGMODULENOBLOCK_H_
#define CONFIGMODULENOBLOCK_H_
#include "stm32f1xx_hal.h"

//缓存数据使用
#define MainBufferLen 1024
extern uint32_t ConfigModuleNoBlockCnt;   //配置函数延时变量
extern char ConfigModuleNoBlockFlage;     //1-配置完 0-未配置完
extern int  ConfigModuleNoBlockCaseValue; //控制执行哪一条Case 语句

//缓存数据使用
extern unsigned char MainBuffer[MainBufferLen];    //缓存数据,全局通用
extern unsigned char MainBufferCopy[MainBufferLen];//缓存数据,全局通用
extern uint32_t  MainLen;                          //全局通用变量
extern unsigned char *MainString;                  //全局通用变量


//连接MQTT
extern unsigned char DomainName[55];     //域名;优先解析域名连接
extern unsigned char IP[55];             //IP地址

extern unsigned char Port[7] ;   //端口号
extern unsigned char MQTTid[50] ;//ClientID
extern unsigned char MQTTUserName[20] ;//用户名
extern unsigned char MQTTPassWord[50] ;//密码
extern unsigned char MQTTkeepAlive ;//心跳包时间

extern unsigned char MQTTPublishTopic[100];//存储MQTT发布的主题
extern unsigned char MQTTWillMessage[50];   //遗嘱消息
extern unsigned char MQTTWillQos;          //消息等级
extern unsigned char MQTTWillRetained ;    //是否需要服务器记录
extern unsigned char MQTTSubscribeTopic[100];//存储MQTT订阅的主题




extern void (*ConfigConnectDispose)(char *data,int len);//定义一个函数指针变量,用来处理模块返回的数据
void ConfigModuleNoBlock(void);
void FunctionParseCompare(char *data,int len);//对比判断是否返回想要的数据
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue);


#endif

