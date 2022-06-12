/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   https://www.cnblogs.com/yangfengwu/p/11674814.html
  ******************************************************************************
	一,使用说明
		1,把以下程序放在1ms定时器中断中
			ConfigModuleNoBlockTimerCnt++;
			
		2,把以下程序放到主函数
		while(1)
		{
			ConfigModuleNoBlock();//配置模块
			
			if(串口接收到一条完整的数据)
			{
			  ConfigConnectDispose(接收的数据地址,接收的数据个数);
			}
		}
			
  ******************************************************************************
  */

#include "ConfigModuleNoBlock.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"
#include "cString.h"
#include "mqtt.h"
#include "rtc.h"
uint32_t ConfigModuleNoBlockCnt =0;   //配置函数延时变量,定时器内部累加
int ConfigModuleNoBlockCaseValue = 0; //控制执行哪一条Case 语句
char ConfigModuleNoBlockFlage  = 0;   //1-配置完连接 0-未配置完连接

uint32_t CompareValue=5000;                //每隔 Ms 发送一次数据
uint32_t SendNextDelay =0;                 //接收SendConfigFunction函数最后一个参数,最终传递给 ConfigModuleNoBlockCnt 控制写一条数据发送的时间
int Cnt = 0;                               //记录运行状态发送的次数
char DataReturnFlage  = 0;                 //是否返回了预期的数据

char HopeReturnData1[20]="";//存储希望返回的数据
char HopeReturnData2[20]="";//存储希望返回的数据



//缓存数据使用
unsigned char MainBuffer[MainBufferLen];    //缓存数据,全局通用
unsigned char MainBufferCopy[MainBufferLen];//缓存数据,全局通用
uint32_t  MainLen=0;                        //全局通用变量
unsigned char *MainString;                  //全局通用变量


//连接MQTT
unsigned char DomainName[55]="mqtt.mitaiot.com";  //域名;优先解析域名连接
unsigned char IP[55]="mqtt.mitaiot.com";          //IP地址

unsigned char Port[7] = "1883";           //端口号
unsigned char MQTTid[50];                 //ClientID
unsigned char MQTTUserName[20] = "mita";  //用户名
unsigned char MQTTPassWord[50] = "123456";//密码
unsigned char MQTTkeepAlive =30;          //心跳包时间

unsigned char MQTTPublishTopic[100];//存储MQTT发布的主题
unsigned char MQTTWillMessage[50] = "{\"MITABOX\":\"Message\"}";          //遗嘱消息
unsigned char MQTTWillQos = 0;     //消息等级
unsigned char MQTTWillRetained = 1;//是否需要服务器记录
unsigned char MQTTSubscribeTopic[100];//存储MQTT订阅的主题



void (*ConfigConnectDispose)(char *data,int len);//定义一个函数指针变量,用来处理模块返回的数据
void ConfigModuleRunNext(int delay);
/*********************************************************************************************************/

void GSMRstSet(int value)
{
	switch(value)
	{
		case 1:
			 HAL_GPIO_WritePin(RST_4G_GPIO_Port,RST_4G_Pin,GPIO_PIN_SET);//拉高复位模块	
		break;
		case 0:
			 HAL_GPIO_WritePin(RST_4G_GPIO_Port,RST_4G_Pin,GPIO_PIN_RESET);//拉低		
		break;
	}
}

