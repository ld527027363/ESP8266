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

#ifndef __FLASH_H__
#define __FLASH_H__

#include "esp_common.h"

#define FLASH_SEC_SIZE						(4*1024)

#define FLASH_USER_DATA_START_ADDR			0x201000		//2052K
#define FLASH_USER_DATA_END_ADDR			0x3FC000		//4080K
#define FLASH_USER_DATA_SIZE				(FLASH_USER_DATA_END_ADDR-FLASH_USER_DATA_START_ADDR)		//2028K
#define FLASH_USER_DATA_SEC_NUM				(FLASH_USER_DATA_SIZE/FLASH_SEC_SIZE)		//507

uint8_t Flash_SectorErase(uint32_t addr);

uint8_t Flash_ReadBuf(uint32 data_addr,uint8 *data_buf,uint8 data_len);

uint8_t Flash_WriteBuf(uint32 data_addr,uint8 *data_buf,uint8 data_len);

bool Flash_Write_check(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite,bool bCheck);


#endif

