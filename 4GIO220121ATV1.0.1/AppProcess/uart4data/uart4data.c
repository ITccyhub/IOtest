#include "uart4data.h"
#include "uart3data.h"
#include "modbus.h"
#include "cmsis_os.h"
extern char Version_Num[30];  //�汾��
uint8_t Count_DataType=1;     //�仯��������
uint8_t Analog_DataType=2;    //ģ����������
uint8_t Modubs_DataType=3;    //Mobus��������

uint8_t Updata_Flag=0;           //�ϴ���־      
void MqttConnect(void);          //������MQTT�ص�
void MqttDisConnect(void);       //MQTT�Ͽ����ӻص�
void subscribedCb(int pdata);    //���ĳɹ��ص�
void failsubscribedCb(int pdata);//����ʧ�ܻص�
void PublishedCb(void);          //�ɹ�������Ϣ�ص�
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//���յ����ݻص�
void Communication_Remote(const char *data);//Զ��ͨ��

void Communication_Remote(const char *data)//Զ��ͨ��
{	 
	 char *name;
	 char temp1[20],temp2[20],temp3[20],temp4[20],temp5[20],temp6[20];	
	 static uint8_t Lora_Ack,Lora_Ack2,Lora_Ack3,Lora_Ack4=0;      //Զ�˽������ݷ��ر�־
	 char TimeBuf[50];
	 uint8_t databuf[30];
	 uint8_t res;
	 static uint8_t OutTemp;
	 static  cJSON *root = NULL;      //cjson����ָ��	
	 static cJSON *TEST = NULL;      //cjson����ָ��
	 static cJSON *Analysis = NULL;  //cjson����ָ��	
		
		TEST = cJSON_Parse((char*)data);   
		if(!TEST) 
		{
				printf("Error before: [%s]\r\n",cJSON_GetErrorPtr());	
				root = cJSON_CreateObject();                                                                   //cjson�������� 
				cJSON_AddItemToObject(root,"JSON Format error",cJSON_CreateString("Please check JSON Format"));//JSON��ʽ����
				name=cJSON_PrintUnformatted(root);					
				MainLen = sprintf((char*)MainBuffer,"%s",name);			
				mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ	
				//COM_Send(&huart4,(uint8_t*)name,strlen(name));											
				cJsonFree(name);		  //�ͷ��ڴ�		
				cJSON_Delete(root);   //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����									
		}
		else
		{			   
				Analysis=cJSON_GetObjectItem(TEST, "SET_RTC");      //����RTCʱ��
				if(Analysis!=NULL)
				{
						sscanf(Analysis->valuestring,"%[^-]-%[^-]-%[^ ] %[^:]:%[^:]:%[^:]",temp1,temp2,temp3,temp4,temp5,temp6);
						sdatestructure.Year=atoi(temp1)-2000;									    
						sdatestructure.Month=atoi(temp2);							
						sdatestructure.Date=atoi(temp3);
						stimestructure.Hours=atoi(temp4);
						stimestructure.Minutes=atoi(temp5);
						stimestructure.Seconds=atoi(temp6);	
						HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);//����ʱ��
						HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);  
						//Զ�����÷��ر���Ӧ����Ϣ								
						printf("%s\r\n", "Զ������ʱ�����ò���:");
						printf("Զ������������ʱ����:%04d-%02d-%02d %02d:%02d:%02d\r\n", sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//	
						//�����ƶ���Ϣ�����д
						Lora_Ack|=0x01;                			
				}

				Analysis=cJSON_GetObjectItem(TEST, "Restart");        //Զ��ϵͳ��λ
				if(Analysis!=NULL)
				{									   
							 printf("%s\r\n", "Զ�����ø�λ�ȴ�ϵͳ��λ!");	
							 //�����ƶ���Ϣ�����д			
							 Lora_Ack|=0x02;																		
				}		

				Analysis=cJSON_GetObjectItem(TEST, "Print_Config");    //Զ�̴�ӡϵͳ��Ϣ
				if(Analysis!=NULL)
				{									  
							 printf("%s\r\n", "Զ������ϵͳ������Ϣ��ӡ!");
							 //�����ƶ���Ϣ�����д										
							 Lora_Ack|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST, "OUT1");           //Զ�̿���OUT1
				if(Analysis!=NULL)
				{									  
							OutTemp=Analysis->valuedouble;
							if(OutTemp>=1)
							{
									HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_SET);
									printf("%s\r\n", "Զ������OUT1_ON!");
							}	
							else 
							{
									HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin,GPIO_PIN_RESET);
									printf("%s\r\n", "Զ������OUT1_OFF!");
							}		
							 //�����ƶ���Ϣ�����д										
							 Lora_Ack|=0x08;		   					
				}	

				Analysis=cJSON_GetObjectItem(TEST, "OUT2");           //Զ�̿���OUT2
				if(Analysis!=NULL)
				{									  
							OutTemp=Analysis->valuedouble;
							if(OutTemp>=1)
							{
									HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_SET);
									printf("%s\r\n", "Զ������OUT2_ON!");
							}	
							else 
							{
									HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin,GPIO_PIN_RESET);
									printf("%s\r\n", "Զ������OUT2_OFF!");
							}		
							 //�����ƶ���Ϣ�����д										
							 Lora_Ack|=0x10;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST, "RS485Type");                  //Զ������485�ӿ�����
				if(Analysis!=NULL)
				{			
							 Device485_type=Analysis->valuedouble;						    								
							 AT24CXX_WriteOne(RS485_Adresss,Device485_type);            //д��EEROM����			
							 printf("RS485Զ��������������Ϊ:%d 0:�� 1:MODBUS��վ 2:MODBUS��վ 3:����Э��\r\n", Device485_type);													 				  			     				  						  
							 //�����ƶ���Ϣ�����д										
							 Lora_Ack|=0x20;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"Format_EEROM");                //Զ�̳�ʼ��EEROM
				if(Analysis!=NULL)
				{									     
								printf("%s\r\n"," Զ��EEROM��ʽ��!");	 
								AT24CXX_WriteOne(0,0xff); //                      		
								//�����ƶ���Ϣ�����д										
								Lora_Ack|=0x40;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"CNC_INUM");                 //Զ�����ü�����ֵ
				if(Analysis!=NULL)
				{   
								TIM3_CAPTURE_VAL1=Analysis->valuedouble;  								
								printf("Զ������TIM3_CAPTURE_VAL1=%d\r\n",TIM3_CAPTURE_VAL1);	                 								
								//�����ƶ���Ϣ�����д										
								Lora_Ack|=0x80;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_UpType");              //CNC�����ϴ���ʽ
				if(Analysis!=NULL)
				{   
								CNC_UpType=Analysis->valuedouble; 
								if(CNC_UpType>2)
								{
										CNC_UpType=2;
								}
								
								if(CNC_UpType==1)
								{								 
									bsp_StopTimer(1);//ֹͣ��ʱ
								}									
								else if(CNC_UpType==2)
								{
										bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */	
									  bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */	
								}
								AT24CXX_WriteOne(CNC_UpType_Adress,CNC_UpType); //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�								
								printf("Զ������CNC_UpType=%d 0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�\r\n",CNC_UpType);	 	                  								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x01;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_UpTime");          //CNC������ʱ�ϴ�ʱ�䵥λ�� 
				if(Analysis!=NULL)
				{   
								CNC_UpTime=Analysis->valuedouble;
								if(CNC_UpTime<5)
								{
										CNC_UpTime=5;
								}
								bsp_StartAutoTimer(1, CNC_UpTime*1000);	        /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */	
								bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */	
								memcpy(databuf,&CNC_UpTime,2);                     //1:��λ��
								AT24CXX_Write(CNC_UpTime_Adress,databuf,2);																			
								printf("Զ������CNC_UpTime=%d��\r\n",CNC_UpTime);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x02;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_YELLOW");             //CNC�Ƶ�״̬����
				if(Analysis!=NULL)
				{   
								CNC_YELLOW=Analysis->valuedouble; 
								if(CNC_YELLOW>=2){
									
									CNC_YELLOW=2;
								}									
								AT24CXX_WriteOne(CNC_YELLOW_Adress,CNC_YELLOW);     //0:�������� 1:��˸����												
								printf("Զ������CNC_Yellow=%d 0:�������� 1:��˸���� 2:�������	\r\n",CNC_YELLOW);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_GREEN");             //CNC�̵�״̬����
				if(Analysis!=NULL)
				{   
								CNC_GREEN=Analysis->valuedouble;
								if(CNC_GREEN>=1){
									
									CNC_GREEN=1;
								}									
								AT24CXX_WriteOne(CNC_GREEN_Adress,CNC_GREEN); //0:�������� 1:��˸����												
								printf("Զ������CNC_GREEN=%d 0:�������� 1:��˸����\r\n",CNC_GREEN);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x08;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"CNC_RED");             //CNC���״̬����
				if(Analysis!=NULL)
				{   
								CNC_RED=Analysis->valuedouble;  
								if(CNC_RED>=1){
									
									CNC_RED=1;
								}											
								AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED); //0:�������� 1:��˸����												
								printf("Զ������CNC_RED=%d 0:�������� 1:��˸����\r\n",CNC_RED);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x10;		   					
				}		

				Analysis=cJSON_GetObjectItem(TEST,"AD1_Type");             //AD1����
				if(Analysis!=NULL)
				{   
								AD1_Type=Analysis->valuedouble; 																	
								AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);        //д��EEROM����											
								printf("Զ������AD1_Type=%d 0:�� 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD1_Type);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x20;		   					
				}						
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_Type");             //AD2����
				if(Analysis!=NULL)
				{   
								AD2_Type=Analysis->valuedouble; 																	
								AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);        //д��EEROM����											
								printf("Զ������AD2_Type=%d 0:�� 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD2_Type);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x40;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"AD1_PhyValue1");      //��������Χ1
				if(Analysis!=NULL)
				{   
								AD1_PhyValue1=Analysis->valuedouble; 																	
								memcpy(databuf,&AD1_PhyValue1,4);               //AD1��������Χ1
								AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);										
								printf("Զ������AD1_PhyValue1=%f\r\n",AD1_PhyValue1);	 ;	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack2|=0x80;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"AD1_PhyValue2");     //��������Χ2
				if(Analysis!=NULL)
				{   
								AD1_PhyValue2=Analysis->valuedouble; 																	
								memcpy(databuf,&AD1_PhyValue2,4);               //AD1��������Χ2
								AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);											
								printf("Զ������AD1_PhyValue2=%f\r\n",AD1_PhyValue2);	 ;	 	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x01;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_PhyValue1");      //��������Χ1
				if(Analysis!=NULL)
				{   
								AD2_PhyValue1=Analysis->valuedouble; 																	
								memcpy(databuf,&AD2_PhyValue1,4);               //AD2��������Χ1
								AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);						
								printf("Զ������AD2_PhyValue1=%f\r\n",AD2_PhyValue1);	 ;	             								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x02;		   					
				}	
				
				Analysis=cJSON_GetObjectItem(TEST,"AD2_PhyValue2");      //��������Χ2
				if(Analysis!=NULL)
				{   
								AD2_PhyValue2=Analysis->valuedouble; 																	
								memcpy(databuf,&AD2_PhyValue2,4);               //AD2��������Χ2
								AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);								
								printf("Զ������AD2_PhyValue2=%f\r\n",AD2_PhyValue2);	 ;	             								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x04;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Slave_Num");            //��վѯ�ʴӻ�����
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
								AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);      //д��EEROM����											
								printf("Զ������Slave_Num=%d\r\n",Slave_Num);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x08;		   					
				}					

				Analysis=cJSON_GetObjectItem(TEST,"Master_ADDR");          //��վѯ���׵�ַ
				if(Analysis!=NULL)
				{   
								Master_ADDR=Analysis->valuedouble;
								if(Master_ADDR<1)
								{
										Master_ADDR=1;
								}						
								AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);  //д��EEROM����											
								printf("Զ������Master_ADDR=%d\r\n",Master_ADDR);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x10;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"StartAddr");            //��ʼ��ַ
				if(Analysis!=NULL)
				{   
								StartAddr=Analysis->valuedouble;
								memcpy(databuf,&StartAddr,2);                       //��ʼ��ַ
								AT24CXX_Write(StartAddr_Adress,databuf,2);																		
								printf("Զ������StartAddr=%04x\r\n",StartAddr);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x20;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ValueOrLenth");            //����or����д����Ĵ��������������ĶԱ�ֵ
				if(Analysis!=NULL)
				{   
								ValueOrLenth=Analysis->valuedouble;
								if(ValueOrLenth>20)
								{
										ValueOrLenth=20;
								}
								memcpy(databuf,&ValueOrLenth,2);                      //����or����д����Ĵ��������������ĶԱ�ֵ
								AT24CXX_Write(ValueOrLenth_Adress,databuf,2);																		
								printf("Զ������ValueOrLenth=%04x\r\n",ValueOrLenth);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x40;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Slave_ADDR");              //��Ϊ�ӻ�ʱID��
				if(Analysis!=NULL)
				{   
								Slave_ADDR=Analysis->valuedouble; 
								if(Slave_ADDR<1)
								{
										Slave_ADDR=1;
								}							
								AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);       //д��EEROM����											
								printf("Զ������Slave_ADDR=%d\r\n",Slave_ADDR);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack3|=0x80;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusBaudRate");           //Modbus���ڲ�����
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
								AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate); //д��EEROM����											
								printf("Զ������ModbusBaudRate=%d\r\n",ModbusBaudRate);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack4|=0x01;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusWordLength");          //Modbus��������λ
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
								AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength); //д��EEROM����											
								printf("Զ������ModbusWordLength=%d\r\n",ModbusWordLength);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack4|=0x02;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"ModbusStopBits");                //Modbus����ֹͣλ
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
								AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);       //д��EEROM����											
								printf("Զ������ModbusStopBits=%d\r\n",ModbusStopBits);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack4|=0x04;		   					
				}
				
				Analysis=cJSON_GetObjectItem(TEST,"ModbusParity");                    //Modbus����У��λ
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
								AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);            //д��EEROM����											
								printf("Զ������ModbusParity=%d\r\n",ModbusParity);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack4|=0x08;		   					
				}

				Analysis=cJSON_GetObjectItem(TEST,"Pulse_Interval");               //������
				if(Analysis!=NULL)
				{   
								TIM3_Pulse_Threshold=Analysis->valuedouble;
								if(TIM3_Pulse_Threshold>4000)
								{
										TIM3_Pulse_Threshold=4000;	
								}					
								memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //���岶׽�������½���֮��ʱ�䵥λms
								AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);																	
								printf("Զ������Pulse_Interval=%d\r\n",TIM3_Pulse_Threshold);	            								
								//�����ƶ���Ϣ�����д										
								Lora_Ack4|=0x10;		   					
				}	
 
				Analysis=cJSON_GetObjectItem(TEST,"URL");          //���¹̼�����
				if(Analysis!=NULL)
        {
				    sscanf(Analysis->valuestring,"%s",IAPUrlUserBinPath);
				    sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);
					  //IAPSetUpdateUrl(IAPStructValue.Url);//����URL					 
			      printf("Զ������URL:%s\r\n", IAPUrlUserBinPath);      
						//�����ƶ���Ϣ�����д										
					  Lora_Ack4|=0x20;
				}				
				
				Analysis=cJSON_GetObjectItem(TEST,"FileSize");        //����FileSize��С
				if(Analysis!=NULL)
				{
				    FileSize=Analysis->valuedouble;
					  printf("Զ��FileSize:%d\r\n",FileSize);
						//�����ƶ���Ϣ�����д										
					  Lora_Ack4|=0x40;
				}				
				
				cJSON_Delete(TEST);                                           //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����  
		}

	
	 if((Lora_Ack!=0)|(Lora_Ack2!=0)|(Lora_Ack3!=0)|(Lora_Ack4!=0))
	 {
		    root = cJSON_CreateObject();      //cjson��������     
	 	    if(Lora_Ack&0x01)    //Զ������ʱ��
				{
						 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);					                                               
						 cJSON_AddItemToObject(root,"SET_RTC",cJSON_CreateString(TimeBuf));                    //����RTCʱ��						
				}
				
				if(Lora_Ack&0x02)   //Զ����������
				{
						cJSON_AddItemToObject(root,"Restart",cJSON_CreateString("wait for system reset!"));             //Զ����������								
				}
				
        if(Lora_Ack&0x04)   //Զ�����ö�ȡ��ӡ��Ϣ
        {
						 HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
						 HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
						 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
						 //root = cJSON_CreateObject();                                                   //cjson�������� 
						 cJSON_AddItemToObject(root,"Version_Num",cJSON_CreateString(Version_Num));     //ϵͳ�汾��								   
						 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //����RTCʱ��			
						 cJSON_AddItemToObject(root,"RS485_Type",cJSON_CreateNumber(Device485_type));   //485 ��������
						 cJSON_AddItemToObject(root,"CNC_UpType",cJSON_CreateNumber(CNC_UpType));       //CNC�����ϴ���ʽ
						 if(CNC_UpType==2){				 
							 cJSON_AddItemToObject(root,"CNC_UpTime",cJSON_CreateNumber(CNC_UpTime));     //CNC������ʱ�ϴ�ʱ�䵥λ���� 
						 }			 
						 cJSON_AddItemToObject(root,"CNC_YELLOW",cJSON_CreateNumber(CNC_YELLOW));   //CNC�Ƶ�״̬����
						 cJSON_AddItemToObject(root,"CNC_GREEN",cJSON_CreateNumber(CNC_GREEN));     //CNC�̵�״̬����		
						 cJSON_AddItemToObject(root,"CNC_RED",cJSON_CreateNumber(CNC_RED));         //CNC���״̬����		
             if(AD1_Type>=1){
				 
								 cJSON_AddItemToObject(root,"AD1_Type",cJSON_CreateNumber(AD1_Type));            //0:��       1:4-20ma   2:0-5V 	
								 cJSON_AddItemToObject(root,"AD1_PhyValue1",cJSON_CreateNumber(AD1_PhyValue1));  //��������Χ1
								 cJSON_AddItemToObject(root,"AD1_PhyValue2",cJSON_CreateNumber(AD1_PhyValue2));  //��������Χ2
						 }						 
						 if(AD2_Type>=1){
							 
								 cJSON_AddItemToObject(root,"AD2_Type",cJSON_CreateNumber(AD2_Type));            //0:��       1:4-20ma   2:0-5V  
								 cJSON_AddItemToObject(root,"AD2_PhyValue1",cJSON_CreateNumber(AD2_PhyValue1));  //��������Χ1
								 cJSON_AddItemToObject(root,"AD2_PhyValue2",cJSON_CreateNumber(AD2_PhyValue2));  //��������Χ2					 
						 }
						 if(Device485_type!=0)
						 {				 
								 cJSON_AddItemToObject(root,"Slave_Num",cJSON_CreateNumber(Slave_Num));          //��վѯ�ʴӻ�����
								 cJSON_AddItemToObject(root,"Master_ADDR",cJSON_CreateNumber(Master_ADDR));      //��վѯ���׵�ַ 
								 cJSON_AddItemToObject(root,"StartAddr",cJSON_CreateNumber(StartAddr));          //��ʼ��ַ
								 cJSON_AddItemToObject(root,"ValueOrLenth",cJSON_CreateNumber(ValueOrLenth));    //����or����д����Ĵ��������������ĶԱ�ֵ 
								 cJSON_AddItemToObject(root,"Slave_ADDR",cJSON_CreateNumber(Slave_ADDR));        //��Ϊ�ӻ�ʱID��
								 cJSON_AddItemToObject(root,"ModbusBaudRate",cJSON_CreateNumber(ModbusBaudRate));//Modbus���ڲ�����
								 cJSON_AddItemToObject(root,"ModbusWordLength",cJSON_CreateNumber(ModbusWordLength));//Modbus��������λ
								 cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits)); //Modbus����ֹͣλ
								 cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));//Modbus����У��λ	 
						 }
            cJSON_AddItemToObject(root,"Pulse_Interval",cJSON_CreateNumber(TIM3_Pulse_Threshold));//������						 
				}
				
				if(Lora_Ack&0x08)    //Զ�̿���OUT1
				{
				    cJSON_AddItemToObject(root,"OUT1",cJSON_CreateNumber(OutTemp));   //Զ�̿���OUT1				
				}
				
				if(Lora_Ack&0x10)    //Զ�̿���OUT2
				{
				    cJSON_AddItemToObject(root,"OUT2",cJSON_CreateNumber(OutTemp));  //Զ�̿���OUT2				
				}
				
			  if(Lora_Ack&0x20)    //Զ�̿���OUT2
				{
				    cJSON_AddItemToObject(root,"RS485Type",cJSON_CreateNumber(Device485_type));   //Զ������485�ӿ�����				
				}
				
				if(Lora_Ack&0x40)    //Զ�̳�ʼ��EEROM
				{
					  sprintf(TimeBuf,"%s","OK");		
				    cJSON_AddItemToObject(root,"Format_EEROM",cJSON_CreateString(TimeBuf));       //Զ�̳�ʼ��EEROM
            Lora_Ack|=0x02; //��λ����					
				}
				
			  if(Lora_Ack&0x80)    //Զ�����ü�����ֵ
				{					  	
				    cJSON_AddItemToObject(root,"CNC_INUM",cJSON_CreateNumber(TIM3_CAPTURE_VAL1));       //Զ�����ü�����ֵ          			
				}
				
				if(Lora_Ack2&0x01)    //CNC�����ϴ���ʽ
				{					  	
				    cJSON_AddItemToObject(root,"CNC_UpType",cJSON_CreateNumber(CNC_UpType));           //CNC�����ϴ���ʽ    			
				}
				
				if(Lora_Ack2&0x02)    //CNC������ʱ�ϴ�ʱ�䵥λ����
				{					  	
				    cJSON_AddItemToObject(root,"CNC_UpTime",cJSON_CreateNumber(CNC_UpTime));           //CNC������ʱ�ϴ�ʱ�䵥λ���� 			
				}
				
				if(Lora_Ack2&0x04)    //CNC�Ƶ�״̬����
				{					  	
				    cJSON_AddItemToObject(root,"CNC_YELLOW",cJSON_CreateNumber(CNC_YELLOW));          //CNC�Ƶ�״̬����	
				}
				
				if(Lora_Ack2&0x08)    //CNC�̵�״̬����
				{					  	
				    cJSON_AddItemToObject(root,"CNC_GREEN",cJSON_CreateNumber(CNC_GREEN));            //CNC�̵�״̬����	
				}
				
				if(Lora_Ack2&0x10)    //CNC���״̬����
				{					  	
				    cJSON_AddItemToObject(root,"CNC_RED",cJSON_CreateNumber(CNC_RED));                //CNC���״̬����	
				}

				if(Lora_Ack2&0x20)    //AD1_Type
				{					  	
				    cJSON_AddItemToObject(root,"AD1_Type",cJSON_CreateNumber(AD1_Type));                //AD1_Type
				}

				if(Lora_Ack2&0x40)    //AD2_Type
				{					  	
				    cJSON_AddItemToObject(root,"AD2_Type",cJSON_CreateNumber(AD2_Type));                //AD2_Type
				}
				
				if(Lora_Ack2&0x80)    //��������Χ1
				{					  	
				    cJSON_AddItemToObject(root,"AD1_PhyValue1",cJSON_CreateNumber(AD1_PhyValue1));      //��������Χ1
				}
				
				if(Lora_Ack3&0x01)    //��������Χ2
				{					  	
				    cJSON_AddItemToObject(root,"AD1_PhyValue2",cJSON_CreateNumber(AD1_PhyValue2));      //��������Χ2
				}	
				
				if(Lora_Ack3&0x02)    //��������Χ1
				{					  	
				    cJSON_AddItemToObject(root,"AD2_PhyValue1",cJSON_CreateNumber(AD2_PhyValue1));      //��������Χ1
				}	
				
				if(Lora_Ack3&0x04)    //��������Χ2
				{					  	
				    cJSON_AddItemToObject(root,"AD2_PhyValue2",cJSON_CreateNumber(AD2_PhyValue2));      //��������Χ2
				}
				
				if(Lora_Ack3&0x08)    //��վѯ�ʴӻ�����
				{					  	
				    cJSON_AddItemToObject(root,"Slave_Num",cJSON_CreateNumber(Slave_Num));              //��վѯ�ʴӻ�����
				}

				if(Lora_Ack3&0x10)    //��վѯ���׵�ַ
				{					  	
				    cJSON_AddItemToObject(root,"Master_ADDR",cJSON_CreateNumber(Master_ADDR));          //��վѯ���׵�ַ
				}	

				if(Lora_Ack3&0x20)    //��ʼ��ַ
				{					  	
				    cJSON_AddItemToObject(root,"StartAddr",cJSON_CreateNumber(StartAddr));              //��ʼ��ַ
				}	
				
				if(Lora_Ack3&0x40)    //����or����д����Ĵ��������������ĶԱ�ֵ
				{					  	
				    cJSON_AddItemToObject(root,"ValueOrLenth",cJSON_CreateNumber(ValueOrLenth));       //����or����д����Ĵ��������������ĶԱ�ֵ
				}	
				
				if(Lora_Ack3&0x80)     //��Ϊ�ӻ�ʱID��		
				{					  	
				    cJSON_AddItemToObject(root,"Slave_ADDR",cJSON_CreateNumber(Slave_ADDR));           //��Ϊ�ӻ�ʱID��		
				}	

				if(Lora_Ack4&0x01)     //Modbus���ڲ�����    
				{					  	
				    cJSON_AddItemToObject(root,"ModbusBaudRate",cJSON_CreateNumber(ModbusBaudRate));     //Modbus���ڲ�����    
				}
				
				if(Lora_Ack4&0x02)      //Modbus��������λ			
				{					  	
				    cJSON_AddItemToObject(root,"ModbusWordLength",cJSON_CreateNumber(ModbusWordLength)); //Modbus��������λ		
				}	 

				if(Lora_Ack4&0x04)      //Modbus����ֹͣλ				
				{					  	
				    cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits));     //Modbus����ֹͣλ		
				}	

 				if(Lora_Ack4&0x08)     //Modbus����У��λ					
				{					  	
				    cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));         //Modbus����У��λ			
				}	

 				if(Lora_Ack4&0x10)  	//������
				{					  	
				    cJSON_AddItemToObject(root,"Pulse_Threshold",cJSON_CreateNumber(TIM3_Pulse_Threshold));   //������
				}	
				
				if(Lora_Ack4&0x20)  	//���¹̼�����
				{					  	
				    cJSON_AddItemToObject(root,"URL",cJSON_CreateString(IAPUrlUserBinPath));   //�̼����ӵ�ַ
				}	
 			  if(Lora_Ack4&0x40)  	//���¹̼�����
				{					  	
				    cJSON_AddItemToObject(root,"FileSize",cJSON_CreateNumber(FileSize));   //�̼����ӳ���
					  bsp_StartTimer(9,2000);	/* ����1��1000ms���Զ���װ�Ķ�ʱ�� */		
				}
				name=cJSON_PrintUnformatted(root);
        MainLen = sprintf((char*)MainBuffer,"%s",name);			
			  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ					
				//COM_Send(&huart4,(uint8_t*)name,strlen(name));
				cJsonFree(name);		   //�ͷ��ڴ�		
				cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	
				
				if(Lora_Ack&0x02)         //Զ����������
				{
						 HAL_Delay(500);
							//�ȴ�ϵͳ����	                    		
						 __set_FAULTMASK(1);	//���������ж� 											
						 NVIC_SystemReset();					 
				}
				Lora_Ack=0;
			  Lora_Ack2=0;
				Lora_Ack3=0;
				Lora_Ack4=0;
	 }

}


