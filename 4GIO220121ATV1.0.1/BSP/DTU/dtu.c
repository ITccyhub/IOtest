#include "dtu.h"
#include "usart.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////
char IMEINUM[30]={0};           //IMEI号
char MqttIP[30]="47.111.12.198";//IP地址
uint16_t  MqttPORT=1884;        //IP端口号
uint8_t SignValue=0;            //信号强度
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//EC20各项测试(拨号测试、短信测试、GPRS测试)共用代码
//EC20发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
uint8_t* DTU_check_cmd(uint8_t *str)
{
	  char *strx=0;
    if(USART4_RX_STA&0X8000)		              //接收到一次数据了
    {
        USART4_RX_BUF[USART4_RX_STA&0X7FFF]=0;//添加结束符
        strx=strstr((const char*)USART4_RX_BUF,(const char*)str);
        printf("ATCMD<-\r\n %s\r\n",USART4_RX_BUF);//发送命令			
    }
    return (uint8_t*)strx;
}


//向EC20发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:100ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t Send_cmd_to_dtu(char *cmd,char *ack,uint16_t waittime)
{
    uint8_t res=0;
	  char temp[100]={0};
    USART4_RX_STA=0;	  
		sprintf(temp,"%s",cmd);
		COM_Send(&huart4,(uint8_t*)temp,strlen(temp));//向DTU发送命令		
    printf("ATCMD->\r\n %s\r\n",cmd);             //发送命令
    if(ack&&waittime)		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {            
					  HAL_Delay(100);
            if(USART4_RX_STA&0X8000)//接收到期待的应答结果
            {
                if(DTU_check_cmd(ack))break;//得到有效数据							 
                USART4_RX_STA=0;
            }
        }
        if(waittime==0)res=1;
    }		
    USART4_RX_STA=0;
    return res;
}

#if Air724_Enable

//向Air724发送保存命令
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_save(void)
{
    if(Send_cmd_to_dtu("config,set,save\r\n", "config,save,ok", 5) ==1)
    {
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 1;
    }
		memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
    return 0;
}

//向Air724发送重启命令
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_restart(void)
{
    if(Send_cmd_to_dtu("config,set,reboot\r\n", "config,reboot,ok", 5) ==1)
    {		
        memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);			
        return 1;
    }
		memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
    return 0;
}

//向Air724发送获取IMEI号命令
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_imei_AskIMEI(void)
{
	  char *strx=0; 
	  char signtemp[20];
    if(Send_cmd_to_dtu("config,get,imei\r\n", "config,imei,ok", 5) ==0)//config,imei,ok,863488051100504
    {
			  strx=strstr((const char*)USART4_RX_BUF,"config,imei,ok");
			  if(strx)
				{
						sscanf(strx,"%[^,],%[^,],%[^,],%s\r\n",signtemp,signtemp,signtemp,IMEINUM);
						printf("IMEINUM->\r\n %s\r\n",IMEINUM);
				}			  
        memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);			
        return 0;
    }
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);		
    return 1;
}

//向Air724发送信号强度
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_AskSigner(void)
{
	  char *strx=0;	
    char signtemp[20];
	  char signtemp2[20];
    if(Send_cmd_to_dtu("config,get,csq\r\n","config,csq,ok", 5) ==0)  // config,csq,ok,30
    {
			  strx=strstr((const char*)USART4_RX_BUF,"config,csq,ok");
			  if(strx)
				{
				  sscanf(strx,"%[^,],%[^,],%[^,],%s\r\n",signtemp2,signtemp2,signtemp2,signtemp);
					SignValue=atoi(signtemp);
					printf("SignValue->\r\n %d\r\n",SignValue);
				}		  
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 0;
    }
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);    
    return 1;
}

