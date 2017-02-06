/*
 * socket_tcp_client.c
 *
 *  Created on: 2016年9月21日
 *      Author: itmaktub
 */

#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/netdb.h"
#include "lwip/api.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"
#include "wifi.h"
#include "socket_tcp_client.h"

#define BUFSIZE 128

Socket_Status_t Socket_Status;

int sockfd, n;
struct sockaddr_in serveraddr;
struct hostent *server;
char buf[BUFSIZE];
wifi_even_t even;

int Socket_TcpClientCreat(int domain, int type, int protocol)
{
	int test_cnt = 0;
	int socket_fd = 0;

	Socket_Status.CreatStatus = 0;
    /* socket: create the socket */
    socket_fd = socket(domain, type, protocol);		//sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == 0)
    {
    	Socket_Status.Socket_fd = socket_fd;
		Socket_Status.CreatStatus = 1;
    }
	else if(socket_fd < 0)
    {
		Socket_Status.CreatStatus = 2;
    }
    even.socket_create_result.socket_fd = socket_fd;
    printf("Socket_TcpClientCreat ok !\n socket_fd = %d\n",socket_fd);

    return socket_fd;
}

void Socket_TcpClientConnect(int socket_fd,char *hostip, int portno)
{
	int connect_cnt = 0;
    int ret = -1;
	wifi_even_t even;
	
	Socket_Status.ConnStatus = 0;
    /* gethostbyname: get the server's DNS entry */
//    server = gethostbyname(hostname);
//    if (server == NULL)
//    {
//        printf("ERROR, no such host as %s\r\n", hostname);
//        return -1;
//    }
//    printf("got server addr \r\n");

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
//    bcopy((char *) server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
//    serveraddr.sin_port = htons(portno);

    serveraddr.sin_port = htons(portno);
    serveraddr.sin_addr.s_addr = inet_addr(hostip);

    /* connect: create a connection with the server */

	ret = connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

	if(ret == 0)
	{
		Socket_Status.ConnStatus = 1;		//成功
	}
	else if(ret < 0)
	{
		Socket_Status.ConnStatus = 2;		//失败
	}
}

int Socket_Select(int socket_fd)
{
	fd_set read_fd;                     // 描述符集 这个将用来测试有没有一个可用的连接
	struct timeval timeout;
	int ret = 0;

//	printf("select socket_fd =%d\n",socket_fd);
	FD_ZERO(&read_fd);                     //总是这样先清空一个描述符集
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;
	FD_SET(socket_fd,&read_fd);    //把sock放入要测试的描述符集 就是说把sock放入了rfd里面 这样下一步调用select对rfd进行测试的时候就会测试sock了(因为我们将sock放入的rdf) 一个描述符集可以包含多个被测试的描述符,

	ret = select(socket_fd+1,&read_fd,0,0, &timeout); //select使用

//	printf("select ret =%d\n",ret);
	if(ret > 0)
	{

		if(FD_ISSET(socket_fd,&read_fd))
		{
			FD_CLR(socket_fd,&read_fd);
//			printf("select return 1 \n");
			return 1;
		}
	}
	return 0;
}


#if 1
uint32_t Socket_Read(int socket_fd,uint8_t *read_buf,uint32_t buf_size)		//socket read 保持阻塞模式
{
	wifi_even_t even;
	int ret = 0;

	Socket_Status.ReadStatus = 0;
	bzero(read_buf, buf_size);
	printf("socket read buf_size = %d\nread_buf addr = %08X\n",buf_size,read_buf);
	ret = read(socket_fd, read_buf, buf_size);
	if (ret > 0)
	{
		printf("Echo from server: %s\n num = %d\n", read_buf,ret);
		Socket_Status.ReadStatus = 1;		//接收成功
		Socket_Status.ReadNum = ret;
	}
	else if (ret == 0)
	{
		printf("Server close\n");
		Socket_Status.ReadStatus = 3;		//服务器关闭socket
		Socket_Status.ReadNum = 0;
	}
	else
	{
		printf("Read false\n");
		Socket_Status.ReadStatus = 2;		//接收失败
		Socket_Status.ReadNum = 0;
	}

	return ret;
}

#else
uint32_t Socket_Read(int socket_fd,uint8_t *read_buf,uint32_t buf_size)		//socket read 为非阻塞模式
{
	int num = 0;
//	char rx_buf[128] = {0};
	wifi_even_t even;
	fd_set read_fd;                     // 描述符集 这个将用来测试有没有一个可用的连接
	struct timeval timeout;
	int ret = 0;

	FD_ZERO(&read_fd);                     //总是这样先清空一个描述符集
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_SET(socket_fd,&read_fd);    //把sock放入要测试的描述符集 就是说把sock放入了rfd里面 这样下一步调用select对rfd进行测试的时候就会测试sock了(因为我们将sock放入的rdf) 一个描述符集可以包含多个被测试的描述符,

	/* print the server's reply */
//	bzero(rx_buf, buf_size);
	bzero(read_buf, buf_size);

	ret = select(socket_fd+1,&read_fd,0,0, &timeout); //select使用
//	printf("ret = %d\n",ret);
	if(ret != 0)
	{
		if(1)		//if(FD_ISSET(socket_fd+1,&read_fd))
		{
			//	num = read(socket_fd, rx_buf, buf_size);
			ret = recv(socket_fd, read_buf, buf_size,MSG_DONTWAIT);
			if (ret > 0)
			{
				//memcpy(read_buf,rx_buf,ret);
				printf("Echo from server: %s\n num = %d\n", read_buf,ret);
				even.resp_state = Resp_Success;
			}
			else
			{
				even.resp_state = Resp_False;
			}
			even.socket_read_result.read_num = ret;
			Wifi.Wifi_StatusChange_Handle(&even);
		}
	}

	return ret;
}

#endif

uint32_t Socket_Write(int socket_fd,uint8_t *write_buf,uint32_t buf_len)
{
	int num = 0;
	wifi_even_t even;

	Socket_Status.WriteStatus = 0;
	printf("write socket_fd = %d\n",socket_fd);
	/* send the message line to the server */
	num = write(socket_fd, write_buf, buf_len);
	if (num < 0)
	{
		Socket_Status.WriteStatus = 2;
		Socket_Status.WriteNum = 0;
		printf("ERROR writing to socket\r\n");
	}
	else
	{
		Socket_Status.WriteStatus = 1;
		Socket_Status.WriteNum = num;
	}
	printf("write num = %d\r\nbuf_len = %d\r\n",num,buf_len);
	printf("write data:  %s\r\n",(char *)write_buf);
	
	return 0;
}

int Socket_Close(int socket_fd)
{
	wifi_even_t even;

	Socket_Status.DisconnStatus = 0;
	close(socket_fd);
	Socket_Status.DisconnStatus = 1;
	return 0;
}