/**
* @brief  //获取唯一ID,WIFI获取MAC,GPRS获取IMEI
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseGetRTC(char *data,int length)
{
	  char *strx;
	  char temp[20],temp1[20],temp2[10],temp3[5],temp4[5],temp5[5],temp6[5];
		strx=strstr(data,(char*)"+CCLK:");                   //设置RTC时间
		if(strx)//+CCLK: "21/08/12,12:49:35+32"
		{
			sscanf(strx,"%[^\"]\"%[^/]/%[^/]/%[^,],%[^:]:%[^:]:%[^+]",temp,temp1,temp2,temp3,temp4,temp5,temp6);
			sdatestructure.Year=atoi(temp1);									    
			sdatestructure.Month=atoi(temp2);							
			sdatestructure.Date=atoi(temp3);
			stimestructure.Hours=atoi(temp4);
			stimestructure.Minutes=atoi(temp5);
			stimestructure.Seconds=atoi(temp6);
			HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN); 
			printf("年月日时分秒:%04d-%02d-%02d %02d:%02d:%02d\r\n",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//
		  ConfigModuleRunNext(CompareValue);//执行下一条
		}

}

/**
* @brief  //获取唯一ID,WIFI获取MAC,GPRS获取IMEI
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseGetID(char *data,int length)
{
	MainString = (unsigned char*)StrBetwString(data,"\r\n","\r\n");
	
	if(strlen((char*)MainString) ==15)
	{
		memset(MQTTid,0,sizeof(MQTTid));
		memcpy(MQTTid,MainString,15);
		//如果想使用自定义ID,请屏蔽以上两行
		
	  ConfigModuleRunNext(CompareValue);//执行下一条
	}
	cStringRestore();
}


/**
* @brief  解析IP地址
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseIP(char *data,int length)
{
	char *split_buff[4] = {0};
	int len;
	MainString = (unsigned char*)StrBetwString(data,"\r\n","\r\n");//获取解析的IP
	if(MainString != NULL)
	{	
		len = split((char*)MainString,".",split_buff,4);//符号 . 分割
		if(len == 4)
		{
			ConfigModuleRunNext(CompareValue);//执行下一条
		}
	}
}


/**
* @brief  组合发送MQTT连接协议
* @param  
* @param 
* @retval 
* @example 
**/
void FunctionParseConnectMqtt(void)
{
	unsigned char *str;
	int len;	
	mymqtt.mqtt_connect_info.client_id = MQTTid;       //client_id
	mymqtt.mqtt_connect_info.keepalive = MQTTkeepAlive;//心跳包时间
	mymqtt.mqtt_connect_info.username = MQTTUserName;//用户名
	mymqtt.mqtt_connect_info.password = MQTTPassWord;///密码	
	
	mymqtt.mqtt_connect_info.will_topic = MQTTPublishTopic;//遗嘱发布的主题
	mymqtt.mqtt_connect_info.will_message = MQTTWillMessage;//遗嘱的消息
	mymqtt.mqtt_connect_info.will_qos = MQTTWillQos;//遗嘱的消息等级
	mymqtt.mqtt_connect_info.will_retain = MQTTWillRetained;//是否需要服务器保留消息
	mymqtt.mqtt_connect_info.clean_session = 1;//清除连接信息
	
	len = mqtt_connect(&mymqtt,&str);//打包连接信息
	
	if(len>0){
		
		COM_Send(&huart4,(uint8_t*)str,len);
	}
}


/**
* @brief  判断是否连接上MQTT
* @param  data:MQTT服务器返回的数据
* @param 
* @retval 
* @example 
**/
void FunctionParseConnectMqttAck(char *data,int length)
{
	if(mqtt_connect_ack((unsigned char*)data)==0)
	{
		ConfigModuleRunNext(CompareValue);//执行下一条
		
		if(mymqtt.connectCb){//调用连接回调函数
			mymqtt.connectCb();
		}
	}
}


