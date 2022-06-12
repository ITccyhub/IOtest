#include "uart4data.h"
#include "uart3data.h"
#include "modbus.h"
#include "cmsis_os.h"
extern char Version_Num[30];  //版本号
uint8_t Count_DataType=1;     //变化数据类型
uint8_t Analog_DataType=2;    //模拟数据类型
uint8_t Modubs_DataType=3;    //Mobus数据类型

uint8_t Updata_Flag=0;           //上传标志      
void MqttConnect(void);          //连接上MQTT回调
void MqttDisConnect(void);       //MQTT断开连接回调
void subscribedCb(int pdata);    //订阅成功回调
void failsubscribedCb(int pdata);//订阅失败回调
void PublishedCb(void);          //成功发布消息回调
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//接收到数据回调
void Communication_Remote(const char *data);//远程通信

void Communication_Remote(const char *data)//远程通信
{	 
	 char *name;
	 char temp1[20],temp2[20],temp3[20],temp4[20],temp5[20],temp6[20];	
	 static uint8_t Lora_Ack,Lora_Ack2,Lora_Ack3,Lora_Ack4=0;      //远端解析数据返回标志
	 char TimeBuf[50];
	 uint8_t databuf[30];
	 uint8_t res;
	 static uint8_t OutTemp;
	 static  cJSON *root = NULL;      //cjson对象指针	
	 static cJSON *TEST = NULL;      //cjson对象指针
	 static cJSON *Analysis = NULL;  //cjson对象指针	
		
		TEST = cJSON_Parse((char*)data);   
		if(!TEST) 
		{
				printf("Error before: [%s]\r\n",cJSON_GetErrorPtr());	
				root = cJSON_CreateObject();                                                                   //cjson创建对象 
				cJSON_AddItemToObject(root,"JSON Format error",cJSON_CreateString("Please check JSON Format"));//JSON格式错误
				name=cJSON_PrintUnformatted(root);					
				MainLen = sprintf((char*)MainBuffer,"%s",name);			
				mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息	
				//COM_Send(&huart4,(uint8_t*)name,strlen(name));											
				cJsonFree(name);		  //释放内存		
				cJSON_Delete(root);   //必须释放对象，否则一段时间后数据读取有误									
		}
		else
		{			   
				Analysis=cJSON_GetObjectItem(TEST, "SET_RTC");      //设置RTC时钟
				if(Analysis!=NULL)
				{
						sscanf(Analysis->valuestring,"%[^-]-%[^-]-%[^ ] %[^:]:%[^:]:%[^:]",temp1,temp2,temp3,temp4,temp5,temp6);
						sdatestructure.Year=atoi(temp1)-2000;									    
						sdatestructure.Month=atoi(temp2);							
						sdatestructure.Date=atoi(temp3);
						stimestructure.Hours=atoi(temp4);
						stimestructure.Minutes=atoi(temp5);
						stimestructure.Seconds=atoi(temp6);	
						HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);//设置时间
						HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);  
						//远程配置返回本地应答信息								
						printf("%s\r\n", "远程配置时钟设置参数:");
						printf("远程配置年月日时分秒:%04d-%02d-%02d %02d:%02d:%02d\r\n", sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//	
						//返回云端信息组包待写
						Lora_Ack|=0x01;                			
				}

				Analysis=cJSON_GetObjectItem(TEST, "Restart");        //远程系统复位
				if(Analysis!=NULL)
				{									   
							 printf("%s\r\n", "远程配置复位等待系统复位!");	
							 //返回云端信息组包待写			
							 Lora_Ack|=0x02;																		
				}		

				Analysis=cJSON_GetObjectItem(TEST, "Print_Config");    //远程打印系统信息
				if(Analysis!=NULL)
				{									  
							 printf("%s\r\n", "远程配置系统配置信息打印!");
							 //返回云端信息组包待写										
							 Lora_Ack|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST, "OUT1");           //远程控制OUT1
				if(Analysis!=NULL)
				{									  
							OutTemp=Analysis->valuedouble;
							if(OutTemp>=1)
							{
									HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_SET);
									printf("%s\r\n", "远程配置OUT1_ON!");
							}	
							else 
							{
									HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_RESET);
									printf("%s\r\n", "远程配置OUT1_OFF!");
							}		
							 //返回云端信息组包待写										
							 Lora_Ack|=0x08;		   					
				}	

				Analysis=cJSON_GetObjectItem(TEST, "OUT2");           //远程控制OUT2
				if(Analysis!=NULL)
				{									  
							OutTemp=Analysis->valuedouble;
							if(OutTemp>=1)
							{
									HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_SET);
									printf("%s\r\n", "远程配置OUT2_ON!");
							}	
							else 
							{
									HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_RESET);
									printf("%s\r\n", "远程配置OUT2_OFF!");
							}		
							 //返回云端信息组包待写										
							 Lora_Ack|=0x10;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST, "RS485Type");                  //远程配置485接口类型
				if(Analysis!=NULL)
				{			
							 Device485_type=Analysis->valuedouble;						    								
							 AT24CXX_WriteOne(RS485_Adresss,Device485_type);            //写入EEROM数据			
							 printf("RS485远程配置数据类型为:%d 0:无 1:MODBUS主站 2:MODBUS从站 3:自由协议\r\n", Device485_type);													 				  			     				  						  
							 //返回云端信息组包待写										
							 Lora_Ack|=0x20;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"Format_EEROM");                //远程初始化EEROM
				if(Analysis!=NULL)
				{									     
								printf("%s\r\n"," 远程EEROM格式化!");	 
								AT24CXX_WriteOne(0,0xff); //                      		
								//返回云端信息组包待写										
								Lora_Ack|=0x40;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"CNC_INUM");                 //远程设置计数初值
				if(Analysis!=NULL)
				{   
								TIM3_CAPTURE_VAL1=Analysis->valuedouble;  								
								printf("远程配置TIM3_CAPTURE_VAL1=%d\r\n",TIM3_CAPTURE_VAL1);	                 								
								//返回云端信息组包待写										
								Lora_Ack|=0x80;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_UpType");              //CNC计数上传方式
				if(Analysis!=NULL)
				{   
								CNC_UpType=Analysis->valuedouble; 
								if(CNC_UpType>2)
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
								printf("远程配置CNC_UpType=%d 0:不上报 1:变化上报 2:定时上报\r\n",CNC_UpType);	 	                  								
								//返回云端信息组包待写										
								Lora_Ack2|=0x01;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_UpTime");          //CNC计数定时上传时间单位秒 
				if(Analysis!=NULL)
				{   
								CNC_UpTime=Analysis->valuedouble;
								if(CNC_UpTime<5)
								{
										CNC_UpTime=5;
								}
								bsp_StartAutoTimer(1, CNC_UpTime*1000);	        /* 启动1个1000ms的自动重装的定时器 */	
								bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* 启动1个1000ms的自动重装的定时器定时上报计数 */	
								memcpy(databuf,&CNC_UpTime,2);                     //1:单位秒
								AT24CXX_Write(CNC_UpTime_Adress,databuf,2);																			
								printf("远程配置CNC_UpTime=%d秒\r\n",CNC_UpTime);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x02;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_YELLOW");             //CNC黄灯状态配置
				if(Analysis!=NULL)
				{   
								CNC_YELLOW=Analysis->valuedouble; 
								if(CNC_YELLOW>=2){
									
									CNC_YELLOW=2;
								}									
								AT24CXX_WriteOne(CNC_YELLOW_Adress,CNC_YELLOW);     //0:常亮结束 1:闪烁结束												
								printf("远程配置CNC_Yellow=%d 0:常亮结束 1:闪烁结束 2:脉冲计数	\r\n",CNC_YELLOW);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_GREEN");             //CNC绿灯状态配置
				if(Analysis!=NULL)
				{   
								CNC_GREEN=Analysis->valuedouble;
								if(CNC_GREEN>=1){
									
									CNC_GREEN=1;
								}									
								AT24CXX_WriteOne(CNC_GREEN_Adress,CNC_GREEN); //0:常亮运行 1:闪烁运行												
								printf("远程配置CNC_GREEN=%d 0:常亮运行 1:闪烁运行\r\n",CNC_GREEN);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x08;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_RED");             //CNC红灯状态配置
				if(Analysis!=NULL)
				{   
								CNC_RED=Analysis->valuedouble;  
								if(CNC_RED>=1){
									
									CNC_RED=1;
								}											
								AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED); //0:常亮故障 1:闪烁故障												
								printf("远程配置CNC_RED=%d 0:常亮故障 1:闪烁故障\r\n",CNC_RED);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x10;		   					
				}		

				Analysis=cJSON_GetObjectItem(TEST,"AD1_Type");             //AD1类型
				if(Analysis!=NULL)
				{   
								AD1_Type=Analysis->valuedouble; 																	
								AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);        //写入EEROM数据											
								printf("远程配置AD1_Type=%d 0:关 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD1_Type);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x20;		   					
				}						
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_Type");             //AD2类型
				if(Analysis!=NULL)
				{   
								AD2_Type=Analysis->valuedouble; 																	
								AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);        //写入EEROM数据											
								printf("远程配置AD2_Type=%d 0:关 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD2_Type);	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x40;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"AD1_PhyValue1");      //物理量范围1
				if(Analysis!=NULL)
				{   
								AD1_PhyValue1=Analysis->valuedouble; 																	
								memcpy(databuf,&AD1_PhyValue1,4);               //AD1物理量范围1
								AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);										
								printf("远程配置AD1_PhyValue1=%f\r\n",AD1_PhyValue1);	 ;	            								
								//返回云端信息组包待写										
								Lora_Ack2|=0x80;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"AD1_PhyValue2");     //物理量范围2
				if(Analysis!=NULL)
				{   
								AD1_PhyValue2=Analysis->valuedouble; 																	
								memcpy(databuf,&AD1_PhyValue2,4);               //AD1物理量范围2
								AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);											
								printf("远程配置AD1_PhyValue2=%f\r\n",AD1_PhyValue2);	 ;	 	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x01;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_PhyValue1");      //物理量范围1
				if(Analysis!=NULL)
				{   
								AD2_PhyValue1=Analysis->valuedouble; 																	
								memcpy(databuf,&AD2_PhyValue1,4);               //AD2物理量范围1
								AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);						
								printf("远程配置AD2_PhyValue1=%f\r\n",AD2_PhyValue1);	 ;	             								
								//返回云端信息组包待写										
								Lora_Ack3|=0x02;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_PhyValue2");      //物理量范围2
				if(Analysis!=NULL)
				{   
								AD2_PhyValue2=Analysis->valuedouble; 																	
								memcpy(databuf,&AD2_PhyValue2,4);               //AD2物理量范围2
								AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);								
								printf("远程配置AD2_PhyValue2=%f\r\n",AD2_PhyValue2);	 ;	             								
								//返回云端信息组包待写										
								Lora_Ack3|=0x04;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Slave_Num");            //主站询问从机个数
				if(Analysis!=NULL)
				{   
								Slave_Num=Analysis->valuedouble; 	
								if(Slave_Num>3){
									
									 Slave_Num=3;
								}
								if(Slave_Num<1)
								{
										Slave_Num=1;
								}										
								AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);      //写入EEROM数据											
								printf("远程配置Slave_Num=%d\r\n",Slave_Num);	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x08;		   					
				}					

				Analysis=cJSON_GetObjectItem(TEST,"Master_ADDR");          //主站询问首地址
				if(Analysis!=NULL)
				{   
								Master_ADDR=Analysis->valuedouble;
								if(Master_ADDR<1)
								{
										Master_ADDR=1;
								}						
								AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);  //写入EEROM数据											
								printf("远程配置Master_ADDR=%d\r\n",Master_ADDR);	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x10;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"StartAddr");            //起始地址
				if(Analysis!=NULL)
				{   
								StartAddr=Analysis->valuedouble;
								memcpy(databuf,&StartAddr,2);                       //起始地址
								AT24CXX_Write(StartAddr_Adress,databuf,2);																		
								printf("远程配置StartAddr=%04x\r\n",StartAddr);	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x20;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ValueOrLenth");            //数据or长度写多个寄存器返回数据量的对比值
				if(Analysis!=NULL)
				{   
								ValueOrLenth=Analysis->valuedouble;
								if(ValueOrLenth>20)
								{
										ValueOrLenth=20;
								}
								memcpy(databuf,&ValueOrLenth,2);                      //数据or长度写多个寄存器返回数据量的对比值
								AT24CXX_Write(ValueOrLenth_Adress,databuf,2);																		
								printf("远程配置ValueOrLenth=%04x\r\n",ValueOrLenth);	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x40;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Slave_ADDR");              //作为从机时ID号
				if(Analysis!=NULL)
				{   
								Slave_ADDR=Analysis->valuedouble; 
								if(Slave_ADDR<1)
								{
										Slave_ADDR=1;
								}							
								AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);       //写入EEROM数据											
								printf("远程配置Slave_ADDR=%d\r\n",Slave_ADDR);	            								
								//返回云端信息组包待写										
								Lora_Ack3|=0x80;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusBaudRate");           //Modbus串口波特率
				if(Analysis!=NULL)
				{   
								ModbusBaudRate=Analysis->valuedouble; 
								if(ModbusBaudRate<1)
								{
										ModbusBaudRate=1;
								}	
								if(ModbusBaudRate>8)
								{
										ModbusBaudRate=8;
								}						
								AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate); //写入EEROM数据											
								printf("远程配置ModbusBaudRate=%d\r\n",ModbusBaudRate);	            								
								//返回云端信息组包待写										
								Lora_Ack4|=0x01;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusWordLength");          //Modbus串口数据位
				if(Analysis!=NULL)
				{   
								ModbusWordLength=Analysis->valuedouble;  
								if(ModbusWordLength<8)
								{
										ModbusWordLength=8;
								}	
								if(ModbusWordLength>9)
								{
										ModbusWordLength=9;
								}							
								AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength); //写入EEROM数据											
								printf("远程配置ModbusWordLength=%d\r\n",ModbusWordLength);	            								
								//返回云端信息组包待写										
								Lora_Ack4|=0x02;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"ModbusStopBits");                //Modbus串口停止位
				if(Analysis!=NULL)
				{   
								ModbusStopBits=Analysis->valuedouble;   
								if(ModbusStopBits<1)
								{
										ModbusStopBits=1;
								}
								if(ModbusStopBits>2)
								{
										ModbusStopBits=2;
								}						
								AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);       //写入EEROM数据											
								printf("远程配置ModbusStopBits=%d\r\n",ModbusStopBits);	            								
								//返回云端信息组包待写										
								Lora_Ack4|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusParity");                    //Modbus串口校验位
				if(Analysis!=NULL)
				{   
								ModbusParity=Analysis->valuedouble;
								if(ModbusParity<1)
								{
										ModbusParity=1;	
								}	
								if(ModbusParity>3)
								{
										ModbusParity=3;	
								}							
								AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);            //写入EEROM数据											
								printf("远程配置ModbusParity=%d\r\n",ModbusParity);	            								
								//返回云端信息组包待写										
								Lora_Ack4|=0x08;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Pulse_Interval");               //脉冲间隔
				if(Analysis!=NULL)
				{   
								TIM3_Pulse_Threshold=Analysis->valuedouble;
								if(TIM3_Pulse_Threshold>4000)
								{
										TIM3_Pulse_Threshold=4000;	
								}					
								memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //脉冲捕捉上升和下降沿之间时间单位ms
								AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);																	
								printf("远程配置Pulse_Interval=%d\r\n",TIM3_Pulse_Threshold);	            								
								//返回云端信息组包待写										
								Lora_Ack4|=0x10;		   					
				}	
 
				Analysis=cJSON_GetObjectItem(TEST,"URL");          //更新固件链接
				if(Analysis!=NULL)
        {
				    sscanf(Analysis->valuestring,"%s",IAPUrlUserBinPath);
				    sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);
					  //IAPSetUpdateUrl(IAPStructValue.Url);//设置URL					 
			      printf("远程配置URL:%s\r\n", IAPUrlUserBinPath);      
						//返回云端信息组包待写										
					  Lora_Ack4|=0x20;
				}				
				
				Analysis=cJSON_GetObjectItem(TEST,"FileSize");        //设置FileSize大小
				if(Analysis!=NULL)
				{
				    FileSize=Analysis->valuedouble;
					  printf("远程FileSize:%d\r\n",FileSize);
						//返回云端信息组包待写										
					  Lora_Ack4|=0x40;
				}				
				
				cJSON_Delete(TEST);                                           //必须释放对象，否则一段时间后数据读取有误  
		}

	
	 if((Lora_Ack!=0)|(Lora_Ack2!=0)|(Lora_Ack3!=0)|(Lora_Ack4!=0))
	 {
		    root = cJSON_CreateObject();      //cjson创建对象     
	 	    if(Lora_Ack&0x01)    //远程配置时间
				{
						 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);					                                               
						 cJSON_AddItemToObject(root,"SET_RTC",cJSON_CreateString(TimeBuf));                    //本地RTC时间						
				}
				
				if(Lora_Ack&0x02)   //远程配置重启
				{
						cJSON_AddItemToObject(root,"Restart",cJSON_CreateString("wait for system reset!"));             //远程配置重启								
				}
				
        if(Lora_Ack&0x04)   //远程配置读取打印信息
        {
						 HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
						 HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
						 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
						 //root = cJSON_CreateObject();                                                   //cjson创建对象 
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
								 cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits)); //Modbus串口停止位
								 cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));//Modbus串口校验位	 
						 }
            cJSON_AddItemToObject(root,"Pulse_Interval",cJSON_CreateNumber(TIM3_Pulse_Threshold));//脉冲间隔						 
				}
				
				if(Lora_Ack&0x08)    //远程控制OUT1
				{
				    cJSON_AddItemToObject(root,"OUT1",cJSON_CreateNumber(OutTemp));   //远程控制OUT1				
				}
				
				if(Lora_Ack&0x10)    //远程控制OUT2
				{
				    cJSON_AddItemToObject(root,"OUT2",cJSON_CreateNumber(OutTemp));  //远程控制OUT2				
				}
				
			  if(Lora_Ack&0x20)    //远程控制OUT2
				{
				    cJSON_AddItemToObject(root,"RS485Type",cJSON_CreateNumber(Device485_type));   //远程配置485接口类型				
				}
				
				if(Lora_Ack&0x40)    //远程初始化EEROM
				{
					  sprintf(TimeBuf,"%s","OK");		
				    cJSON_AddItemToObject(root,"Format_EEROM",cJSON_CreateString(TimeBuf));       //远程初始化EEROM
            Lora_Ack|=0x02; //复位重启					
				}
				
			  if(Lora_Ack&0x80)    //远程设置计数初值
				{					  	
				    cJSON_AddItemToObject(root,"CNC_INUM",cJSON_CreateNumber(TIM3_CAPTURE_VAL1));       //远程设置计数初值          			
				}
				
				if(Lora_Ack2&0x01)    //CNC计数上传方式
				{					  	
				    cJSON_AddItemToObject(root,"CNC_UpType",cJSON_CreateNumber(CNC_UpType));           //CNC计数上传方式    			
				}
				
				if(Lora_Ack2&0x02)    //CNC计数定时上传时间单位分钟
				{					  	
				    cJSON_AddItemToObject(root,"CNC_UpTime",cJSON_CreateNumber(CNC_UpTime));           //CNC计数定时上传时间单位分钟 			
				}
				
				if(Lora_Ack2&0x04)    //CNC黄灯状态配置
				{					  	
				    cJSON_AddItemToObject(root,"CNC_YELLOW",cJSON_CreateNumber(CNC_YELLOW));          //CNC黄灯状态配置	
				}
				
				if(Lora_Ack2&0x08)    //CNC绿灯状态配置
				{					  	
				    cJSON_AddItemToObject(root,"CNC_GREEN",cJSON_CreateNumber(CNC_GREEN));            //CNC绿灯状态配置	
				}
				
				if(Lora_Ack2&0x10)    //CNC红灯状态配置
				{					  	
				    cJSON_AddItemToObject(root,"CNC_RED",cJSON_CreateNumber(CNC_RED));                //CNC红灯状态配置	
				}

				if(Lora_Ack2&0x20)    //AD1_Type
				{					  	
				    cJSON_AddItemToObject(root,"AD1_Type",cJSON_CreateNumber(AD1_Type));                //AD1_Type
				}

				if(Lora_Ack2&0x40)    //AD2_Type
				{					  	
				    cJSON_AddItemToObject(root,"AD2_Type",cJSON_CreateNumber(AD2_Type));                //AD2_Type
				}
				
				if(Lora_Ack2&0x80)    //物理量范围1
				{					  	
				    cJSON_AddItemToObject(root,"AD1_PhyValue1",cJSON_CreateNumber(AD1_PhyValue1));      //物理量范围1
				}
				
				if(Lora_Ack3&0x01)    //物理量范围2
				{					  	
				    cJSON_AddItemToObject(root,"AD1_PhyValue2",cJSON_CreateNumber(AD1_PhyValue2));      //物理量范围2
				}	
				
				if(Lora_Ack3&0x02)    //物理量范围1
				{					  	
				    cJSON_AddItemToObject(root,"AD2_PhyValue1",cJSON_CreateNumber(AD2_PhyValue1));      //物理量范围1
				}	
				
				if(Lora_Ack3&0x04)    //物理量范围2
				{					  	
				    cJSON_AddItemToObject(root,"AD2_PhyValue2",cJSON_CreateNumber(AD2_PhyValue2));      //物理量范围2
				}
				
				if(Lora_Ack3&0x08)    //主站询问从机个数
				{					  	
				    cJSON_AddItemToObject(root,"Slave_Num",cJSON_CreateNumber(Slave_Num));              //主站询问从机个数
				}

				if(Lora_Ack3&0x10)    //主站询问首地址
				{					  	
				    cJSON_AddItemToObject(root,"Master_ADDR",cJSON_CreateNumber(Master_ADDR));          //主站询问首地址
				}	

				if(Lora_Ack3&0x20)    //起始地址
				{					  	
				    cJSON_AddItemToObject(root,"StartAddr",cJSON_CreateNumber(StartAddr));              //起始地址
				}	
				
				if(Lora_Ack3&0x40)    //数据or长度写多个寄存器返回数据量的对比值
				{					  	
				    cJSON_AddItemToObject(root,"ValueOrLenth",cJSON_CreateNumber(ValueOrLenth));       //数据or长度写多个寄存器返回数据量的对比值
				}	
				
				if(Lora_Ack3&0x80)     //作为从机时ID号		
				{					  	
				    cJSON_AddItemToObject(root,"Slave_ADDR",cJSON_CreateNumber(Slave_ADDR));           //作为从机时ID号		
				}	

				if(Lora_Ack4&0x01)     //Modbus串口波特率    
				{					  	
				    cJSON_AddItemToObject(root,"ModbusBaudRate",cJSON_CreateNumber(ModbusBaudRate));     //Modbus串口波特率    
				}
				
				if(Lora_Ack4&0x02)      //Modbus串口数据位			
				{					  	
				    cJSON_AddItemToObject(root,"ModbusWordLength",cJSON_CreateNumber(ModbusWordLength)); //Modbus串口数据位		
				}	 

				if(Lora_Ack4&0x04)      //Modbus串口停止位				
				{					  	
				    cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits));     //Modbus串口停止位		
				}	

 				if(Lora_Ack4&0x08)     //Modbus串口校验位					
				{					  	
				    cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));         //Modbus串口校验位			
				}	

 				if(Lora_Ack4&0x10)  	//脉冲间隔
				{					  	
				    cJSON_AddItemToObject(root,"Pulse_Threshold",cJSON_CreateNumber(TIM3_Pulse_Threshold));   //脉冲间隔
				}	
				
				if(Lora_Ack4&0x20)  	//更新固件链接
				{					  	
				    cJSON_AddItemToObject(root,"URL",cJSON_CreateString(IAPUrlUserBinPath));   //固件链接地址
				}	
 			  if(Lora_Ack4&0x40)  	//更新固件长度
				{					  	
				    cJSON_AddItemToObject(root,"FileSize",cJSON_CreateNumber(FileSize));   //固件链接长度
					  bsp_StartTimer(9,2000);	/* 启动1个1000ms的自动重装的定时器 */		
				}
				name=cJSON_PrintUnformatted(root);
        MainLen = sprintf((char*)MainBuffer,"%s",name);			
			  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息					
				//COM_Send(&huart4,(uint8_t*)name,strlen(name));
				cJsonFree(name);		   //释放内存		
				cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	
				
				if(Lora_Ack&0x02)         //远程配置重启
				{
						 HAL_Delay(500);
							//等待系统重启	                    		
						 __set_FAULTMASK(1);	//屏蔽所有中断 											
						 NVIC_SystemReset();					 
				}
				Lora_Ack=0;
			  Lora_Ack2=0;
				Lora_Ack3=0;
				Lora_Ack4=0;
	 }

}


