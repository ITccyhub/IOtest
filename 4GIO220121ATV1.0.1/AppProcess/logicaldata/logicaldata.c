#include "logicaldata.h"

extern uint16_t   TIM3CH2_CAPTURE_STA1;	    //输入捕获状态	
extern uint16_t   TIM3CH2_CAPTURE_VAL1;     //输入捕获值(TIM2/TIM5是32位)
uint16_t  TIM3_Pulse_Threshold=0;          //脉冲捕捉上升和下降沿之间时间单位ms
void Count_Save(void)                              //定时保存计数值
{
			static uint32_t TIM3_CAPTURE_VAL1temp=0;     //定时器3通道1计数	   
	    static  uint8_t TIM3_VAL1_WriteFlag=0;
	    uint8_t databuf[30];
			if(TIM3_CAPTURE_VAL1temp!=TIM3_CAPTURE_VAL1)//计数变化3S保存EEPROM
			{
					bsp_StartTimer(3, 1000);	              /* 启动1个1000ms的自动重装的定时器 */				
					TIM3_VAL1_WriteFlag=1;
					TIM3_CAPTURE_VAL1temp=TIM3_CAPTURE_VAL1;
			}
			if((bsp_CheckTimer(3)==1)&&(TIM3_VAL1_WriteFlag==1)){
				
				 memcpy(databuf,&TIM3_CAPTURE_VAL1,4);           //TIM3_CAPTURE_VAL1计数值清零
				 AT24CXX_Write(TIM3_CAPTURE_VAL1_Adress,databuf,4);				
				 TIM3_VAL1_WriteFlag=0;						 
		 }
}

void Io_Collect(void)
{
     if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_SET)
		 {
		     IN1=1;
		 }
		 else
		 {
			   IN1=0;
		 }
		 
		 if(HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin)==GPIO_PIN_SET)
		 {
		     IN2=1;
		 }
		 else
		 {
			   IN2=0;
		 }
		 
		 if(HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin)==GPIO_PIN_SET)
		 {
		     IN3=1;
		 }
		 else
		 {
			   IN3=0;
		 }
		 
		 if(HAL_GPIO_ReadPin(DTU_Work_GPIO_Port, DTU_Work_Pin)==GPIO_PIN_SET)
		 {
		     DTU_Heart=1;
		 }
		 else
		 {
			   DTU_Heart=0;
		 }
		 
}


void Count_Work(void)
{
	 static uint8_t RED0=0;//0:常亮故障步骤
	 static uint8_t RED1=0;//1:闪烁故障步骤
	
	 static uint8_t GREEN0=0;//0:常亮运行步骤
	 static uint8_t GREEN1=0;//1:闪烁运行步骤
	
	 static uint8_t CNC_YELLOW0=0;//0:常亮结束步骤
	 static uint8_t CNC_YELLOW1=0;//1:闪烁结束步骤
	
	 static uint8_t IN1_Temp=0;   //IN1输入状态缓存
	 static uint8_t IN2_Temp=0;
	 static uint8_t IN3_Temp=0;
	 
	 static uint8_t IN1_ChTime=0;//IN1变化次数
	 static uint8_t IN2_ChTime=0;//IN1变化次数
	 static uint8_t IN3_ChTime=0;//IN1变化次数
	 uint32_t temp=0; 
/*********************************************************/
/***********红灯运行模式**********************************/		
    if(CNC_RED==0){//0:常亮故障
			
			switch(RED0){
				
				case 0:
					     if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(4, 5*1000);	    /* 启动1个1000ms的自动重装的定时器 */		
							     RED0=1;								 
							 }
               else if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_RESET)
               {								   
							     RED_Value=0;//没输入
							 }								 
				 break;
				
				case 1:
					     if(bsp_CheckTimer(4)){
								 
								   RED0=2;	
							 }
							 else if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_RESET)
							 {							 
							     RED0=0;				
							 }				 
					
				 break;
				
				case 2:
					     if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_SET)
							 {
								   RED_Value=1;//有输入
							 }
							 else if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_RESET)
							 {							 
							     RED_Value=0;//没输入
							 }
							  RED0=0;		
				 break;
							 
				default:
					      RED0=0;
				  break;				
			}
			
		}
		else if(CNC_RED==1){//1:闪烁故障
			
			switch(RED1){
				
				case 0:
					     if(IN1_Temp!=IN1)
							 {
							    if(IN1_ChTime++>=8)//判断4次变化
									{
										 IN1_ChTime=0;
										 RED_Value=1;//有输入										 
									   RED1=1;
									}
									IN1_Temp=IN1;
							 }		     
					
         break;
							 
				case 1:
					       bsp_StartTimer(4, 10*1000);	    /* 启动1个1000ms的自动重装的定时器 */
				         RED1=2;
				 break;			 
							 
				case 2://常灭10S钟算结束从新开始判断
					       if(bsp_CheckTimer(4)){
									 
									  RED1=0;
									  RED_Value=0;//无输入				
								 }
								 else if(IN1==1)
                 {								 
								    RED1=1;
								 } 								 
				 break;		

        default:
					       RED1=0;
          break;				
			}
			
		}
