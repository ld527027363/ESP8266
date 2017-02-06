#ifndef __LINKLAYER_H
#define __LINKLAYER_H

#include "protocol_typedef.h"
#include "types.h"
#include "system_paramter.h"
//#include "nrf_delay.h"



#define TEST_REQ_RF_TX			(1<<0)
#define TEST_REQ_PRINT_RSSI	(1<<1)
#define TEST_REQ_PRINT_COMM	(1<<2)
#define TEST_MEM_PRINT_COMM	(1<<3)
#define TEST_MEM_PRINT_TAGS	(1<<4)



extern unsigned char DHCP_Req ;			//DHCP 申请
extern unsigned char TestReq ;


//#define DEBUG_Printf //printf


void ProtocolInitial(void);

void LinkPrintPortRecv(void);

void ServerCommStateInit(void);
void Server_CommTask(void);
void Server_RcvDataHandle(void);

extern unsigned char g_arrRecvBuffer[];
extern unsigned char g_arrSendBuffer[];
//extern unsigned short g_iRevStartPos;

extern PProtocol_ST Pprotocol;
extern SERVER_COMM_CTRL g_stServerComCtrl;

extern unsigned char  g_bServerUploadReq;			//数据上传请求，间隔时间超过1分钟且有数据，或缓存超过10条将触发上传
extern unsigned short g_iServerUploadCommTimeCnt;		//数据发送间隔计时，1秒钟加1。
extern unsigned short g_iServerHeartCommTimeCnt;		//数据发送间隔计时，1秒钟加1。
extern volatile  unsigned char  g_bServerHeartbeatReq;
extern volatile unsigned char  g_bServerHeartbeatSucReq;

extern unsigned char  g_bServerLoginSucFlag;

extern unsigned char  g_bStartUpgradeFlag;

extern SERVER_REPLY_CODE ServerReplyCode;
volatile extern unsigned char  g_bCommReceiveFlag;

extern const unsigned char g_arrHttp500Error[];
extern const unsigned char g_arrHttp400Error[];
extern const unsigned char g_arrHttp20[];


extern unsigned char  g_bUploadOkFlag;

#endif

