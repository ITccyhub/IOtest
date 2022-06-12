#ifndef __DTU_H__
#define __DTU_H__
#include "uart4data.h"
//////////////////////////////////////////////////////////////////////////////////
#define Air724_Enable       1

typedef struct
{
    uint32_t timeout; /*指令等待超时时间，单位：100ms*/
    char *read_cmd;   /*查询参数指令      请参考DTU AT用户手册说明进行填写*/
	  char *ask_cmd;    /*返回参数指令      请参考DTU AT用户手册说明进行填写*/
    char *write_cmd;  /*配置参数指令      请参考DTU AT用户手册说明进行填写*/
} _dtu_atcmd_st;

extern char IMEINUM[30];
extern char MqttIP[30];
extern uint16_t  MqttPORT;
extern uint8_t SignValue;
uint8_t Send_cmd_to_dtu(char *cmd,char *ack,uint16_t waittime);
uint8_t* DTU_check_cmd(uint8_t *str);
uint8_t dtu_status(void);                            //4G模块状态
uint8_t dtu_TCPON(uint8_t state);                    //TCP  0:关闭 1:开启
#endif





