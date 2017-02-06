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

#include "esp_common.h"
#include "gpio.h"
#include "wifi.h"
#include "esp8266_wifi.h"
#include "flash.h"
#include "memory.h"
#include "socket_tcp_client.h"
#include "schedule_task.h"
#include "system_paramter.h"
#include "linklayer.h"
#include "platform.h"
#include "rf433_task.h"


LOCAL os_timer_t Rf433_Timer;	//

char state = 0;

void timer_callback()
{

//	system_soft_wdt_feed();
	if(state)
	{
		state = 0;
		GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 1);		//设置GPIO 输出 高电平
//	    printf("LED OFF\n");
	}
	else
	{
		state = 1;
		GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 0);		//设置GPIO 输出低电平
//	    printf("LED ON\n");
	}
}


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void delay_ms(uint16 ms)
{
#if 1
	vTaskDelay(ms/portTICK_RATE_MS);
#else

	while(ms--)
	{
		os_delay_us(1000);
	}
#endif
}






void Led_Task(void *pvParameters)
{
	uint32 cnt = 0;
	uint8_t Rf433RxData[16] = {0};
//	uint8_t write_buf[4] = {1,2,3,4};
//	uint8_t read_buf[4] = {0};
	while(1)
	{
		Sys_GetRtcTime();
		vTaskDelay(5000/portTICK_RATE_MS);
	}
//	FlashQueue_GetParame(&TagRecordFlashQueue);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	FlashQueue_ClearParamFlash(&TagRecordFlashQueue);
	Flash_TagRecordQueueInit();
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);		//
    printf("start led task\n");
    Rf433RxData[0] = 0x55;
    Rf433RxData[1] = 0x01;
    Rf433RxData[2] = 0x00;
    Rf433RxData[3] = 0x12;
    Rf433RxData[4] = 0x34;
    Rf433RxData[5] = 0x50;
    Rf433RxData[6] = 0x00;
    Rf433RxData[7] = 0x04;
    Rf433RxData[8] = 0xAB;
    Rf433RxData[9] = 0xCD;
    Rf433RxData[10] = 0x56;
    Rf433RxData[11] = 0x78;

	while(1)
	{
		Rf433RxData[4]++;
//		GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);		//设置GPIO 输出 高电平
//		delay_ms(200);
//		GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);		//设置GPIO 输出 高电平
//		delay_ms(200);
		cnt++;
		FlashQueue_EnQueue(&TagRecordFlashQueue, Rf433RxData);
		if((cnt % 100) == 0)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);		//设置GPIO 输出 高电平
			FlashQueue_GetParam(&TagRecordFlashQueue);
			printf("cnt = %d\n",cnt);
			printf("TagRecordFlashQueue.cnt = %d\n",TagRecordFlashQueue.cnt);
			printf("TagRecordFlashQueue.head_addr = %X\n",TagRecordFlashQueue.head_addr);
			printf("TagRecordFlashQueue.tail_addr = %X\n",TagRecordFlashQueue.tail_addr);
			printf("TagRecordFlashQueue.param_addr_index = %d\n",TagRecordFlashQueue.param_addr_index);
			printf("TagRecordFlashQueue.max_cnt = %d\n",TagRecordFlashQueue.max_cnt);
			GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);		//设置GPIO 输出 高电平
		}

	}
	vTaskDelete(NULL);
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	uint8_t i = 0;

	uart_init_new();
	printf("Ai-Thinker Technology Co. Ltd.\r\n%s %s\r\n", __DATE__, __TIME__);
    printf("SDK version:%s\n", system_get_sdk_version());

    for(i = 0; i < 5; i++)
    {
        delay_ms(1000);
    	system_soft_wdt_feed();
        printf("delay i = %d\n",i);
    }
	Sys_PaltformParamInit();
    ProtocolInitial();
	ServerCommStateInit();	//初始化 服务器标记

	FlashQueue_ClearParamFlash(&TagRecordFlashQueue);
	Flash_TagRecordQueueInit();
	Rf433_Init();

	os_timer_disarm(&Rf433_Timer);//关闭定时器，相当于清零计时器计数
	os_timer_setfn(&Rf433_Timer, (os_timer_func_t *)Rf433_CheckRecv_state, NULL);//初始化定时器
	os_timer_arm(&Rf433_Timer, 1, 1);//开始定时器计数,1000毫秒后，会调用前面的callback函数 （后面的0表示只运行一次 为1表示循环运行）

//    Flash_TagRecordQueueInit();

//	xTaskCreate(Led_Task, "LED_Task", 256, NULL, 4, NULL);
	xTaskCreate(Wifi_Task, "Wifi_Task", 1024, NULL, 5, NULL);
	xTaskCreate(Schedule_Task, "Schedule_Task", 1024, NULL, 4, NULL);
	xTaskCreate(Platform_Task, "Main_Task", 2048, NULL, 4, NULL);

}

