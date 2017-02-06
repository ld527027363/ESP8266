
#include <string.h>
#include "wifi.h"
#include "esp8266_wifi.h"
#include "espconn.h"
#include "lwip/sockets.h"
#include "socket_tcp_client.h"
#include "platform.h"
#include "system_paramter.h"

#define WIFI_TASK_RUN_INTERVAL		(100/portTICK_RATE_MS)

Wifi_Register_T Wifi;


void Wifi_Init(WIFI_MODE wifi_mode,uint8_t * wifi_ssid,uint8_t *wifi_password,WifiCallback_Handle Callback)
{
	Wifi.wifi_mode = wifi_mode;
	sprintf(Wifi.ssid, wifi_ssid);
	sprintf(Wifi.password, wifi_password);
	
	Wifi.Even.even_id  = wifi_init;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	Wifi_TaskStateSet(task_run);
}

void Wifi_Connect(WifiCallback_Handle Callback)
{
	Wifi.Even.even_id  = wifi_connect;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	Wifi_TaskStateSet(task_run);
}

void Wifi_Socket_Create(WifiCallback_Handle Callback)
{
	Wifi.socket_domain = AF_INET;			//默认socket为TCP、UDP协议族
	Wifi.socket_type = SOCK_STREAM;			//默认socket为TCP/IP模式
	Wifi.socket_protocol = 0;				//默认IPPROTO_IP

	Wifi.Even.even_id  = socket_creat;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	Wifi_TaskStateSet(task_run);
}

void Wifi_Socket_Connect(int fd,char *host_ip,int port_num,WifiCallback_Handle Callback)
{
	Wifi.socket_fd = fd;
	memcpy(Wifi.server_ip,host_ip,strlen(host_ip));
	Wifi.server_port_num = port_num;

	Wifi.Even.even_id  = scoket_connect;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	Wifi_TaskStateSet(task_run);
}

uint32_t Wifi_socket_write(uint8_t *buf,uint32_t len,uint32_t offset,WifiCallback_Handle Callback)
{
	Wifi.write_buf = buf + offset;
	Wifi.write_len = len;

	Wifi.Even.even_id  = socket_write;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	Wifi_TaskStateSet(task_run);
	return 0;
}

uint32_t Wifi_socket_read(uint8_t *buf,uint32_t len,uint32_t offset,WifiCallback_Handle Callback)
{

	Wifi.read_buf = buf;
	Wifi.read_len = len;
	Wifi.read_offset = offset;

	Wifi.Even.even_id  = socket_read;
	Wifi.Even.resp_state = Resp_Null;
	Wifi.Even.Callback = Callback;

	bzero(Wifi.read_buf + Wifi.read_offset, Wifi.read_len - Wifi.read_offset);
	Wifi_TaskStateSet(task_run);
	return 0;
}


void Wifi_Socket_Close(int fd)
{
	Wifi.socket_fd = fd;

	Wifi.Even.even_id  = socket_close;
	Wifi.Even.resp_state = Resp_Null;
	//Wifi.Even.Callback = NULL;

	Wifi_TaskStateSet(task_run);
}

void Wif_TimeOutSet(uint32_t cnt)
{
	if(Wifi.timeout_state == 0)
	{
		Wifi.timeout_state = 1;
		Wifi.timeout_tick = cnt;
	}
}

void Wifi_TimeoutStop(void)
{
	Wifi.timeout_state = 0;
	Wifi.timeout_tick = 0;
}

void Wifi_TimeOutProcess(void)
{
	if(Wifi.timeout_state == 0)
		return;

	if(Wifi.Even.resp_state != Resp_Null)
	{
		Wifi.timeout_state = 0;
		Wifi.timeout_tick = 0;
		return;
	}

	if(Wifi.timeout_tick)
	{
		Wifi.timeout_tick--;
		if(Wifi.timeout_tick == 0)
		{
			printf("wifi task time out!!!! \n\n");
			Wifi.timeout_state = 0;
			Wifi_TaskStateSet(task_null);
			Wifi.Even.resp_state = Resp_TimeOut;
			Wifi.Even.Callback(0,&Wifi.Even.resp_state);
			//memcpy(&Wifi.Even.wifi_null_result,&wifi_even->wifi_null_result,WIFI_EVEN_RESULT_MAX_CNT);
		}
	}
}