void DTU_Send(void)//发送IO数据+模拟量数据
{
     static cJSON *root = NULL;      //cjson对象指针
     char *name;	
	   char TimeBuf[50];
		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
		 root = cJSON_CreateObject();                                                   //cjson创建对象 		   
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //本地RTC时间
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Analog_DataType));   //数据类型	
	
	   sprintf(TimeBuf,"%d",GREEN_Value);
		 cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //绿灯值
	
	   sprintf(TimeBuf,"%d",RED_Value);	
	   cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));          //红灯值
	
	   sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
		 cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf));  //定时器3通道1计数		 
			
	   if(AD1_Type!=0){
		 cJSON_AddItemToObject(root,"adc1",cJSON_CreateNumber(adc1)); 
		 cJSON_AddItemToObject(root,"AD1_EValue",cJSON_CreateNumber(AD1_Value));      //工程量
		 cJSON_AddItemToObject(root,"AD1_PhyValue",cJSON_CreateNumber(AD1_PhyValue)); //物理量  		
		 }
		 
		 if(AD2_Type!=0){
		 cJSON_AddItemToObject(root,"adc2",cJSON_CreateNumber(adc2)); 
		 cJSON_AddItemToObject(root,"AD2_EValue",cJSON_CreateNumber(AD2_Value)); 
		 cJSON_AddItemToObject(root,"AD2_PhyValue",cJSON_CreateNumber(AD2_PhyValue)); 		 
		 }
     name=cJSON_PrintUnformatted(root);
     MainLen = sprintf((char*)MainBuffer,"%s",name);			
		 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息
			
		 cJsonFree(name);		   //释放内存		
		 cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	
}

