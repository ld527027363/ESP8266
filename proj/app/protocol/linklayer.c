#include "linklayer.h"
#include "protocol_typedef.h"
#include "protocol.h"
#include "types.h"
#include <string.h>
#include "upgrade.h"
#include "memory.h"
//#include "log.h"
//#include "rtc.h"
//#include "led.h"
#include <stdio.h>
#include <stdlib.h>
//#include "sys_config.h"
//#include "calendar.h"
//#include "uart.h"
#include "wifi.h"
//#include "rs485.h"
#include "protocol_base64.h"

#include "schedule_task.h"
#include "system_paramter.h"
#include "socket_tcp_client.h"



UpgradeStruct UpgradeList;		//��������  --ld 20161222 ������������������������������


unsigned char g_arrRecvBuffer[TCP_MAX_RCV_LEN];
unsigned char g_arrSendBuffer[TCP_MAX_SEND_LEN];
//unsigned short g_iRevStartPos = 0;

SERVER_COMM_CTRL g_stServerComCtrl;
PProtocol_ST Pprotocol;

SERVER_REPLY_CODE ServerReplyCode;

unsigned char  g_bUploadOkFlag = 0;

unsigned char  g_bServerUploadReq = 0;			//�����ϴ����󣬼��ʱ�䳬��1�����������ݣ��򻺴泬��10���������ϴ�
unsigned char g_bServerUploadSucFlag = 0;			//�����ϱ��ɹ���־
unsigned short g_iServerUploadCommTimeCnt = 0;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
unsigned short g_iServerHeartCommTimeCnt = 0;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
volatile unsigned char  g_bServerHeartbeatReq = 0;
volatile unsigned char  g_bServerHeartbeatSucReq = 0;  //hcp 2015-10-19
unsigned char  g_bServerLoginReq = 0;
unsigned char  g_bServerLoginSucFlag = 0;

unsigned short g_iTotalSendRecordCnt = 0;
unsigned short g_iMaxSendRecordCnt = 0xFFFF;

unsigned char  g_bStartUpgradeFlag = 0;
unsigned char  g_bSendAgainFlag = 0;
unsigned short g_iSendLength = 0;

volatile unsigned char  g_bCommReceiveFlag = 0;

static unsigned int RepeatLoginTimeCnt = 15;			//��¼���ɹ����·���ʱ�����
static unsigned int g_iResendTimes = 0;

static int g_ttlLoginOtCnt = 0;
#define RESTART_LOGIN_TIMEOUT       10           //10min  ����  hcp

Pchar buff = 0;

enum 
{
	comm_reg,
	comm_login,
	comm_upload
}EmServerCommState;


void linksend(unsigned char* data,unsigned short datalen)
{
#if 1
	memcpy(Platform_Param.Tx_Buff+Platform_Param.Tx_Len, data, datalen);
   // g_iSendLength = g_iSendLength + datalen;
	Platform_Param.Tx_Len += datalen;
#else
    memcpy(&g_arrSendBuffer[g_iSendLength], data, datalen);
    g_iSendLength = g_iSendLength + datalen;
#endif
}

void LinkSendDataFunc(void)
{
	Schedule_SendData();
}

unsigned char* linkrecv(unsigned short *datalen)
{
	*datalen = Platform_Param.Rx_Offset;
//	printf("Platform_Param.Rx_Offset = %d\n",Platform_Param.Rx_Offset);
//	Platform_Param.Rx_Buff[Platform_Param.Rx_Offset] = 0;
//	printf("recv buf = %s\n",Platform_Param.Rx_Buff);
//	printf("linkrecv end\n");
	
	return Platform_Param.Rx_Buff;
}

void linkfree()
{
//	printf("linkfree start\n");
	memset(Platform_Param.Rx_Buff, 0, sizeof(Platform_Param.Rx_Buff));
	Platform_Param.Rx_Offset = 0;
	Platform_Param.Tx_Len = 0;
//	printf("linkfree end\n");
}

