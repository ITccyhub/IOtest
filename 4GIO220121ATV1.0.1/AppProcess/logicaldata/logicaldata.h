#ifndef _LOGICALDATA_H
#define _LOGICALDATA_H
#include "uart1data.h"
#include "gpio.h"

extern uint16_t  TIM3_Pulse_Threshold;         //���岶׽�������½���֮��ʱ�䵥λms
void Io_Collect(void);
void Count_Save(void);
void Count_Work(void);
void AD_CAL(void);


#endif
