/**
 *  spi_interface.c
 *
 * Defines and Macros for the SPI.
 *
 * Copyright @ 2015 Espressif System Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Espressif System Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/**
 * @file spi_interface.c
 * @brief Defines and Macros for the SPI.
 */
#include "flash.h"
#include "spi_interface.h"
#include "esp8266/eagle_soc.h"
#include "esp8266/ets_sys.h"
#include "esp_libc.h"
#include "utility.h"

uint8_t Flash_AddrCheck(uint32_t addr)
{
	if((addr >= FLASH_USER_DATA_END_ADDR) || (addr < FLASH_USER_DATA_START_ADDR))
		return 1;
	else
		return 0;
}

uint8_t Flash_SectorErase(uint32_t addr)
{
	if(Flash_AddrCheck(addr))
		return 1;
	spi_flash_erase_sector(addr/FLASH_SEC_SIZE);
	printf("flash sec erase %08X\n",addr);
	return 0;
}


uint8_t Flash_ReadBuf(uint32 data_addr,uint8 *data_buf,uint8 data_len)
{
	if(Flash_AddrCheck(data_addr))
		return 1;

	if(data_len%4)
		return 2;
	//printf("spi_flash_read data_len = %d\n data_addr = %d",data_len,data_addr);
	spi_flash_read(data_addr, (uint32 *)data_buf, data_len);

	return 0;
}



uint8_t Flash_WriteBuf(uint32 data_addr,uint8 *data_buf,uint8 data_len)
{
	if(Flash_AddrCheck(data_addr))
		return 1;

	if(data_len%4)
		return 2;

	spi_flash_write(data_addr, (uint32 *)data_buf, data_len);

	return 0;
}

/*
uint8_t  CheckSum(uint8_t *uBuff, uint32_t uBuffLen) //reentrant
{
	uint32_t i;
	uint8_t uSum=0;
	for(i=0;i<uBuffLen;i++)
	{
		uSum = uSum + uBuff[i];
	}
	uSum = (~uSum) + 1;
	return uSum;
}
*/
/*
 *函数功能：	Flash写数组，带回读校验
 *
 */

bool Flash_Write_check(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite,bool bCheck)
{
	uint8_t tmpBuf[64] = {0};
	int len = 0,i = 0;
	bool bRet = false;


	len = NumByteToWrite;
	pBuffer[len-1] = CheckSum(pBuffer, len-1);

	for (i=0; i< 3; i++)
	{
		Flash_WriteBuf(WriteAddr,pBuffer,NumByteToWrite);			//Flash_Write(pBuffer,WriteAddr,NumByteToWrite);

		if (!bCheck)
		{
			bRet = true;
		  break;
		}
		os_delay_us(100);
		Flash_ReadBuf(WriteAddr,tmpBuf,NumByteToWrite);				//Flash_BufferRead(tmpBuf,WriteAddr,NumByteToWrite);
		if (CheckSum(tmpBuf, len-1) == pBuffer[len-1])
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}



