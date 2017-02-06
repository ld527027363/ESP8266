
#ifndef __USERTIMER_H
#define __USERTIMER_H

/********************************************************
 * File	        :  UserTimer.c
 * Description  :  用户定时器管理，该功能提供应用需要使用的定时器管理
 * Author        :  左俊 
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司 
 * Update       :  2016-10-26 
 * History       :  V1.0.0.1 初次构造
 *                    用户定时器支持两种类型，一种是单次一种是循环
 *                    用户定时器触发方式支持非阻塞式和事件通知方式
 *                    在订阅用户定时器时如果设置了Handle，则为事件通知方式
 *                    如果Handle为空，则为非阻塞方式，在非阻塞方式如果定时器被触发
 *                    会停止下来，直到通过获取状态函数获取了定时器的状态后才会继续
 */
//#include "TypeDefine.h"
//#include "DeviceDefine.h"
#include <stdint.h>
#include <stdbool.h>

typedef char*	Pchar;

typedef void*	pointer;

#define TIMER_INTERVAL			(100)				//单位毫秒
#define USER_TIMER_DELAY_MS(num)	(num)				//((num+TIMER_INTERVAL-1)/TIMER_INTERVAL)
#define USER_TIMER_DELAY_S(num)	(num * 1000)		//((num * 1000)/TIMER_INTERVAL)

#define USERTIMER_ID uint32_t

typedef enum
{
    TS_Single,
	TS_Loop,
}TimeStyle;


typedef void (*UserTime_Handle)(USERTIMER_ID timeid,pointer Data);


///定时器ID无效
#define ERROR_IDInvalid        (0xFFFFFFFF)
///定时器不存在
#define ERROR_TimerInvalid     (0xFFFFFFFE)
///定时器已经达到申请的最大个数
#define ERROR_TimerFull        (0xFFFFFFFD)


/**
 * @brief 定时器初始化
 *
 *
 * @param 无
 * @return  无
 * @author 左俊
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_Initialize(void);


/**
 * @brief 用户定时器订阅
 *
 *
 * @param interval 定时器周期 ，定时器周期采用50ms的倍数，如果参数不是50ms的倍数会被取整为50的倍数
 * @param Handle  定时器订阅的事件，如果事件为空，系统将认为是由应用自己通过状态获取去查询是否已触发
 * @param style    定时器的类型，参见TimeStyle 。TS_Single为单次定时器，TS_Loop为循环定时器
 * @return  返回用户定时器的唯一标识号
 * @exception  ERROR_TimerFull 表示无用户定时器供请求
 * @author 左俊
 * @version V5.0.0.1 2016-10-25<BR>
 */
USERTIMER_ID UserTimer_Register(uint32_t interval,
	                                 UserTime_Handle Handle,
									 pointer   Data,
	                                 TimeStyle style);

/**
 * @brief 用户定时器取消订阅
 *
 *
 * @param id 定时器ID
 * @return  无
 * @author 左俊
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_UnRegister(USERTIMER_ID id);

/**
 * @brief 获取用户定时器状态
 *
 *
 * @param id 定时器ID
 * @return  返回定时器状态
 * @exception  返回定时器状态ERROR_IDInvalid 用户定时器ID无效。ERROR_TimerInvalid 用户定时器不存在
 * @author 左俊
 * @version V5.0.0.1 2016-10-25<BR>
 */
uint32_t UserTimer_GetStatus(USERTIMER_ID id);

/**
 * @brief定时器处理函数
 *
 *
 * @param 无
 * @return  无
 * @note 需要由提供计时的中断函数调用
 * @author 左俊
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_Process(void);


#endif


