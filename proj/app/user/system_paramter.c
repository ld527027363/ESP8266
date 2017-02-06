
#include "system_paramter.h"

#define RTC_CYCLE_TO_SECOND			(1000*1000)

System_Parameters_t strSystemParameter;

uint32_t RtcSec = 0;

RTCDAY SysDate,StandDate;
RTCTIME	SysTime,StandTime;

uint32_t SysTick1sFlag = 0;
uint32_t SystemTimeSimple = 0;

uint8_t g_reset_flag = 0;

uint16_t WiFi_SendDataFlag = 0;
uint8_t WifiConnectFlag = 0;


Platform_Param_t  Platform_Param;		//平台通讯数据接收发送缓冲区

static uint32_t Rtc_OffsetValue = 0;
static uint64_t Rtc_LastTick = 0;

void Sys_PaltformParamInit(void)
{
	Platform_Param.Rx_Index = 0;
	Platform_Param.Rx_Len = 0;
	Platform_Param.Rx_Offset = 0;
	Platform_Param.Rx_Status = Resp_Null;
	Platform_Param.Tx_Len = 0;
	Platform_Param.Tx_Offset = 0;
	Platform_Param.Tx_Status = Resp_Null;
	strSystemParameter.HraerBeatTimeout = 20;
}

void Sys_GetRtcTime(void)
{
	uint64_t rtc_tick = 0;
	uint32_t cal = 0;
	uint32_t rtc_cycle = 0;
	uint32_t sec_num = 0;

	rtc_tick = system_get_rtc_time();
	cal = system_rtc_clock_cali_proc();

	rtc_cycle = (cal*1000)>>12;

	sec_num = rtc_cycle * ((rtc_tick - Rtc_LastTick)/1000);
	sec_num += Rtc_OffsetValue;
	Rtc_OffsetValue = sec_num % RTC_CYCLE_TO_SECOND;
	RtcSec += sec_num / RTC_CYCLE_TO_SECOND;


//	os_printf("sec_num = %d  Rtc_OffsetValue = %d  RtcSec = %d\n",sec_num,Rtc_OffsetValue,RtcSec);
	Rtc_LastTick = rtc_tick;
}


void Sys_TimeFormatToInt(void)
{
	SystemTimeSimple = SysDate.year-2000;
	SystemTimeSimple <<= 26;
	SystemTimeSimple += ((SysDate.month)<<22);
	SystemTimeSimple += ((SysDate.daily)<<17);
	SystemTimeSimple += ((SysTime.hour)<<12);
	SystemTimeSimple += ((SysTime.minute)<<6);
	SystemTimeSimple += (SysTime.second);
}



void Sys_GetRecordDate(unsigned char *date_str,unsigned char *tim_src)
{
	unsigned short reduce_tim;
	uint8 day = 0;

	RTC_DateTypeDef rtc_date;

	reduce_tim = tim_src[0]*256 + tim_src[1];

	rtc_date.year = tim_src[2];
	day = tim_src[3];
	rtc_date.year = SysDate.year;
	rtc_date.month = SysDate.month;

	rtc_date.daily = reduce_tim>>11;	//date

	if(day < rtc_date.daily)
	{
		if (rtc_date.month == 1)
		{
			rtc_date.month = 12;
		}
		else
		{
			rtc_date.month = rtc_date.month-1;
		}
	}

	if (rtc_date.month > SysDate.month)
	{
		rtc_date.year = SysDate.year -1;
	}

    reduce_tim = (unsigned short)(rtc_date.year-2000)<<10;
    reduce_tim = reduce_tim + (unsigned short)(rtc_date.month<<5);
    reduce_tim = reduce_tim + (unsigned short)(rtc_date.daily);
    date_str[0] = (unsigned char)(reduce_tim / 256);
    date_str[1] = (unsigned char)(reduce_tim % 256);

}


void Sys_GetRecordTime(unsigned char *time_str,unsigned char *tim_src)
{
	unsigned short reduce_tim;
	reduce_tim = tim_src[0]*256 + tim_src[1];
	time_str[0] = ((reduce_tim>>6)&0x1F);
	time_str[1] = (reduce_tim&0x3F);
}

void Sys_SetDateAndTime(char *DateBuff,char *TimeBuff)
{
	uint8_t Temp_flag = 0;
	RTCDAY 	Temp_Date;
	RTCTIME	TempTime;

	do
	{
		memset(&Temp_Date,0,sizeof(Temp_Date));
		memset(&TempTime,0,sizeof(TempTime));
	///////////////////////////////////////////////
		Temp_Date.year += (DateBuff[0] - '0')*1000;
		Temp_Date.year += (DateBuff[1] - '0')*100;
		Temp_Date.year += (DateBuff[2] - '0')*10;
		Temp_Date.year += (DateBuff[3] - '0');
		if((Temp_Date.year < 2015)||(Temp_Date.year > 2020))
			break;

		Temp_Date.month += (DateBuff[5] - '0')*10;
		Temp_Date.month += (DateBuff[6] - '0');
		if((Temp_Date.month < 1)||(Temp_Date.month > 12))
			break;

		Temp_Date.daily += (DateBuff[8] - '0')*10;
		Temp_Date.daily += (DateBuff[9] - '0');
		if((Temp_Date.daily < 1)||(Temp_Date.daily > 31))
			break;


	//////////////////////////////////////////////
		TempTime.hour += (TimeBuff[0] - '0')*10;
		TempTime.hour += (TimeBuff[1] - '0');
		if(TempTime.hour > 23)
			break;

		TempTime.minute += (TimeBuff[3] - '0')*10;
		TempTime.minute += (TimeBuff[4] - '0');
		if(TempTime.hour > 59)
			break;

		TempTime.second += (TimeBuff[6] - '0')*10;
		TempTime.second += (TimeBuff[7] - '0');
		if(TempTime.hour > 60)
			break;
		Temp_flag = 1;
	}while(0);
	if(Temp_flag)
	{
		memcpy(&SysDate,&Temp_Date,sizeof(Temp_Date));
		memcpy(&SysTime,&TempTime,sizeof(TempTime));

		//RTC_SetTime();
	}
}

void WriteSystemParameter(void)
{


}


void Sys_Delay_ms(uint16 ms)
{
	vTaskDelay(ms/portTICK_RATE_MS);
}



void Sys_Delay_s(uint16 s)
{
	vTaskDelay((s*1000)/portTICK_RATE_MS);
}
