void DTU_Send(void)//����IO����+ģ��������
{
     static cJSON *root = NULL;      //cjson����ָ��
     char *name;	
	   char TimeBuf[50];
		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
		 root = cJSON_CreateObject();                                                   //cjson�������� 		   
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //����RTCʱ��
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Analog_DataType));   //��������	
	
	   sprintf(TimeBuf,"%d",GREEN_Value);
		 cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //�̵�ֵ
	
	   sprintf(TimeBuf,"%d",RED_Value);	
	   cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));          //���ֵ
	
	   sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
		 cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf));  //��ʱ��3ͨ��1����		 
			
	   if(AD1_Type!=0){
		 cJSON_AddItemToObject(root,"adc1",cJSON_CreateNumber(adc1)); 
		 cJSON_AddItemToObject(root,"AD1_EValue",cJSON_CreateNumber(AD1_Value));      //������
		 cJSON_AddItemToObject(root,"AD1_PhyValue",cJSON_CreateNumber(AD1_PhyValue)); //������  		
		 }
		 
		 if(AD2_Type!=0){
		 cJSON_AddItemToObject(root,"adc2",cJSON_CreateNumber(adc2)); 
		 cJSON_AddItemToObject(root,"AD2_EValue",cJSON_CreateNumber(AD2_Value)); 
		 cJSON_AddItemToObject(root,"AD2_PhyValue",cJSON_CreateNumber(AD2_PhyValue)); 		 
		 }
     name=cJSON_PrintUnformatted(root);
     MainLen = sprintf((char*)MainBuffer,"%s",name);			
		 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ
			
		 cJsonFree(name);		   //�ͷ��ڴ�		
		 cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	
}

