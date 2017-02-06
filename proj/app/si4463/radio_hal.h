/*!
 * File:
 *  radio_hal.h
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

#ifndef _RADIO_HAL_H_
#define _RADIO_HAL_H_


#include "si446x_bsp.h"


void radio_hal_AssertShutdown(void);
void radio_hal_DeassertShutdown(void);

void radio_hal_ClearNsel(void);
void radio_hal_SetNsel(void);
bit radio_hal_NirqLevel(void);

void radio_hal_SpiWriteByte(U8 byteToWrite);
U8 radio_hal_SpiReadByte(void);

void radio_hal_SpiWriteData(U8 byteCount, U8* pData);
void radio_hal_SpiReadData(U8 byteCount, U8* pData);
uint8_t SPI_IRQ(void);

#endif //_RADIO_HAL_H_
