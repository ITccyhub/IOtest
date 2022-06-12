#include "uart1data.h"
/*****************************************************************************************
************************固件Version_Num说明***********************************************
******V1.0.0 基础版本可采集模拟量数据、IO、标准Modbus数据*********************************
******V1.0.1 增加固德威光伏逆变器数据采集功能 、优化通用Modbus主站************************
*************增加MQTT客户端用户名称和密码配置功能*****************************************
******************************************************************************************
******************************************************************************************/
char Version_Num[30]="V1.0.1";//版本号
uint8_t Print_Flag=0;           //允许打印标志      1:允许     0:关闭
uint8_t Print_ConfigFlag;       //打印配置信息      1:打印     0:结束
uint8_t GREEN_Value=0;          //绿灯值            0:不运行   1:运行
uint8_t RED_Value=0;            //红灯值            0:无故障   1:故障

uint8_t IN1=0;            //IN1值  0:无输入 1:有输入
uint8_t IN2=0;            //IN2值
uint8_t IN3=0;            //IN3值

uint8_t DTU_Heart=0;      //DTU心跳状态
float AD1_Value=0;        //工程量 0-5V 4-20MA
float AD2_Value=0;        //工程量
float AD1_PhyValue=0;     //物理量值
float AD2_PhyValue=0;     //物理量值

/*****************************存EEROM参数*************************/

uint8_t Device485_type=0;  //设备485类型  0:不用  1:MODBUS主站 2:MODBUS从站 3:GWHT 4:GWMT
uint8_t CNC_UpType=2;      //0:不上报 1:变化上报 2:定时上报
uint16_t CNC_UpTime=60;    //1:单位秒
uint8_t CNC_YELLOW=2;      //0:常亮结束 1:闪烁结束  2:脉冲计数
uint8_t CNC_GREEN=0;       //0:常亮运行 1:闪烁运行
uint8_t CNC_RED=0;         //0:常亮故障 1:闪烁故障
uint8_t AD1_Type=0;        //0:关       1:4-20ma   2:0-5V 
uint8_t AD2_Type=0;        //0:关       1:4-20ma   2:0-5V
float AD1_PhyValue1=0;     //物理量范围1
float AD1_PhyValue2=100;   //物理量范围2
float AD2_PhyValue1=0;     //物理量范围1
float AD2_PhyValue2=100;   //物理量范围2
uint32_t TIM3_CAPTURE_VAL1=0;//定时器3通道1计数

uint32_t FileSize=0;       //OTA文件大小
uint8_t  UpdateFlage=0;    //升级标志

void DTU_SendModbus(uint8_t Addr)//Modbus从机数据
{
      static cJSON *root = NULL;      //cjson对象指针
      char *name;	
	    char TimeBuf[50];		  
	    char DataBuf[50];
	    static uint8_t i;      
		  struct ModbusPack *p =NULL;//
			p=&Modbus03Slave[Addr];

			 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			 root = cJSON_CreateObject();
			 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));       //本地RTC时间	         
			 if(p->Modbus_Err!=0){//从机功能码通许有故障
				 
					sprintf(TimeBuf,"%d",p->Modbus_Err);
					sprintf(DataBuf,"Slave%d",Addr+Master_ADDR);	
					cJSON_AddItemToObject(root,DataBuf,cJSON_CreateString(TimeBuf));          //从机故障类型						 
			 }
			 else if(p->Modbus_Err==0){//从机通许正常
				 
					sprintf(TimeBuf,"%d",p->Modbus_Err);
					sprintf(DataBuf,"Slave%d",Addr+Master_ADDR);	
					cJSON_AddItemToObject(root,DataBuf,cJSON_CreateString(TimeBuf));          //从机故障类型						 
					for(i=0;i<ValueOrLenth;i++){//打包数据
						
							sprintf(TimeBuf,"Add%d",StartAddr+i);
							sprintf(DataBuf,"0x%X",p->MobusTemp[i]);				
							cJSON_AddItemToObject(root,TimeBuf,cJSON_CreateString(DataBuf));      //从机数据
					}					 
			 }					 
			 name=cJSON_PrintUnformatted(root);					
			 printf("\r\n%s\r\n",name);				
			 cJsonFree(name);		   //释放内存		
			 cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	 			
		
}