void DTU_SendSlave(uint8_t Addr)//ͨ��Modbus����
{
		static cJSON *root = NULL;      //cjson����ָ��
		char *name;	
		char TimeBuf[50];		  
		char DataBuf[50];
		uint8_t i; 
		//struct ModbusPack *p = (struct ModbusPack *)malloc(sizeof(struct ModbusPack) * 1);	
		struct ModbusPack *p =NULL;//
		p=&Modbus03Slave[Addr];

		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
		 root = cJSON_CreateObject();
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));       //����RTCʱ��	
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));//��������		
		 if(p->Modbus_Err!=0){//�ӻ�������ͨ���й���
			 
				sprintf(TimeBuf,"%d",p->Modbus_Err);
				sprintf(DataBuf,"Slave%d",Addr+Master_ADDR);	
				cJSON_AddItemToObject(root,DataBuf,cJSON_CreateString(TimeBuf));          //�ӻ���������						 
		 }
		 else if(p->Modbus_Err==0){//�ӻ�ͨ������
			 
				sprintf(TimeBuf,"%d",p->Modbus_Err);
				sprintf(DataBuf,"Slave%d",Addr+Master_ADDR);	
				cJSON_AddItemToObject(root,DataBuf,cJSON_CreateString(TimeBuf));          //�ӻ���������						 
				for(i=0;i<ValueOrLenth;i++){//�������
					
						sprintf(TimeBuf,"Add%d",StartAddr+i);
						sprintf(DataBuf,"%x",p->MobusTemp[i]);				
						cJSON_AddItemToObject(root,TimeBuf,cJSON_CreateString(DataBuf));      //�ӻ�����
				}					 
		 }	 
		 name=cJSON_PrintUnformatted(root);	
		 MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ			
		 cJsonFree(name);		    //�ͷ��ڴ�		
		 cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 
		
}