void ProtocolInitial(void)
{
	char accountID[22];
	char str[20]={0};

	memset(accountID, 0, 22);

	if(strlen((char *)strSystemParameter.ProductSn) != 16) 
		strcpy((char *)strSystemParameter.ProductSn,(char *)"0000000000000001");
	sprintf((char *)accountID, "%s%s", BOARDVersion, strSystemParameter.ProductSn);

	Sys_Delay_ms(100);
	
	RTC_GetTime();
	RTC_GetDate();
#if 1
	Pprotocol = Protocol_Builder(PT_LAN,linksend,linkrecv,linkfree);
#else
	if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		Pprotocol = Protocol_Builder(PT_LAN,linksend,linkrecv,linkfree);
	}
	else if (strSystemParameter.WorkMode == COMM_PORT_RS485)
	{
		Pprotocol = Protocol_Builder(PT_485,linksend,linkrecv,linkfree);
	}
#endif
	Pprotocol->setparam(Pprotocol,PP_ACCOUNT,accountID);
	Pprotocol->setparam(Pprotocol,PP_MAGIC,"02");
	Pprotocol->setparam(Pprotocol,PP_SESSIONID,"0");

	sprintf(str,"%d-%02d-%02d",SysDate.year,SysDate.month,SysDate.daily);
	Pprotocol->setparam(Pprotocol,PP_CREATEDATE,str);
	sprintf(str,"%02d:%02d:%02d",SysTime.hour,SysTime.minute,SysTime.second);
	Pprotocol->setparam(Pprotocol,PP_CREATETIME,str);

#if 1
	Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"wifi");
#else
	if(strSystemParameter.WorkMode == COMM_PORT_RS485)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"485");
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_LAN)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"lan");
	}
	else
		if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"wifi");
	}
#endif
#if 0
	sprintf(str, "%d.%d.%d.%d",strSystemParameter.DistinIp[0],strSystemParameter.DistinIp[1],\
							  strSystemParameter.DistinIp[2],strSystemParameter.DistinIp[3]);
#else
	memcpy(str,SOCK_TCP_CLIENT_HOST,strlen(SOCK_TCP_CLIENT_HOST));
#endif
	Pprotocol->setparam(Pprotocol,PP_HOST,str);
	sprintf(str, "%d",SOCK_TCP_CLIENT_PORT);
	Pprotocol->setparam(Pprotocol,PP_PORT,str);
	Pprotocol->setparam(Pprotocol,PP_PASSWWORD,"a4F06cKAil8=");
	Pprotocol->setparam(Pprotocol,PP_PSDMD5,"6E0C2BC650D2810B");

}

void FlushProtocalTime(void)
{
	char str[32] = {0};

	sprintf(str,"%d-%02d-%02d",SysDate.year,SysDate.month,SysDate.daily);
	Pprotocol->setparam(Pprotocol,PP_CREATEDATE,str);
	sprintf(str,"%02d:%02d:%02d",SysTime.hour,SysTime.minute,SysTime.second);
	Pprotocol->setparam(Pprotocol,PP_CREATETIME,str);
}


void LinkPrintPortRecv(void)
{
	u8_t * pDat = 0;
	u32_t  len = 0;

#if 1
	WIFI_CommTask();
#else
	if (uart_get_len())
	{
		
		pDat = uart_get_buf();
		len = uart_get_len();
		if (strSystemParameter.WorkMode == COMM_PORT_RS485 || g_bDebug_state)
		{	
					if (g_bDebug_state)
					{
						Uart_DebugSendData(pDat, len);
					}
					else
					{
						uart_19200_handle(pDat, len);
					}
					
//					Uart_DebugSendData(pDat, len);
					rs485_irq_handle(pDat, len);
					
					uart_recv_reset();
		}
//		else if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
//		{
//			WIFI_RecvDataHandle(pDat);
//		}
			
	}
	
	
	if(DebugUartPort.rcv_all_flag)
	{
		DebugUartPort.rcv_len=0;
		DebugUartPort.rcv_all_flag=0; //�������
		DebugUartPort.rcv_state=0;

		DealSysParameter();		
	}
	
	if (strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		WIFI_CommTask();
	}
	else if (strSystemParameter.WorkMode == COMM_PORT_RS485)
	{
		rs485_CommTask();
	}
	
	if (sys_get_buf() != 0)
	{
		DealSysParamBlue(sys_get_buf());
	}
#endif
}