void DTU_SendSearch_GWHT(void)//固德威逆变器HT协议包
{
      static cJSON *root = NULL;      //cjson对象指针
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
	    char TimeBuf[50];
	    uint8_t i; 
			sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
			root = cJSON_CreateObject();                                                              //cjson创建对象
      cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));
	    cJSON_AddItemToObject(root,"Inverter",cJSON_CreateString("GOODWE_HT"));
	    for(i=0;i<12;i++){
	      sprintf(KeyBuf,"Vpv%d",i+1);
        sprintf(DataBuf,"%.1f",GWHT.Vpv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT电压	
				
	      sprintf(KeyBuf,"Ipv%d",i+1);
        sprintf(DataBuf,"%.2f",GWHT.Ipv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT电流				
			}
	
	    for(i=0;i<24;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.2f",GWHT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //组串电流			
			}
			
			sprintf(KeyBuf,"Va");
			sprintf(DataBuf,"%.1f",GWHT.Va);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A相电压
			
			sprintf(KeyBuf,"Vb");
			sprintf(DataBuf,"%.1f",GWHT.Vb);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B相电压
			
			sprintf(KeyBuf,"Vc");
			sprintf(DataBuf,"%.1f",GWHT.Vc);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C相电压		

			sprintf(KeyBuf,"Ia");
			sprintf(DataBuf,"%.3f",GWHT.Ia);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A相电流
			
			sprintf(KeyBuf,"Ib");
			sprintf(DataBuf,"%.3f",GWHT.Ib);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B相电流
			
			sprintf(KeyBuf,"Ic");
			sprintf(DataBuf,"%.3f",GWHT.Ic);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C相电流

			sprintf(KeyBuf,"P");
			sprintf(DataBuf,"%.3f",GWHT.P);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //有功功率

			sprintf(KeyBuf,"F");
			sprintf(DataBuf,"%.2f",GWHT.Frequency);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //频率

			sprintf(KeyBuf,"Temper");
			sprintf(DataBuf,"%.1f",GWHT.Temper_Inside);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //温度

			sprintf(KeyBuf,"E_Total");
			sprintf(DataBuf,"%.2f",GWHT.E_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //累计发电量

			sprintf(KeyBuf,"E_Day");
			sprintf(DataBuf,"%.2f",GWHT.E_Day);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //当日发电量
			
			sprintf(KeyBuf,"Error_DSP");
			sprintf(DataBuf,"0x%x",GWHT.Error_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //DSP故障代码		

			sprintf(KeyBuf,"Error_PV");
			sprintf(DataBuf,"%d",GWHT.Error_PV);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //组串故障
			
			sprintf(KeyBuf,"Error_Sub_DSP");
			sprintf(DataBuf,"0x%x",GWHT.Error_Sub_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //副DSP故障代码	

			sprintf(KeyBuf,"H_Total");
			sprintf(DataBuf,"%d",GWHT.H_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //总发电时间		
			
			sprintf(KeyBuf,"WorkMode");
			sprintf(DataBuf,"%d",GWHT.WorkMode);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //工作模式	
			
			sprintf(KeyBuf,"Error_485");
			sprintf(DataBuf,"%d",GWHT.Inverter_Fault);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //逆变器通许故障	
			
			name=cJSON_PrintUnformatted(root);					
			printf("\r\n%s\r\n",name);				
			cJsonFree(name);		    //释放内存		
			cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	 
}

void DTU_SendSearch_GWMT(void)//固德威逆变器MT协议包
{
      static cJSON *root = NULL;      //cjson对象指针
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
		  char TimeBuf[50];
	    uint8_t i;
			sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
			root = cJSON_CreateObject(); 
	    cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf)); 
	    cJSON_AddItemToObject(root,"Inverter",cJSON_CreateString("GOODWE_MT"));
	    for(i=0;i<4;i++){
	      sprintf(KeyBuf,"Vpv%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Vpv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT电压		

	      sprintf(KeyBuf,"Ipv%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Ipv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT电流				
			}		
	
	    for(i=0;i<16;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //组串电流1-16	
			}
			
	    for(i=17;i<20;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //组串电流18-20	
			}
			
      sprintf(KeyBuf,"IsSta");
			sprintf(DataBuf,"0x%x",GWMT.IsSta);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //组串1-16状态
			
			sprintf(KeyBuf,"Va");
			sprintf(DataBuf,"%.1f",GWMT.Va);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A相电压
			
			sprintf(KeyBuf,"Vb");
			sprintf(DataBuf,"%.1f",GWMT.Vb);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B相电压
			
			sprintf(KeyBuf,"Vc");
			sprintf(DataBuf,"%.1f",GWMT.Vc);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C相电压		

			sprintf(KeyBuf,"Ia");
			sprintf(DataBuf,"%.1f",GWMT.Ia);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A相电流
			
			sprintf(KeyBuf,"Ib");
			sprintf(DataBuf,"%.1f",GWMT.Ib);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B相电流
			
			sprintf(KeyBuf,"Ic");
			sprintf(DataBuf,"%.1f",GWMT.Ic);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C相电流

			sprintf(KeyBuf,"P");
			sprintf(DataBuf,"%.3f",GWMT.P);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //有功功率
			sprintf(KeyBuf,"Fa");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyA);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //频率A
			
			sprintf(KeyBuf,"Fb");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyB);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //频率B
			
			sprintf(KeyBuf,"Fc");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyC);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //频率C
			
			sprintf(KeyBuf,"Temper");
			sprintf(DataBuf,"%.1f",GWMT.Temper_Inside);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //温度

			sprintf(KeyBuf,"E_Total");
			sprintf(DataBuf,"%.1f",GWMT.E_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //累计发电量

			sprintf(KeyBuf,"E_Day");
			sprintf(DataBuf,"%.1f",GWMT.E_Day);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //当日发电量
			
			sprintf(KeyBuf,"Error");
			sprintf(DataBuf,"0x%x",GWMT.Error_Message);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //故障代码		
			
			sprintf(KeyBuf,"H_Total");
			sprintf(DataBuf,"%d",GWMT.H_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //总发电时间		
			
			sprintf(KeyBuf,"WorkMode");
			sprintf(DataBuf,"%d",GWMT.WorkMode);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //工作模式	
			
			sprintf(KeyBuf,"Error_485");
			sprintf(DataBuf,"%d",GWMT.Inverter_Fault);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //逆变器通许故障	
			
			name=cJSON_PrintUnformatted(root);					
			printf("\r\n%s\r\n",name);				
			cJsonFree(name);		    //释放内存		
			cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	 

}

void Config_Param(void)//配置参数
{
  char *strx,*strx1,*name;
	char temp[20],temp1[20],temp2[10],temp3[5],temp4[5],temp5[5],temp6[5];
	char TimeBuf[50];
	uint8_t databuf[30];
  static cJSON *root = NULL;      //cjson对象指针	
	uint8_t Out_Control;	
	static  uint8_t i=0;
	 		
	if(Print_ConfigFlag==1)//打印配置信息
	{
		   HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
       HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
		   sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
			 root = cJSON_CreateObject();                                                   //cjson创建对象 
			 cJSON_AddItemToObject(root,"Version_Num",cJSON_CreateString(Version_Num));     //系统版本号								   
			 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //本地RTC时间			
			 cJSON_AddItemToObject(root,"RS485_Type",cJSON_CreateNumber(Device485_type));   //485 数据类型
			 cJSON_AddItemToObject(root,"CNC_UpType",cJSON_CreateNumber(CNC_UpType));       //CNC计数上传方式
		   if(CNC_UpType==2){				 
				 cJSON_AddItemToObject(root,"CNC_UpTime",cJSON_CreateNumber(CNC_UpTime));     //CNC计数定时上传时间单位分钟 
			 }			 
			 cJSON_AddItemToObject(root,"CNC_YELLOW",cJSON_CreateNumber(CNC_YELLOW));   //CNC黄灯状态配置
			 cJSON_AddItemToObject(root,"CNC_GREEN",cJSON_CreateNumber(CNC_GREEN));     //CNC绿灯状态配置		
			 cJSON_AddItemToObject(root,"CNC_RED",cJSON_CreateNumber(CNC_RED));         //CNC红灯状态配置	
			 
			 if(AD1_Type>=1){
				 
				   cJSON_AddItemToObject(root,"AD1_Type",cJSON_CreateNumber(AD1_Type));            //0:关       1:4-20ma   2:0-5V 	
				   cJSON_AddItemToObject(root,"AD1_PhyValue1",cJSON_CreateNumber(AD1_PhyValue1));  //物理量范围1
				   cJSON_AddItemToObject(root,"AD1_PhyValue2",cJSON_CreateNumber(AD1_PhyValue2));  //物理量范围2
			 }
			 
			 if(AD2_Type>=1){
				 
				   cJSON_AddItemToObject(root,"AD2_Type",cJSON_CreateNumber(AD2_Type));            //0:关       1:4-20ma   2:0-5V  
				   cJSON_AddItemToObject(root,"AD2_PhyValue1",cJSON_CreateNumber(AD2_PhyValue1));  //物理量范围1
				   cJSON_AddItemToObject(root,"AD2_PhyValue2",cJSON_CreateNumber(AD2_PhyValue2));  //物理量范围2					 
			 }
			 if(Device485_type!=0)
			 {				 
				   cJSON_AddItemToObject(root,"Slave_Num",cJSON_CreateNumber(Slave_Num));          //主站询问从机个数
				   cJSON_AddItemToObject(root,"Master_ADDR",cJSON_CreateNumber(Master_ADDR));      //主站询问首地址 
				   cJSON_AddItemToObject(root,"StartAddr",cJSON_CreateNumber(StartAddr));          //起始地址
				   cJSON_AddItemToObject(root,"ValueOrLenth",cJSON_CreateNumber(ValueOrLenth));    //数据or长度写多个寄存器返回数据量的对比值 
				   cJSON_AddItemToObject(root,"Slave_ADDR",cJSON_CreateNumber(Slave_ADDR));        //作为从机时ID号
				   cJSON_AddItemToObject(root,"ModbusBaudRate",cJSON_CreateNumber(ModbusBaudRate));//Modbus串口波特率
				   cJSON_AddItemToObject(root,"ModbusWordLength",cJSON_CreateNumber(ModbusWordLength));//Modbus串口数据位
				   cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits));    //Modbus串口停止位
				   cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));        //Modbus串口校验位	 
			 }
			 cJSON_AddItemToObject(root,"Pulse_Interval",cJSON_CreateNumber(TIM3_Pulse_Threshold));//脉冲间隔
			 name=cJSON_PrintUnformatted(root);						 
			 printf("\r\n%s\r\n",name);					
			 cJsonFree(name);		   //释放内存		
			 cJSON_Delete(root);   //必须释放对象，否则一段时间后数据读取有误 		
	     Print_ConfigFlag=0;
	}
	
	if(bsp_CheckTimer(2)){
	
		  if(Print_Flag==2)
			{				
					HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
					HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
					sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
					root = cJSON_CreateObject();                                                              //cjson创建对象
          cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));                     //本地RTC时间							
				  sprintf(TimeBuf,"%d",GREEN_Value);
				  cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //绿灯值
			
				  sprintf(TimeBuf,"%d",RED_Value);	
				  cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));          //红灯值
			
				  sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
				  cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf)); //定时器3通道1计数	
				  if(AD1_Type>=1){
						
						 cJSON_AddItemToObject(root,"adc1",cJSON_CreateNumber(adc1));
             cJSON_AddItemToObject(root,"AD1_EValue",cJSON_CreateNumber(AD1_Value)); 
						 cJSON_AddItemToObject(root,"AD1_PhyValue",cJSON_CreateNumber(AD1_PhyValue));              		 				
				  }						
				  if(AD2_Type>=1){
						
						 cJSON_AddItemToObject(root,"adc2",cJSON_CreateNumber(adc2)); 
             cJSON_AddItemToObject(root,"AD2_EValue",cJSON_CreateNumber(AD2_Value)); 
             cJSON_AddItemToObject(root,"AD2_PhyValue",cJSON_CreateNumber(AD2_PhyValue)); 						
				  }					
					name=cJSON_PrintUnformatted(root);						 
					printf("\r\n%s\r\n",name);					
					cJsonFree(name);		   //释放内存		
					cJSON_Delete(root);   //必须释放对象，否则一段时间后数据读取有误 									 
			}			
			else if(Print_Flag==1)
			{			
					if(Device485_type==1){//Modbus主站数据								
									
							DTU_SendModbus(i);	
              if(++i>Slave_Num-1)	
              {
						    i=0;	
							}								
					}
					else if(Device485_type==3){//固德威HT协议
						
						DTU_SendSearch_GWHT();//固德威逆变器HT协议包
					}
					else if(Device485_type==4){//固德威MT协议
						
						DTU_SendSearch_GWMT();//固德威逆变器MT协议包
					}
				
					else {
						
					    sprintf(TimeBuf,"Device485_type=%d",Device485_type);	
						  printf("\r\n%s\r\n", TimeBuf);
					}				 	
			}	
	}	
	
  if((UpdateFlage==1)||(bsp_CheckTimer(9)))
  {	      	
			IAPInfoPathInit();                  //解析URL
		  //sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);
      //printf("IAPStructValue.Url:%s\r\n",IAPStructValue.Url); 		
		  IAPSetUpdateUrl(IAPStructValue.Url);//设置URL
			IAPSetFileSize(FileSize); //设置文件大小
			IAPSetUpdateFlage();      //设置更新标志	
      IAPResetMCU();            //复位更新	
      UpdateFlage=0;
      bsp_StopTimer(9);		
  }
	 
	if(USART_RX_STA&0x8000)
	{
		if(USART_RX_BUF[0]==0xe1&USART_RX_BUF[1]==0xe2&USART_RX_BUF[2]==0xe3&USART_RX_BUF[3]==0x1e&USART_RX_BUF[4]==0x2e)//判断进入BootLoader状态命令APP中要加入
		{			
           AT24CXX_WriteOne(OTA_Adress,1);	//进入BootLoader等待状态			
					 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
					 USART_RX_STA=0;
					 __set_FAULTMASK(1);	//屏蔽所有中断 											
					 NVIC_SystemReset();							
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"SET_RTC"); //设置RTC时间
		if(strx)
		{
					sscanf(strx,"%[^=]=%[^-]-%[^-]-%[^ ] %[^:]:%[^:]:%[^:]",temp,temp1,temp2,temp3,temp4,temp5,temp6);
					sdatestructure.Year=atoi(temp1)-2000;									    
					sdatestructure.Month=atoi(temp2);							
					sdatestructure.Date=atoi(temp3);
					stimestructure.Hours=atoi(temp4);
					stimestructure.Minutes=atoi(temp5);
					stimestructure.Seconds=atoi(temp6);
					HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
          HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);               							
					printf("%s\r\n", "时钟设置参数:");
					printf("年月日时分秒:%04d-%02d-%02d %02d:%02d:%02d\r\n",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Restart");       //复位系统
		if(strx)
		{
				 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
				 USART_RX_STA=0;
				 printf("%s\r\n", "等待系统复位!");			
					//等待系统重启	                    		
				 __set_FAULTMASK(1);	//屏蔽所有中断 											
				 NVIC_SystemReset();									  							 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_ON");      //系统打印开启
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Print_Flag=atoi(temp1);
				 printf("Print_ON:%d 1:485数据 2:计数数据\r\n", Print_Flag);									   						    								  							 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_Config");    //系统配置打印
		if(strx)
		{
				 Print_ConfigFlag=1;
				 printf("\r\n%s\r\n", "系统配置信息打印!");								    								  							 
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_OFF");      //系统打印关闭
		if(strx)
		{
				 Print_Flag=0;
				 printf("\r\n%s\r\n", "系统打印信息Stop!");								    								  							 
		}	
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Format_EEROM");   //格式化外部EEROM
		if(strx)
		{
				 printf("%s\r\n","EEROM格式化!");	 
				 AT24CXX_WriteOne(0,0XFF); //
				 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
				 USART_RX_STA=0;
				 printf("%s\r\n", "等待系统复位!");			
					//等待系统重启	                    		
				 __set_FAULTMASK(1);	//屏蔽所有中断 											
				 NVIC_SystemReset();									 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_INUM");       //CNC计数初值
		if(strx) 
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					TIM3_CAPTURE_VAL1=atoi(temp1);
					printf("TIM3_CAPTURE_VAL1=%d\r\n",TIM3_CAPTURE_VAL1);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_UpType");     //CNC计数上传方式
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_UpType=atoi(temp1);
					if(CNC_UpType>=2)
					{
							CNC_UpType=2;
					}
				  if(CNC_UpType==1)
					{
							bsp_StopTimer(1);//停止定时
					}
					else if(CNC_UpType==2)
					{
							bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器 */	
						  bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器定时上报计数 */	
					}
					AT24CXX_WriteOne(CNC_UpType_Adress,CNC_UpType); //0:不上报 1:变化上报 2:定时上报
					printf("CNC_UpType=%d 0:不上报 1:变化上报 2:定时上报\r\n",CNC_UpType);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_UpTime"); //CNC计数定时上传时间单位秒
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_UpTime=atoi(temp1);
			    if(CNC_UpTime<5)
					{
					    CNC_UpTime=5;
					}
			    memcpy(databuf,&CNC_UpTime,2);                     //1:单位秒
			    AT24CXX_Write(CNC_UpTime_Adress,databuf,2);	
		    	bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器 */						
					bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器定时上报计数 */						   
					printf("CNC_UpTime=%d秒\r\n",CNC_UpTime);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_YELLOW"); //CNC黄灯状态配置
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_YELLOW=atoi(temp1);	
					if(CNC_YELLOW>=2){						
						CNC_YELLOW=2;
					}									
					AT24CXX_WriteOne(CNC_YELLOW_Adress,CNC_YELLOW); //0:常亮结束 1:闪烁结束								   
					printf("CNC_Yellow=%d 0:常亮结束 1:闪烁结束  2:脉冲计数\r\n",CNC_YELLOW);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_GREEN"); //CNC绿灯状态配置
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_GREEN=atoi(temp1);	
					if(CNC_GREEN>=1){						
						CNC_GREEN=1;
					}									
					AT24CXX_WriteOne(CNC_GREEN_Adress,CNC_GREEN); //0:常亮运行 1:闪烁运行								   
					printf("CNC_GREEN=%d 0:常亮运行 1:闪烁运行\r\n",CNC_GREEN);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_RED");   //CNC红灯状态配置
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_RED=atoi(temp1);
					if(CNC_RED>=1){						
						CNC_RED=1;
					}												
					AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED);       //0:常亮故障 1:闪烁故障 								   
					printf("CNC_RED=%d 0:常亮故障 1:闪烁故障\r\n",CNC_RED);	 									
		} 	
		
		strx=strstr((char*)USART_RX_BUF,(char*)"RS485Type");        //设置485串口数据类型 0:无 1:MODBUS主站 2:自由协议
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Device485_type=atoi(temp1);				           									
				 AT24CXX_WriteOne(RS485_Adresss,Device485_type);        //写入EEROM数据			
			   printf("RS485数据类型为:%d 0:无 1:MODBUS主站 2:MODBUS从站 3:GWHT 4:GWMT\r\n", Device485_type);					  								    								  							 
		}			
		
		strx=strstr((char*)USART_RX_BUF,(char*)"OUT1");            //控制OUT1开关
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Out_Control=atoi(temp1);
				 if(Out_Control>=1)
				 {
							HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_SET);
							printf("%s\r\n", "OUT1_ON!");
				 }
				 else
				 {
							HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_RESET);
							printf("%s\r\n", "OUT1_OFF!");
				 }									  								    								  							 
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"OUT2");            //控制OUT2开关
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Out_Control=atoi(temp1);
				 if(Out_Control>=1)
				 {
							HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_SET);
							printf("%s\r\n", "OUT2_ON!");
				 }
				 else
				 {
							HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_RESET);
							printf("%s\r\n", "OUT2_OFF!");
				 }									  								    								  							 
		} 

    strx=strstr((char*)USART_RX_BUF,(char*)"AD1_Type");         //AD1类型
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_Type=atoi(temp1);
			    AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);            //写入EEROM数据			
			    printf("AD1_Type=%d 0:关 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD1_Type);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"AD2_Type");          //AD2类型
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_Type=atoi(temp1);
			    AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);            //写入EEROM数据	
					printf("AD2_Type=%d 0:关 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD2_Type);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD1_PhyValue1"); //AD1物理量范围1
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_PhyValue1=atof(temp1);
			    memcpy(databuf,&AD1_PhyValue1,4);                 //AD1物理量范围1
			    AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);			   
					printf("AD1_PhyValue1=%f\r\n",AD1_PhyValue1);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD1_PhyValue2");//AD1物理量范围2
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_PhyValue2=atof(temp1);
			    memcpy(databuf,&AD1_PhyValue2,4);                 //AD1物理量范围2
			    AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);	
					printf("AD1_PhyValue2=%f\r\n",AD1_PhyValue2);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"AD2_PhyValue1"); //AD2物理量范围1
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_PhyValue1=atof(temp1);
			    memcpy(databuf,&AD2_PhyValue1,4);                  //AD2物理量范围1
			    AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);
					printf("AD2_PhyValue1=%f\r\n",AD2_PhyValue1);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD2_PhyValue2"); //AD2物理量范围2
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_PhyValue2=atof(temp1);
			    memcpy(databuf,&AD2_PhyValue2,4);                  //AD2物理量范围2
			    AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);
					printf("AD2_PhyValue2=%f\r\n",AD2_PhyValue2);	 									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Slave_Num");     //主站询问从机个数
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					Slave_Num=atoi(temp1);
			    if(Slave_Num>3){
										
					    Slave_Num=3;
					}
          if(Slave_Num<1)
          {
					    Slave_Num=1;
					}						
			    AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);       //写入EEROM数据			
			    printf("Slave_Num=%d\r\n",Slave_Num);	     									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Master_ADDR");    //主站询问首地址
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					Master_ADDR=atoi(temp1);
          if(Master_ADDR<1)
					{
					    Master_ADDR=1;
					}						
					AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);   //写入EEROM数据											
					printf("Master_ADDR=%d\r\n",Master_ADDR);	    									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"StartAddr");      //起始地址
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					StartAddr=atoi(temp1);			   
					memcpy(databuf,&StartAddr,2);                        //起始地址
					AT24CXX_Write(StartAddr_Adress,databuf,2);																		
					printf("StartAddr=%04x\r\n",StartAddr);	    									
		}		
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ValueOrLenth");    //数据or长度写多个寄存器返回数据量的对比值
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					ValueOrLenth=atoi(temp1);			   
					if(ValueOrLenth>20)
					{
							ValueOrLenth=20;
					}
					memcpy(databuf,&ValueOrLenth,2);                     //数据or长度写多个寄存器返回数据量的对比值
					AT24CXX_Write(ValueOrLenth_Adress,databuf,2);																		
					printf("ValueOrLenth=%04x\r\n",ValueOrLenth);    									
		}

    strx=strstr((char*)USART_RX_BUF,(char*)"Slave_ADDR");     //作为从机时ID号
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					Slave_ADDR=atoi(temp1);	
          if(Slave_ADDR<1)
          {
					    Slave_ADDR=1;
					}						
					AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);          //写入EEROM数据											
					printf("Slave_ADDR=%d\r\n",Slave_ADDR);	   									
		}		
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusBaudRate");      //Modbus串口波特率
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);			
					ModbusBaudRate=atoi(temp1);	
          if(ModbusBaudRate<1)
          {
					    ModbusBaudRate=1;
					}	
          if(ModbusBaudRate>8)
          {
					    ModbusBaudRate=8;
					}						
					AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate);   //写入EEROM数据											
					printf("ModbusBaudRate=%d\r\n",ModbusBaudRate);	   									
		}			
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusWordLength");    //Modbus串口数据位
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					ModbusWordLength=atoi(temp1);	
          if(ModbusWordLength<8)
          {
					    ModbusWordLength=8;
					}	
          if(ModbusWordLength>9)
          {
					    ModbusWordLength=9;
					}						
					AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength);  //写入EEROM数据											
					printf("ModbusWordLength=%d\r\n",ModbusWordLength);   									
		}	

    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusStopBits");          //Modbus串口停止位
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					ModbusStopBits=atoi(temp1);	
          if(ModbusStopBits<1)
          {
					    ModbusStopBits=1;
					}
          if(ModbusStopBits>2)
          {
					    ModbusStopBits=2;
					}						
					AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);      //写入EEROM数据											
					printf("ModbusStopBits=%d\r\n",ModbusStopBits);	  									
		}	
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusParity");            //Modbus串口校验位
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					ModbusParity=atoi(temp1);		
          if(ModbusParity<1)
          {
				      ModbusParity=1;	
					}	
          if(ModbusParity>3)
          {
				      ModbusParity=3;	
					}					
					AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);          //写入EEROM数据											
					printf("ModbusParity=%d\r\n",ModbusParity);	  									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Pulse_Interval");          //脉冲间隔
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					TIM3_Pulse_Threshold=atoi(temp1);          
          if(TIM3_Pulse_Threshold>4000)
          {
				      TIM3_Pulse_Threshold=40000;	
					}					
				  memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //脉冲捕捉上升和下降沿之间时间单位ms
          AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);									
					printf("Pulse_Interval=%d\r\n",TIM3_Pulse_Threshold);	  									
		}
				
	  strx=strstr((char*)USART_RX_BUF,(char*)"URL");    //设置URL
		if(strx)			
		{		 strx1=strstr((char*)strx,(char*)"="); //
			   if(strx1)
				 {
				     sscanf(strx1+1,"%s",IAPUrlUserBinPath);
						 sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);					
			       printf("URL:%s\r\n", IAPUrlUserBinPath);      
				 }				       									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"FileSize");//设置FileSize大小
		if(strx)			
		{		 
			  sscanf(strx,"%[^=]=%s",temp,temp1);
			  FileSize=atoi(temp1);	
			  printf("FileSize:%d\r\n",FileSize); 			
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"Update");//设置UpdateFlage
		if(strx)			
		{		 
			  sscanf(strx,"%[^=]=%s",temp,temp1);
			  UpdateFlage=atoi(temp1);	
			  printf("UpdateFlage:%d\r\n",UpdateFlage); 			
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"Server_IP"); //设置服务器IP地址
		if(strx)			
		{		 
			   strx1=strstr((char*)strx,(char*)"="); 
			   if(strx1){
				    sscanf(strx1+1,"%s",IP);			
				    printf("Server_IP:%s\r\n", IP);
            SetServerIP((char*)IP);						 
				 }	
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"Server_Port"); //设置服务器IP端口
		if(strx)			
		{		 
		     strx1=strstr((char*)strx,(char*)"="); // 
			   if(strx1){
				    sscanf(strx1+1,"%s",Port);			
				    printf("Server_Port:%s\r\n", Port);
					  SetServerPort((char*)Port);           	 
				 }
		}

		strx=strstr((char*)USART_RX_BUF,(char*)"UserName"); //设置MQTT客户端UserName
		if(strx)			
		{		 
		     strx1=strstr((char*)strx,(char*)"="); // 
			   if(strx1){
				    sscanf(strx1+1,"%s",MQTTUserName);			
				    printf("UserName:%s\r\n", MQTTUserName);
					  SetUserName((char*)MQTTUserName);           	 
				 }
		}

		strx=strstr((char*)USART_RX_BUF,(char*)"PassWord"); //设置MQTT客户端PassWord
		if(strx)			
		{		 
		     strx1=strstr((char*)strx,(char*)"="); // 
			   if(strx1){
				    sscanf(strx1+1,"%s",MQTTPassWord);			
				    printf("PassWord:%s\r\n", MQTTPassWord);
					  SetPassWord((char*)MQTTPassWord);           	 
				 }
		}
		
		memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
		USART_RX_STA=0;
	}
}