void DTU_SendSlave(uint8_t Addr)//通用Modbus数据
{
		static cJSON *root = NULL;      //cjson对象指针
		char *name;	
		char TimeBuf[50];		  
		char DataBuf[50];
		uint8_t i; 
		//struct ModbusPack *p = (struct ModbusPack *)malloc(sizeof(struct ModbusPack) * 1);	
		struct ModbusPack *p =NULL;//
		p=&Modbus03Slave[Addr];

		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
		 root = cJSON_CreateObject();
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));       //本地RTC时间	
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));//数据类型		
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
						sprintf(DataBuf,"%x",p->MobusTemp[i]);				
						cJSON_AddItemToObject(root,TimeBuf,cJSON_CreateString(DataBuf));      //从机数据
				}					 
		 }	 
		 name=cJSON_PrintUnformatted(root);	
		 MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息			
		 cJsonFree(name);		    //释放内存		
		 cJSON_Delete(root);    //必须释放对象，否则一段时间后数据读取有误	 
		
}

void DTU_SendIO(void)// 发送IO数据变化上报
{
     static cJSON *root = NULL;      //cjson对象指针
     char *name;	
	   char TimeBuf[50];
		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
		 root = cJSON_CreateObject();                                                   //cjson创建对象 		   
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //本地RTC时间
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Count_DataType));    //数据类型	
	   sprintf(TimeBuf,"%d",GREEN_Value);
		 cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //绿灯值
	
	   sprintf(TimeBuf,"%d",RED_Value);	
	   cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));           //红灯值
	
	   sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
		 cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf));    //定时器3通道1计数
		
     name=cJSON_PrintUnformatted(root);
     MainLen = sprintf((char*)MainBuffer,"%s",name);			
		 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);     //发布消息		
		 cJsonFree(name);		   //释放内存		
		 cJSON_Delete(root);   //必须释放对象，否则一段时间后数据读取有误	
}


