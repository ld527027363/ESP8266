/*
 * system.h
 *
 *  Created on: 2016Äê12ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef APP_USER_SYSTEM_H_
#define APP_USER_SYSTEM_H_

#include "esp_common.h"
#include "calendar.h"

#define Linklay_Printf printf

#define MYVERSION              8 // hcp 2016-05-03
#define VERSION_STR           "0.0.1 Bulid-161222"
#define VERSION_STR_T           "Version:0.0.1 Bulid-161222\r\n"

#define USING_LOG 0

#define BOARDVersion "0405"

#define TCP_MAX_RCV_LEN  (2*1024)
#define TCP_MAX_SEND_LEN  (2*1024)


#define SERVER_COMM_TIMEOUT	 40

typedef struct
{
		unsigned char  ProductSn[18];
		unsigned char  LocalIp[4];
		unsigned char  LocalSubWay[4];
		unsigned char  LocalGateway[4];
		unsigned short LocalPort;
		unsigned char  DistinIp[4];
		unsigned short DistinPort;
		unsigned short HraerBeatTimeout;
		unsigned short SendDataDealyTime;
		unsigned short SendDataMaxCnt;
		char			Param_Ver[16];

}System_Parameters_t;

extern System_Parameters_t strSystemParameter;

extern uint32_t RtcSec;

extern RTCDAY SysDate,StandDate;
extern RTCTIME	SysTime,StandTime;

extern uint8_t g_reset_flag;

extern uint16_t WiFi_SendDataFlag;

extern uint8_t WifiConnectFlag;

extern uint32_t SysTick1sFlag;

extern uint32_t SystemTimeSimple;

typedef enum
{
	Connect_Null = 0,
	Connect_Waitting,
	Connect_Success,
	Connect_False
}Connect_Status_t;

typedef enum
{
	Null_Status = 0,
	Tx_Status,
	Rx_Status
}Communication_Status_t;

#define PLATFORM_TX_BUFF_SIZE		(2*1024)
#define PLATFORM_RX_BUFF_SIZE		(2*1024)


typedef enum
{
	Resp_Null = 0,
	Resp_Working,
	Resp_Success,
	Resp_False,
	Resp_TimeOut
}Resp_Status_t;

typedef struct
{
	uint8_t		Tx_Buff[PLATFORM_TX_BUFF_SIZE];
	uint32_t	Tx_Len;
	uint32_t	Tx_Offset;
	uint8_t		Rx_Buff[PLATFORM_RX_BUFF_SIZE];
	uint32_t	Rx_Len;
	uint32_t	Rx_Offset;
	uint32_t	Rx_Index;
	Resp_Status_t			Tx_Status;
	Resp_Status_t			Rx_Status;
	Connect_Status_t		Connect_Status;
	Communication_Status_t	Comm_status;
}Platform_Param_t;

extern Platform_Param_t  Platform_Param;
/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef;
/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
  uint8_t day_in_week;
  uint8_t month;
  uint8_t daily;
  uint8_t year;
}RTC_DateTypeDef;


void Sys_TimeFormatToInt(void);

void Sys_GetRecordDate(unsigned char *date_str,unsigned char *tim_src);

void Sys_GetRecordTime(unsigned char *time_str,unsigned char *tim_src);

void Sys_GetRtcTime(void);

void Sys_SetDateAndTime(char *DateBuff,char *TimeBuff);

void WriteSystemParameter(void);

void Sys_PaltformParamInit(void);





void Sys_Delay_ms(uint16 ms);

void Sys_Delay_s(uint16 s);











#endif /* APP_USER_SYSTEM_H_ */
