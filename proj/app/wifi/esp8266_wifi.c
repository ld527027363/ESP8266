
#include <string.h>
#include "esp_common.h"
#include "esp8266_wifi.h"
#include "wifi.h"
#include "espconn.h"
#include "socket_tcp_client.h"

Esp8266_Status_t Esp8266_Status;

#define DATA_NUM_MAX  128

typedef struct {
	uint8 len;
	uint8 data[DATA_NUM_MAX];
}data_tx_info;

uint8_t Wifi_Statuse = 0;

struct espconn user_tcp_espconn;//注意在uart.c中添加声明
data_tx_info data_tx_buff;//在uart.h中添加还有

uint8 Wifi_ApIndex = 0;
uint8 Wifi_ApSelectNum = 0;
Wifi_ApInfo_t Wifi_ApInfoList[WIFI_AP_MAX_NUM] = {0};


void Wifi_InitStationMode(void)
{

}

/******************************************************
 *
 *
 *
 *
 ******************************************************/

uint8_t Esp8266_WifiInit(WIFI_MODE wifi_mode,uint8_t *wifi_ssid,uint8_t *wifi_password)
{
	wifi_even_t even;
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));

	Esp8266_Status.InitSatus = Resp_Null;

	if(wifi_mode == STATION_MODE)
		printf("wifi_mode = STATION_MODE\n");
	printf("wifi_ssid %s\n",wifi_ssid);
	printf("wifi_password %s\n",wifi_password);


	wifi_set_opmode(wifi_mode);
	sprintf(config->ssid, wifi_ssid);
	sprintf(config->password, wifi_password);
	wifi_station_set_config(config);
	free(config);

	Esp8266_Status.InitSatus = 1;

	return 0;
}


void Esp8266_WifiConnect(void)
{
	wifi_station_connect();
	wifi_set_event_handler_cb(wifi_handle_event_cb);
}


void Esp8266_WifiDisconnect(void)
{
	wifi_station_disconnect();
}

/******************************************************
 *
 *
 *
 *
 ******************************************************/
bool Wifi_SetPassword(Wifi_ApInfo_t *ApInfo,char *password)
{
	uint8 password_len = strlen(password);

	if(password_len >= WIFI_PASSWORD_MAX_LEN)
		return false;

	memcpy(Wifi_ApInfoList[Wifi_ApSelectNum].password,password,strlen(password));

	return true;
}

void Wifi_LinkAp(Wifi_ApInfo_t *ApInfo)
{
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));

	sprintf(config->ssid, Wifi_ApInfoList[Wifi_ApSelectNum].ssid);
	sprintf(config->password, Wifi_ApInfoList[Wifi_ApSelectNum].password);
	wifi_station_set_config(config);
	free(config);
	wifi_station_connect();
}

void Wifi_ScanDone(void *arg, STATUS status)
{
	uint8 ssid[33];
	char temp[128];

	Wifi_ApIndex = 0;
	memset(Wifi_ApInfoList,'\0',sizeof(Wifi_ApInfo_t)*WIFI_AP_MAX_NUM);

	if (status == OK)
	{
		struct bss_info *bss_link = (struct bss_info *)arg;

		while (bss_link != NULL)
		{
			memcpy(&Wifi_ApInfoList[Wifi_ApIndex],bss_link->bssid,sizeof(Wifi_ApInfo_t)-WIFI_PASSWORD_MAX_LEN);
			memset(ssid, 0, 33);

			if (strlen(bss_link->ssid) <= 32)
				memcpy(ssid, bss_link->ssid, strlen(bss_link->ssid));
			else
				memcpy(ssid, bss_link->ssid, 32);

			printf("(%d,\"%s\",%d,\""MACSTR"\",%d)\r\n",
					Wifi_ApInfoList[Wifi_ApIndex].authmode, ssid,Wifi_ApInfoList[Wifi_ApIndex].rssi,MAC2STR(Wifi_ApInfoList[Wifi_ApIndex].bssid),Wifi_ApInfoList[Wifi_ApIndex].channel);
			Wifi_ApIndex++;
			bss_link = bss_link->next.stqe_next;
		}
		printf("Get Ap Cnt %d\r\n",Wifi_ApIndex);
	}
	else
	{
		printf("scan fail !!!\r\n");
	}
}


void wifi_handle_event_cb(System_Event_t *evt)
{
	wifi_even_t even;

	printf("event %x\n", evt->event_id);
	switch (evt->event_id)
	{
		case EVENT_STAMODE_CONNECTED:
			printf("connect to ssid %s, channel %d\n",
			evt->event_info.connected.ssid,
			evt->event_info.connected.channel);
			break;

		case EVENT_STAMODE_DISCONNECTED:
			printf("disconnect from ssid %s, reason %d\n",
			evt->event_info.disconnected.ssid,
			evt->event_info.disconnected.reason);
			Esp8266_Status.Station_ConnStatus = 0;
			break;

		case EVENT_STAMODE_AUTHMODE_CHANGE:
			printf("mode: %d -> %d\n",
			evt->event_info.auth_change.old_mode,
			evt->event_info.auth_change.new_mode);
			break;

		case EVENT_STAMODE_GOT_IP:
			printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
			IP2STR(&evt->event_info.got_ip.ip),
			IP2STR(&evt->event_info.got_ip.mask),
			IP2STR(&evt->event_info.got_ip.gw));
			printf("\n");
			Esp8266_Status.Station_ConnStatus = 1;
			break;

		case EVENT_SOFTAPMODE_STACONNECTED:
			printf("station: " MACSTR "join, AID = %d\n",
			MAC2STR(evt->event_info.sta_connected.mac),
			evt->event_info.sta_connected.aid);
			Esp8266_Status.Ap_ConnStatus++;					//ap mode slave connect number increase
			break;

		case EVENT_SOFTAPMODE_STADISCONNECTED:
			printf("station: " MACSTR "leave, AID = %d\n",
			MAC2STR(evt->event_info.sta_disconnected.mac),
			evt->event_info.sta_disconnected.aid);
			if(Esp8266_Status.Ap_ConnStatus)				//ap mode slave connect number decrease
				Esp8266_Status.Ap_ConnStatus++;
			break;

		default:
			break;
	}
}

void esp8266_wifi_init()
{

	//    Wifi_InitStationMode();
	//    wifi_set_event_handler_cb(wifi_handle_event_cb);
}


