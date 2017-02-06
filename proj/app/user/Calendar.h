#ifndef __CALENDAR_H
#define __CALENDAR_H

#define  RTCCLK  32768
#define  SEC_DAY  86400
#define  SEC_HOUR  3600

typedef struct      
{  
	unsigned short 	year;
	unsigned char 	month;
	unsigned char 	daily;   
	unsigned char 	day_in_week;

}RTCDAY; //数据存放在外部RAM中

typedef struct      
{  
    unsigned char hour;
    unsigned char minute;
    unsigned char second;

}RTCTIME; //数据存放在外部RAM中

void  RTC_SetStandTime(void);
void RTC_SetTime(void);
void RTC_GetTime(void);
void RTC_GetDate(void);
void RTC_SetDefaultTime(void);

 void UpData_Time(void);
#endif
