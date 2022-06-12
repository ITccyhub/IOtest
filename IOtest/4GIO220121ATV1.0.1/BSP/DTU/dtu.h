#ifndef __DTU_H__
#define __DTU_H__
#include "uart4data.h"
//////////////////////////////////////////////////////////////////////////////////
#define Air724_Enable       1

typedef struct
{
    uint32_t timeout; /*ָ��ȴ���ʱʱ�䣬��λ��100ms*/
    char *read_cmd;   /*��ѯ����ָ��      ��ο�DTU AT�û��ֲ�˵��������д*/
	  char *ask_cmd;    /*���ز���ָ��      ��ο�DTU AT�û��ֲ�˵��������д*/
    char *write_cmd;  /*���ò���ָ��      ��ο�DTU AT�û��ֲ�˵��������д*/
} _dtu_atcmd_st;

extern char IMEINUM[30];
extern char MqttIP[30];
extern uint16_t  MqttPORT;
extern uint8_t SignValue;
uint8_t Send_cmd_to_dtu(char *cmd,char *ack,uint16_t waittime);
uint8_t* DTU_check_cmd(uint8_t *str);
uint8_t dtu_status(void);                            //4Gģ��״̬
uint8_t dtu_TCPON(uint8_t state);                    //TCP  0:�ر� 1:����
#endif