void SendSearch_GWHT(void)//固德威逆变器HT协议包
{
      static cJSON *root = NULL;      //cjson对象指针
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
		  char TimeBuf[50];
	    uint8_t i;
		  sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //本地RTC时间
	    cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));   //数据类型	
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
			sprintf(DataBuf,"%d",GWHT.Error_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //DSP故障代码		

			sprintf(KeyBuf,"Error_PV");
			sprintf(DataBuf,"%d",GWHT.Error_PV);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //组串故障
			
			sprintf(KeyBuf,"Error_Sub_DSP");
			sprintf(DataBuf,"%d",GWHT.Error_Sub_DSP);	
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
		  MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息		
			cJsonFree(name);		    //释放内存		
			cJSON_Delete(root);     //必须释放对象，否则一段时间后数据读取有误	 
}

void SendSearch_GWMT(void)//固德威逆变器MT协议包
{
      static cJSON *root = NULL;      //cjson对象指针
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
		  char TimeBuf[50];
	    uint8_t i;
		  sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //本地RTC时间
	    cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));   //数据类型	
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
			sprintf(DataBuf,"%d",GWMT.IsSta);	
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
			sprintf(DataBuf,"%d",GWMT.Error_Message);	
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
		  MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息				
			cJsonFree(name);		    //释放内存		
			cJSON_Delete(root);     //必须释放对象，否则一段时间后数据读取有误	 
}