void DTU_SendIO(void)// ����IO���ݱ仯�ϱ�
{
     static cJSON *root = NULL;      //cjson����ָ��
     char *name;	
	   char TimeBuf[50];
		 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
		 root = cJSON_CreateObject();                                                   //cjson�������� 		   
		 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //����RTCʱ��
		 cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Count_DataType));    //��������	
	   sprintf(TimeBuf,"%d",GREEN_Value);
		 cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //�̵�ֵ
	
	   sprintf(TimeBuf,"%d",RED_Value);	
	   cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));           //���ֵ
	
	   sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
		 cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf));    //��ʱ��3ͨ��1����
		
     name=cJSON_PrintUnformatted(root);
     MainLen = sprintf((char*)MainBuffer,"%s",name);			
		 mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);     //������Ϣ		
		 cJsonFree(name);		   //�ͷ��ڴ�		
		 cJSON_Delete(root);   //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	
}


void SendSearch_GWHT(void)//�̵��������HTЭ���
{
      static cJSON *root = NULL;      //cjson����ָ��
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
		  char TimeBuf[50];
	    uint8_t i;
		  sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //����RTCʱ��
	    cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));   //��������	
	    cJSON_AddItemToObject(root,"Inverter",cJSON_CreateString("GOODWE_HT"));	
	    for(i=0;i<12;i++){
	      sprintf(KeyBuf,"Vpv%d",i+1);
        sprintf(DataBuf,"%.1f",GWHT.Vpv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT��ѹ	
				
	      sprintf(KeyBuf,"Ipv%d",i+1);
        sprintf(DataBuf,"%.2f",GWHT.Ipv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT����				
			}
	
	    for(i=0;i<24;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.2f",GWHT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //�鴮����			
			}
			
			sprintf(KeyBuf,"Va");
			sprintf(DataBuf,"%.1f",GWHT.Va);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A���ѹ
			
			sprintf(KeyBuf,"Vb");
			sprintf(DataBuf,"%.1f",GWHT.Vb);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B���ѹ
			
			sprintf(KeyBuf,"Vc");
			sprintf(DataBuf,"%.1f",GWHT.Vc);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C���ѹ		

			sprintf(KeyBuf,"Ia");
			sprintf(DataBuf,"%.3f",GWHT.Ia);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A�����
			
			sprintf(KeyBuf,"Ib");
			sprintf(DataBuf,"%.3f",GWHT.Ib);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B�����
			
			sprintf(KeyBuf,"Ic");
			sprintf(DataBuf,"%.3f",GWHT.Ic);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C�����

			sprintf(KeyBuf,"P");
			sprintf(DataBuf,"%.3f",GWHT.P);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�й�����

			sprintf(KeyBuf,"F");
			sprintf(DataBuf,"%.2f",GWHT.Frequency);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //Ƶ��

			sprintf(KeyBuf,"Temper");
			sprintf(DataBuf,"%.1f",GWHT.Temper_Inside);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�¶�

			sprintf(KeyBuf,"E_Total");
			sprintf(DataBuf,"%.2f",GWHT.E_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�ۼƷ�����

			sprintf(KeyBuf,"E_Day");
			sprintf(DataBuf,"%.2f",GWHT.E_Day);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //���շ�����
			
			sprintf(KeyBuf,"Error_DSP");
			sprintf(DataBuf,"%d",GWHT.Error_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //DSP���ϴ���		

			sprintf(KeyBuf,"Error_PV");
			sprintf(DataBuf,"%d",GWHT.Error_PV);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�鴮����
			
			sprintf(KeyBuf,"Error_Sub_DSP");
			sprintf(DataBuf,"%d",GWHT.Error_Sub_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //��DSP���ϴ���	

			sprintf(KeyBuf,"H_Total");
			sprintf(DataBuf,"%d",GWHT.H_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�ܷ���ʱ��		
			
			sprintf(KeyBuf,"WorkMode");
			sprintf(DataBuf,"%d",GWHT.WorkMode);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //����ģʽ	
			
			sprintf(KeyBuf,"Error_485");
			sprintf(DataBuf,"%d",GWHT.Inverter_Fault);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�����ͨ�����	

		  name=cJSON_PrintUnformatted(root);	
		  MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ		
			cJsonFree(name);		    //�ͷ��ڴ�		
			cJSON_Delete(root);     //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 
}

void SendSearch_GWMT(void)//�̵��������MTЭ���
{
      static cJSON *root = NULL;      //cjson����ָ��
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
		  char TimeBuf[50];
	    uint8_t i;
		  sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));          //����RTCʱ��
	    cJSON_AddItemToObject(root,"Data_Type",cJSON_CreateNumber(Modubs_DataType));   //��������	
	    cJSON_AddItemToObject(root,"Inverter",cJSON_CreateString("GOODWE_MT"));	
	    for(i=0;i<4;i++){
	      sprintf(KeyBuf,"Vpv%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Vpv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT��ѹ		

	      sprintf(KeyBuf,"Ipv%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Ipv[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //MPPT����	
				
			}		
	
	    for(i=0;i<16;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //�鴮����1-16	
			}

	    for(i=17;i<20;i++){
	      sprintf(KeyBuf,"Is%d",i+1);
        sprintf(DataBuf,"%.1f",GWMT.Istring[i]);	
        cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));             //�鴮����18-20
			}
			
      sprintf(KeyBuf,"IsSta");
			sprintf(DataBuf,"%d",GWMT.IsSta);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�鴮1-16״̬
			
			sprintf(KeyBuf,"Va");
			sprintf(DataBuf,"%.1f",GWMT.Va);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A���ѹ
			
			sprintf(KeyBuf,"Vb");
			sprintf(DataBuf,"%.1f",GWMT.Vb);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B���ѹ
			
			sprintf(KeyBuf,"Vc");
			sprintf(DataBuf,"%.1f",GWMT.Vc);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C���ѹ		

			sprintf(KeyBuf,"Ia");
			sprintf(DataBuf,"%.1f",GWMT.Ia);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //A�����
			
			sprintf(KeyBuf,"Ib");
			sprintf(DataBuf,"%.1f",GWMT.Ib);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //B�����
			
			sprintf(KeyBuf,"Ic");
			sprintf(DataBuf,"%.1f",GWMT.Ic);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //C�����

			sprintf(KeyBuf,"P");
			sprintf(DataBuf,"%.3f",GWMT.P);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�й�����
			sprintf(KeyBuf,"Fa");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyA);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //Ƶ��A
			
			sprintf(KeyBuf,"Fb");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyB);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //Ƶ��B
			
			sprintf(KeyBuf,"Fc");
			sprintf(DataBuf,"%.2f",GWMT.FrequencyC);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //Ƶ��C
			
			sprintf(KeyBuf,"Temper");
			sprintf(DataBuf,"%.1f",GWMT.Temper_Inside);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�¶�

			sprintf(KeyBuf,"E_Total");
			sprintf(DataBuf,"%.1f",GWMT.E_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�ۼƷ�����

			sprintf(KeyBuf,"E_Day");
			sprintf(DataBuf,"%.1f",GWMT.E_Day);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //���շ�����
			
			sprintf(KeyBuf,"Error");
			sprintf(DataBuf,"%d",GWMT.Error_Message);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //���ϴ���		
			
			sprintf(KeyBuf,"H_Total");
			sprintf(DataBuf,"%d",GWMT.H_Total);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�ܷ���ʱ��		
			
			sprintf(KeyBuf,"WorkMode");
			sprintf(DataBuf,"%d",GWMT.WorkMode);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //����ģʽ	
			
			sprintf(KeyBuf,"Error_485");
			sprintf(DataBuf,"%d",GWMT.Inverter_Fault);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�����ͨ�����	
			
		  name=cJSON_PrintUnformatted(root);	
		  MainLen = sprintf((char*)MainBuffer,"%s",name);			
	 	  mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ				
			cJsonFree(name);		    //�ͷ��ڴ�		
			cJSON_Delete(root);     //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 
}

void DTU_Work(void)//4Gģ�鹤������
{	 
	  static uint8_t GREEN_Temp=0;  //�̵ƻ���ֵ
	  static uint8_t RED_Temp=0;    //�̵ƻ���ֵ
	  static uint32_t TIM3_CAPTURE_Temp=0;//��ʱ��3ͨ��1��������ֵ
	  static uint8_t i=0;
	 	
    if(ConfigModuleNoBlockFlage){//(ģ��������MQTT)
			
			if(CNC_UpType==2){//��ʱ�ϱ�
				
					if((GREEN_Temp!=GREEN_Value)||(RED_Temp!=RED_Value)||(TIM3_CAPTURE_Temp!=TIM3_CAPTURE_VAL1))//��ʱ�ϱ��а����仯�ϱ�
					{
						 DTU_SendIO();          //��������
						 GREEN_Temp=GREEN_Value;
						 RED_Temp=RED_Value;
						 TIM3_CAPTURE_Temp=TIM3_CAPTURE_VAL1;
					}
				  if(bsp_CheckTimer(1)){//��ʱ���뵥λ							 
							  DTU_Send();       //��������				
					}	
					if(Device485_type==0){//����485��
										
					}
					else if(Device485_type==1){//ModbusD��վģʽ												
									
									
							if(bsp_CheckTimer(7))//��ʱ���뵥λ
							{									
								  DTU_SendSlave(i);								
									if(++i>Slave_Num-1){
										
										 i=0;
									}													
							}																		
				 }
				 else if(Device485_type==2){//Modbus ��վģʽ
					 
										 
				 }
         else if(Device485_type==3){//Modbus �̵���HTЭ��	5�����ϴ�һ��
					 
					 if(Updata_Flag==1)
					 {
						   SendSearch_GWHT();//�̵��������HTЭ���
					     Updata_Flag=0;						 
					 }
					
				 }
         else if(Device485_type==4){//Modbus �̵���MTЭ��	5�����ϴ�һ��
					 
					 if(Updata_Flag==1)
					 {
						   SendSearch_GWMT();//�̵��������MTЭ���		
					     Updata_Flag=0;
					 }								 
				 }				 
			}
			else if(CNC_UpType==1){//�仯�ϱ�
				
					if((GREEN_Temp!=GREEN_Value)||(RED_Temp!=RED_Value)||(TIM3_CAPTURE_Temp!=TIM3_CAPTURE_VAL1))
					{
						 DTU_SendIO();          //��������
						 GREEN_Temp=GREEN_Value;
						 RED_Temp=RED_Value;
						 TIM3_CAPTURE_Temp=TIM3_CAPTURE_VAL1;
					}									
			}	 

			if((timestructure.Hours==00)&(timestructure.Minutes==00)&&(timestructure.Seconds==15)){//ÿ����賿00:00:15����4Gģ��
				
						MqttDisConnect();
			}			
			IAPUpdateDispose();//��ȡ,���,��ӡ����״̬�����ƶ˷��͸���״̬				
	}
}


/**
* @brief   ������MQTT�ص�����
* @param   None
* @retval  None
* @warning None
* @example
**/
void MqttConnect()
{ 
	memset(MQTTPublishTopic,NULL,sizeof(MQTTPublishTopic));
	sprintf((char*)MQTTPublishTopic,"%s%s/rx","application/4/io/",&MQTTid[0]);//��Ϸ���������
	printf("��������:%s\r\n",MQTTPublishTopic);
	
	memset(MQTTSubscribeTopic,NULL,sizeof(MQTTSubscribeTopic));
	sprintf((char*)MQTTSubscribeTopic,"%s%s/tx","application/4/io/",&MQTTid[0]);//����ַ���
	mqtt_subscribe(&mymqtt, MQTTSubscribeTopic, 0,subscribedCb,failsubscribedCb);//��������	
	printf("��������:%s\r\n",MQTTSubscribeTopic);
	
  MainLen = sprintf((char*)MainBuffer,"{\"MITABOX\":\"UpLine\"}");//MQTT��һ��ע�����Ϣ	
	mqtt_publish(&mymqtt,MQTTPublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ
}

/**
* @brief   MQTT�Ͽ����ӻص�
* @param   None
* @retval  None
* @warning None
* @example
**/
void MqttDisConnect()
{
	mqtt_init(&mymqtt);
	
	//��������ģ������MQTT
	ConfigModuleNoBlockCaseValue=0;
	ConfigModuleNoBlockFlage = 0;
	
	printf("\r\n���ӶϿ�**********************************************************\r\n");
}


/**
* @brief   MQTT�������ݻص�
* @param   topic:����
* @param   topic_len:���ⳤ��
* @param   data:���յ�����
* @param   lengh:���յ����ݳ���
* @retval  None
* @warning None
* @example
**/
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh)
{		
	Communication_Remote(data);//Զ��ͨ��	
}


/**
* @brief   ��������ɹ�
* @param   None
* @retval  None
* @warning None
* @example
**/
void subscribedCb(int pdata)
{
	printf("\r\n�ɹ���������\r\n");
}

/**
* @brief   ��������ʧ��
* @param   None
* @retval  None
* @warning None
* @example
**/
void failsubscribedCb(int pdata)
{
	printf("\r\n��������ʧ��\r\n");
}


/**
* @brief   �����ɹ�
* @param   None
* @retval  None
* @warning None
* @example
**/
void PublishedCb()
{
	printf("\r\n�����ɹ�\r\n");
}

void Usart4Data(void)
{

	//��ȡ���ڽ��յ�����
	if(USART4_RX_STA&0x8000){
		/*��ȡģ�鷵�ص�����*/
		MainLen = USART4_RX_STA&0X3FFF;
		printf("����4<<%s\r\n",USART4_RX_BUF);
		memset(usart4_read_buff_copy,0,USART4_REC_LEN);
		if(MainLen > 0)//�����������ݸ�������0
		{//��ȡ���ζ������������
			//rbRead(&rb_t_usart4_read, usart4_read_buff_copy, MainLen);
			memcpy(usart4_read_buff_copy,USART4_RX_BUF,MainLen);
			/*TCP���ӶϿ�*/
			if(strstr((char*)usart4_read_buff_copy, "CLOSED")){
				if(mymqtt.disconnectCb)mymqtt.disconnectCb();
			}
			//+CCLK: "21/08/11,12:40:58+32"

		
			//����ģ������MQTT 
			if(ConfigConnectDispose != NULL){
				ConfigConnectDispose((char*)usart4_read_buff_copy,MainLen);
			}
			//ģ��������MQTT
			if(ConfigModuleNoBlockFlage)
			{
				//��������MQTT����
				mqtt_read_function(&mymqtt,usart4_read_buff_copy,MainLen);
			}
		}
		memset(USART4_RX_BUF,0,USART4_RX_STA&0X3FFF);
		USART4_RX_STA=0;
	}
}