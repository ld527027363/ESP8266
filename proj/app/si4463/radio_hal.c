/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */


#include "si446x_bsp.h"
//#include "Spi_master.h"
//#include "BoardCfg.h"



uint8_t SPI_IRQ()		
{
	return (Si4463_IRQ_GetStatus());
}

void RF_NSEL(uint8_t b)
{
	if(b)
	{
		Si4463_CS_High();
	}
	else
	{
		Si4463_CS_Low();
	}
}



void radio_hal_AssertShutdown(void)
{
	Si4463_SDN_High();
}

void radio_hal_DeassertShutdown(void)
{
	Si4463_SDN_Low();
}


void radio_hal_ClearNsel(void)
{
	Si4463_CS_Low();		//RF_NSEL(0);
}

void radio_hal_SetNsel(void)
{
	Si4463_CS_High();		//RF_NSEL(1);
}


bit radio_hal_NirqLevel(void)
{
	U8 return_data = 0;
	return_data = SPI_IRQ();
	return return_data;
}


void radio_hal_SpiWriteByte(U8 byteToWrite)
{
//	bSpi_ReadWriteSpi1(byteToWrite);

	Si4463_Spi_WriteByte(byteToWrite);		//	Spi0WrByte(byteToWrite);
}

U8 radio_hal_SpiReadByte(void)
{
	return Si4463_Spi_ReadByte();			//return Spi0WrByte(0xFF);
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
//	vSpi_WriteDataSpi1(byteCount, pData);
//	U8 i;
//	for(i = 0;i < byteCount;i++)
//	{
//		Spi0WrByte(pData[i]);
//	}
	Si4463_Spi_WriteBuf(pData,byteCount);
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
	//vSpi_ReadDataSpi1(byteCount, pData);
	U8 i;
	for(i = 0;i < byteCount;i++)
	{
		pData[i] = Si4463_Spi_ReadByte(); 		//pData[i] = Spi0WrByte(0xFF);
	}
}