void ServerCommStateInit(void)
{
	EmServerCommState = comm_login;
	Platform_Param.Comm_status = Tx_Status;
	g_stServerComCtrl.tx_flag= 0;
	Platform_Param.Rx_Status = Resp_Null;
	g_bServerHeartbeatReq = 0;
	g_bServerUploadReq = 0;
	g_bServerLoginReq = 0; 
	g_bServerLoginSucFlag = 0 ;
	g_bServerUploadSucFlag = 0;
	g_bSendAgainFlag = 0;
	g_bUploadOkFlag = 0;
}


void InitServerValue(void)
{
	g_bServerHeartbeatReq = 0; //����������
	g_iServerHeartCommTimeCnt = 0;
	g_bServerUploadReq = 0;
	g_iServerUploadCommTimeCnt = 0;
}

uint8_t Get_String_Num_Len(char * data)
{
	uint8_t len = 0;
	
	while(1)
	{
		if((data[len] >= '0')&&(data[len] <= '9'))
			len++;
		else
			break;
	}
	return len;
}

/************************************************************************************************* 
* ��������	: ���ݽ��մ���                                                         
* ����			:  ��                                                            
*                               
* ����			:                                                              
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void Server_RcvDataHandle(void)
{
	uint8_t *para_data;
	uint8_t i = 0,len = 0;
	char prtf_buf[32] = {0};
	char *pTemp = NULL, *pData = NULL, *pTime = NULL, *pParam = NULL, *pParamver = NULL;
	int  iReturn = 0;

	/*���޸� ����������Ҫ���ж�����PP_ERRORCODE��Ȼ�����ж�PP_RESULTCODE*/
	pTemp = Pprotocol->getparam(Pprotocol,PP_ERRORCODE);
	iReturn = atoi(pTemp);
//	printf("pTemp = %s \n",pTemp);	
	if(iReturn == 0x01) 	//��������
	{
		Platform_Param.Rx_Status = Resp_False;
		Linklay_Printf("Resp Error Relogin \r\n");
		ServerCommStateInit();	//���¿�ʼͨ������
		return;
	}
	pTemp = Pprotocol->getparam(Pprotocol,PP_RESULTCODE);		//���ؿ�?????????
//	printf("pTemp = %s \n",pTemp);	
	if(pTemp != NULL)
	{
		ServerReplyCode.err_code = atoi(pTemp);  
		iReturn = atoi(pTemp);
	}
	else
	{
		ServerReplyCode.err_code = 0;  
		iReturn = 0;
	}
//	printf("pTemp = %s \n",pTemp);	
	if(ServerReplyCode.err_code == 99) 		//���ݰ�δ������
		return;
	
	if(ServerReplyCode.err_code != 0)			//���ݰ����ؽ������
	{
		Linklay_Printf("server rep err!\r\n");
		g_bServerLoginSucFlag = 0;
		Platform_Param.Rx_Status = Resp_False;
		return;
	}
//	printf("get pdata ptime\n");
	pData = Pprotocol->getparam(Pprotocol,PP_CREATEDATE);
	pTime = Pprotocol->getparam(Pprotocol,PP_CREATETIME);
		
