#include "uart1data.h"
/*****************************************************************************************
************************�̼�Version_Num˵��***********************************************
******V1.0.0 �����汾�ɲɼ�ģ�������ݡ�IO����׼Modbus����*********************************
******V1.0.1 ���ӹ̵��������������ݲɼ����� ���Ż�ͨ��Modbus��վ************************
*************����MQTT�ͻ����û����ƺ��������ù���*****************************************
******************************************************************************************
******************************************************************************************/
char Version_Num[30]="V1.0.1";//�汾��
uint8_t Print_Flag=0;           //�����ӡ��־      1:����     0:�ر�
uint8_t Print_ConfigFlag;       //��ӡ������Ϣ      1:��ӡ     0:����
uint8_t GREEN_Value=0;          //�̵�ֵ            0:������   1:����
uint8_t RED_Value=0;            //���ֵ            0:�޹���   1:����

uint8_t IN1=0;            //IN1ֵ  0:������ 1:������
uint8_t IN2=0;            //IN2ֵ
uint8_t IN3=0;            //IN3ֵ

uint8_t DTU_Heart=0;      //DTU����״̬
float AD1_Value=0;        //������ 0-5V 4-20MA
float AD2_Value=0;        //������
float AD1_PhyValue=0;     //������ֵ
float AD2_PhyValue=0;     //������ֵ

/*****************************��EEROM����*************************/

uint8_t Device485_type=0;  //�豸485����  0:����  1:MODBUS��վ 2:MODBUS��վ 3:GWHT 4:GWMT
uint8_t CNC_UpType=2;      //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�
uint16_t CNC_UpTime=60;    //1:��λ��
uint8_t CNC_YELLOW=2;      //0:�������� 1:��˸����  2:�������
uint8_t CNC_GREEN=0;       //0:�������� 1:��˸����
uint8_t CNC_RED=0;         //0:�������� 1:��˸����
uint8_t AD1_Type=0;        //0:��       1:4-20ma   2:0-5V 
uint8_t AD2_Type=0;        //0:��       1:4-20ma   2:0-5V
float AD1_PhyValue1=0;     //��������Χ1
float AD1_PhyValue2=100;   //��������Χ2
float AD2_PhyValue1=0;     //��������Χ1
float AD2_PhyValue2=100;   //��������Χ2
uint32_t TIM3_CAPTURE_VAL1=0;//��ʱ��3ͨ��1����

uint32_t FileSize=0;       //OTA�ļ���С
uint8_t  UpdateFlage=0;    //������־

void DTU_SendModbus(uint8_t Addr)//Modbus�ӻ�����
{
      static cJSON *root = NULL;      //cjson����ָ��
      char *name;	
	    char TimeBuf[50];		  
	    char DataBuf[50];
	    static uint8_t i;      
		  struct ModbusPack *p =NULL;//
			p=&Modbus03Slave[Addr];

			 sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);	
			 root = cJSON_CreateObject();
			 cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));       //����RTCʱ��	         
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
							sprintf(DataBuf,"0x%X",p->MobusTemp[i]);				
							cJSON_AddItemToObject(root,TimeBuf,cJSON_CreateString(DataBuf));      //�ӻ�����
					}					 
			 }					 
			 name=cJSON_PrintUnformatted(root);					
			 printf("\r\n%s\r\n",name);				
			 cJsonFree(name);		   //�ͷ��ڴ�		
			 cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 			
		
}

void DTU_SendSearch_GWHT(void)//�̵��������HTЭ���
{
      static cJSON *root = NULL;      //cjson����ָ��
      char *name;	
	    char KeyBuf[50];		  
	    char DataBuf[50];
	    char TimeBuf[50];
	    uint8_t i; 
			sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
			root = cJSON_CreateObject();                                                              //cjson��������
      cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));
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
			sprintf(DataBuf,"0x%x",GWHT.Error_DSP);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //DSP���ϴ���		

			sprintf(KeyBuf,"Error_PV");
			sprintf(DataBuf,"%d",GWHT.Error_PV);	
			cJSON_AddItemToObject(root,KeyBuf,cJSON_CreateString(DataBuf));               //�鴮����
			
			sprintf(KeyBuf,"Error_Sub_DSP");
			sprintf(DataBuf,"0x%x",GWHT.Error_Sub_DSP);	
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
			printf("\r\n%s\r\n",name);				
			cJsonFree(name);		    //�ͷ��ڴ�		
			cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 
}

