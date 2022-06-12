#include "uart3data.h"
#include "modbus.h"
#include "uart1data.h"
#include "cmsis_os.h"
#include "iwdg.h"
uint8_t test;

struct ModbusPack Modbus03Slave[3];
	      	

InverterHT GWHT={0};//�̵���HT��������
InverterMT GWMT={0};//�̵���MT��������

uint32_t ModbusPollingCnt =0;              //���ú�����ʱ����,��ʱ���ڲ��ۼ�
uint32_t ModbusValue=5000;                 //ÿ�� Ms ����һ������
uint32_t ModbusSendNextDelay =0;           //����SendConfigFunction�������һ������,���մ��ݸ�ModbusPollingCnt����дһ�����ݷ��͵�ʱ��
int MasterCaseValue = 0;                   //����ִ����һ��Case ���
uint8_t MasterCnt = 0;                     //��¼����״̬���͵Ĵ���
uint8_t ModbusReturnFlage  = 0;            //�Ƿ񷵻���Ԥ�ڵ�����


/*
* @brief  
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
Modbus��վ��ѵ��������̵��������HTЭ���
*/
void Master_Search_GWHT(void)
{	
	uint8_t i;
	int32_t temp;
	uint32_t U_temp;
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(ModbusReturnFlage == 1)//��һ��ָ����OK��
		{
			MasterCnt=0;
			ModbusReturnFlage = 0;
			MasterCaseValue ++;//ִ����һ��
			GWHT.Inverter_Fault=0;
		}
		else
		{
			MasterCnt ++;
			if(MasterCnt>=3)//����3������ִ��
			{
				MasterCnt=0;
				MasterCaseValue = 0;
        GWHT.Inverter_Fault=1;				
			}
		}
	}
	switch(MasterCaseValue)
	{
	   case 0://
			        Master_03_Slove(&huart3,Master_ADDR,30000,25);//�豸���ƺ��豸���к�
		          bsp_StartTimer(8, 1*50);	                    /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){

								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//��������OK
								{
								  memcpy(&GWHT.serial_Number,Master_ReadReg,10);  //����ϵ�к�
							    memcpy(&GWHT.model,Master_ReadReg+15,16);       //�����ͺ�	
									ModbusReturnFlage=1;								
								}
	             	osDelay(20);						
								if(ModbusPollingCnt>=ModbusValue)
								{
	                  break;
								}							   
						  }							
		  break;
							
	   case 1://
			        Master_03_Slove(&huart3,Master_ADDR,32016,48);//PV1-24��ѹ���� PV1/PV2ΪMPPT1�ĵ�ѹ��ֵ����ͬ�ģ�PV1������Ϊ�鴮1���� ��12��MPPT��ѹ ���� 24���鴮����
		          bsp_StartTimer(8, 1*50);	                     /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;		
              ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);		 
		          while(1){							
								
								  HAL_IWDG_Refresh(&hiwdg);   //ι��
									MODBUSMaster_PROCESS(Master_ADDR);
									if(TX_RX_SET==0)//��������OK
									{	
										for(i=0;i<12;i++){
											
											GWHT.Vpv[i]=(float)Master_ReadReg[4*i]/10;               //MTTP��ѹ									
											GWHT.Istring[2*i]=(float)Master_ReadReg[4*i+1]/100;      //�鴮1����							
											GWHT.Istring[2*i+1]=(float)Master_ReadReg[4*i+3]/100;    //�鴮2����								
											GWHT.Ipv[i]=GWHT.Istring[2*i]+GWHT.Istring[2*i+1];       //MTTP����=�鴮1����+�鴮2����												
										}										
										ModbusReturnFlage=1;										
									}
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}	
								osDelay(20);	
							}
	 
		  break;
							
	   case 2://
			        Master_03_Slove(&huart3,Master_ADDR,32069,13);//˲ʱ����
		          bsp_StartTimer(8, 1*50);	                    /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								  HAL_IWDG_Refresh(&hiwdg);   //ι��
									MODBUSMaster_PROCESS(Master_ADDR);

									if(TX_RX_SET==0)//��������OK
									{
									  GWHT.Va=Master_ReadReg[0]/10;//L1��ѹ
									  GWHT.Vb=Master_ReadReg[1]/10;//L2��ѹ
									  GWHT.Vc=Master_ReadReg[2]/10;//L3��ѹ
										
										temp=(int32_t)Master_ReadReg[3];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[4];
										GWHT.Ia=(float)temp/1000;	            //L1����										
										
										temp=(int32_t)Master_ReadReg[5];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[6];
										GWHT.Ib=(float)temp/1000;	            //L2����	

										temp=(int32_t)Master_ReadReg[7];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[8];
										GWHT.Ic=(float)temp/1000;	            //L3����		
										
										temp=(int32_t)Master_ReadReg[9];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[10];
										GWHT.P=(float)temp/1000;			        //����						
										ModbusReturnFlage=1;									
									}	
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}										
                osDelay(20);	
						  }							
		  break;
							
	   case 3://
			        Master_03_Slove(&huart3,Master_ADDR,32085,3);//����Ƶ��+�ڲ��¶�
		          bsp_StartTimer(8, 1*50);	                   /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//��������OK
								{
									GWHT.Frequency=(float)Master_ReadReg[0]/100;            //LƵ��
									GWHT.Temper_Inside=(float)Master_ReadReg[2]/10;         //�ڲ��¶�
									ModbusReturnFlage=1;									
								}	
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}										
                osDelay(20);	
						  }							
		  break;

	   case 4://
			        Master_03_Slove(&huart3,Master_ADDR,32106,10);//�ۼӷ������͵��շ�����
		          bsp_StartTimer(8, 1*50);	                   /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//��������OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.E_Total=(double)U_temp/100;                  //�ۼƷ�����										
									U_temp=(uint32_t)Master_ReadReg[8];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[9];
									GWHT.E_Day=(double)U_temp/100;                    //���շ�����										
									ModbusReturnFlage=1;									
								}	
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}										
                osDelay(20);	
						  }							
		  break;


							
	   case 5://
			        Master_03_Slove(&huart3,Master_ADDR,35710,10);//DSP���ϴ���
		          bsp_StartTimer(8, 1*50);	                   /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);                //ι��
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//��������OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.Error_DSP=U_temp;                  //DSP����

									U_temp=(uint32_t)Master_ReadReg[4];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[5];
									GWHT.Error_PV=U_temp;                   //�鴮���ϱ�־

									U_temp=(uint32_t)Master_ReadReg[6];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[7];
									GWHT.Error_Temper=U_temp;               //�¶ȹ��ϱ�־
									
									U_temp=(uint32_t)Master_ReadReg[8];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[9];
									GWHT.Error_Sub_DSP=U_temp;               //��DSP������			
									ModbusReturnFlage=1;									
								}	
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}										
                osDelay(20);	
						  }							
		  break;
							
	   case 6://
			        Master_03_Slove(&huart3,Master_ADDR,35749,10);//�ܷ���ʱ��+����ģʽ
		          bsp_StartTimer(8, 1*50);	                   /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//��������OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.H_Total=U_temp;                   //�ܷ���ʱ��		

									GWHT.WorkMode=Master_ReadReg[9];	     //����״̬									
									ModbusReturnFlage=1;									
								}	
								
								if(ModbusPollingCnt>ModbusValue )
								{
	                  break;
								}										
                osDelay(20);	
						  }							
		  break;
							
		 default:	 MasterCnt=0;
				       MasterCaseValue = 0;
			 break;		
	}	
	
}
/*
* @brief  
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
Modbus��վ��ѵ��������̵��������MTЭ���
*/
void Master_Search_GWMT(void)
{
	int32_t temp;
	uint32_t U_temp;
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(ModbusReturnFlage == 1)//��һ��ָ����OK��
		{
			MasterCnt=0;
			ModbusReturnFlage = 0;
			MasterCaseValue++;//ִ����һ��
			GWMT.Inverter_Fault=0;
		}
		else
		{
			MasterCnt ++;
			if(MasterCnt>=3)//����3������ִ��
			{
				MasterCnt=0;
				MasterCaseValue = 0;
        GWMT.Inverter_Fault=1;				
			}
		}
	}	
	switch(MasterCaseValue)
	{							
	   case 0://
			        Master_03_Slove(&huart3,Master_ADDR,0x300,0x20);//MPPT1��ѹ�����շ�����
		          bsp_StartTimer(8, 1*50);	                    /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//��������OK
								{
									  GWMT.Vpv[0]=(float)Master_ReadReg[0]/10;//MTTP1��ѹ
									  GWMT.Vpv[1]=(float)Master_ReadReg[1]/10;//MTTP2��ѹ
									  GWMT.Ipv[0]=(float)Master_ReadReg[2]/10;//MTTP1����
										GWMT.Ipv[1]=(float)Master_ReadReg[3]/10;//MTTP2����
									  GWMT.Va=(float)Master_ReadReg[4]/10;//L1��ѹ
									  GWMT.Vb=(float)Master_ReadReg[5]/10;//L2��ѹ
									  GWMT.Vc=(float)Master_ReadReg[6]/10;//L3��ѹ
									  GWMT.Ia=(float)Master_ReadReg[7]/10;//L1����
									  GWMT.Ib=(float)Master_ReadReg[8]/10;//L2����
									  GWMT.Ic=(float)Master_ReadReg[9]/10;//L3����								
									  GWMT.FrequencyA=(float)Master_ReadReg[10]/100;//L1Ƶ��
			              GWMT.FrequencyB=(float)Master_ReadReg[11]/100;//L2Ƶ��
                    GWMT.FrequencyC=(float)Master_ReadReg[12]/100;//L3Ƶ��	
                    GWMT.WorkMode=Master_ReadReg[14];//����ģʽ
									  GWMT.Temper_Inside=(float)Master_ReadReg[15]/10;//�¶�									
									  U_temp=(uint32_t)Master_ReadReg[16];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[17];									
									  GWMT.Error_Message=U_temp;//������Ϣ									 
									  U_temp=(uint32_t)Master_ReadReg[18];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[19];
									  GWMT.E_Total=(double)U_temp/10;                //�ܷ�����
									  U_temp=(uint32_t)Master_ReadReg[20];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[21];
									  GWMT.H_Total=U_temp;                            //�ܷ���ʱ��								
										GWMT.E_Day=(double)Master_ReadReg[32]/10;       //���շ�����								
								    ModbusReturnFlage=1;								
								}
	             	osDelay(20);						
								if(ModbusPollingCnt>=ModbusValue)
								{
	                  break;
								}	
						   
						  }							
		  break;

			case 1://
			        Master_03_Slove(&huart3,Master_ADDR,0x352,2);//����
		          bsp_StartTimer(8, 1*50);	                    /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//��������OK
								{
									temp=(int32_t)Master_ReadReg[0];
									temp=temp<<16;
									temp+=(int32_t)Master_ReadReg[1];
                  GWMT.P=(float)temp/1000;                  
									ModbusReturnFlage=1;								
								}
	             	osDelay(20);						
								if(ModbusPollingCnt>=ModbusValue)
								{
	                  break;
								}	
						   
						  }							
		  break;

			case 2://
			        Master_03_Slove(&huart3,Master_ADDR,0x357,0x18);//MPPT3��ѹ��ʼ���鴮20����
		          bsp_StartTimer(8, 1*50);	                    /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //ι��
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//��������OK
								{
                  GWMT.Vpv[2]=(float)Master_ReadReg[0]/10;//MPPT3��ѹ							
									GWMT.Vpv[3]=(float)Master_ReadReg[1]/10;//MPPT4��ѹ			
                  GWMT.Ipv[2]=(float)Master_ReadReg[2]/10;//MPPT3����						
									GWMT.Ipv[3]=(float)Master_ReadReg[3]/10;//MPPT4����
                  GWMT.Istring[0]=(float)Master_ReadReg[5]/10;//�鴮1����
                  GWMT.Istring[1]=(float)Master_ReadReg[6]/10;//�鴮2����
                  GWMT.Istring[2]=(float)Master_ReadReg[7]/10;//�鴮3����
                  GWMT.Istring[3]=(float)Master_ReadReg[8]/10;//�鴮4����
                  GWMT.Istring[4]=(float)Master_ReadReg[9]/10;//�鴮5����
                  GWMT.Istring[5]=(float)Master_ReadReg[10]/10;//�鴮6����
                  GWMT.Istring[6]=(float)Master_ReadReg[11]/10;//�鴮7����
                  GWMT.Istring[7]=(float)Master_ReadReg[12]/10;//�鴮8����
                  GWMT.Istring[8]=(float)Master_ReadReg[13]/10;//�鴮9����
                  GWMT.Istring[9]=(float)Master_ReadReg[14]/10;//�鴮10����
                  GWMT.Istring[10]=(float)Master_ReadReg[15]/10;//�鴮11����
                  GWMT.Istring[11]=(float)Master_ReadReg[16]/10;//�鴮12����
                  GWMT.Istring[12]=(float)Master_ReadReg[17]/10;//�鴮13����
                  GWMT.Istring[13]=(float)Master_ReadReg[18]/10;//�鴮14����
                  GWMT.Istring[14]=(float)Master_ReadReg[19]/10;//�鴮15����
                  GWMT.Istring[15]=(float)Master_ReadReg[20]/10;//�鴮16����
									GWMT.IsSta=Master_ReadReg[21];                //�鴮1-16״̬
                  //GWMT.Istring[16]//���鴮17����
                  GWMT.Istring[17]=(float)Master_ReadReg[22]/10;//�鴮18����	
                  GWMT.Istring[18]=(float)Master_ReadReg[23]/10;//�鴮19����
                  GWMT.Istring[19]=(float)Master_ReadReg[24]/10;//�鴮20����			
									ModbusReturnFlage=1;								
								}
	             	osDelay(20);						
								if(ModbusPollingCnt>=ModbusValue)
								{
	                  break;
								}	
						   
						  }							
		  break;
							
		 default:
			 				 MasterCnt=0;
				       MasterCaseValue = 0;
       break;		
	}	
}