/*********************************************************/
/***********绿灯运行模式**********************************/		
		if(CNC_GREEN==0){//0:常亮运行 
			
			switch(GREEN0){
				
				case 0:
					     if(IN2==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(5, 5*1000);	    /* 启动1个1000ms的自动重装的定时器 */		
							     GREEN0=1;								 
							 }
               else if(IN2==GPIO_PIN_RESET)
               {
							     GREEN_Value=0;//没输入
							 }	
				 break;
				
				case 1:
					     if(bsp_CheckTimer(5)){
								 
								    GREEN0=2;	
							 }
							 else if(IN2==GPIO_PIN_RESET)
							 {							 
							      GREEN0=0;				
							 }				 
				 break;
				
				case 2:
					     if(IN2==GPIO_PIN_SET)
							 {
								   GREEN_Value=1;//有输入
							 }
							 else if(IN2==GPIO_PIN_RESET)
							 {							 
							     GREEN_Value=0;//没输入
							 }
							  GREEN0=0;			
				 break;				
				
			 default:
				        GREEN0=0;		
				 break;
				
			}
		}
		else if(CNC_GREEN==1){//1:闪烁运行
			
			switch(GREEN1){
				
				case 0:
					     if(IN2_Temp!=IN2)
							 {
							    if(IN2_ChTime++>=8)//判断4次变化
									{
										 IN2_ChTime=0;
										 GREEN_Value=1;//有输入										 
									   GREEN1=1;
									}
									IN2_Temp=IN2;
							 }	
				 break;
				
			  case 1:
					       bsp_StartTimer(5, 10*1000);	    /* 启动1个1000ms的自动重装的定时器 */
				         GREEN1=2;
				 break;
				
				case 2:
					       if(bsp_CheckTimer(5)){
									 
									  GREEN1=0;
									  GREEN_Value=0;//无输入				
								 }
								 else if(IN2==1)
                 {								 
								    GREEN1=1;
								 } 
				 break;
				
				
			 default:
				          GREEN1=0;
				 break;
				
			}
			
		}
