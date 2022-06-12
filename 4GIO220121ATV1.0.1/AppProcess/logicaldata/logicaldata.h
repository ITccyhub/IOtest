#ifndef _LOGICALDATA_H
#define _LOGICALDATA_H
#include "uart1data.h"
#include "gpio.h"

extern uint16_t  TIM3_Pulse_Threshold;         //脉冲捕捉上升和下降沿之间时间单位ms
void Io_Collect(void);
void Count_Save(void);
void Count_Work(void);
void AD_CAL(void);


#endif