void DTU_SendSearch_GWMT(void)//�̵��������MTЭ���
{
      static cJSON *root = NULL;      //cjson����ָ��
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
			sprintf(DataBuf,"0x%x",GWMT.IsSta);	
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
			sprintf(DataBuf,"0x%x",GWMT.Error_Message);	
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
			printf("\r\n%s\r\n",name);				
			cJsonFree(name);		    //�ͷ��ڴ�		
			cJSON_Delete(root);    //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ����	 

}

void Config_Param(void)//���ò���
{
  char *strx,*strx1,*name;
	char temp[20],temp1[20],temp2[10],temp3[5],temp4[5],temp5[5],temp6[5];
	char TimeBuf[50];
	uint8_t databuf[30];
  static cJSON *root = NULL;      //cjson����ָ��	
	uint8_t Out_Control;	
	static  uint8_t i=0;
	 		
	if(Print_ConfigFlag==1)//��ӡ������Ϣ
	{
		   HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
       HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
		   sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
			 root = cJSON_CreateObject();                                                   //cjson�������� 
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
				   cJSON_AddItemToObject(root,"ModbusStopBits",cJSON_CreateNumber(ModbusStopBits));    //Modbus����ֹͣλ
				   cJSON_AddItemToObject(root,"ModbusParity",cJSON_CreateNumber(ModbusParity));        //Modbus����У��λ	 
			 }
			 cJSON_AddItemToObject(root,"Pulse_Interval",cJSON_CreateNumber(TIM3_Pulse_Threshold));//������
			 name=cJSON_PrintUnformatted(root);						 
			 printf("\r\n%s\r\n",name);					
			 cJsonFree(name);		   //�ͷ��ڴ�		
			 cJSON_Delete(root);   //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ���� 		
	     Print_ConfigFlag=0;
	}
	
	if(bsp_CheckTimer(2)){
	
		  if(Print_Flag==2)
			{				
					HAL_RTC_GetTime(&hrtc, &timestructure, RTC_FORMAT_BIN);
					HAL_RTC_GetDate(&hrtc, &datestructure, RTC_FORMAT_BIN);
					sprintf(TimeBuf,"%04d-%02d-%02d %02d:%02d:%02d",datestructure.Year+2000,datestructure.Month,datestructure.Date,timestructure.Hours,timestructure.Minutes,timestructure.Seconds);
					root = cJSON_CreateObject();                                                              //cjson��������
          cJSON_AddItemToObject(root,"Local_Time",cJSON_CreateString(TimeBuf));                     //����RTCʱ��							
				  sprintf(TimeBuf,"%d",GREEN_Value);
				  cJSON_AddItemToObject(root,"GREEN_Value",cJSON_CreateString(TimeBuf));         //�̵�ֵ
			
				  sprintf(TimeBuf,"%d",RED_Value);	
				  cJSON_AddItemToObject(root,"RED_Value",cJSON_CreateString(TimeBuf));          //���ֵ
			
				  sprintf(TimeBuf,"%d",TIM3_CAPTURE_VAL1);
				  cJSON_AddItemToObject(root,"TIM1_CAPTURE_VAL1",cJSON_CreateString(TimeBuf)); //��ʱ��3ͨ��1����	
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
					cJsonFree(name);		   //�ͷ��ڴ�		
					cJSON_Delete(root);   //�����ͷŶ��󣬷���һ��ʱ������ݶ�ȡ���� 									 
			}			
			else if(Print_Flag==1)
			{			
					if(Device485_type==1){//Modbus��վ����								
									
							DTU_SendModbus(i);	
              if(++i>Slave_Num-1)	
              {
						    i=0;	
							}								
					}
					else if(Device485_type==3){//�̵���HTЭ��
						
						DTU_SendSearch_GWHT();//�̵��������HTЭ���
					}
					else if(Device485_type==4){//�̵���MTЭ��
						
						DTU_SendSearch_GWMT();//�̵��������MTЭ���
					}
				
					else {
						
					    sprintf(TimeBuf,"Device485_type=%d",Device485_type);	
						  printf("\r\n%s\r\n", TimeBuf);
					}				 	
			}	
	}	
	
  if((UpdateFlage==1)||(bsp_CheckTimer(9)))
  {	      	
			IAPInfoPathInit();                  //����URL
		  //sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);
      //printf("IAPStructValue.Url:%s\r\n",IAPStructValue.Url); 		
		  IAPSetUpdateUrl(IAPStructValue.Url);//����URL
			IAPSetFileSize(FileSize); //�����ļ���С
			IAPSetUpdateFlage();      //���ø��±�־	
      IAPResetMCU();            //��λ����	
      UpdateFlage=0;
      bsp_StopTimer(9);		
  }
	 
	if(USART_RX_STA&0x8000)
	{
		if(USART_RX_BUF[0]==0xe1&USART_RX_BUF[1]==0xe2&USART_RX_BUF[2]==0xe3&USART_RX_BUF[3]==0x1e&USART_RX_BUF[4]==0x2e)//�жϽ���BootLoader״̬����APP��Ҫ����
		{			
           AT24CXX_WriteOne(OTA_Adress,1);	//����BootLoader�ȴ�״̬			
					 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
					 USART_RX_STA=0;
					 __set_FAULTMASK(1);	//���������ж� 											
					 NVIC_SystemReset();							
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"SET_RTC"); //����RTCʱ��
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
					printf("%s\r\n", "ʱ�����ò���:");
					printf("������ʱ����:%04d-%02d-%02d %02d:%02d:%02d\r\n",sdatestructure.Year+2000,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);//
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Restart");       //��λϵͳ
		if(strx)
		{
				 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
				 USART_RX_STA=0;
				 printf("%s\r\n", "�ȴ�ϵͳ��λ!");			
					//�ȴ�ϵͳ����	                    		
				 __set_FAULTMASK(1);	//���������ж� 											
				 NVIC_SystemReset();									  							 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_ON");      //ϵͳ��ӡ����
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Print_Flag=atoi(temp1);
				 printf("Print_ON:%d 1:485���� 2:��������\r\n", Print_Flag);									   						    								  							 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_Config");    //ϵͳ���ô�ӡ
		if(strx)
		{
				 Print_ConfigFlag=1;
				 printf("\r\n%s\r\n", "ϵͳ������Ϣ��ӡ!");								    								  							 
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Print_OFF");      //ϵͳ��ӡ�ر�
		if(strx)
		{
				 Print_Flag=0;
				 printf("\r\n%s\r\n", "ϵͳ��ӡ��ϢStop!");								    								  							 
		}	
		
		strx=strstr((char*)USART_RX_BUF,(char*)"Format_EEROM");   //��ʽ���ⲿEEROM
		if(strx)
		{
				 printf("%s\r\n","EEROM��ʽ��!");	 
				 AT24CXX_WriteOne(0,0XFF); //
				 memset(USART_RX_BUF,0,USART_RX_STA&0X3FFF);
				 USART_RX_STA=0;
				 printf("%s\r\n", "�ȴ�ϵͳ��λ!");			
					//�ȴ�ϵͳ����	                    		
				 __set_FAULTMASK(1);	//���������ж� 											
				 NVIC_SystemReset();									 
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_INUM");       //CNC������ֵ
		if(strx) 
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					TIM3_CAPTURE_VAL1=atoi(temp1);
					printf("TIM3_CAPTURE_VAL1=%d\r\n",TIM3_CAPTURE_VAL1);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_UpType");     //CNC�����ϴ���ʽ
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
							bsp_StopTimer(1);//ֹͣ��ʱ
					}
					else if(CNC_UpType==2)
					{
							bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */	
						  bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */	
					}
					AT24CXX_WriteOne(CNC_UpType_Adress,CNC_UpType); //0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�
					printf("CNC_UpType=%d 0:���ϱ� 1:�仯�ϱ� 2:��ʱ�ϱ�\r\n",CNC_UpType);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_UpTime"); //CNC������ʱ�ϴ�ʱ�䵥λ��
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_UpTime=atoi(temp1);
			    if(CNC_UpTime<5)
					{
					    CNC_UpTime=5;
					}
			    memcpy(databuf,&CNC_UpTime,2);                     //1:��λ��
			    AT24CXX_Write(CNC_UpTime_Adress,databuf,2);	
		    	bsp_StartAutoTimer(1, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */						
					bsp_StartAutoTimer(7, CNC_UpTime*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ����ʱ�ϱ����� */						   
					printf("CNC_UpTime=%d��\r\n",CNC_UpTime);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_YELLOW"); //CNC�Ƶ�״̬����
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_YELLOW=atoi(temp1);	
					if(CNC_YELLOW>=2){						
						CNC_YELLOW=2;
					}									
					AT24CXX_WriteOne(CNC_YELLOW_Adress,CNC_YELLOW); //0:�������� 1:��˸����								   
					printf("CNC_Yellow=%d 0:�������� 1:��˸����  2:�������\r\n",CNC_YELLOW);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_GREEN"); //CNC�̵�״̬����
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_GREEN=atoi(temp1);	
					if(CNC_GREEN>=1){						
						CNC_GREEN=1;
					}									
					AT24CXX_WriteOne(CNC_GREEN_Adress,CNC_GREEN); //0:�������� 1:��˸����								   
					printf("CNC_GREEN=%d 0:�������� 1:��˸����\r\n",CNC_GREEN);	 									
		} 
		
		strx=strstr((char*)USART_RX_BUF,(char*)"CNC_RED");   //CNC���״̬����
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					CNC_RED=atoi(temp1);
					if(CNC_RED>=1){						
						CNC_RED=1;
					}												
					AT24CXX_WriteOne(CNC_RED_Adress,CNC_RED);       //0:�������� 1:��˸���� 								   
					printf("CNC_RED=%d 0:�������� 1:��˸����\r\n",CNC_RED);	 									
		} 	
		
		strx=strstr((char*)USART_RX_BUF,(char*)"RS485Type");        //����485������������ 0:�� 1:MODBUS��վ 2:����Э��
		if(strx)
		{
				 sscanf(strx,"%[^=]=%s",temp,temp1);
				 Device485_type=atoi(temp1);				           									
				 AT24CXX_WriteOne(RS485_Adresss,Device485_type);        //д��EEROM����			
			   printf("RS485��������Ϊ:%d 0:�� 1:MODBUS��վ 2:MODBUS��վ 3:GWHT 4:GWMT\r\n", Device485_type);					  								    								  							 
		}			
		
		strx=strstr((char*)USART_RX_BUF,(char*)"OUT1");            //����OUT1����
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
		
		strx=strstr((char*)USART_RX_BUF,(char*)"OUT2");            //����OUT2����
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

    strx=strstr((char*)USART_RX_BUF,(char*)"AD1_Type");         //AD1����
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_Type=atoi(temp1);
			    AT24CXX_WriteOne(AD1_Type_Adress,AD1_Type);            //д��EEROM����			
			    printf("AD1_Type=%d 0:�� 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD1_Type);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"AD2_Type");          //AD2����
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_Type=atoi(temp1);
			    AT24CXX_WriteOne(AD2_Type_Adress,AD2_Type);            //д��EEROM����	
					printf("AD2_Type=%d 0:�� 1:4-20ma 2:0-5V 3:0-10V 4:0-1V 5:0-20ma 6:0-2.5V\r\n",AD2_Type);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD1_PhyValue1"); //AD1��������Χ1
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_PhyValue1=atof(temp1);
			    memcpy(databuf,&AD1_PhyValue1,4);                 //AD1��������Χ1
			    AT24CXX_Write(AD1_PhyValue1_Adress,databuf,4);			   
					printf("AD1_PhyValue1=%f\r\n",AD1_PhyValue1);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD1_PhyValue2");//AD1��������Χ2
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD1_PhyValue2=atof(temp1);
			    memcpy(databuf,&AD1_PhyValue2,4);                 //AD1��������Χ2
			    AT24CXX_Write(AD1_PhyValue2_Adress,databuf,4);	
					printf("AD1_PhyValue2=%f\r\n",AD1_PhyValue2);	 									
		}
		
		strx=strstr((char*)USART_RX_BUF,(char*)"AD2_PhyValue1"); //AD2��������Χ1
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_PhyValue1=atof(temp1);
			    memcpy(databuf,&AD2_PhyValue1,4);                  //AD2��������Χ1
			    AT24CXX_Write(AD2_PhyValue1_Adress,databuf,4);
					printf("AD2_PhyValue1=%f\r\n",AD2_PhyValue1);	 									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"AD2_PhyValue2"); //AD2��������Χ2
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					AD2_PhyValue2=atof(temp1);
			    memcpy(databuf,&AD2_PhyValue2,4);                  //AD2��������Χ2
			    AT24CXX_Write(AD2_PhyValue2_Adress,databuf,4);
					printf("AD2_PhyValue2=%f\r\n",AD2_PhyValue2);	 									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Slave_Num");     //��վѯ�ʴӻ�����
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
			    AT24CXX_WriteOne(Slave_Num_Adress,Slave_Num);       //д��EEROM����			
			    printf("Slave_Num=%d\r\n",Slave_Num);	     									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Master_ADDR");    //��վѯ���׵�ַ
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					Master_ADDR=atoi(temp1);
          if(Master_ADDR<1)
					{
					    Master_ADDR=1;
					}						
					AT24CXX_WriteOne(Master_ADDR_Adress,Master_ADDR);   //д��EEROM����											
					printf("Master_ADDR=%d\r\n",Master_ADDR);	    									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"StartAddr");      //��ʼ��ַ
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					StartAddr=atoi(temp1);			   
					memcpy(databuf,&StartAddr,2);                        //��ʼ��ַ
					AT24CXX_Write(StartAddr_Adress,databuf,2);																		
					printf("StartAddr=%04x\r\n",StartAddr);	    									
		}		
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ValueOrLenth");    //����or����д����Ĵ��������������ĶԱ�ֵ
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					ValueOrLenth=atoi(temp1);			   
					if(ValueOrLenth>20)
					{
							ValueOrLenth=20;
					}
					memcpy(databuf,&ValueOrLenth,2);                     //����or����д����Ĵ��������������ĶԱ�ֵ
					AT24CXX_Write(ValueOrLenth_Adress,databuf,2);																		
					printf("ValueOrLenth=%04x\r\n",ValueOrLenth);    									
		}

    strx=strstr((char*)USART_RX_BUF,(char*)"Slave_ADDR");     //��Ϊ�ӻ�ʱID��
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					Slave_ADDR=atoi(temp1);	
          if(Slave_ADDR<1)
          {
					    Slave_ADDR=1;
					}						
					AT24CXX_WriteOne(Slave_ADDR_Adress,Slave_ADDR);          //д��EEROM����											
					printf("Slave_ADDR=%d\r\n",Slave_ADDR);	   									
		}		
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusBaudRate");      //Modbus���ڲ�����
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
					AT24CXX_WriteOne(ModbusBaudRate_Adress,ModbusBaudRate);   //д��EEROM����											
					printf("ModbusBaudRate=%d\r\n",ModbusBaudRate);	   									
		}			
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusWordLength");    //Modbus��������λ
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
					AT24CXX_WriteOne(ModbusWordLength_Adress,ModbusWordLength);  //д��EEROM����											
					printf("ModbusWordLength=%d\r\n",ModbusWordLength);   									
		}	

    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusStopBits");          //Modbus����ֹͣλ
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
					AT24CXX_WriteOne(ModbusStopBits_Adress,ModbusStopBits);      //д��EEROM����											
					printf("ModbusStopBits=%d\r\n",ModbusStopBits);	  									
		}	
		
    strx=strstr((char*)USART_RX_BUF,(char*)"ModbusParity");            //Modbus����У��λ
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
					AT24CXX_WriteOne(ModbusParity_Adress,ModbusParity);          //д��EEROM����											
					printf("ModbusParity=%d\r\n",ModbusParity);	  									
		}
		
    strx=strstr((char*)USART_RX_BUF,(char*)"Pulse_Interval");          //������
		if(strx)
		{
					sscanf(strx,"%[^=]=%s",temp,temp1);	
					TIM3_Pulse_Threshold=atoi(temp1);          
          if(TIM3_Pulse_Threshold>4000)
          {
				      TIM3_Pulse_Threshold=40000;	
					}					
				  memcpy(databuf,&TIM3_Pulse_Threshold,2);                   //���岶׽�������½���֮��ʱ�䵥λms
          AT24CXX_Write(TIM3_Pulse_Threshold_Adress,databuf,2);									
					printf("Pulse_Interval=%d\r\n",TIM3_Pulse_Threshold);	  									
		}
				
	  strx=strstr((char*)USART_RX_BUF,(char*)"URL");    //����URL
		if(strx)			
		{		 strx1=strstr((char*)strx,(char*)"="); //
			   if(strx1)
				 {
				     sscanf(strx1+1,"%s",IAPUrlUserBinPath);
						 sprintf(IAPStructValue.Url,"%s",IAPUrlUserBinPath);					
			       printf("URL:%s\r\n", IAPUrlUserBinPath);      
				 }				       									
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"FileSize");//����FileSize��С
		if(strx)			
		{		 
			  sscanf(strx,"%[^=]=%s",temp,temp1);
			  FileSize=atoi(temp1);	
			  printf("FileSize:%d\r\n",FileSize); 			
		}
		
	  strx=strstr((char*)USART_RX_BUF,(char*)"Update");//����UpdateFlage
		if(strx)			
		{		 
			  sscanf(strx,"%[^=]=%s",temp,temp1);
			  UpdateFlage=atoi(temp1);	
			  printf("UpdateFlage:%d\r\n",UpdateFlage); 			
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"Server_IP"); //���÷�����IP��ַ
		if(strx)			
		{		 
			   strx1=strstr((char*)strx,(char*)"="); 
			   if(strx1){
				    sscanf(strx1+1,"%s",IP);			
				    printf("Server_IP:%s\r\n", IP);
            SetServerIP((char*)IP);						 
				 }	
		}
		strx=strstr((char*)USART_RX_BUF,(char*)"Server_Port"); //���÷�����IP�˿�
		if(strx)			
		{		 
		     strx1=strstr((char*)strx,(char*)"="); // 
			   if(strx1){
				    sscanf(strx1+1,"%s",Port);			
				    printf("Server_Port:%s\r\n", Port);
					  SetServerPort((char*)Port);           	 
				 }
		}

		strx=strstr((char*)USART_RX_BUF,(char*)"UserName"); //����MQTT�ͻ���UserName
		if(strx)			
		{		 
		     strx1=strstr((char*)strx,(char*)"="); // 
			   if(strx1){
				    sscanf(strx1+1,"%s",MQTTUserName);			
				    printf("UserName:%s\r\n", MQTTUserName);
					  SetUserName((char*)MQTTUserName);           	 
				 }
		}

		strx=strstr((char*)USART_RX_BUF,(char*)"PassWord"); //����MQTT�ͻ���PassWord
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