void DTU_Work(void)//4G模块工作步骤
{	 
	  static uint8_t GREEN_Temp=0;  //绿灯缓存值
	  static uint8_t RED_Temp=0;    //绿灯缓存值
	  static uint32_t TIM3_CAPTURE_Temp=0;//定时器3通道1计数缓冲值
	  static uint8_t i=0;
	 	
    if(ConfigModuleNoBlockFlage){//(模块连接了MQTT)
			
			if(CNC_UpType==2){//定时上报
				
					if((GREEN_Temp!=GREEN_Value)||(RED_Temp!=RED_Value)||(TIM3_CAPTURE_Temp!=TIM3_CAPTURE_VAL1))//定时上报中包含变化上报
					{
						 DTU_SendIO();          //发送数据
						 GREEN_Temp=GREEN_Value;
						 RED_Temp=RED_Value;
						 TIM3_CAPTURE_Temp=TIM3_CAPTURE_VAL1;
					}
				  if(bsp_CheckTimer(1)){//定时是秒单位							 
							  DTU_Send();       //发送数据				
					}	
					if(Device485_type==0){//不用485口
										
					}
					else if(Device485_type==1){//ModbusD主站模式												
									
									
							if(bsp_CheckTimer(7))//定时是秒单位
							{									
								  DTU_SendSlave(i);								
									if(++i>Slave_Num-1){
										
										 i=0;
									}													
							}																		
				 }
				 else if(Device485_type==2){//Modbus 从站模式
					 
										 
				 }
         else if(Device485_type==3){//Modbus 固德威HT协议	5分钟上传一次
					 
					 if(Updata_Flag==1)
					 {
						   SendSearch_GWHT();//固德威逆变器HT协议包
					     Updata_Flag=0;						 
					 }
					
				 }
         else if(Device485_type==4){//Modbus 固德威MT协议	5分钟上传一次
					 
					 if(Updata_Flag==1)
					 {
						   SendSearch_GWMT();//固德威逆变器MT协议包		
					     Updata_Flag=0;
					 }								 
				 }				 
			}
			else if(CNC_UpType==1){//变化上报
				
					if((GREEN_Temp!=GREEN_Value)||(RED_Temp!=RED_Value)||(TIM3_CAPTURE_Temp!=TIM3_CAPTURE_VAL1))
					{
						 DTU_SendIO();          //发送数据
						 GREEN_Temp=GREEN_Value;
						 RED_Temp=RED_Value;
						 TIM3_CAPTURE_Temp=TIM3_CAPTURE_VAL1;
					}									
			}	 

			if((timestructure.Hours==00)&(timestructure.Minutes==00)&&(timestructure.Seconds==15)){//每天过凌晨00:00:15重启4G模块
				
						MqttDisConnect();
			}			
			IAPUpdateDispose();//获取,清除,打印更新状态并向云端发送更新状态				
	}
}