/*********************************************************/
/***********黄灯运行模式**********************************/	
	 if(CNC_YELLOW==0){//0:常亮结束
		 
			switch(CNC_YELLOW0){
				
			  case 0:
					     if(IN3==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(6, 5*1000);	    /* 启动1个1000ms的自动重装的定时器 */		
							     CNC_YELLOW0=1;								 
							 }
              
				 break;
							 
			  case 1:
					     if(bsp_CheckTimer(6)){
								 
								    CNC_YELLOW0=2;	
							 }
							 else if(IN3==GPIO_PIN_RESET)
							 {							 
							      CNC_YELLOW0=0;				
							 }
				 break;
				
				case 2:
								if(IN3==GPIO_PIN_SET)
								{
									TIM3_CAPTURE_VAL1++;									
								}
         				CNC_YELLOW0=3;				
							 
				 break;

				case 3:
								if(IN3==GPIO_PIN_RESET)
								{
								    CNC_YELLOW0=0;
								}
							 
				 break;								
			 default:
				         CNC_YELLOW0=0;	
				 break;
				
			}
			
		}
		else if(CNC_YELLOW==1){//1:闪烁结束
			
			switch(CNC_YELLOW1){
				
				case 0:
					     if(IN3_Temp!=IN3)
							 {
							    if(IN3_ChTime++>=8)//判断4次变化
									{
										 IN3_ChTime=0;
										 TIM3_CAPTURE_VAL1++;//有输入										 
									   CNC_YELLOW1=1;
									}
									IN3_Temp=IN3;
							 }
				 break;
			
			 case 1:
				       bsp_StartTimer(6, 10*1000);	    /* 启动1个1000ms的自动重装的定时器 */
				       CNC_YELLOW1=2;
				break;
			 
			 case 2:
				      	 if(bsp_CheckTimer(6)){
									 
									  CNC_YELLOW1=0;									 	
								 }
								 else if(IN3==1)
                 {								 
								     CNC_YELLOW1=1;
								 }				 
				break;
				
			 default:
				        CNC_YELLOW1=0;
				 break;
				
			}
			
		}
		else if(CNC_YELLOW==2){// 2:脉冲计数
			
			if(TIM3CH2_CAPTURE_STA1&0X80)//成功捕获到了一次上升沿
			{						
				   temp=TIM3CH2_CAPTURE_STA1&0X3F;
					 temp*=65536;//溢出时间总和
					 temp+=TIM3CH2_CAPTURE_VAL1;//得到总的高电平时间
				   if(TIM3_Pulse_Threshold<=temp/1000)
					 {
					     TIM3_CAPTURE_VAL1++;
					 }
					 //printf("HIGH:%d us\r\n",temp);//打印总的高点平时间
					 TIM3CH2_CAPTURE_STA1=0;//开启下一次捕获
			}
			
		}			
		
}

