/*!
 * File:
 *  radio_comm.h
 *
 * Description:
 *  This file contains the RADIO communication layer.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */
#ifndef _RADIO_COMM_H_
#define _RADIO_COMM_H_

#include "si446x_bsp.h"

    //#define RADIO_CTS_TIMEOUT 255
   // #define RADIO_CTS_TIMEOUT 1000
			#define RADIO_CTS_TIMEOUT 2000

 //   extern SEGMENT_VARIABLE(radioCmd[16u], U8, SEG_XDATA);
	extern U8 radioCmd[16u];

    U8 radio_comm_GetResp(U8 byteCount, U8* pData);
    void radio_comm_SendCmd(U8 byteCount, U8* pData);
    void radio_comm_ReadData(U8 cmd, bit pollCts, U8 byteCount, U8* pData);
    void radio_comm_WriteData(U8 cmd, bit pollCts, U8 byteCount, U8* pData);

    U8 radio_comm_PollCTS(void);
    U8 radio_comm_SendCmdGetResp(U8 cmdByteCount, U8* pCmdData, \
                             U8 respByteCount, U8* pRespData);
    void radio_comm_ClearCTS(void);



#endif //_RADIO_COMM_H_