//向Air724发送网络时间
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_nettime(void)
{
	  char *strx=0;	  
	  char signtemp[20],signtemp2[20],signtemp3[20],signtemp4[20],signtemp5[20],signtemp6[20],signtemp7[20],signtemp8[20];
    if(Send_cmd_to_dtu("config,get,nettime\r\n", "config,nettime,ok", 5) ==0)  //config,nettime,ok,2021,6,17,12,48,28,4
    {
			  strx=strstr((const char*)USART4_RX_BUF,"config,nettime,ok");
			  if(strx)
				{
				    sscanf(strx,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\r\n]",signtemp8,signtemp8,signtemp8,signtemp,signtemp2,signtemp3,signtemp4
					                                                           ,signtemp5,signtemp6,signtemp7);
						
				  sdatestructure.Year=atoi(signtemp)-2000;									    
					sdatestructure.Month=atoi(signtemp2);							
					sdatestructure.Date=atoi(signtemp3);
					stimestructure.Hours=atoi(signtemp4);
					stimestructure.Minutes=atoi(signtemp5);
					stimestructure.Seconds=atoi(signtemp6);
				  HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
          HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
				  printf("网络校时年月日时分秒:%04d-%02d-%02d %02d:%02d:%02d\r\n",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//
		
				}
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 0;
    }
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);    
    return 1;
}

//向Air724发送基站定位
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_lbsloc(void)
{
	  char signtemp[20],signtemp2[20],signtemp3[20];
	  char *strx=0;	
	  float Latitude;   //经度
	  float longitude;  //纬度
    if(Send_cmd_to_dtu("config,get,lbsloc\r\n", "config,lbsloc,ok", 60) ==0)  //config,lbsloc,ok,120.6700007,031.4332115
    {
			  strx=strstr((const char*)USART4_RX_BUF,"config,lbsloc,ok");
			  if(strx)
				{
				    sscanf(strx,"%[^,],%[^,],%[^,],%[^,],%[^\r\n]",signtemp3,signtemp3,signtemp3,signtemp,signtemp2);
						Latitude=atof(signtemp);
				    longitude=atof(signtemp2);
						printf("Latitude->\r\n %f\r\n",Latitude);
					 	printf("longitude->\r\n %f\r\n",longitude);
				}			 
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 0;
    } 
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);		
    return 1;
}

//向Air724发送第二路TCP模式
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_TCP(void)
{
	  char datatemp[200];
	  sprintf(datatemp,"config,set,tcp,2,ttluart,1,1,keepaliv,60,%s,%d,0,,0,,3,OTA\r\n",MqttIP,MqttPORT);
    if(Send_cmd_to_dtu(datatemp,"config,tcp,ok", 5) ==0)  // config,tcp,ok
    {			  
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 0;
    } 
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);		
    return 1;
}

//向Air724发送第二路取消TCP模式
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t dtu_enter_delnetchan(void)
{	 
    if(Send_cmd_to_dtu("config,set,delnetchan,2\r\n", "config,delnetchan,ok", 5) ==0)  //config,delnetchan,ok
    {
			  memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
        return 0;
    } 
    memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);		
    return 1;
}

uint8_t dtu_status(void)//4G模块状态
{
    uint8_t res;
		res =dtu_enter_AskSigner(); //信号强度
		if ( res != 0 )
    {
        return 1;
    }	
    res =dtu_imei_AskIMEI();	  //IMEI号	
		if ( res != 0 )
    {
        return 1;
    }	

    res =dtu_enter_nettime();	  //获取网络时间
		if ( res != 0 )
    {
        return 1;
    }

//    res=dtu_enter_lbsloc();    //获取基站定位
//		if ( res != 0 )
//    {
//        return 1;
//    }		
    return 0;
}


uint8_t dtu_TCPON(uint8_t state)//TCP  0:关闭 1:开启
{
	uint8_t res;
	if(state==1){//开启TCP
		
		res =dtu_enter_TCP();
	  if( res != 0 )
    {
        return 1;
    }		
		res=dtu_enter_save();
	  if( res != 0 )
    {
        return 1;
    }		
	}
	
	if(state==0){//关闭TCP
		
		res = dtu_enter_delnetchan();
	  if( res != 0 )
    {
        return 1;
    }		
		res=dtu_enter_save();
	  if( res != 0 )
    {
        return 1;
    }		
		
	}
	return 0;
	
}

#endif
