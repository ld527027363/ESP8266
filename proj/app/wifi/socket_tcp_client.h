/*
 * socket_tcp_client.h
 *
 *  Created on: 2016Äê9ÔÂ21ÈÕ
 *      Author: itmaktub
 */

#ifndef APP_LWIPDEMO_SOCKET_TCP_SOCKET_TCP_CLIENT_H_
#define APP_LWIPDEMO_SOCKET_TCP_SOCKET_TCP_CLIENT_H_

#if 0
#define SOCK_TCP_CLIENT_HOST  "121.9.230.130"
#define SOCK_TCP_CLIENT_PORT  20482
#else
#define SOCK_TCP_CLIENT_HOST  "218.17.157.214"
#define SOCK_TCP_CLIENT_PORT  4950
#endif

typedef struct
{
	uint8_t		Socket_fd;
	uint8_t		CreatStatus;
	uint8_t		ConnStatus;
	uint8_t		ReadStatus;
	uint32_t	ReadNum;
	uint32_t	WriteNum;
	uint8_t		WriteStatus;
	uint8_t		DisconnStatus;
}Socket_Status_t;

extern Socket_Status_t Socket_Status;

void socket_tcp_client_task(void);

int Socket_TcpClientCreat(int domain, int type, int protocol);

void Socket_TcpClientConnect(int socket_fd,char *hostip, int portno);

int Socket_Select(int socket_fd);

uint32_t Socket_Read(int sock_fd,uint8_t *read_buf,uint32_t buf_size);

uint32_t Socket_Write(int socket_fd,uint8_t *write_buf,uint32_t buf_len);

int Socket_Close(int socket_fd);

void socket_tcp_client_handle(void *pvParameters);

#endif /* APP_LWIPDEMO_SOCKET_TCP_SOCKET_TCP_CLIENT_H_ */
