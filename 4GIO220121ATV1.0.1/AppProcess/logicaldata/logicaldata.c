#include "logicaldata.h"

extern uint16_t   TIM3CH2_CAPTURE_STA1;	    //���벶��״̬	
extern uint16_t   TIM3CH2_CAPTURE_VAL1;     //���벶��ֵ(TIM2/TIM5��32λ)
uint16_t  TIM3_Pulse_Threshold=0;          //���岶׽�������½���֮��ʱ�䵥λms
void Count_Save(void)                              //��ʱ�������ֵ
{
			static uint32_t TIM3_CAPTURE_VAL1temp=0;     //��ʱ��3ͨ��1����	   
	    static  uint8_t TIM3_VAL1_WriteFlag=0;
	    uint8_t databuf[30];
			if(TIM3_CAPTURE_VAL1temp!=TIM3_CAPTURE_VAL1)//�����仯3S����EEPROM
			{
					bsp_StartTimer(3, 1000);	              /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */				
					TIM3_VAL1_WriteFlag=1;
					TIM3_CAPTURE_VAL1temp=TIM3_CAPTURE_VAL1;
			}
			if((bsp_CheckTimer(3)==1)&&(TIM3_VAL1_WriteFlag==1)){
				
				 memcpy(databuf,&TIM3_CAPTURE_VAL1,4);           //TIM3_CAPTURE_VAL1����ֵ����
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
	 static uint8_t RED0=0;//0:�������ϲ���
	 static uint8_t RED1=0;//1:��˸���ϲ���
	
	 static uint8_t GREEN0=0;//0:�������в���
	 static uint8_t GREEN1=0;//1:��˸���в���
	
	 static uint8_t CNC_YELLOW0=0;//0:������������
	 static uint8_t CNC_YELLOW1=0;//1:��˸��������
	
	 static uint8_t IN1_Temp=0;   //IN1����״̬����
	 static uint8_t IN2_Temp=0;
	 static uint8_t IN3_Temp=0;
	 
	 static uint8_t IN1_ChTime=0;//IN1�仯����
	 static uint8_t IN2_ChTime=0;//IN1�仯����
	 static uint8_t IN3_ChTime=0;//IN1�仯����
	 uint32_t temp=0; 
/*********************************************************/
/***********�������ģʽ**********************************/		
    if(CNC_RED==0){//0:��������
			
			switch(RED0){
				
				case 0:
					     if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(4, 5*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */		
							     RED0=1;								 
							 }
               else if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_RESET)
               {								   
							     RED_Value=0;//û����
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
								   RED_Value=1;//������
							 }
							 else if(HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)==GPIO_PIN_RESET)
							 {							 
							     RED_Value=0;//û����
							 }
							  RED0=0;		
				 break;
							 
				default:
					      RED0=0;
				  break;				
			}
			
		}
		else if(CNC_RED==1){//1:��˸����
			
			switch(RED1){
				
				case 0:
					     if(IN1_Temp!=IN1)
							 {
							    if(IN1_ChTime++>=8)//�ж�4�α仯
									{
										 IN1_ChTime=0;
										 RED_Value=1;//������										 
									   RED1=1;
									}
									IN1_Temp=IN1;
							 }		     
					
         break;
							 
				case 1:
					       bsp_StartTimer(4, 10*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */
				         RED1=2;
				 break;			 
							 
				case 2://����10S����������¿�ʼ�ж�
					       if(bsp_CheckTimer(4)){
									 
									  RED1=0;
									  RED_Value=0;//������				
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
/***********�̵�����ģʽ**********************************/		
		if(CNC_GREEN==0){//0:�������� 
			
			switch(GREEN0){
				
				case 0:
					     if(IN2==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(5, 5*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */		
							     GREEN0=1;								 
							 }
               else if(IN2==GPIO_PIN_RESET)
               {
							     GREEN_Value=0;//û����
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
								   GREEN_Value=1;//������
							 }
							 else if(IN2==GPIO_PIN_RESET)
							 {							 
							     GREEN_Value=0;//û����
							 }
							  GREEN0=0;			
				 break;				
				
			 default:
				        GREEN0=0;		
				 break;
				
			}
		}
		else if(CNC_GREEN==1){//1:��˸����
			
			switch(GREEN1){
				
				case 0:
					     if(IN2_Temp!=IN2)
							 {
							    if(IN2_ChTime++>=8)//�ж�4�α仯
									{
										 IN2_ChTime=0;
										 GREEN_Value=1;//������										 
									   GREEN1=1;
									}
									IN2_Temp=IN2;
							 }	
				 break;
				
			  case 1:
					       bsp_StartTimer(5, 10*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */
				         GREEN1=2;
				 break;
				
				case 2:
					       if(bsp_CheckTimer(5)){
									 
									  GREEN1=0;
									  GREEN_Value=0;//������				
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
/***********�Ƶ�����ģʽ**********************************/	
	 if(CNC_YELLOW==0){//0:��������
		 
			switch(CNC_YELLOW0){
				
			  case 0:
					     if(IN3==GPIO_PIN_SET)
				       {
								   bsp_StartTimer(6, 5*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */		
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
		else if(CNC_YELLOW==1){//1:��˸����
			
			switch(CNC_YELLOW1){
				
				case 0:
					     if(IN3_Temp!=IN3)
							 {
							    if(IN3_ChTime++>=8)//�ж�4�α仯
									{
										 IN3_ChTime=0;
										 TIM3_CAPTURE_VAL1++;//������										 
									   CNC_YELLOW1=1;
									}
									IN3_Temp=IN3;
							 }
				 break;
			
			 case 1:
				       bsp_StartTimer(6, 10*1000);	    /* ����1��1000ms���Զ���װ�Ķ�ʱ�� */
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
		else if(CNC_YELLOW==2){// 2:�������
			
			if(TIM3CH2_CAPTURE_STA1&0X80)//�ɹ�������һ��������
			{						
				   temp=TIM3CH2_CAPTURE_STA1&0X3F;
					 temp*=65536;//���ʱ���ܺ�
					 temp+=TIM3CH2_CAPTURE_VAL1;//�õ��ܵĸߵ�ƽʱ��
				   if(TIM3_Pulse_Threshold<=temp/1000)
					 {
					     TIM3_CAPTURE_VAL1++;
					 }
					 //printf("HIGH:%d us\r\n",temp);//��ӡ�ܵĸߵ�ƽʱ��
					 TIM3CH2_CAPTURE_STA1=0;//������һ�β���
			}
			
		}			
		
}

void AD_CAL(void)
{
	 static float AD1_Voltage;//AD1оƬʵ�ʲ�����ѹ
	 static float AD2_Voltage;//AD2оƬʵ�ʲ�����ѹ
	 static uint16_t Sampling_R1=500;//��������1
	 static uint16_t Sampling_R2=500;//��������2
	 static uint16_t Voltage_Ratio1=4;//��ѹ��1
	 static uint16_t Voltage_Ratio2=4;//��ѹ��2
	 static float Voltage_Compensation=0;   //0�㲹��
	 static float Voltage_CompensationH=0;  //�����̲���
	 if(AD1_Type==1){//4-20ma  Ӳ�������ѹ1-5V ��ѹ֮��Ϊ0.25V-1.25V 250ŷ��������
		 
		 AD1_Voltage=(float)(2.5*adc1/65535);	
     if(AD1_Voltage>Voltage_CompensationH)	//����
     {
		    AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		 }
		 if(AD1_Voltage<=Voltage_CompensationH)
		 {
			  AD1_Voltage=AD1_Voltage-Voltage_Compensation;	
		 }			 
		 AD1_Value=AD1_Voltage*Voltage_Ratio1*1000/Sampling_R1;//��ѹΪ3��1���Գ���4����250ŷ���軻�����
		 if((AD1_Value>=20)&&(AD1_Value<=21))
		 {
		    AD1_Value=20;
		 }
     else if((AD1_Value>=3)&(AD1_Value<=4))//������ֳ���Χ�����������Χ�й���
     {
		    AD1_Value=4;
		 }			 
     AD1_PhyValue=(AD1_Value-4)/(20-4)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }		
   else if(AD1_Type==2){//0-5V  ��ѹΪ0-1.25V	 
		 		 
		  AD1_Voltage=(float)(2.5*adc1/65535);	
      if(AD1_Voltage>Voltage_CompensationH)	//����
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
		  AD1_Value=AD1_Voltage*Voltage_Ratio1;//��ѹΪ3��1���Գ���4
		 	if(AD1_Value>=5)
		  {
		     AD1_Value=5;
		  }
		  AD1_PhyValue=(AD1_Value-0)/(5-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==3){//0-10V  ��ѹΪ0-2.5V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);	
      if(AD1_Voltage>Voltage_CompensationH)	//����
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
			AD1_Value=AD1_Voltage*Voltage_Ratio1;//��ѹΪ3��1���Գ���4
			if(AD1_Value>=10)
			{
				 AD1_Value=10;
			}
			AD1_PhyValue=(AD1_Value-0)/(10-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==4){//0-1V  ��ѹΪ0-0.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);
      if(AD1_Voltage>Voltage_CompensationH)	//����
      {
		     AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}				
			AD1_Value=AD1_Voltage*Voltage_Ratio1;//��ѹΪ3��1���Գ���4
			if(AD1_Value>=1)
			{
				 AD1_Value=1;
			}
			AD1_PhyValue=(AD1_Value-0)/(1-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }
	 else if(AD1_Type==5){//0-20ma  ��ѹΪ0-1.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);
      if(AD1_Voltage>Voltage_CompensationH)	//����
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
	 else if(AD1_Type==6){//0-2.5V  ��ѹΪ0-1.25V	 
			 
			AD1_Voltage=(float)(2.5*adc1/65535);	
		  if(AD1_Voltage>Voltage_CompensationH)	//����
		  {
				AD1_Voltage=AD1_Voltage+Voltage_CompensationH;	
		  }
      if(AD1_Voltage<=Voltage_CompensationH)
      {
			   AD1_Voltage=AD1_Voltage+Voltage_Compensation;	
			}					
				AD1_Value=AD1_Voltage*Voltage_Ratio1;//��ѹΪ3��1���Գ���4
			if(AD1_Value>=2.5)
			{
				 AD1_Value=2.5;
			}
			AD1_PhyValue=(AD1_Value-0)/(2.5-0)*(AD1_PhyValue2-AD1_PhyValue1)+AD1_PhyValue1;
	 }	 
	 else if(AD1_Type==0){//���ù���ֵ
		 
		  AD1_Value=0;
		  AD1_PhyValue=0;
	 }
	 
	 if(AD2_Type==1){//4-20ma  Ӳ�������ѹ1-5V ��ѹ֮��Ϊ0.25V-1.25V 250ŷ��������
		 
		 AD2_Voltage=(float)(2.5*adc2/65535);	
		 if(AD2_Voltage>Voltage_CompensationH)	//����
		 {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		 }
		 if(AD2_Voltage<=Voltage_CompensationH)
		 {
			  AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
		 }				 
		 AD2_Value=AD2_Voltage*Voltage_Ratio2*1000/Sampling_R2;//��ѹΪ3��1���Գ���4����250ŷ���軻�����
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
   else if(AD2_Type==2){//0-5V  ��ѹΪ0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//����
		  {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//��ѹΪ3��1���Գ���4
		  if(AD2_Value>=5)
		  {
		     AD2_Value=5;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(5-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==3){//0-10V  ��ѹΪ0-2.5V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//����
		  {
			  AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//��ѹΪ3��1���Գ���4
		  if(AD2_Value>=10)
		  {
		     AD2_Value=10;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(10-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==4){//0-1V  ��ѹΪ0-0.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//����
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//��ѹΪ3��1���Գ���4
		  if(AD2_Value>=1)
		  {
		     AD2_Value=1;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(1-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==5){//0-20ma  ��ѹΪ0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);
		  if(AD2_Voltage>Voltage_CompensationH)	//����
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}					
		  AD2_Value=AD2_Voltage*Voltage_Ratio2*1000/Sampling_R2;//��ѹΪ3��1���Գ���4����250ŷ���軻�����
		  if(AD2_Value>=20)
		  {
		     AD2_Value=20;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(20-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
   else if(AD2_Type==6){//0-2.5V  ��ѹΪ0-1.25V	 
		 		 
		  AD2_Voltage=(float)(2.5*adc2/65535);	
		  if(AD2_Voltage>Voltage_CompensationH)	//����
		  {
			   AD2_Voltage=AD2_Voltage+Voltage_CompensationH;	
		  }	
      if(AD2_Voltage<=Voltage_CompensationH)
      {
			   AD2_Voltage=AD2_Voltage+Voltage_Compensation;	
			}				
		  AD2_Value=AD2_Voltage*Voltage_Ratio2;//��ѹΪ3��1���Գ���4
		  if(AD2_Value>=2.5)
		  {
		     AD2_Value=2.5;
		  }
		  AD2_PhyValue=(AD2_Value-0)/(2.5-0)*(AD2_PhyValue2-AD2_PhyValue1)+AD2_PhyValue1;
	 }
	 else if(AD2_Type==0){//���ù���ֵ
		 
		  AD2_Value=0;
		  AD2_PhyValue=0;
	 }
}
