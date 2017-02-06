/*****************************************************
Project : EPC&ISO18000 Series interrogator  
Version  : 1.0   
File name: "utility.h"
Date     : 2012-11-10
Author   : GAOJUN
Company  : Technology
Comments : 
*****************************************************/

#ifndef __UTILITY_h
#define __UTILITY_h


unsigned char CheckSum(unsigned char *uBuff, unsigned int uBuffLen) ;
unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);
void crc_ccitt_old(unsigned char * CrcValue,unsigned char * Data,unsigned short Length);
void crc_ccitt_upgrade(unsigned char * CrcValue,unsigned char * Data,unsigned short Length);
unsigned short CalBinCRC16(unsigned short iCrcValue,unsigned char * Data,unsigned short Length);
unsigned short CRC16(unsigned char * Data,unsigned short Length);
//校验内容
unsigned char checkCrc16(unsigned char * dat, int len);

unsigned char  CheckSumEx(unsigned char *uBuff, unsigned int uBuffLen);

int DectoBCD(int Dec, unsigned char *Bcd, int length);
unsigned char BCD2HEX(unsigned char bcd_data);
unsigned char HEX2BCD(unsigned char hex_data);




#define EN_FUN_TIMEOUT         1

#define EN_TST_TIMEOUT				0


//超时影响因子
typedef enum timeout_mode_e
{
	timeout_mode_null = 0,
	timeout_mode_max,  //最大值优先
	timeout_mode_cnt,
	
}timeout_mode_t;


/*
*  初始化
*/
//HANDLE  timeout_init(u32_t * pTick, u32_t min, u32_t max, timeout_mode_t mode);


/*
*  启动  根据cnt 延迟超时
*/
//void  timeout_start(HANDLE hdl, u32_t cnt);


/*
*  停止  传入 是否事实超时
*/
//void  timeout_stop(HANDLE hdl, BOOL bTimeout);



/*
*  获取超时值
*/
//u32_t  timeout_get(HANDLE hdl);
//

//extern HANDLE g_timeout_cur_hdl;
//#define timeout_get_cur()  (g_timeout_cur_hdl)

#endif