//	printf("pData = %s \n pTime = %s\n",pData,pTime);	
	Sys_SetDateAndTime(pData,pTime);		//�ǲ���ÿ�ζ��������������������--20160120
	printf("Sys_SetDateAndTime\n");
	if(EmServerCommState == comm_login) 
	{
		g_bServerLoginSucFlag = 1;
		Platform_Param.Rx_Status = Resp_Success;
		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		return;
	}
	if(g_bServerHeartbeatReq)
	{
		g_bServerHeartbeatReq = 0; //����������
		g_iServerHeartCommTimeCnt = 0;

		Linklay_Printf("Heartbeet respons.\r\n");
		len = 0;
		pParamver = Pprotocol->getparam(Pprotocol,PP_APPARAMVER);
		if(pParamver != NULL)
		{
			if(*pParamver >= '0')
			{
				len = strlen(pParamver);
				if(len <= 15)
				{
					if(memcmp(&strSystemParameter.Param_Ver[0],pParamver,len))
					{
						memset(strSystemParameter.Param_Ver,0,16);
						memcpy(strSystemParameter.Param_Ver,pParamver,len+1);
						Linklay_Printf(pParamver);
						Linklay_Printf("\r\n");
					}
					else
					{
						Linklay_Printf("PararmVer Same!\r\n");
					}
				}
			}
			else
			{
				Linklay_Printf("PararmVer Err!\r\n");
			}
		}
		else
		{
			Linklay_Printf("No PararmVer!\r\n");
		}
		
		if(len)
		{
			pParam = Pprotocol->getparam(Pprotocol,PP_APPARAM);
			if(pParam != NULL)
			{
				i = 0;
				para_data = protocol_base64_decode((uint8_t *)pParam);
				while(para_data[i] == 0xF0)
				{
					if(i == 0)
						strSystemParameter.HraerBeatTimeout = ((para_data[i+4]<<8) + para_data[i+5])*60+1;
					else if(i == 6)
						strSystemParameter.SendDataDealyTime = ((para_data[i+4]<<8) + para_data[i+5])*60+1;
					else if(i == 12)
						strSystemParameter.SendDataMaxCnt = (para_data[i+4]<<8) + para_data[i+5];
					else
						break;
					sprintf(prtf_buf,"%02X %02X %02X %02X %02X %02X\r\n",
									para_data[i+0],para_data[i+1],para_data[i+2],para_data[i+3],para_data[i+4],para_data[i+5]);
					Linklay_Printf(prtf_buf);
					i += 6;
				}
				if(i > 0)
					i = 0xAA;
				if(para_data)
				free(para_data);
				para_data = NULL;
			}
		}
		if(i == 0xAA)
		{
			Linklay_Printf("Save PararmVer!\r\n");
			WriteSystemParameter();
		}
		pTemp = Pprotocol->getparam(Pprotocol,PP_SOFTVERSION);
		
		pTemp = Pprotocol->getparam(Pprotocol,PP_URL);
		
		if(pTemp != NULL) 
		{		
			strcpy((char *)UpgradeList.arrUrl, pTemp); 
			pTemp = Pprotocol->getparam(Pprotocol,PP_SOFTVERSION);
			UpgradeList.bVersion = atoi(pTemp); 
		
			if(MYVERSION != UpgradeList.bVersion             //hcp 2015-10-24 ��ǿ�°汾�ж�����
				 &&UpgradeList.bVersion != 0)
			{
				g_bStartUpgradeFlag = 1;
				//Upgrade_ShiftFW((char *)&UpgradeList.bVersion,1);		//ԭ�汾��û�õ� --ld 20161222
			}
		}
		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		Platform_Param.Rx_Status = Resp_Success;
		g_bServerHeartbeatSucReq = 1;
		g_iServerHeartCommTimeCnt = 0;
		g_bSendAgainFlag = 0;
		return;
	}
	if(g_bServerUploadReq)
	{
		//��� flash ������ temp flash���е�ͬ��У�鲢д��־ hcp 2015-10-12
		if (TagRecordFlashQueue.tail_addr != FlashTempQueue.bk_tail_addr
		 || TagRecordFlashQueue.cnt < FlashTempQueue.bk_cnt
		 || TagRecordFlashQueue.cnt > TagRecordFlashQueue.max_cnt)
		{
			//����ع� ���������ع�
			Linklay_Printf("roll back \r\n");
		}
		else
		{
			TagRecordFlashQueue.tail_addr = FlashTempQueue.tail_addr;
			TagRecordFlashQueue.cnt -= g_iTotalSendRecordCnt;
		}
		
		FlashQueue_SaveParam(&TagRecordFlashQueue);		//liding 20160122
		
		WiFi_SendDataFlag = 0;
		os_printf("ServerUploadReq send data ok cnt =%d\r\n",g_iTotalSendRecordCnt);

		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		Platform_Param.Rx_Status = Resp_Success;
		g_bServerUploadSucFlag = 1;
		g_iMaxSendRecordCnt = 0xFFFF;
		g_iTotalSendRecordCnt = 0;
		return;
	}
}

