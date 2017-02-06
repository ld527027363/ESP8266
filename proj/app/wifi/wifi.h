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

#ifndef __WIFI_H__
#define __WIFI_H__

#include "esp_common.h"
#include "system_paramter.h"

typedef void (*WifiCallback_Handle)(uint32_t ID,void * Data);

#define WIFI_EVEN_RESULT_MAX_CNT		128


typedef enum
{
	task_null = 0,
	task_run,
	task_timeout
}Wifi_Task_State_t;

typedef struct Wifi_Register_t Wifi_Register_T;
typedef Wifi_Register_T* P_Wifi_Register_t;


typedef enum
{
	wifi_null = 0,
	wifi_init,
	wifi_connect,
	wifi_disconnect,
	socket_creat,
	scoket_connect,
	socket_write,
	socket_read,
	socket_close
}Wifi_Even_ID_t;


typedef struct
{
	uint8_t result_buf[WIFI_EVEN_RESULT_MAX_CNT];
}wifi_null_result_t;


typedef struct
{
	uint8_t data[16];
}wifi_init_result_t;


typedef struct
{
	uint8_t data[16];
}wifi_Connect_result_t;

typedef struct
{
	uint8_t data[16];
	int		socket_fd;
}socket_creat_result_t;

typedef struct
{
	uint8_t data[16];
}socket_connect_result_t;

typedef struct
{
	uint8_t data[16];
}socket_close_result_t;

typedef struct
{
	uint8_t data[16];
	uint32_t write_num;
}socket_write_result_t;

typedef struct
{
	uint8_t data[16];
	uint32_t read_num;
}socket_read_result_t;

typedef struct
{
	Wifi_Even_ID_t		even_id;
	Resp_Status_t		resp_state;
	WifiCallback_Handle	Callback;
	uint32_t			Timeout_ID;
	union
	{
		wifi_null_result_t		wifi_null_result;
		wifi_init_result_t		wifi_init_result;
		wifi_Connect_result_t	wifi_Connect_result;
		socket_creat_result_t	socket_create_result;
		socket_connect_result_t	socket_connect_result;
		socket_write_result_t	socket_write_result;
		socket_read_result_t	socket_read_result;
		socket_close_result_t	socket_close_result;
	};

}wifi_even_t;

struct Wifi_Register_t
{
	Wifi_Task_State_t task_state;

	wifi_even_t Even;
	uint8_t timeout_state;
	uint32_t timeout_tick;
	uint8_t *write_buf;
	uint32_t write_len;
	uint8_t *read_buf;
	uint32_t read_len;
	uint32_t read_offset;
	uint8_t ssid[64];
	uint8_t password[64];
	WIFI_MODE wifi_mode;

	int socket_fd;
	int socket_domain;
	int socket_type;
	int socket_protocol;

	char server_ip[16];
	int  server_port_num;

	uint8_t (*Wifi_Init_Handle)(WIFI_MODE wifi_mode,uint8_t * wifi_ssid,uint8_t *password);
	void (*Wifi_Connect_Handle)(void);
	void (*Wifi_Disconnect_Handle)(void);

	int			(*Socket_Creat_Handle)(int domain, int type, int protocol);
	void		(*Socket_Connect_Handle)(int socket_fd,char *hostip, int portno);
	int			(*socket_Select_Handle)(int socket_fd);
	uint32_t	(*Socket_Read_Handle)(int socket_fd,uint8_t *read_buf,uint32_t buf_size);
	uint32_t	(*Socket_Write_Handle)(int socket_fd,uint8_t *write_buf,uint32_t buf_len);
	int			(*Socket_Disconnect_Handle)(int socket_fd);

	void		(*Wifi_StatusChange_Handle)(wifi_even_t *wifi_even);
};

extern Wifi_Register_T Wifi;


void Wifi_TaskStateSet(Wifi_Task_State_t mode);

void Wifi_Init(WIFI_MODE wifi_mode,uint8_t * wifi_ssid,uint8_t *wifi_password,WifiCallback_Handle	Wifi_Callback);

void Wifi_Connect(WifiCallback_Handle Callback);

void Wifi_Socket_Create(WifiCallback_Handle Callback);

void Wifi_Socket_Connect(int fd,char *host_ip,int port_num,WifiCallback_Handle Callback);

uint32_t Wifi_socket_write(uint8_t *buf,uint32_t len,uint32_t offset,WifiCallback_Handle Callback);

uint32_t Wifi_socket_read(uint8_t *buf,uint32_t len,uint32_t offset,WifiCallback_Handle Callback);

void Wifi_SockedReadStop(Resp_Status_t status);

void Wifi_Socket_Close(int fd);

void Wifi_Task(void *pvParameters);

void Wifi_Register(void);

#endif