void Master_Search(uint8_t Addr)//��վ��ѯ
{		
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(Modbus03Slave[Addr].Modbus_ReturnFlage == 1)//��һ��ָ����OK��
		{
			Modbus03Slave[Addr].Modbus_MasterCnt=0;
			Modbus03Slave[Addr].Modbus_ReturnFlage = 0;
			MasterCaseValue++;//ִ����һ��	
      	
		}
		else
		{			
			Modbus03Slave[Addr].Modbus_MasterCnt++;
			if(Modbus03Slave[Addr].Modbus_MasterCnt>=3)//����3������ִ��
			{
				Modbus03Slave[Addr].Modbus_MasterCnt=0;
				MasterCaseValue=0;
				Modbus03Slave[Addr].Modbus_Err=1;	      		
			}
		}
	}	
	switch(MasterCaseValue)
	{							
		   case 0:               						 
				      Master_03_Slove(&huart3,Addr+Master_ADDR,StartAddr,ValueOrLenth);			       
		          bsp_StartTimer(8, 1*50);	            /* ����1��50ms��һ���Զ�ʱ�� ���ض�ʱ��ʾ*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);			      
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);         //ι��
								MODBUSMaster_PROCESS(Addr+Master_ADDR);
								if(TX_RX_SET==0)                  //��������OK
								{	
                   	Modbus03Slave[Addr].Modbus_Err=0;								
							      memcpy(Modbus03Slave[Addr].MobusTemp,Master_ReadReg,ValueOrLenth*2);		
								    Modbus03Slave[Addr].Modbus_ReturnFlage=1;								
								}
								
								switch(ComErr){
									
									case 11://CRC����					
											Modbus03Slave[Addr].Modbus_Err=2;					
									 break; 
									
									case 22://���������									
											Modbus03Slave[Addr].Modbus_Err=3;									
									 break;

									case 3://�������ݳ��ڴ淶Χ����									
											Modbus03Slave[Addr].Modbus_Err=4;							
									 break;
									
									default:
										   ComErr=0xff;
									 break;
								}
								
	             	osDelay(20);						
								if(ModbusPollingCnt>=ModbusValue)
								{	   
									 break;
								}						   
						  }		
				break;
							
			 default:
			 				 Modbus03Slave[Addr].Modbus_MasterCnt=0;
				       MasterCaseValue = 0;
				 break;
	}  
}

void RS485Work(void)//ModbusЭ�鹤������
{
	  static uint8_t i=0;  

	  if(Device485_type==1){//MODUBS��վͨ��	
			
				  Master_Search(i);					 
					if(++i>Slave_Num-1) 
					{
						 i=0;
					}						        
		}
		else if(Device485_type==2){//MODBUS��վ
		
         Modbus_RegMap();             //��վ���ݸ���
         MODBUSSlave_PROCESS(&huart3);//Modbus��վЭ�����			
		}
    else if(Device485_type==3){//�̵���HTЭ��
			
			   Master_Search_GWHT();			
    }	
    else if(Device485_type==4){//�̵���MTЭ��
			
			   Master_Search_GWMT();			
    }		
}
