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

#ifndef __SI4463_SPI_H__
#define __SI4463_SPI_H__

#include "esp_common.h"
#include "gpio.h"
//#include "si446x_defs.h"

#define SI4463_SPI_DELAY_US_CNT		1

#define SI4463_SPI_MOSI_HIGH()  GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 1)
#define SI4463_SPI_MOSI_LOW()	GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0)

#define SI4463_SPI_CLK_HIGH()   GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 1)
#define SI4463_SPI_CLK_LOW()	GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 0)
#define SI4463_SPI_GET_MISO_STATUS()	GPIO_INPUT_GET(GPIO_ID_PIN(12))

#define RF_FREQ_TX_CHANNEL		26
#define RF_TX_LEN		10
#define RF433_TX_TIMEOUT    	8000
#define RF433_RX_TIMEOUT		4100
#define SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP	 1
#define RF_ERROR_SEND_ERR 		1
#define RF_ERROR_RCV_ERR		2
#define RF_FREQ_RX_CHANNEL		6
#define RF_RX_LEN		10
#define SI446X_CMD_FIFO_INFO_ARG_RX_BIT  0x02


void nrf_delay(uint16 ms);

void Delay_10us(uint16 num);

void Si4463_SpiInit(void);

void Si4463_Spi_WriteByte(uint8 data);

uint8 Si4463_Spi_ReadByte(void);

void Si4463_Spi_WriteBuf(uint8 *data_buf,uint8 data_len);

uint8 Si4463_IRQ_GetStatus(void);

void Si4463_CS_High(void);

void Si4463_CS_Low(void);

void Si4463_SDN_High(void);

void Si4463_SDN_Low(void);

void Si4463_Debug_High(void);

void Si4463_Debug_Low(void);

uint32 Si4463_RF433_Write(void* Buffer,uint32 BufferLength);

uint8_t Rf433SendData(void);


#endif