void ChangeDataFormat(unsigned char *pDest, unsigned char *pSrc)
{
	memcpy(pDest, pSrc, 4);
	Sys_GetRecordDate(&pDest[4], &pSrc[7]);
	pDest[6] = pSrc[4];
	Sys_GetRecordTime(&pDest[7], &pSrc[7]);
	pDest[9] = pSrc[5];
	pDest[10] = pSrc[6];
}

/************************************************************************************************* 
* ��������	: ����ͨ�����̿���                                                        
* ����			:  ��                                                            
*                               
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 

#define SERVER_COMM_LOGIN_REPEAT   10 //20 SECC

static unsigned char strSrcBuf[32];

void Server_CommTask(void)	
{
	unsigned char i = 0;
	unsigned char *strSrc = (unsigned char *)((((int)strSrcBuf+3)>>2)<< 2);
	unsigned char strDst[16];
	unsigned char strTemp[48];
	unsigned short iRet = 0,iFlashQueueRet = 0; 
	int  serverCommTimeout = SERVER_COMM_TIMEOUT;
	int temp = 0;
	unsigned char   bTagDatOk = 0;

#if 0
	if(WifiConnectFlag == 0)
	{
		if(EmServerCommState == comm_upload)
		{
			Linklay_Printf("\r\n Net error,Repair data, Buf tag cnt: \r\n");
		}
		ServerCommStateInit();
		
		/////hcp 2015-10-30
		temp = RESTART_LOGIN_TIMEOUT*15*60/10;
		if (g_ttlLoginOtCnt++ >= temp)  // ���س�ʱ����
		{
			g_ttlLoginOtCnt = 0;
			g_reset_flag = 1;
		}
		return;
	}
#endif
	RepeatLoginTimeCnt++;
	switch(EmServerCommState)
	{
		case comm_login://��¼
			//Linklay_Printf("Platform_Param.Comm_status %d\n",Platform_Param.Comm_status);
			if(Platform_Param.Comm_status == Tx_Status)		//����״̬,Uart2BusyFlag�����ĵ����ͳ�����ȥ�ж�
			{
				if(RepeatLoginTimeCnt > SERVER_COMM_LOGIN_REPEAT)
				{
					RepeatLoginTimeCnt = 0;
					Pprotocol->clear(Pprotocol);
					linkfree();
					FlushProtocalTime();			//��ʼ��ʱ��
					Pprotocol->login(Pprotocol); 	//��װ����
					LinkSendDataFunc();				//���͵�¼����
					Linklay_Printf("Login...\r\n");
					Platform_Param.Comm_status = Rx_Status;
					g_stServerComCtrl.timeout_cnt = 0;
				}
				break;
			}
			else											 //����״̬
			{
				if(Platform_Param.Rx_Status == Resp_Success)		
				{
					Platform_Param.Rx_Status = Resp_Null;
					if(g_bServerLoginSucFlag)			//��¼�ɹ�
					{
						EmServerCommState = comm_upload;		//�������ݽ����׶�		
						Linklay_Printf("Login Server sucess\r\n");
						g_ttlLoginOtCnt = 0;
					}
					else
					{
						Linklay_Printf("Login error\r\n");
					}
					Platform_Param.Comm_status = Tx_Status;
					RepeatLoginTimeCnt = 0;
					InitServerValue();
					break;
				}
				else
				{
					g_stServerComCtrl.timeout_cnt++;
					if(g_stServerComCtrl.timeout_cnt >= serverCommTimeout)		//��ʱ20��
					{
						Linklay_Printf("Login timeout\r\n");
						RepeatLoginTimeCnt = 0;
						Platform_Param.Comm_status = Tx_Status;
						InitServerValue();
#if 0
						 WIFI_Reset();						//����ǲ��Ϸ�����������WIFI����û����gaojun 2015-07-23
#endif
						/////hcp 2015-10-30
						temp = RESTART_LOGIN_TIMEOUT*15/10;

						 if (g_ttlLoginOtCnt++ >= RESTART_LOGIN_TIMEOUT)  // ���س�ʱ����
						{
							g_ttlLoginOtCnt = 0;
							g_reset_flag = 1;
						}
					}		
				}
			}				 
			break;
		case comm_upload:
			if(Platform_Param.Comm_status == Tx_Status)						//����״̬
			{
				printf("g_bServerHeartbeatReq = %d\n",g_bServerHeartbeatReq);
				printf("g_bServerUploadReq = %d\n",g_bServerUploadReq);
				
				 if(g_bServerHeartbeatReq)	//����������
				{
					if(g_bSendAgainFlag == 0)	
					{
						Pprotocol->clear(Pprotocol);
						Pprotocol->setparam(Pprotocol,PP_MODEL,BOARDVersion);
						sprintf((char *)strTemp, "%d", MYVERSION);
						Pprotocol->setparam(Pprotocol,PP_SOFTVERSION,(char *)strTemp); 
						Pprotocol->setparam(Pprotocol,PP_APPARAMVER,strSystemParameter.Param_Ver); //��Ӳ����汾 --LD 20160729 
						linkfree();
						FlushProtocalTime();
						Pprotocol->heartpack(Pprotocol);
						LinkSendDataFunc();
						Platform_Param.Comm_status = Rx_Status;
						g_stServerComCtrl.timeout_cnt = 0;	
						g_bServerHeartbeatSucReq = 0;
						Linklay_Printf(">>Heartbeat...\r\n");
						g_iResendTimes = 3;
					}
					else
					{
						if (!g_iResendTimes)    // ���޸� 2015-07-09 �������ط��������ƣ�����ط�����3����Ϊ�Ͽ�������
						{
								g_bSendAgainFlag = 0;
								ServerCommStateInit();			//���¿�ʼ��¼
								break;
						}
							linkfree();
							FlushProtocalTime();
							Pprotocol->resend(Pprotocol); 
							LinkSendDataFunc();
							g_iResendTimes--;
							Linklay_Printf(">>Resend... \r\n");
							Platform_Param.Comm_status = Rx_Status;
							g_stServerComCtrl.timeout_cnt = 0;	
						
					}
				}
				else if(g_bServerUploadReq)		//���ݰ�����
				{
					if(g_bSendAgainFlag == 0)		//���ݰ��״�
					{
						if(TagRecordFlashQueue.cnt == 0)	//����Ϊ��		//liding 20160122
						{
							g_bServerUploadReq = 0;
							break;
						}
						  
						g_iTotalSendRecordCnt = 0;
				
						//printf("Pprotocol->clear(Pprotocol)\n");
						Pprotocol->clear(Pprotocol);
						iFlashQueueRet = 1;
						
						//printf("FlashQueue_Copy(&FlashTempQueue,&TagRecordFlashQueue)\n");
						FlashQueue_Copy(&FlashTempQueue,&TagRecordFlashQueue);
						while(iFlashQueueRet)	
						{
							iFlashQueueRet = FlashQueue_GetTagData((FLASH_QUEUE *)&FlashTempQueue,strSrc);		//liding 20160122
							if(iFlashQueueRet == 0) 
							{
								break;  //û��������
							}
							
							if((strSrc[1] == 0xFF)&&(strSrc[2] == 0xFF)&&(strSrc[3] == 0xFF)&&(strSrc[4] == 0xFF))							
							{
								FlashQueue_FinTagGet((FLASH_QUEUE *)&FlashTempQueue);  //2016-01-26 �������������������������������
								g_iTotalSendRecordCnt++;
//								break;	//���ݴ���						gaojun   2015-09-11
							}
							else
							{
								
									//��֤flash ����  2016-1-20 hcp
								bTagDatOk = (strSrc[15]  == CheckSum(strSrc,15)?1:0);
								if (!bTagDatOk)
								{
//									Linklay_Printf("flash error:%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\r\n",
//									strSrc[0],strSrc[1],strSrc[2],strSrc[3],
//									strSrc[4],strSrc[5],strSrc[6],strSrc[7],
//									strSrc[8],strSrc[9],strSrc[10],strSrc[11]);
									//#ifdef EN_USING_LOG
//									sprintf(LogStrBuf,"flash error addr:%08x\r\n",FlashTempQueue.tail_addr);
//									AddLog(LogStrBuf);
//									//#endif
//									Linklay_Printf(LogStrBuf);
									
								}
								ChangeDataFormat(strDst, &strSrc[1]);
								iRet = Pprotocol->puttag(Pprotocol,PT_DEVICE,strDst,11);
								if(iRet == 0)
								{
									FlashQueue_FinTagGet((FLASH_QUEUE *)&FlashTempQueue);  //hcp 2015-10-21 �ɹ�ȡ�������޸�tail
									g_iTotalSendRecordCnt++;
								}
								else 
								{
									break;
								}
							}
						}
						
						if (!g_iTotalSendRecordCnt)	//  ���޸� 2015-07-09 ��������Ч���ݵ��жϣ��������������Ϊ��ʱ�������з���
						{
							break;
						}
						Platform_Param.Tx_Len = 0;
						linkfree();
						FlushProtocalTime();
						Pprotocol->syncbatch(Pprotocol); 						//�������� ��������
						LinkSendDataFunc();
						g_iResendTimes = 3;		//�ط�������ʼ��
						WiFi_SendDataFlag = 1;
						Linklay_Printf("send data\r\n");
					}
					else												//���ݰ��ط�
					{
					    
						if (!g_iResendTimes)    // ���޸� 2015-07-09 �������ط��������ƣ�����ط�����3����Ϊ�Ͽ�������
						{
							g_bSendAgainFlag = 0;
							ServerCommStateInit();			//���¿�ʼ��¼
							Linklay_Printf(" Send failed \r\n");
							break;
						}
						linkfree();
						FlushProtocalTime();
						Pprotocol->resend(Pprotocol); 
						LinkSendDataFunc();
						g_iResendTimes--;
						Linklay_Printf("Resend Data \r\n");
					}
					Platform_Param.Comm_status = Rx_Status;
					g_stServerComCtrl.timeout_cnt = 0;		
				}
				break;
			}
			else
			{
				if(Platform_Param.Rx_Status == Resp_Success)
				{
					Platform_Param.Rx_Status = Resp_Null;
					Platform_Param.Comm_status = Tx_Status;
					g_iServerUploadCommTimeCnt = 0;
					if(g_bServerUploadSucFlag)			//�����ϱ��ɹ�
					{
						g_bServerUploadSucFlag = 0;
						g_bSendAgainFlag = 0;
						g_bServerUploadReq = 0;
					}
					else if(g_bServerHeartbeatSucReq)
					{
						g_bServerHeartbeatSucReq = 0;
					}
					else
					{
						Linklay_Printf("Data Upload error.\r\n");
						g_bSendAgainFlag = 1;				//���������ط�
					}
				}
				else
				{
					g_stServerComCtrl.timeout_cnt++;
					if(g_stServerComCtrl.timeout_cnt >= serverCommTimeout)		//��ʱ30 ��
					{
						Platform_Param.Comm_status = Tx_Status;
						Linklay_Printf("Comm time out\r\n");
						g_bSendAgainFlag = 1;				//���������ط�

					}
				}
			}
			break;

		default:
			break;
	}

}
