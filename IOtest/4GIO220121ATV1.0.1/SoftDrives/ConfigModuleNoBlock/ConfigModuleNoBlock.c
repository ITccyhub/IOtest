/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   https://www.cnblogs.com/yangfengwu/p/11674814.html
  ******************************************************************************
	һ,ʹ��˵��
		1,�����³������1ms��ʱ���ж���
			ConfigModuleNoBlockTimerCnt++;
			
		2,�����³���ŵ�������
		while(1)
		{
			ConfigModuleNoBlock();//����ģ��
			
			if(���ڽ��յ�һ������������)
			{
			  ConfigConnectDispose(���յ����ݵ�ַ,���յ����ݸ���);
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
uint32_t ConfigModuleNoBlockCnt =0;   //���ú�����ʱ����,��ʱ���ڲ��ۼ�
int ConfigModuleNoBlockCaseValue = 0; //����ִ����һ��Case ���
char ConfigModuleNoBlockFlage  = 0;   //1-���������� 0-δ����������

uint32_t CompareValue=5000;                //ÿ�� Ms ����һ������
uint32_t SendNextDelay =0;                 //����SendConfigFunction�������һ������,���մ��ݸ� ConfigModuleNoBlockCnt ����дһ�����ݷ��͵�ʱ��
int Cnt = 0;                               //��¼����״̬���͵Ĵ���
char DataReturnFlage  = 0;                 //�Ƿ񷵻���Ԥ�ڵ�����

char HopeReturnData1[20]="";//�洢ϣ�����ص�����
char HopeReturnData2[20]="";//�洢ϣ�����ص�����



//��������ʹ��
unsigned char MainBuffer[MainBufferLen];    //��������,ȫ��ͨ��
unsigned char MainBufferCopy[MainBufferLen];//��������,ȫ��ͨ��
uint32_t  MainLen=0;                        //ȫ��ͨ�ñ���
unsigned char *MainString;                  //ȫ��ͨ�ñ���


//����MQTT
unsigned char DomainName[55]="mqtt.mitaiot.com";  //����;���Ƚ�����������
unsigned char IP[55]="mqtt.mitaiot.com";          //IP��ַ

unsigned char Port[7] = "1883";           //�˿ں�
unsigned char MQTTid[50];                 //ClientID
unsigned char MQTTUserName[20] = "mita";  //�û���
unsigned char MQTTPassWord[50] = "123456";//����
unsigned char MQTTkeepAlive =30;          //������ʱ��

unsigned char MQTTPublishTopic[100];//�洢MQTT����������
unsigned char MQTTWillMessage[50] = "{\"MITABOX\":\"Message\"}";          //������Ϣ
unsigned char MQTTWillQos = 0;     //��Ϣ�ȼ�
unsigned char MQTTWillRetained = 1;//�Ƿ���Ҫ��������¼
unsigned char MQTTSubscribeTopic[100];//�洢MQTT���ĵ�����



void (*ConfigConnectDispose)(char *data,int len);//����һ������ָ�����,��������ģ�鷵�ص�����
void ConfigModuleRunNext(int delay);
/*********************************************************************************************************/

void GSMRstSet(int value)
{
	switch(value)
	{
		case 1:
			 HAL_GPIO_WritePin(RST_4G_GPIO_Port,RST_4G_Pin,GPIO_PIN_SET);//���߸�λģ��	
		break;
		case 0:
			 HAL_GPIO_WritePin(RST_4G_GPIO_Port,RST_4G_Pin,GPIO_PIN_RESET);//����		
		break;
	}
}

/**
* @brief  //��ȡΨһID,WIFI��ȡMAC,GPRS��ȡIMEI
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseGetRTC(char *data,int length)
{
	  char *strx;
	  char temp[20],temp1[20],temp2[10],temp3[5],temp4[5],temp5[5],temp6[5];
		strx=strstr(data,(char*)"+CCLK:");                   //����RTCʱ��
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
			printf("������ʱ����:%04d-%02d-%02d %02d:%02d:%02d\r\n",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//
		  ConfigModuleRunNext(CompareValue);//ִ����һ��
		}

}

/**
* @brief  //��ȡΨһID,WIFI��ȡMAC,GPRS��ȡIMEI
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
		//�����ʹ���Զ���ID,��������������
		
	  ConfigModuleRunNext(CompareValue);//ִ����һ��
	}
	cStringRestore();
}


/**
* @brief  ����IP��ַ
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseIP(char *data,int length)
{
	char *split_buff[4] = {0};
	int len;
	MainString = (unsigned char*)StrBetwString(data,"\r\n","\r\n");//��ȡ������IP
	if(MainString != NULL)
	{	
		len = split((char*)MainString,".",split_buff,4);//���� . �ָ�
		if(len == 4)
		{
			ConfigModuleRunNext(CompareValue);//ִ����һ��
		}
	}
}


/**
* @brief  ��Ϸ���MQTT����Э��
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
	mymqtt.mqtt_connect_info.keepalive = MQTTkeepAlive;//������ʱ��
	mymqtt.mqtt_connect_info.username = MQTTUserName;//�û���
	mymqtt.mqtt_connect_info.password = MQTTPassWord;///����	
	
	mymqtt.mqtt_connect_info.will_topic = MQTTPublishTopic;//��������������
	mymqtt.mqtt_connect_info.will_message = MQTTWillMessage;//��������Ϣ
	mymqtt.mqtt_connect_info.will_qos = MQTTWillQos;//��������Ϣ�ȼ�
	mymqtt.mqtt_connect_info.will_retain = MQTTWillRetained;//�Ƿ���Ҫ������������Ϣ
	mymqtt.mqtt_connect_info.clean_session = 1;//���������Ϣ
	
	len = mqtt_connect(&mymqtt,&str);//���������Ϣ
	
	if(len>0){
		
		COM_Send(&huart4,(uint8_t*)str,len);
	}
}


/**
* @brief  �ж��Ƿ�������MQTT
* @param  data:MQTT���������ص�����
* @param 
* @retval 
* @example 
**/
void FunctionParseConnectMqttAck(char *data,int length)
{
	if(mqtt_connect_ack((unsigned char*)data)==0)
	{
		ConfigModuleRunNext(CompareValue);//ִ����һ��
		
		if(mymqtt.connectCb){//�������ӻص�����
			mymqtt.connectCb();
		}
	}
}


/**
* @brief  ����ָ������ģ��,��������
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
		if(DataReturnFlage == 1)//��һ��ָ����OK��
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//ִ����һ��
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//����3������ִ��
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
				//ConfigModuleNoBlockCaseValue ++;//ִ����һ��
			}
		}
		
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0://�ر�GPRSģ�� ,��ʱ500ms
				GSMRstSet(0); ConfigModuleRunNext(CompareValue-1000); break;
			case 1://����GPRSģ�� ,��ʱ3000ms
				GSMRstSet(1); ConfigModuleRunNext(CompareValue-1000); break;
			case 2://����AT
				SendConfigFunction("AT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 3://�رջ���
				SendConfigFunction("ATE0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 4://ȡʱ��
				SendConfigFunction("AT+CCLK?\r\n",NULL,"+CCLK:",NULL,FunctionParseGetRTC,CompareValue);break;
			case 5://��ȡIMEI
				SendConfigFunction("AT+GSN\r\n",NULL,NULL,NULL,FunctionParseGetID,CompareValue);break;			  
			case 6://��ȡSIM״̬����
				SendConfigFunction("AT+CPIN?\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 7://GPRSע��״̬			
				SendConfigFunction("AT+CFUN=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 8://�Ƿ�����GPRS  
				SendConfigFunction("AT+CGATT?\r\n",NULL,"+CGATT: 1",NULL,FunctionParseCompare,CompareValue);break;			
			case 9://����Ϊ������
				SendConfigFunction("AT+CIPMUX=0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;			
			case 10://͸��ģʽ 
				SendConfigFunction("AT+CIPMODE=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 11://����APN
				SendConfigFunction("AT+CSTT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 12://�����ƶ�����
				SendConfigFunction("AT+CIICR\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 13://��ѯ�����IP��ַ
				SendConfigFunction("AT+CIFSR\r\n",NULL,".",NULL,FunctionParseCompare,CompareValue);
				ConfigModuleRunNext(CompareValue-500);
				break;
			//case 13://����SSL
				//SendConfigFunction("AT+CIPSSL=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 14://����TCP ������
				MainLen = sprintf((char*)MainBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",IP,Port);
				MainBuffer[MainLen]=0;
				SendConfigFunction((char*)MainBuffer,NULL,"CONNECT",NULL,FunctionParseCompare,CompareValue);break;
			case 15://����MQTT
				SendConfigFunction(NULL,FunctionParseConnectMqtt,NULL,NULL,FunctionParseConnectMqttAck,CompareValue);break;
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//�������,������е�ִ�к���
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  ִ����һ��
* @param  delay ��ʱ����ʱ����ִ����һ��
* @param  
* @retval 
* @example 
��ʱ1000ms��ִ����һ��
ConfigModuleRunNext(CompareValue-1000);
����ִ����һ��
ConfigModuleRunNext(CompareValue);
**/
void ConfigModuleRunNext(int delay){
	SendNextDelay = delay;
	DataReturnFlage=1;
	ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
}


/**
* @brief  ��������ָ���
* @param  order          ��������(�ַ���)
* @param  FunctionSend   ��������(���з������ݺ���)
* @param  HopeReturn1    ��������(ϣ�����ص��ַ���1)
* @param  HopeReturn2    ��������(ϣ�����ص��ַ���2)
* @param  FunctionParse  ���ݴ�����
* @param  ConfigFunctionValue  ������һ�����ݵķ���ʱ��,ȡֵΪ ConfigFunctionCntCom:��һ����������
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data,int len),uint32_t ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//�������ݵ����� HopeReturn1,ϣ�����ص�����1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//�������ݵ����� HopeReturn2,ϣ�����ص�����2
	if(FunctionSend!=NULL) FunctionSend();//����һ����������ָ��
	
	ConfigConnectDispose = FunctionParse;//���ݴ�����ָ��
	SendNextDelay = ConfigFunctionValue;//�����һ�����ݴ���ɹ�,���Ʒ�����һ�����ݵ�ʱ��
	
	if(order!=NULL){
		
		COM_Send(&huart4,(uint8_t*)order,strlen(order));
		printf("����4>>%s\r\n",order);
	}
}


/**
* @brief  �������÷�������:�ж��Ƿ񷵻���Ҫ������
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