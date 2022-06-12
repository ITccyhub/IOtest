#include "uart3data.h"
#include "modbus.h"
#include "uart1data.h"
#include "cmsis_os.h"
#include "iwdg.h"
uint8_t test;

struct ModbusPack Modbus03Slave[3];
	      	

InverterHT GWHT={0};//固德威HT数据类型
InverterMT GWMT={0};//固德威MT数据类型

uint32_t ModbusPollingCnt =0;              //配置函数延时变量,定时器内部累加
uint32_t ModbusValue=5000;                 //每隔 Ms 发送一次数据
uint32_t ModbusSendNextDelay =0;           //接收SendConfigFunction函数最后一个参数,最终传递给ModbusPollingCnt控制写一条数据发送的时间
int MasterCaseValue = 0;                   //控制执行哪一条Case 语句
uint8_t MasterCnt = 0;                     //记录运行状态发送的次数
uint8_t ModbusReturnFlage  = 0;            //是否返回了预期的数据


/*
* @brief  
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
Modbus主站轮训发送命令固德威逆变器HT协议包
*/
void Master_Search_GWHT(void)
{	
	uint8_t i;
	int32_t temp;
	uint32_t U_temp;
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(ModbusReturnFlage == 1)//上一条指令是OK的
		{
			MasterCnt=0;
			ModbusReturnFlage = 0;
			MasterCaseValue ++;//执行下一条
			GWHT.Inverter_Fault=0;
		}
		else
		{
			MasterCnt ++;
			if(MasterCnt>=3)//超过3次重新执行
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
			        Master_03_Slove(&huart3,Master_ADDR,30000,25);//设备名称和设备序列号
		          bsp_StartTimer(8, 1*50);	                    /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){

								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//解析数据OK
								{
								  memcpy(&GWHT.serial_Number,Master_ReadReg,10);  //机器系列号
							    memcpy(&GWHT.model,Master_ReadReg+15,16);       //机器型号	
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
			        Master_03_Slove(&huart3,Master_ADDR,32016,48);//PV1-24电压电流 PV1/PV2为MPPT1的电压，值是相同的；PV1电流即为组串1电流 共12个MPPT电压 电流 24个组串电流
		          bsp_StartTimer(8, 1*50);	                     /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;		
              ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);		 
		          while(1){							
								
								  HAL_IWDG_Refresh(&hiwdg);   //喂狗
									MODBUSMaster_PROCESS(Master_ADDR);
									if(TX_RX_SET==0)//解析数据OK
									{	
										for(i=0;i<12;i++){
											
											GWHT.Vpv[i]=(float)Master_ReadReg[4*i]/10;               //MTTP电压									
											GWHT.Istring[2*i]=(float)Master_ReadReg[4*i+1]/100;      //组串1电流							
											GWHT.Istring[2*i+1]=(float)Master_ReadReg[4*i+3]/100;    //组串2电流								
											GWHT.Ipv[i]=GWHT.Istring[2*i]+GWHT.Istring[2*i+1];       //MTTP电流=组串1电流+组串2电流												
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
			        Master_03_Slove(&huart3,Master_ADDR,32069,13);//瞬时电量
		          bsp_StartTimer(8, 1*50);	                    /* 启动1个50ms的一次性定时器 本地定时显示*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								  HAL_IWDG_Refresh(&hiwdg);   //喂狗
									MODBUSMaster_PROCESS(Master_ADDR);

									if(TX_RX_SET==0)//解析数据OK
									{
									  GWHT.Va=Master_ReadReg[0]/10;//L1电压
									  GWHT.Vb=Master_ReadReg[1]/10;//L2电压
									  GWHT.Vc=Master_ReadReg[2]/10;//L3电压
										
										temp=(int32_t)Master_ReadReg[3];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[4];
										GWHT.Ia=(float)temp/1000;	            //L1电流										
										
										temp=(int32_t)Master_ReadReg[5];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[6];
										GWHT.Ib=(float)temp/1000;	            //L2电流	

										temp=(int32_t)Master_ReadReg[7];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[8];
										GWHT.Ic=(float)temp/1000;	            //L3电流		
										
										temp=(int32_t)Master_ReadReg[9];
										temp=temp<<16;
										temp+=(int32_t)Master_ReadReg[10];
										GWHT.P=(float)temp/1000;			        //功率						
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
			        Master_03_Slove(&huart3,Master_ADDR,32085,3);//电网频率+内部温度
		          bsp_StartTimer(8, 1*50);	                   /* 启动1个50ms的一次性定时器 本地定时显示*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//解析数据OK
								{
									GWHT.Frequency=(float)Master_ReadReg[0]/100;            //L频率
									GWHT.Temper_Inside=(float)Master_ReadReg[2]/10;         //内部温度
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
			        Master_03_Slove(&huart3,Master_ADDR,32106,10);//累加发电量和当日发电量
		          bsp_StartTimer(8, 1*50);	                   /* 启动1个50ms的一次性定时器 本地定时显示*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//解析数据OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.E_Total=(double)U_temp/100;                  //累计发电量										
									U_temp=(uint32_t)Master_ReadReg[8];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[9];
									GWHT.E_Day=(double)U_temp/100;                    //当日发电量										
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
			        Master_03_Slove(&huart3,Master_ADDR,35710,10);//DSP故障代码
		          bsp_StartTimer(8, 1*50);	                   /* 启动1个50ms的一次性定时器 本地定时显示*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);                //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//解析数据OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.Error_DSP=U_temp;                  //DSP故障

									U_temp=(uint32_t)Master_ReadReg[4];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[5];
									GWHT.Error_PV=U_temp;                   //组串故障标志

									U_temp=(uint32_t)Master_ReadReg[6];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[7];
									GWHT.Error_Temper=U_temp;               //温度故障标志
									
									U_temp=(uint32_t)Master_ReadReg[8];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[9];
									GWHT.Error_Sub_DSP=U_temp;               //副DSP故障码			
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
			        Master_03_Slove(&huart3,Master_ADDR,35749,10);//总发电时间+工作模式
		          bsp_StartTimer(8, 1*50);	                   /* 启动1个50ms的一次性定时器 本地定时显示*/	
		 		      ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);

								if(TX_RX_SET==0)//解析数据OK
								{
									U_temp=(uint32_t)Master_ReadReg[0];
									U_temp=U_temp<<16;
									U_temp+=(uint32_t)Master_ReadReg[1];
									GWHT.H_Total=U_temp;                   //总发电时间		

									GWHT.WorkMode=Master_ReadReg[9];	     //工作状态									
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
Modbus主站轮训发送命令固德威逆变器MT协议包
*/
void Master_Search_GWMT(void)
{
	int32_t temp;
	uint32_t U_temp;
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(ModbusReturnFlage == 1)//上一条指令是OK的
		{
			MasterCnt=0;
			ModbusReturnFlage = 0;
			MasterCaseValue++;//执行下一条
			GWMT.Inverter_Fault=0;
		}
		else
		{
			MasterCnt ++;
			if(MasterCnt>=3)//超过3次重新执行
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
			        Master_03_Slove(&huart3,Master_ADDR,0x300,0x20);//MPPT1电压至当日发电量
		          bsp_StartTimer(8, 1*50);	                    /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//解析数据OK
								{
									  GWMT.Vpv[0]=(float)Master_ReadReg[0]/10;//MTTP1电压
									  GWMT.Vpv[1]=(float)Master_ReadReg[1]/10;//MTTP2电压
									  GWMT.Ipv[0]=(float)Master_ReadReg[2]/10;//MTTP1电流
										GWMT.Ipv[1]=(float)Master_ReadReg[3]/10;//MTTP2电流
									  GWMT.Va=(float)Master_ReadReg[4]/10;//L1电压
									  GWMT.Vb=(float)Master_ReadReg[5]/10;//L2电压
									  GWMT.Vc=(float)Master_ReadReg[6]/10;//L3电压
									  GWMT.Ia=(float)Master_ReadReg[7]/10;//L1电流
									  GWMT.Ib=(float)Master_ReadReg[8]/10;//L2电流
									  GWMT.Ic=(float)Master_ReadReg[9]/10;//L3电流								
									  GWMT.FrequencyA=(float)Master_ReadReg[10]/100;//L1频率
			              GWMT.FrequencyB=(float)Master_ReadReg[11]/100;//L2频率
                    GWMT.FrequencyC=(float)Master_ReadReg[12]/100;//L3频率	
                    GWMT.WorkMode=Master_ReadReg[14];//工作模式
									  GWMT.Temper_Inside=(float)Master_ReadReg[15]/10;//温度									
									  U_temp=(uint32_t)Master_ReadReg[16];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[17];									
									  GWMT.Error_Message=U_temp;//故障信息									 
									  U_temp=(uint32_t)Master_ReadReg[18];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[19];
									  GWMT.E_Total=(double)U_temp/10;                //总发电量
									  U_temp=(uint32_t)Master_ReadReg[20];
									  U_temp=U_temp<<16;
									  U_temp+=(uint32_t)Master_ReadReg[21];
									  GWMT.H_Total=U_temp;                            //总发电时间								
										GWMT.E_Day=(double)Master_ReadReg[32]/10;       //当日发电量								
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
			        Master_03_Slove(&huart3,Master_ADDR,0x352,2);//功率
		          bsp_StartTimer(8, 1*50);	                    /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//解析数据OK
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
			        Master_03_Slove(&huart3,Master_ADDR,0x357,0x18);//MPPT3电压开始至组串20电流
		          bsp_StartTimer(8, 1*50);	                    /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);   //喂狗
								MODBUSMaster_PROCESS(Master_ADDR);
								if(TX_RX_SET==0)//解析数据OK
								{
                  GWMT.Vpv[2]=(float)Master_ReadReg[0]/10;//MPPT3电压							
									GWMT.Vpv[3]=(float)Master_ReadReg[1]/10;//MPPT4电压			
                  GWMT.Ipv[2]=(float)Master_ReadReg[2]/10;//MPPT3电流						
									GWMT.Ipv[3]=(float)Master_ReadReg[3]/10;//MPPT4电流
                  GWMT.Istring[0]=(float)Master_ReadReg[5]/10;//组串1电流
                  GWMT.Istring[1]=(float)Master_ReadReg[6]/10;//组串2电流
                  GWMT.Istring[2]=(float)Master_ReadReg[7]/10;//组串3电流
                  GWMT.Istring[3]=(float)Master_ReadReg[8]/10;//组串4电流
                  GWMT.Istring[4]=(float)Master_ReadReg[9]/10;//组串5电流
                  GWMT.Istring[5]=(float)Master_ReadReg[10]/10;//组串6电流
                  GWMT.Istring[6]=(float)Master_ReadReg[11]/10;//组串7电流
                  GWMT.Istring[7]=(float)Master_ReadReg[12]/10;//组串8电流
                  GWMT.Istring[8]=(float)Master_ReadReg[13]/10;//组串9电流
                  GWMT.Istring[9]=(float)Master_ReadReg[14]/10;//组串10电流
                  GWMT.Istring[10]=(float)Master_ReadReg[15]/10;//组串11电流
                  GWMT.Istring[11]=(float)Master_ReadReg[16]/10;//组串12电流
                  GWMT.Istring[12]=(float)Master_ReadReg[17]/10;//组串13电流
                  GWMT.Istring[13]=(float)Master_ReadReg[18]/10;//组串14电流
                  GWMT.Istring[14]=(float)Master_ReadReg[19]/10;//组串15电流
                  GWMT.Istring[15]=(float)Master_ReadReg[20]/10;//组串16电流
									GWMT.IsSta=Master_ReadReg[21];                //组串1-16状态
                  //GWMT.Istring[16]//无组串17电流
                  GWMT.Istring[17]=(float)Master_ReadReg[22]/10;//组串18电流	
                  GWMT.Istring[18]=(float)Master_ReadReg[23]/10;//组串19电流
                  GWMT.Istring[19]=(float)Master_ReadReg[24]/10;//组串20电流			
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

void Master_Search(uint8_t Addr)//主站轮询
{		
	if(ModbusPollingCnt>ModbusValue)
	{
		ModbusPollingCnt=0;
		if(Modbus03Slave[Addr].Modbus_ReturnFlage == 1)//上一条指令是OK的
		{
			Modbus03Slave[Addr].Modbus_MasterCnt=0;
			Modbus03Slave[Addr].Modbus_ReturnFlage = 0;
			MasterCaseValue++;//执行下一条	
      	
		}
		else
		{			
			Modbus03Slave[Addr].Modbus_MasterCnt++;
			if(Modbus03Slave[Addr].Modbus_MasterCnt>=3)//超过3次重新执行
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
		          bsp_StartTimer(8, 1*50);	            /* 启动1个50ms的一次性定时器 本地定时显示*/	
		          ModbusSendNextDelay=4000;
		          ModbusPollingCnt = ( ModbusSendNextDelay == 0 ? ModbusPollingCnt: ModbusSendNextDelay);			      
		          while(1){
								
								HAL_IWDG_Refresh(&hiwdg);         //喂狗
								MODBUSMaster_PROCESS(Addr+Master_ADDR);
								if(TX_RX_SET==0)                  //解析数据OK
								{	
                   	Modbus03Slave[Addr].Modbus_Err=0;								
							      memcpy(Modbus03Slave[Addr].MobusTemp,Master_ReadReg,ValueOrLenth*2);		
								    Modbus03Slave[Addr].Modbus_ReturnFlage=1;								
								}
								
								switch(ComErr){
									
									case 11://CRC错误					
											Modbus03Slave[Addr].Modbus_Err=2;					
									 break; 
									
									case 22://功能码错误									
											Modbus03Slave[Addr].Modbus_Err=3;									
									 break;

									case 3://接收数据超内存范围错误									
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

void RS485Work(void)//Modbus协议工作步骤
{
	  static uint8_t i=0;  

	  if(Device485_type==1){//MODUBS主站通用	
			
				  Master_Search(i);					 
					if(++i>Slave_Num-1) 
					{
						 i=0;
					}						        
		}
		else if(Device485_type==2){//MODBUS从站
		
         Modbus_RegMap();             //从站数据更新
         MODBUSSlave_PROCESS(&huart3);//Modbus从站协议解析			
		}
    else if(Device485_type==3){//固德威HT协议
			
			   Master_Search_GWHT();			
    }	
    else if(Device485_type==4){//固德威MT协议
			
			   Master_Search_GWMT();			
    }		
}
