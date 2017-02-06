
#ifndef __SCHEDULE_TASK_H__
#define __SCHEDULE_TASK_H__



typedef enum
{
	state_wifi_null = 0,
	state_wifi_init,
	state_wifi_connect,
	state_socket_create,
	state_socket_connect,
	state_socket_write,
	state_socket_read,
	state_socket_close
}Wifi_TaskState_t;

typedef enum
{
	cmd_null = 0,
	cmd_send,
	cmd_check_timeout,
}Wifi_CmdState_t;

void Schedule_SetState(Wifi_TaskState_t state);

void Schedule_SendData(void);

void Schedule_Task(void *pvParameters);







#endif