/**
* @brief   连接上MQTT回调函数
* @param   None
* @retval  None
* @warning None
* @example
**/
void MqttConnect()
{ 
	memset(MQTTPublishTopic,NULL,sizeof(MQTTPublishTopic));
	sprintf((char*)MQTTPublishTopic,"%s%s/rx","application/4/io/",&MQTTid[0]);//组合发布的主题
	printf("发布主题:%s\r\n",MQTTPublishTopic);
	
	memset(MQTTSubscribeTopic,NULL,sizeof(MQTTSubscribeTopic));
	sprintf((char*)MQTTSubscribeTopic,"%s%s/tx","application/4/io/",&MQTTid[0]);//组合字符串
	mqtt_subscribe(&mymqtt, MQTTSubscribeTopic, 0,subscribedCb,failsubscribedCb);//订阅主题	
	printf("订阅主题:%s\r\n",MQTTSubscribeTopic);
	
  MainLen = sprintf((char*)MainBuffer,"{\"MITABOX\":\"UpLine\"}");//MQTT第一次注册包信息	
	mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息
}

/**
* @brief   MQTT断开连接回调
* @param   None
* @retval  None
* @warning None
* @example
**/
void MqttDisConnect()
{
	mqtt_init(&mymqtt);
	
	//重新配置模块连接MQTT
	ConfigModuleNoBlockCaseValue=0;
	ConfigModuleNoBlockFlage = 0;
	
	printf("\r\n连接断开**********************************************************\r\n");
}


