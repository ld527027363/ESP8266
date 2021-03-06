/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "special_defs.h"

/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
    U8   *Radio_ConfigurationArray;

    U8   Radio_ChannelNumber;
    U8   Radio_PacketLength;
    U8   Radio_State_After_Power_Up;

    U16  Radio_Delay_Cnt_After_Reset;

    U8   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
} tRadioConfiguration;

/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
//extern const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE);
extern const	tRadioConfiguration	*pRadioConfiguration;

/*! Si446x configuration array */
//extern  SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE);
extern U8	Radio_Configuration_Data_Array[];
/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
void vRadio_Init(void);
BIT   gRadio_CheckTransmitted(void);
void  vRadio_StartTx(U8 channel, U8 len,U8 *pioFixRadioPacket);
BIT gRadio_CheckReceived(U8 *rx_buf,U8 rcv_len);
void vRadio_StartRX(U8 channel,U8 rcv_len);
void Rf433Init(void);
#endif /* RADIO_H_ */
