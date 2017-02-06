/*
 * platform.c
 *
 *  Created on: 2016年12月22日
 *      Author: Administrator
 */

#include "platform.h"
#include "system_paramter.h"
#include "linklayer.h"
#include "memory.h"
#include "wifi.h"

uint8_t Wifi_RecvState = 0;

uint8_t Socket_ConnectState = 0;

const unsigned char g_arrHttp500Error[]={"HTTP/1.1 500"};
const unsigned char g_arrHttp400Error[]={"HTTP/1.1 400"};
const unsigned char g_arrHttp20[]      ={"HTTP/1.1 20"};



void WIFI_CommTask(void)
{
	if(Platform_Param.Rx_Status != Resp_Working)			//	wifi收到数据 才开始处理
		return;

	if( memcmp(g_arrHttp500Error, Wifi.read_buf, 12) == 0)
	{
		Platform_Param.Rx_Status = Resp_False;
		printf("HTTP/1.1 500 ERROR\r\n");
		memset(Wifi.read_buf, 0, Platform_Param.Rx_Offset);
		Platform_Param.Rx_Offset = 0;
		g_bCommReceiveFlag = 0;
	}
	else if( memcmp(g_arrHttp400Error, Wifi.read_buf, 12) == 0)
	{
		Platform_Param.Rx_Status = Resp_False;
		printf("HTTP/1.1 400 ERROR\r\n");
		memset(Wifi.read_buf, 0, Platform_Param.Rx_Offset);
		Platform_Param.Rx_Offset = 0;
		g_bCommReceiveFlag = 0;
	}
	else if( memcmp(g_arrHttp20, Wifi.read_buf, 11) == 0)
	{
		//Platform_Param.Rx_Status = Resp_Success;
		//printf("HTTP/1.1 200 OK\r\n");
		g_bCommReceiveFlag = 1;
	}
	else
	{
		Platform_Param.Rx_Status = Resp_False;
		printf("HTTP/1.1 200 ERROR\r\n");
		memset(Wifi.read_buf, 0, Platform_Param.Rx_Offset);
		Platform_Param.Rx_Offset = 0;
		g_bCommReceiveFlag = 0;
	}
}



void OneSendcondLookup(void)
{
	unsigned char bReturn = 0;
	static unsigned char bLedFlag = 0;
	static unsigned char  s_bDebug_show = 0;
	static u32_t s_nDebug_cnt = 0;
	static unsigned char bFlag = 0;
	static unsigned char s_rfRecvFlag = 0;
	Resp_Status_t Wifi_ReadStatus = Resp_Success;

	//g_iServerHeartCommTimeCnt >= strSystemParameter.HraerBeatTimeout
	if(g_bServerLoginSucFlag == 1)		//连上平台更新心跳时间
	{
		g_iServerUploadCommTimeCnt++;
		g_iServerHeartCommTimeCnt++;
	}
#if 0
	if(g_bServerLoginSucFlag)		//没连上服务器就不给手环配时间
	{
		g_bUploadOkFlag = 1;
		Sys_TimeFormatToInt();
	}
	else if (!g_bUploadOkFlag)
	{
		SystemTimeSimple = 0;
	}
#endif
	if(g_bCommReceiveFlag == 1)
	{
		g_bCommReceiveFlag = 0;
		//printf("platform recv data: %s\n",Wifi.read_buf);
		printf("bReturn = Pprotocol->getresponse(Pprotocol); \n");
		bReturn = Pprotocol->getresponse(Pprotocol);
		printf("bReturn = %d\n",bReturn);
		if(bReturn == PR_SUCCESS)
		{
			printf("Wifi_SocketReadCallBack\n");
			Wifi_SocketReadCallBack(Wifi.Even.Timeout_ID,&Wifi_ReadStatus);
			Platform_Param.Rx_Status = Resp_Success;
			Server_RcvDataHandle();
		}
	}
}




void ServerComm_task(void)
{
#if 0
	if(g_bStartUpgradeFlag == 1)		//升级操作
	{
		UpgradeOperation();
	}
#endif
	strSystemParameter.HraerBeatTimeout = 20;
	//os_printf("g_iServerHeartCommTimeCnt = %d\n",g_iServerHeartCommTimeCnt);
	//os_printf("strSystemParameter.HraerBeatTimeout = %d\n",strSystemParameter.HraerBeatTimeout);
	//os_printf("g_bServerLoginSucFlag = %d\n",g_bServerLoginSucFlag);
	//os_printf("g_bServerUploadReq = %d\n",g_bServerUploadReq);
	if((g_iServerHeartCommTimeCnt >= strSystemParameter.HraerBeatTimeout) && (g_bServerLoginSucFlag == 1)&&(g_bServerUploadReq == 0))	// gaojun 2015-08-19
	{
		g_iServerHeartCommTimeCnt = 0;
		g_iServerUploadCommTimeCnt = 0;
		g_bServerUploadReq = 0;
		g_bServerHeartbeatReq = 1; //心跳包请求
	}
#if 0
	if((TagRecordFlashQueue.cnt)&&(g_bServerLoginSucFlag == 1)&&(TagDataQueue.cnt == 0))//在缓存区间数据未写入Flash之前不上传数据 20160119
#else
	if((TagRecordFlashQueue.cnt)&&(g_bServerLoginSucFlag == 1))
#endif
	{
		if((g_iServerUploadCommTimeCnt >= strSystemParameter.SendDataDealyTime)
			||(TagRecordFlashQueue.cnt >= strSystemParameter.SendDataMaxCnt))
		{
			g_bServerUploadReq = 1;
			g_iServerUploadCommTimeCnt = 0;
		}
	}
}

static uint32_t test_tick = 0;

void Platform_Task(void *pvParameters)
{
	uint32_t TickCnt_100ms= 0;
	
	while(1)
	{
		if(Platform_Param.Connect_Status == Connect_Success)	//平台连接成功，开始平台通讯		//socket connect success
		{
			LinkPrintPortRecv();	//接收判断
			if((TickCnt_100ms++%10) == 0)
			{
				ServerComm_task();
				OneSendcondLookup();	//接收处理
				Server_CommTask();		//任务调度
				test_tick++;
//				printf("test_tick = %d\n",test_tick);
			}
		}
		vTaskDelay(100/portTICK_RATE_MS);
	}
}




















