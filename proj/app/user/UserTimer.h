
#ifndef __USERTIMER_H
#define __USERTIMER_H

/********************************************************
 * File	        :  UserTimer.c
 * Description  :  �û���ʱ�������ù����ṩӦ����Ҫʹ�õĶ�ʱ������
 * Author        :  �� 
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾ 
 * Update       :  2016-10-26 
 * History       :  V1.0.0.1 ���ι���
 *                    �û���ʱ��֧���������ͣ�һ���ǵ���һ����ѭ��
 *                    �û���ʱ��������ʽ֧�ַ�����ʽ���¼�֪ͨ��ʽ
 *                    �ڶ����û���ʱ��ʱ���������Handle����Ϊ�¼�֪ͨ��ʽ
 *                    ���HandleΪ�գ���Ϊ��������ʽ���ڷ�������ʽ�����ʱ��������
 *                    ��ֹͣ������ֱ��ͨ����ȡ״̬������ȡ�˶�ʱ����״̬��Ż����
 */
//#include "TypeDefine.h"
//#include "DeviceDefine.h"
#include <stdint.h>
#include <stdbool.h>

typedef char*	Pchar;

typedef void*	pointer;

#define TIMER_INTERVAL			(100)				//��λ����
#define USER_TIMER_DELAY_MS(num)	(num)				//((num+TIMER_INTERVAL-1)/TIMER_INTERVAL)
#define USER_TIMER_DELAY_S(num)	(num * 1000)		//((num * 1000)/TIMER_INTERVAL)

#define USERTIMER_ID uint32_t

typedef enum
{
    TS_Single,
	TS_Loop,
}TimeStyle;


typedef void (*UserTime_Handle)(USERTIMER_ID timeid,pointer Data);


///��ʱ��ID��Ч
#define ERROR_IDInvalid        (0xFFFFFFFF)
///��ʱ��������
#define ERROR_TimerInvalid     (0xFFFFFFFE)
///��ʱ���Ѿ��ﵽ�����������
#define ERROR_TimerFull        (0xFFFFFFFD)


/**
 * @brief ��ʱ����ʼ��
 *
 *
 * @param ��
 * @return  ��
 * @author ��
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_Initialize(void);


/**
 * @brief �û���ʱ������
 *
 *
 * @param interval ��ʱ������ ����ʱ�����ڲ���50ms�ı����������������50ms�ı����ᱻȡ��Ϊ50�ı���
 * @param Handle  ��ʱ�����ĵ��¼�������¼�Ϊ�գ�ϵͳ����Ϊ����Ӧ���Լ�ͨ��״̬��ȡȥ��ѯ�Ƿ��Ѵ���
 * @param style    ��ʱ�������ͣ��μ�TimeStyle ��TS_SingleΪ���ζ�ʱ����TS_LoopΪѭ����ʱ��
 * @return  �����û���ʱ����Ψһ��ʶ��
 * @exception  ERROR_TimerFull ��ʾ���û���ʱ��������
 * @author ��
 * @version V5.0.0.1 2016-10-25<BR>
 */
USERTIMER_ID UserTimer_Register(uint32_t interval,
	                                 UserTime_Handle Handle,
									 pointer   Data,
	                                 TimeStyle style);

/**
 * @brief �û���ʱ��ȡ������
 *
 *
 * @param id ��ʱ��ID
 * @return  ��
 * @author ��
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_UnRegister(USERTIMER_ID id);

/**
 * @brief ��ȡ�û���ʱ��״̬
 *
 *
 * @param id ��ʱ��ID
 * @return  ���ض�ʱ��״̬
 * @exception  ���ض�ʱ��״̬ERROR_IDInvalid �û���ʱ��ID��Ч��ERROR_TimerInvalid �û���ʱ��������
 * @author ��
 * @version V5.0.0.1 2016-10-25<BR>
 */
uint32_t UserTimer_GetStatus(USERTIMER_ID id);

/**
 * @brief��ʱ��������
 *
 *
 * @param ��
 * @return  ��
 * @note ��Ҫ���ṩ��ʱ���жϺ�������
 * @author ��
 * @version V5.0.0.1 2016-10-25<BR>
 */
void UserTimer_Process(void);


#endif


