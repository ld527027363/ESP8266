

#include "wifi.h"
#include "system_paramter.h"
#include "schedule_task.h"
#include "UserTimer.h"


Resp_Status_t		TimerRespTimeout = Resp_TimeOut;
Wifi_TaskState_t	Wifi_TaskState = state_wifi_init;
Wifi_CmdState_t		Wifi_CmdState = cmd_null;

uint8_t rx_buf[128] = {0};
uint8_t tx_buf[128] = {0};

void Schedule_SetState(Wifi_TaskState_t state)
{
	Wifi_TaskState = state;
}


void Schedule_SendData(void)
{
	printf("Schedule_SendData\n");
	memset(Platform_Param.Rx_Buff,0,sizeof(Platform_Param.Rx_Buff));
	Platform_Param.Rx_Offset = 0;
	Platform_Param.Rx_Len = 1024;
	Platform_Param.Rx_Status = Resp_Null;
	Platform_Param.Tx_Offset = 0;
	Platform_Param.Tx_Status = Resp_Null;
	Schedule_SetState(state_socket_create);
}

void Schdule_TimeOutJob(void)
{

}

void Wifi_InitCallBack(USERTIMER_ID timeid,pointer Data)
{
	
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	printf("Wifi_InitCallBack Wifi.Even.Timeout_ID = %d\n",Wifi.Even.Timeout_ID);
	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		printf("WIfi init success\n");
		Schedule_SetState(state_wifi_connect);
	}
	else if(Wifi.Even.resp_state == Resp_False)
	{
		printf("WIfi init Resp_False\n");

	}
	else if(Wifi.Even.resp_state == Resp_TimeOut)
	{
		printf("WIfi init Resp_TimeOut\n");

	}
}

void Wifi_ConnCallBack(USERTIMER_ID timeid,pointer Data)
{
	
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		printf("Wifi Conn success\n");
		Schedule_SetState(state_wifi_null);
		Platform_Param.Connect_Status = Connect_Success;
	}
	else if(Wifi.Even.resp_state == Resp_False)
	{
		printf("Wifi Conn False\n");
		Schedule_SetState(state_wifi_init);
		Platform_Param.Connect_Status = Connect_False;
	}
	else if(Wifi.Even.resp_state == Resp_TimeOut)
	{
		printf("Wifi Conn False\n");
		Schedule_SetState(state_wifi_init);
		Platform_Param.Connect_Status = Connect_False;
	}
}

void Wifi_SocketCreateCallBack(USERTIMER_ID timeid,pointer Data)
{
	
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		printf("Wifi SocketCreate Resp_Success\n");
		Schedule_SetState(state_socket_connect);
	}
	else if(Wifi.Even.resp_state == Resp_False)
	{
		printf("Wifi SocketCreate Resp_False\n");
		Schedule_SetState(state_wifi_init);
	}
	else if(Wifi.Even.resp_state == Resp_TimeOut)
	{
		printf("Wifi SocketCreate Resp_TimeOut\n");
		Schedule_SetState(state_wifi_init);
	}
	
}

void Wifi_SocketConnCallBack(USERTIMER_ID timeid,pointer Data)
{
	
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		Schedule_SetState(state_socket_write);
		printf("Wifi_Socket_Connect success \n");
	}
	else if(Wifi.Even.resp_state == Resp_False)
	{
		Schedule_SetState(state_socket_close);
		printf("Wifi_Socket_Connect false \n");
	}
	else if(Wifi.Even.resp_state == Resp_TimeOut)
	{
		Schedule_SetState(state_socket_close);
		printf("Wifi_Socket_Connect timeout \n");
	}
	
}

void Wifi_SocketWriteCallBack(USERTIMER_ID timeid,pointer Data)
{
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		Schedule_SetState(state_socket_read);
		printf("socket write success\n");
	}
	if((Wifi.Even.resp_state == Resp_False)||(Wifi.Even.resp_state == Resp_TimeOut))
	{
		Schedule_SetState(state_socket_close);
		printf("socket write false\n");
	}
	
}


void Wifi_SocketReadCallBack(USERTIMER_ID timeid,pointer Data)
{
	Wifi.Even.resp_state = *(Resp_Status_t *)Data;

	UserTimer_UnRegister(Wifi.Even.Timeout_ID);
	if(Wifi.Even.resp_state == Resp_Success)
	{
		//Platform_Param.Rx_Offset = Wifi.Even.socket_read_result.read_num;
		Schedule_SetState(state_socket_close);
		Wifi_SockedReadStop(Resp_Success);
		printf("socket read success\n");
	}
	if((Wifi.Even.resp_state == Resp_False)||(Wifi.Even.resp_state == Resp_TimeOut))
	{
		Schedule_SetState(state_socket_close);
		Wifi_SockedReadStop(Resp_False);
		printf("socket read false\n");
	}

}


void Schdule_NetworkCommJob(void)
{
	switch(Wifi_TaskState)
	{
		case state_wifi_null:
			break;

		case state_wifi_init:
			printf("Wifi_Init start \n");
			Wifi_Init(Wifi.wifi_mode,Wifi.ssid,Wifi.password,Wifi_InitCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(1),Wifi_InitCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			printf("Wifi.Even.Timeout_ID = %d\n",Wifi.Even.Timeout_ID);
			break;

		case state_wifi_connect:
			printf("Wifi_Connect start \n");
			Wifi_Connect(Wifi_ConnCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(30),Wifi_ConnCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			break;

		case state_socket_create:
			printf("Wifi_Soclet_Creat start \n");
			Wifi_Socket_Create(Wifi_SocketCreateCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(20),Wifi_SocketCreateCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			break;

		case state_socket_connect:
			printf("Wifi_Socket_Connect start \n");
			Wifi_Socket_Connect(Wifi.socket_fd,Wifi.server_ip,Wifi.server_port_num,Wifi_SocketConnCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(30),Wifi_SocketConnCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			break;

		case state_socket_write:		//一次只能写 N字节，考虑多次写问题
			printf("Wifi_socket_write data \n");
			Wifi_socket_write(Platform_Param.Tx_Buff,Platform_Param.Tx_Len,Platform_Param.Tx_Offset,Wifi_SocketWriteCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(30),Wifi_SocketWriteCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			break;

		case state_socket_read:
			printf("\n\nwifi task get resp \n\n");
			Wifi_socket_read(Platform_Param.Rx_Buff,Platform_Param.Rx_Len,Platform_Param.Rx_Offset,Wifi_SocketReadCallBack);
			Wifi.Even.Timeout_ID = UserTimer_Register(	USER_TIMER_DELAY_S(30),Wifi_SocketReadCallBack,&TimerRespTimeout,TS_Single);
			Schedule_SetState(state_wifi_null);
			break;

		case state_socket_close:
			printf("Wifi_Socket_Close \n");
			Wifi_Socket_Close(Wifi.socket_fd);
			Schedule_SetState(state_wifi_null);
			break;

		default:
			break;

	}
}


void Schdule_EventJop(void)
{

}
void Schedule_Task(void *pvParameters)
{
	uint32_t cnt = 0;
	uint8_t read_num = 0;

	printf("Schedule_Task start \n");
	Wifi_Register();

	while(1)
	{
		Schdule_NetworkCommJob();
		Schdule_EventJop();
		UserTimer_Process();
		vTaskDelay(100/portTICK_RATE_MS);
//		printf("Schedule_Task \n");
	}
}







