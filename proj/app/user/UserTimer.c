
/********************************************************
 * File	        :  UserTimer.c
 * Description  :  用户定时器管理，该功能提供应用需要使用的定时器管理
 * Author        :  左俊 
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司 
 * Update       :  2016-10-26 
 * History       :  V1.0.0.1 初次构造
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "UserTimer.h"
//#include <TypeDefine.h>


#define USERTIMER_MAX (17)
#define USERTIMER_USERMAX (16)



typedef struct timer_st
{
    uint32_t          interval;
    TimeStyle       style;
    UserTime_Handle Handle;
    pointer         Data;
    uint32_t          Enabled;
    uint32_t          counter;
    uint32_t          status;
    uint32_t          ID;
	
}UserTimer;

typedef UserTimer* PUserTimer;


static UserTimer UserTImerList[USERTIMER_MAX+1] = {0};




#define TIMER_FULL  (0xFFFF)
#define TIMER_KEYPAD USERTIMER_USERMAX



static uint32_t timermask = 0;



void UserTimer_Initialize(void)
{

}


USERTIMER_ID UserTimer_Register(uint32_t interval,
	                                 UserTime_Handle Handle,
	                                 pointer   Data,
	                                 TimeStyle style)
{
    uint32_t timerid = 0;
    PUserTimer currtimer = NULL;

    while (timerid < USERTIMER_USERMAX)
    {
    
        currtimer = &UserTImerList[timerid];
        
        if (currtimer->Enabled == false)
        {
        
            currtimer->interval = (interval + TIMER_INTERVAL -1)/TIMER_INTERVAL;
            currtimer->Handle = Handle;
            currtimer->style = style;
            currtimer->counter = currtimer->interval;
            currtimer->status = 0;
            currtimer->Enabled = 1;
            currtimer->Data = Data;
            currtimer->ID = timerid+1;
            timermask |= (1<< timerid);
            //SEGGER_RTT_printf(0,"timermask:%08X\r\n",timermask);
            return timerid+1;
        }
        timerid++;
    }

    return ERROR_TimerFull;
    
}

void UserTimer_UnRegister(USERTIMER_ID id)
{
    if (!id)
    {
    
        return;
    }

    id--;

    if (id >= USERTIMER_MAX)
    {
    
        return;
    }

    UserTImerList[id].Enabled = false;
    
    timermask &= (~(1<< id));
    //SEGGER_RTT_printf(0,"timermask:%08X\r\n",timermask);
}

USERTIMER_ID UserTimer_KeypadRegister(uint32_t interval,
	                                 UserTime_Handle Handle,
	                                 pointer   Data,
	                                 TimeStyle style)
{
    PUserTimer currtimer = NULL;
    
    if (UserTImerList[TIMER_KEYPAD].Enabled)
    {
        return ERROR_TimerFull;
    }
    
    currtimer = &UserTImerList[TIMER_KEYPAD];
    currtimer->interval = (interval + TIMER_INTERVAL -1)/TIMER_INTERVAL;
    currtimer->Handle = Handle;
    currtimer->style = style;
    currtimer->counter = currtimer->interval;
    currtimer->status = 0;
    currtimer->Enabled = 1;
    currtimer->Data = Data;
    currtimer->ID = TIMER_KEYPAD+1;
    return TIMER_KEYPAD+1;
    

    
}

void UserTimer_KeypadUnRegister(USERTIMER_ID id)
{
    if (id != TIMER_KEYPAD+1)
    {
        return;
    }
    UserTImerList[TIMER_KEYPAD].Enabled = false;
    
}



uint32_t UserTimer_GetStatus(USERTIMER_ID id)
{
    uint32_t ret = 0;
    if (!id)
    {
    
        return 0;
    }

    id--;
    if (id >= USERTIMER_MAX)
    {
    
        return ERROR_IDInvalid;
    }


    ret = UserTImerList[id].status;

    if ((UserTImerList[id].status == 1) 
        && (UserTImerList[id].style == TS_Loop))
    {
    
        UserTImerList[id].Enabled = true;
        UserTImerList[id].status = 0;
    }

    return ret;
	
}



void UserTimer_Process(void)
{
    uint32_t timerCnt = USERTIMER_MAX;
    PUserTimer currTimer = &UserTImerList[0];
    while(timerCnt)
    {

        if (currTimer->Enabled)
        {

            if ((currTimer->counter) && (!currTimer->status))
            {

                currTimer->counter--;
            }

            if (!currTimer->counter) 
            {

                if (currTimer->Handle)
                {
					printf("currTimer->Handle(currTimer->ID,currTimer->Data)\n");
                    currTimer->Handle(currTimer->ID,currTimer->Data);
                }
                else
                {

                    currTimer->status = 1;
                    if (currTimer->style == TS_Single)
                    {
                        currTimer->Enabled = 0;
                    }
                    
                }


                if (currTimer->style == TS_Loop)
                {
                    
                    currTimer->counter = currTimer->interval;
                }
                else
                {

                    if (currTimer->ID == (TIMER_KEYPAD+1))
                    {
                        UserTimer_KeypadUnRegister(currTimer->ID);
                    }
                    else
                    {
                        UserTimer_UnRegister(currTimer->ID);
                    }
                    
                }

            }

        }
        timerCnt--;
        currTimer++;
    }
}




