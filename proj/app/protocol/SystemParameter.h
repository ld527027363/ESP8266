#ifndef SYSTEMPARAMETER_H
#define SYSTEMPARAMETER_H

#include "types.h"
typedef struct
{
    unsigned char  InitialFlag;  //必须为 0x55
    unsigned char  WorkMode;     //工作模式 0x00: RS485  0x01:  LAN  0x02: WIFI
    unsigned char  Reversed1[3]; //保留
    unsigned char  BeepFlag;
	unsigned char  ProductSn[18]; //产品序列号 16个字节 暂时未用   
    unsigned char  DHCPEnable;   //DHCP使能   1 打开  0 关闭  适用于LAN   
	unsigned char  LocalIp[4];   //本地IP  适用于LAN
	unsigned char  LocalSubWay[4];   //子网掩码 适用于LAN     
	unsigned char  LocalGateway[4];  //网关     适用于LAN
    unsigned short LocalPort;    //本地IP端口  适用于LAN  用于UDP广播搜索本设备 适用于DHCP使能时查找设备
    unsigned char  DistinIp[4];  //远程IP
	unsigned short DistinPort;   //远程端口
	unsigned char  WIFISSID[34]; //WIFI SSID  32个字节 最后2个字节置0 
    unsigned char  WIFICypt;     //WIFI加密方式  高4位表示加密格式 xxxx 0x00:OPEN  0x10: WPAPSK  0x20: WPA2PSK
                                 //              低4位表示加密协议 0x00: NONE 0x01: WEP 0x02:TKIP 0x03: AES
                                 //高4位与低4位 用 | 进行组合
    unsigned char  WIFIPass[66]; //WIFI密码  64个字节有效字节   ASC明文 不能为0  最后2个字节置0
    unsigned char  RFTxChannel;
    unsigned char  RFRxChannel;
    unsigned char  Reversed2[6]; //保留2    
}Struct_System_Parameters;           //?字节

void SetDefaultParameter(void);

/* 检测链接状态 */
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