/**
* @brief  发送指令配置模块,非阻塞版
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
**/
void ConfigModuleNoBlock(void)
{
	if(ConfigModuleNoBlockCnt>CompareValue && ConfigModuleNoBlockFlage==0)
	{
		ConfigModuleNoBlockCnt=0;
		if(DataReturnFlage == 1)//上一条指令是OK的
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//执行下一条
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//超过3次重新执行
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
				//ConfigModuleNoBlockCaseValue ++;//执行下一条
			}
		}
		
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0://关闭GPRS模块 ,延时500ms
				GSMRstSet(0); ConfigModuleRunNext(CompareValue-1000); break;
			case 1://启动GPRS模块 ,延时3000ms
				GSMRstSet(1); ConfigModuleRunNext(CompareValue-1000); break;
			case 2://发送AT
				SendConfigFunction("AT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 3://关闭回显
				SendConfigFunction("ATE0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 4://取时间
				SendConfigFunction("AT+CCLK?\r\n",NULL,"+CCLK:",NULL,FunctionParseGetRTC,CompareValue);break;
			case 5://获取IMEI
				SendConfigFunction("AT+GSN\r\n",NULL,NULL,NULL,FunctionParseGetID,CompareValue);break;			  
			case 6://读取SIM状态正常
				SendConfigFunction("AT+CPIN?\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 7://GPRS注册状态			
				SendConfigFunction("AT+CFUN=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 8://是否附着了GPRS  
				SendConfigFunction("AT+CGATT?\r\n",NULL,"+CGATT: 1",NULL,FunctionParseCompare,CompareValue);break;			
			case 9://设置为单链接
				SendConfigFunction("AT+CIPMUX=0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;			
			case 10://透传模式 
				SendConfigFunction("AT+CIPMODE=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 11://设置APN
				SendConfigFunction("AT+CSTT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 12://激活移动场景
				SendConfigFunction("AT+CIICR\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 13://查询分配的IP地址
				SendConfigFunction("AT+CIFSR\r\n",NULL,".",NULL,FunctionParseCompare,CompareValue);
				ConfigModuleRunNext(CompareValue-500);
				break;
			//case 13://启用SSL
				//SendConfigFunction("AT+CIPSSL=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 14://连接TCP 服务器
				MainLen = sprintf((char*)MainBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",IP,Port);
				MainBuffer[MainLen]=0;
				SendConfigFunction((char*)MainBuffer,NULL,"CONNECT",NULL,FunctionParseCompare,CompareValue);break;
			case 15://连接MQTT
				SendConfigFunction(NULL,FunctionParseConnectMqtt,NULL,NULL,FunctionParseConnectMqttAck,CompareValue);break;
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//这句必须加,清除所有的执行函数
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  执行下一条
* @param  delay 延时多少时间再执行下一条
* @param  
* @retval 
* @example 
延时1000ms再执行下一条
ConfigModuleRunNext(CompareValue-1000);
立即执行下一条
ConfigModuleRunNext(CompareValue);
**/
void ConfigModuleRunNext(int delay){
	SendNextDelay = delay;
	DataReturnFlage=1;
	ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
}


/**
* @brief  发送配置指令函数
* @param  order          发送数据(字符串)
* @param  FunctionSend   发送数据(运行发送数据函数)
* @param  HopeReturn1    接收数据(希望返回的字符串1)
* @param  HopeReturn2    接收数据(希望返回的字符串2)
* @param  FunctionParse  数据处理函数
* @param  ConfigFunctionValue  控制下一条数据的发送时间,取值为 ConfigFunctionCntCom:下一条立即发送
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//拷贝数据到数组 HopeReturn1,希望返回的数据1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//拷贝数据到数组 HopeReturn2,希望返回的数据2
	if(FunctionSend!=NULL) FunctionSend();//调用一个函数发送指令
	
	ConfigConnectDispose = FunctionParse;//传递处理函数指针
	SendNextDelay = ConfigFunctionValue;//如果上一条数据处理成功,控制发送下一条数据的时间
	
	if(order!=NULL){
		
		COM_Send(&huart4,(uint8_t*)order,strlen(order));
		printf("串口4>>%s\r\n",order);
	}
}


/**
* @brief  处理配置返回数据:判断是否返回想要的数据
* @param  data
* @param  
* @retval 
* @example 
**/
void FunctionParseCompare(char *data,int len)
{
	if(strlen(HopeReturnData1) != 0 && strstr(data, HopeReturnData1))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
	if(strlen(HopeReturnData2) != 0 && strstr(data, HopeReturnData2))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
}