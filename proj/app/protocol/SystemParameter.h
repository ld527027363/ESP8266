#ifndef SYSTEMPARAMETER_H
#define SYSTEMPARAMETER_H

#include "types.h"
typedef struct
{
    unsigned char  InitialFlag;  //����Ϊ 0x55
    unsigned char  WorkMode;     //����ģʽ 0x00: RS485  0x01:  LAN  0x02: WIFI
    unsigned char  Reversed1[3]; //����
    unsigned char  BeepFlag;
	unsigned char  ProductSn[18]; //��Ʒ���к� 16���ֽ� ��ʱδ��   
    unsigned char  DHCPEnable;   //DHCPʹ��   1 ��  0 �ر�  ������LAN   
	unsigned char  LocalIp[4];   //����IP  ������LAN
	unsigned char  LocalSubWay[4];   //�������� ������LAN     
	unsigned char  LocalGateway[4];  //����     ������LAN
    unsigned short LocalPort;    //����IP�˿�  ������LAN  ����UDP�㲥�������豸 ������DHCPʹ��ʱ�����豸
    unsigned char  DistinIp[4];  //Զ��IP
	unsigned short DistinPort;   //Զ�̶˿�
	unsigned char  WIFISSID[34]; //WIFI SSID  32���ֽ� ���2���ֽ���0 
    unsigned char  WIFICypt;     //WIFI���ܷ�ʽ  ��4λ��ʾ���ܸ�ʽ xxxx 0x00:OPEN  0x10: WPAPSK  0x20: WPA2PSK
                                 //              ��4λ��ʾ����Э�� 0x00: NONE 0x01: WEP 0x02:TKIP 0x03: AES
                                 //��4λ���4λ �� | �������
    unsigned char  WIFIPass[66]; //WIFI����  64���ֽ���Ч�ֽ�   ASC���� ����Ϊ0  ���2���ֽ���0
    unsigned char  RFTxChannel;
    unsigned char  RFRxChannel;
    unsigned char  Reversed2[6]; //����2    
}Struct_System_Parameters;           //?�ֽ�

void SetDefaultParameter(void);

/* �������״̬ */
#define  sys_check_linktype( linktype)\
((strSystemParameter.WorkMode == linktype)?1:0)


BOOL  system_set_pram(uint8 * dat);
unsigned char WriteSystemParameter(void);
unsigned char GetSystemParameter(unsigned char *InitialFlag);
unsigned char WriteSystemParameter(void);
unsigned char ParameterInitial(void);
void DealSysParameter(void);

void DealSysParamBlue();

void DealSysParameter(void);
void DealSysParamLan(void);

extern uint8 system_get_param(uint8 ** dat);
extern uint8 system_set_param(uint8 * paramBuf, uint32 len);

extern Struct_System_Parameters strSystemParameter;
extern unsigned char g_bStopReadFlag;

extern int bytesdec2hex(uint8 *dec,uint8 *hex,int len);

#endif