void AD_CAL(void)
{
	 static float AD1_Voltage;//AD1芯片实际采样电压
	 static float AD2_Voltage;//AD2芯片实际采样电压
	 static uint16_t Sampling_R1=500;//采样电阻1
	 static uint16_t Sampling_R2=500;//采样电阻2
	 static uint16_t Voltage_Ratio1=4;//分压比1
	 static uint16_t Voltage_Ratio2=4;//分压比2
	 static float Voltage_Compensation=0;   //0点补偿
	 static float Voltage_CompensationH=0;  //满量程补偿
	 if(AD1_Type==1){//4-20ma  硬件换算电压1-5V 分压之后为0.25V-1.25V 250欧采样电阻
		 
		 AD1_Voltage=(float)(2.5*adc1/65535);	
     if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
     {
		    AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		 }
		 if(AD1_Voltage<=Voltage_CompensationH)
		 {
			  AD1_Voltage=AD1_Voltage-Voltage_Compensation;	
		 }			 
		 AD1_Value=AD1_Voltage*Voltage_Ratio1*1000/Sampling_R1;//分压为3比1所以乘以4采样250欧电阻换算电流
		 if((AD1_Value>=20)&&(AD1_Value<=21))
		 {
		    AD1_Value=20;
		 }
     else if((AD1_Value>=3)&(AD1_Value<=4))//避免出现超范围数据如果超范围有故障
     {
		    AD1_Value=4;
		 }			 
     AD1_PhyValue=(AD1_Value-4)/(20-4)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }		
   else if(AD1_Type==2){//0-5V  分压为0-1.25V	 
		 		 
		  AD1_Voltage=(float)(2.5*adc1/65535);	
      if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
		  AD1_Value=AD1_Voltage*Voltage_Ratio1;//分压为3比1所以乘以4
		 	if(AD1_Value>=5)
		  {
		     AD1_Value=5;
		  }
		  AD1_PhyValue=(AD1_Value-0)/(5-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==3){//0-10V  分压为0-2.5V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);	
      if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
			AD1_Value=AD1_Voltage*Voltage_Ratio1;//分压为3比1所以乘以4
			if(AD1_Value>=10)
			{
				 AD1_Value=10;
			}
			AD1_PhyValue=(AD1_Value-0)/(10-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==4){//0-1V  分压为0-0.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);
      if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
			AD1_Value=AD1_Voltage*Voltage_Ratio1;//分压为3比1所以乘以4
			if(AD1_Value>=1)
			{
				 AD1_Value=1;
			}
			AD1_PhyValue=(AD1_Value-0)/(1-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==5){//0-20ma  分压为0-1.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);
      if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
			AD1_Value=AD1_Voltage*Voltage_Ratio1*1000/Sampling_R1;
			if(AD1_Value>=20)
			{
				 AD1_Value=20;
			}
			AD1_PhyValue=(AD1_Value-0)/(20-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==6){//0-2.5V  分压为0-1.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);	
		  if(AD1_Voltage>Voltage_CompensationH)	//误差补偿
		  {
				AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}					
				AD1_Value=AD1_Voltage*Voltage_Ratio1;//分压为3比1所以乘以4
			if(AD1_Value>=2.5)
			{
				 AD1_Value=2.5;
			}
			AD1_PhyValue=(AD1_Value-0)/(2.5-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }	 
	 else if(AD1_Type==0){//不用工程值
		 
		  AD1_Value=0;
		  AD1_PhyValue=0;
	 }
	 
	 if(AD2_Type==1){//4-20ma  硬件换算电压1-5V 分压之后为0.25V-1.25V 250欧采样电阻
		 
		 AD2_Voltage=(float)(2.5*adc2/65535);	
		 if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		 {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		 }
		 if(AD2_Voltage<=Voltage_CompensationH)
		 {
			  AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
		 }				 
		 AD2_Value=AD2_Voltage*Voltage_Ratio2*1000/Sampling_R2;//分压为3比1所以乘以4采样250欧电阻换算电流
		 if((AD2_Value>=20)&&(AD2_Value<=21))
		 {
		    AD2_Value=20;
		 }
     else if((AD2_Value>=3)&&(AD2_Value<=4))
     {
		    AD2_Value=4;
		 }			 
     AD2_PhyValue=(AD2_Value-4)/(20-4)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;		 
	 }		
   else if(AD2_Type==2){//0-5V  分压为0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		  {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//分压为3比1所以乘以4
		  if(AD2_Value>=5)
		  {
		     AD2_Value=5;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(5-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==3){//0-10V  分压为0-2.5V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		  {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//分压为3比1所以乘以4
		  if(AD2_Value>=10)
		  {
		     AD2_Value=10;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(10-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==4){//0-1V  分压为0-0.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//分压为3比1所以乘以4
		  if(AD2_Value>=1)
		  {
		     AD2_Value=1;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(1-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==5){//0-20ma  分压为0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2*1000/Sampling_R2;//分压为3比1所以乘以4采样250欧电阻换算电流
		  if(AD2_Value>=20)
		  {
		     AD2_Value=20;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(20-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==6){//0-2.5V  分压为0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);	
		  if(AD2_Voltage>Voltage_CompensationH)	//误差补偿
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}				
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//分压为3比1所以乘以4
		  if(AD2_Value>=2.5)
		  {
		     AD2_Value=2.5;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(2.5-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
	 else if(AD2_Type==0){//不用工程值
		 
		  AD2_Value=0;
		  AD2_PhyValue=0;
	 }
}