/**
* @brief   MQTT接收数据回调
* @param   topic:主题
* @param   topic_len:主题长度
* @param   data:接收的数据
* @param   lengh:接收的数据长度
* @retval  None
* @warning None
* @example
**/
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh)
{		
	Communication_Remote(data);//远程通信	
}


/**
* @brief   订阅主题成功
* @param   None
* @retval  None
* @warning None
* @example
**/
void subscribedCb(int pdata)
{
	printf("\r\n成功订阅主题\r\n");
}

/**
* @brief   订阅主题失败
* @param   None
* @retval  None
* @warning None
* @example
**/
void failsubscribedCb(int pdata)
{
	printf("\r\n订阅主题失败\r\n");
}


/**
* @brief   发布成功
* @param   None
* @retval  None
* @warning None
* @example
**/
void PublishedCb()
{
	printf("\r\n发布成功\r\n");
}

void Usart4Data(void)
{

	//读取串口接收的数据
	if(USART4_RX_STA&0x8000){
		/*获取模组返回的数据*/
		MainLen = USART4_RX_STA&0X3FFF;
		printf("串口4<<%s\r\n",USART4_RX_BUF);
		memset(usart4_read_buff_copy,0,USART4_REC_LEN);
		if(MainLen > 0)//如果里面的数据个数大于0
		{//提取环形队列里面的数据
			//rbRead(&rb_t_usart4_read, usart4_read_buff_copy, MainLen);
			memcpy(usart4_read_buff_copy,USART4_RX_BUF,MainLen);
			/*TCP连接断开*/
			if(strstr((char*)usart4_read_buff_copy, "CLOSED")){
				if(mymqtt.disconnectCb)mymqtt.disconnectCb();
			}
			//+CCLK: "21/08/11,12:40:58+32"

		
			//配置模块连接MQTT 
			if(ConfigConnectDispose != NULL){
				ConfigConnectDispose((char*)usart4_read_buff_copy,MainLen);
			}
			//模块连接了MQTT
			if(ConfigModuleNoBlockFlage)
			{
				//函数处理MQTT数据
				mqtt_read_function(&mymqtt,usart4_read_buff_copy,MainLen);
			}
		}
		memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
		USART4_RX_STA=0;
	}
}