void Wifi_SockedReadStop(Resp_Status_t status)
{
	Wifi.Even.resp_state = status;
	Wifi_TimeoutStop();
	Wifi_TaskStateSet(task_null);
}

void Wifi_EvenCheckProcess(void)
{
	
	if(Wifi.Even.resp_state != Resp_Null)
		return ;

	switch(Wifi.Even.even_id)
	{
		case wifi_init:
			printf("Esp8266_Status.InitSatus = %d\n",Esp8266_Status.InitSatus);
			if(Esp8266_Status.InitSatus == 1)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;
				printf("Wifi init callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case wifi_connect:
			if(Esp8266_Status.Station_ConnStatus == 1)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;	
				printf("Wifi connect callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case wifi_disconnect:
			if(Esp8266_Status.Station_ConnStatus == 0)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;		
				printf("Wifi disconnect callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case socket_creat:
			if(Socket_Status.CreatStatus == 1)
			{
				printf("if(Socket_Status.CreatStatus == 1)\n");
				Wifi_TimeoutStop();
				Wifi.Even.socket_create_result.socket_fd = Socket_Status.Socket_fd;
				Wifi.Even.resp_state = Resp_Success;	
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			else if(Socket_Status.CreatStatus == 2)
			{
				printf("if(Socket_Status.CreatStatus == 2)\n");
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_False;
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case scoket_connect:
			if(Socket_Status.ConnStatus == 1)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;		
				printf("scoket_connect callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			else if(Socket_Status.ConnStatus == 2)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_False;			
				printf("scoket_connect callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case socket_write:
			if(Socket_Status.WriteStatus == 1)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;
				Wifi.Even.socket_write_result.write_num = Socket_Status.WriteNum;			
				printf("socket_write callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			else if(Socket_Status.WriteStatus == 2)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_False;
				Wifi.Even.socket_write_result.write_num = 0;		
				printf("socket_write callback\n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case socket_read:
			#if 0
			if(Socket_Status.ReadStatus == 1)
			{
				printf("Socket_Status.ReadStatus == 1\n");
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;
				Wifi.Even.socket_read_result.read_num = Socket_Status.ReadNum;	
				//Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			else if((Socket_Status.ReadStatus == 2)||(Socket_Status.ReadStatus == 3))
			#else
			if((Socket_Status.ReadStatus == 2)||(Socket_Status.ReadStatus == 3))
			#endif
			{
				printf("Socket_Status.ReadStatus == 2\n");
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_False;
				Wifi.Even.socket_read_result.read_num = 0;		
				printf("socket_read callback 2 \n");
				Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		case socket_close:
			if(Socket_Status.DisconnStatus == 1)
			{
				Wifi_TimeoutStop();
				Wifi.Even.resp_state = Resp_Success;	
				printf("socket_close ok\n");
				//Wifi.Even.Callback(Wifi.Even.Timeout_ID,&Wifi.Even.resp_state);
				Wifi_TaskStateSet(task_null);
			}
			break;

		default:
			break;
	}
}


void Wifi_TaskStateSet(Wifi_Task_State_t mode)
{
	Wifi.task_state = mode;
}

void Wifi_Task(void *pvParameters)
{
	uint8_t resp_value = 0;

	while(1)
	{
		switch(Wifi.task_state)
		{
			case task_null:
				break;

			case task_run:
				switch(Wifi.Even.even_id)
				{
					case wifi_null:
						break;

					case wifi_init:
						Wifi.Wifi_Init_Handle(Wifi.wifi_mode,Wifi.ssid,Wifi.password);		
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(30);			//30 sec timeout
						printf("Wifi init\n");
						break;

					case wifi_connect:
						printf("wifi connect\n");
						Wifi.Wifi_Connect_Handle();
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(300);			//30 sec timeout
						break;

					case wifi_disconnect:
						Wifi.Wifi_Disconnect_Handle();
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(50);			//30 sec timeout
						break;

					case socket_creat:
						Wifi.socket_fd = Wifi.Socket_Creat_Handle(Wifi.socket_domain, Wifi.socket_type,Wifi.socket_protocol);
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(100);			//10 sec timeout
						break;

					case scoket_connect:
						Wifi.Socket_Connect_Handle(Wifi.socket_fd, Wifi.server_ip,Wifi.server_port_num);
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(200);			//20 sec timeout
						break;

					case socket_write:
						Wifi.Socket_Write_Handle(Wifi.socket_fd,Wifi.write_buf,Wifi.write_len);
						Wifi_TaskStateSet(task_null);
						break;

					case socket_read:
						//printf("Wifi.socket_Select_Handle \n");
						if(Wifi.socket_Select_Handle(Wifi.socket_fd) > 0)
						{
							printf("socket read \n");
							Wifi.read_offset += Wifi.Socket_Read_Handle(Wifi.socket_fd,Wifi.read_buf + Wifi.read_offset,Wifi.read_len-Wifi.read_offset);
							Wifi.Even.socket_read_result.read_num = Wifi.read_offset;
							Platform_Param.Rx_Offset = Wifi.Even.socket_read_result.read_num;
							if(Platform_Param.Rx_Status == Resp_Null)
								Platform_Param.Rx_Status = Resp_Working;
							printf("Wifi.read_offset = %d\n",Wifi.read_offset);
						}
						//Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(300);			//10 sec timeout
						break;

					case socket_close:
						Wifi.Socket_Disconnect_Handle(Wifi.socket_fd);
						Wifi_TaskStateSet(task_null);
						Wif_TimeOutSet(30);			//10 sec timeout
						break;

					default:
						break;
				}
				break;

		}
		Wifi_EvenCheckProcess();
		Wifi_TimeOutProcess();
		vTaskDelay(WIFI_TASK_RUN_INTERVAL);
		//printf("wifi task running\n");
	}
}

void Wifi_CallBack(wifi_even_t *wifi_even)
{
	Wifi.Even.resp_state = wifi_even->resp_state;
	memcpy(&Wifi.Even.wifi_null_result,&wifi_even->wifi_null_result,WIFI_EVEN_RESULT_MAX_CNT);
}




void Wifi_Register(void)
{
	Wifi.task_state = task_null;		//初始化wifi任务为空

	Wifi.Even.even_id = wifi_null;		//初始化wifi事件为空

	Wifi.write_buf = NULL;
	Wifi.write_len = 0;
	Wifi.read_buf = NULL;
	Wifi.read_len = 0;

	Wifi.wifi_mode = STATION_MODE;					//默认wifi为station模式
	sprintf(Wifi.ssid, WIFI_DEFAULT_SSID);			//默认wifi SSID 为“AIKOU”
	sprintf(Wifi.password, WIFI_DEFAULT_PASSWORD);	//默认wifi 密码为“aidekoukou”

	Wifi.socket_domain = AF_INET;					//默认socket为TCP、UDP协议族
	Wifi.socket_type = SOCK_STREAM;					//默认socket为TCP/IP模式
	Wifi.socket_protocol = 0;						//默认IPPROTO_IP

	memcpy(Wifi.server_ip,SOCK_TCP_CLIENT_HOST,strlen(SOCK_TCP_CLIENT_HOST));
	Wifi.server_port_num = SOCK_TCP_CLIENT_PORT;

	Wifi.Wifi_Init_Handle 			= Esp8266_WifiInit;
	Wifi.Wifi_Connect_Handle 		= Esp8266_WifiConnect;
	Wifi.Wifi_Disconnect_Handle 	= Esp8266_WifiDisconnect;

	Wifi.Socket_Creat_Handle 		= Socket_TcpClientCreat;
	Wifi.Socket_Connect_Handle 		= Socket_TcpClientConnect;
	Wifi.socket_Select_Handle		= Socket_Select;
	Wifi.Socket_Read_Handle 		= Socket_Read;
	Wifi.Socket_Write_Handle 		= Socket_Write;
	Wifi.Socket_Disconnect_Handle 	= Socket_Close;

	Wifi.Wifi_StatusChange_Handle 	= Wifi_CallBack;
}



