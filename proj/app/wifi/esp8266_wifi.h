/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP8266_WIFI_H__
#define __ESP8266_WIFI_H__

#include "esp_common.h"
#include "system_paramter.h"


extern uint8_t Wifi_Statuse;

#define WIFI_AP_SSID		"TP-LINK_C350"		//"ilovicoco"
#define WIFI_AP_PASSWORD	"Marktrace2017"		//"aidekoukou"

#define WIFI_DEFAULT_SSID			"AIKOU"					//"ilovicoco"			//"zuojun"		//		//"TP-LINK_C350"
#define WIFI_DEFAULT_PASSWORD		"aidekoukou"			//"aidekoukou"		//"1234567890"				//	//"Marktrace2017"

typedef struct
{
	uint8_t	InitSatus;
	uint8_t	Station_ConnStatus;
	uint8_t Ap_ConnStatus;
}Esp8266_Status_t;

extern Esp8266_Status_t Esp8266_Status;

#define WIFI_WORK_MOD		0

#define WIFI_PASSWORD_MAX_LEN		32
#define WIFI_AP_MAX_NUM				20
typedef struct
{
    uint8 bssid[6];                     /**< MAC address of AP */
    uint8 ssid[32];                     /**< SSID of AP */
    uint8 ssid_len;                     /**< SSID length */
    uint8 channel;                      /**< channel of AP */
    sint8 rssi;                         /**< single strength of AP */
    AUTH_MODE authmode;                 /**< authmode of AP */
    uint8 is_hidden;                    /**< SSID of current AP is hidden or not. */
    sint16 freq_offset;                 /**< frequency offset */
    sint16 freqcal_val;
    uint8 *esp_mesh_ie;
    uint8 password[WIFI_PASSWORD_MAX_LEN];
}Wifi_ApInfo_t;

extern Wifi_ApInfo_t Wifi_ApInfoList[WIFI_AP_MAX_NUM];

extern uint8 Wifi_ApSelectNum;

void Wifi_InitStationMode(void);

void Wifi_ScanDone(void *arg, STATUS status);

bool Wifi_SetPassword(Wifi_ApInfo_t *ApInfo,char *password);

void Wifi_LinkAp(Wifi_ApInfo_t *ApInfo);

void wifi_handle_event_cb(System_Event_t *evt);





uint8_t Esp8266_WifiInit(WIFI_MODE wifi_mode,uint8_t *wifi_ssid,uint8_t *wifi_password);

void Esp8266_WifiConnect(void);

void Esp8266_WifiDisconnect(void);






#endif
