/*
 * platform.h
 *
 *  Created on: 2016Äê12ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef APP_USER_PLATFORM_H_
#define APP_USER_PLATFORM_H_


#define Platform_Printf printf



void WIFI_CommTask(void);

void Platform_Task(void *pvParameters);

void ServerComm_task(void);

void OneSendcondLookup(void);




#endif /* APP_USER_PLATFORM_H_ */
