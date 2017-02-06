/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "si446x_bsp.h"
//#include "HrAlgorithm.h"
//#include "BoardCfg.h"
//#include "nrf_delay.h"
//#include "mr_wdt.h"

//extern
short Si4463_Rssi_Val;

//const uint8_t RfPwrUpArr[8]= {0x07, RF_POWER_UP};
//const uint8_t RfGpioCfgArr[9] = {0x08,RF_GPIO_PIN_CFG};
//const uint8_t RfGlobalXoTune1Arr[7] = {0x06, RF_GLOBAL_XO_TUNE_2};
//const uint8_t RfGlobalConfig1Arr[6] = {0x05, RF_GLOBAL_CONFIG_1};
//const uint8_t RfIntCtlEnable2Arr[7] = {0x06, RF_INT_CTL_ENABLE_2};
//const uint8_t RfFrrCtlAMode4Arr[9] = {0x08, RF_FRR_CTL_A_MODE_4};
//const uint8_t RfPrembleTxLen9Arr[14] = {0x0D, RF_PREAMBLE_TX_LENGTH_9};
//const uint8_t RfSyncConfig5Arr[10] = {0x09, RF_SYNC_CONFIG_5};
//const uint8_t RfPktCrcConfig1Arr[12] = {0x0B, RF_PKT_CRC_CONFIG_7};


//const uint8_t RfPktLen12Arr[17] = {0x10, RF_PKT_LEN_12};
//const uint8_t RfPktField2CrcArr[17]= {0x10, RF_PKT_FIELD_2_CRC_CONFIG_12};
//const uint8_t RfPktField5CrcArr [17]= {0x10, RF_PKT_FIELD_5_CRC_CONFIG_12};

//const uint8_t RfPktWhtSeed15_8_4Arr[14] = {0x0D, RF_PKT_RX_FIELD_3_CRC_CONFIG_9};

//const uint8_t  RfModemMode12Arr[17]= {0x10, RF_MODEM_MOD_TYPE_12};
//const uint8_t  RfModemFreqdev01Arr[6]= { 0x05, RF_MODEM_FREQ_DEV_0_1};
//const uint8_t  RfModecTxRampDelay8Arr[13]= {0x0C, RF_MODEM_TX_RAMP_DELAY_8};
//const uint8_t  RfModemBcrOsr19Arr[14]= {0x0D, RF_MODEM_BCR_OSR_1_9};
//const uint8_t  RfModemAfcGear7Arr[12]= {0x0B, RF_MODEM_AFC_GEAR_7};
//const uint8_t  RfModemAgcCtl1Arr[6]= {0x05, RF_MODEM_AGC_CONTROL_1};
//const uint8_t  RfModemAgcWindowArr[14]= {0x0D, RF_MODEM_AGC_WINDOW_SIZE_9};

//const uint8_t  RfModemOokCnt18Arr[13]= { 0x0C, RF_MODEM_OOK_CNT1_8};
//const uint8_t  RfModemRssiCompArr[6]= {0x05, RF_MODEM_RSSI_COMP_1};
//const uint8_t  RfModemClkgenBand1Arr[6]= {0x05, RF_MODEM_CLKGEN_BAND_1};
//const uint8_t  RfModemChfltRx1_1Arr[17]= {0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12};
//const uint8_t  RfModemChfltRx1_2Arr[17]= {0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12};
//const uint8_t  RfModemChfltRx2_1Arr[17]= {0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12};
//const uint8_t  RfPaMode4Arr[9]= { 0x08, RF_PA_MODE_4};
//const uint8_t  RfSynthPfdcpArr[12]= {0x0B, RF_SYNTH_PFDCP_CPFF_7};
//const uint8_t RfMatchValueArr[17]= { 0x10, RF_MATCH_VALUE_1_12};
//const uint8_t RfFreqCtlInte8Arr[13]= {0x0C, RF_FREQ_CONTROL_INTE_8};

//const uint8_t	RfRssiValueArr[6] = {0x05,RF_RSSI_VALUE_5};
//const uint8_t	RfTxCrc8Arr[9] = {0x08,RF_TX_CRC_8};
//const uint8_t	RfRxCrc8Arr[9] = {0x08,RF_RX_CRC_8};
//const uint8_t	RfRxFiled8Arr[9] = {0x08,RF_RX_FILED_8};
//const uint8_t	RfPropPktCrcConfig5Arr[6] = {0x05,RF_PROP_PKT_CRC_CONFIG_5};

//const tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;

//const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
 //                       &RadioConfiguration;
//const tRadioConfiguration * pRadioConfiguration =  &RadioConfiguration;

U8 Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
//const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
//                        RADIO_CONFIGURATION_DATA;	
const tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;

//const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
 //                       &RadioConfiguration;
const tRadioConfiguration * pRadioConfiguration =  &RadioConfiguration;


void vRadio_PowerUp(void)
{
  /* Hardware reset the chip */
	si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
//	for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
	nrf_delay_ms(260);

}


uint8_t  AfterCmdHandle(void)
{
	uint8_t cnt = 0;
	while(radio_hal_NirqLevel() == 1)
	{
		nrf_delay_ms(1);
		cnt++;
		if(cnt >= 20)
		{
			break;
		}
	}
	if (radio_hal_NirqLevel() == 0)
	{
			/* Get and clear all interrupts.  An error has occured... */
		si446x_get_int_status(0, 0, 0);		
		
		if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_MASK)
		{
			return SI446X_COMMAND_ERROR;
		}
	}
	return 0;
}


//uint8_t vRadio_Init(void)
//{
//	/* Power Up the radio chip */
//	uint8_t status = 0;
//	
//	vRadio_PowerUp();
//	
//	
//	status = radio_comm_SendCmdGetResp(RfPwrUpArr[0], (uint8_t *)(RfPwrUpArr+1), 0, 0);			//       1
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfGpioCfgArr[0], (uint8_t *)(RfGpioCfgArr+1), 0, 0);          //            2
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfGlobalXoTune1Arr[0], (uint8_t *)(RfGlobalXoTune1Arr+1), 0, 0);//           3
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfGlobalConfig1Arr[0], (uint8_t *)(RfGlobalConfig1Arr+1), 0, 0);//        4
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfIntCtlEnable2Arr[0], (uint8_t *)(RfIntCtlEnable2Arr+1), 0, 0);//        5
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfFrrCtlAMode4Arr[0], (uint8_t *)(RfFrrCtlAMode4Arr+1), 0, 0);//    6
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfPrembleTxLen9Arr[0], (uint8_t *)(RfPrembleTxLen9Arr+1), 0, 0);// 7
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfSyncConfig5Arr[0], (uint8_t *)(RfSyncConfig5Arr+1), 0, 0);//   8
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfPktCrcConfig1Arr[0], (uint8_t *)(RfPktCrcConfig1Arr+1), 0, 0);//   9
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();



//	status = radio_comm_SendCmdGetResp(RfPktLen12Arr[0], (uint8_t *)(RfPktLen12Arr+1), 0, 0);//           11
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfPktField2CrcArr[0], (uint8_t *)(RfPktField2CrcArr+1), 0, 0);    //        12
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfPktField5CrcArr[0], (uint8_t *)(RfPktField5CrcArr+1), 0, 0);//               13
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	
//	status = radio_comm_SendCmdGetResp(RfPktWhtSeed15_8_4Arr[0], (uint8_t *)(RfPktWhtSeed15_8_4Arr+1), 0, 0);  //         10
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	status = radio_comm_SendCmdGetResp(RfModemMode12Arr[0], (uint8_t *)(RfModemMode12Arr+1), 0, 0);//            14
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemFreqdev01Arr[0], (uint8_t *)(RfModemFreqdev01Arr+1), 0, 0); //             15
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModecTxRampDelay8Arr[0], (uint8_t *)(RfModecTxRampDelay8Arr+1), 0, 0);//       16
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemBcrOsr19Arr[0], (uint8_t *)(RfModemBcrOsr19Arr+1), 0, 0);//   17
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemAfcGear7Arr[0], (uint8_t *)(RfModemAfcGear7Arr+1), 0, 0);//     18
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemAgcCtl1Arr[0], (uint8_t *)(RfModemAgcCtl1Arr+1), 0, 0);//    19
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemAgcWindowArr[0], (uint8_t *)(RfModemAgcWindowArr+1), 0, 0);//  20
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemOokCnt18Arr[0], (uint8_t *)(RfModemOokCnt18Arr+1), 0, 0);// 21
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemRssiCompArr[0], (uint8_t *)(RfModemRssiCompArr+1), 0, 0);// 22
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();


//	status = radio_comm_SendCmdGetResp(RfModemClkgenBand1Arr[0], (uint8_t *)(RfModemClkgenBand1Arr+1), 0, 0);// 23
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemChfltRx1_1Arr[0], (uint8_t *)(RfModemChfltRx1_1Arr+1), 0, 0);// 24
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemChfltRx1_2Arr[0], (uint8_t *)(RfModemChfltRx1_2Arr+1), 0, 0);// 25
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfModemChfltRx2_1Arr[0], (uint8_t *)(RfModemChfltRx2_1Arr+1), 0, 0);// 26
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfPaMode4Arr[0], (uint8_t *)(RfPaMode4Arr+1), 0, 0);// 27
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfSynthPfdcpArr[0], (uint8_t *)(RfSynthPfdcpArr+1), 0, 0);// 28
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfMatchValueArr[0], (uint8_t *)(RfMatchValueArr+1), 0, 0);// 29
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();

//	status = radio_comm_SendCmdGetResp(RfFreqCtlInte8Arr[0], (uint8_t *)(RfFreqCtlInte8Arr+1), 0, 0);// 30
//	if (status != 0xFF)
//	{
//		return 2;
//	}	
//	AfterCmdHandle();
//	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////新添加的寄存器：打开CRC 16校验//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
////	const uint8_t	RfRssiValueArr[6] = {0x05,RF_RSSI_VALUE_5};
////	status = radio_comm_SendCmdGetResp(RfRssiValueArr[0], (uint8_t *)(RfRssiValueArr+1), 0, 0);// 30
////	if (status != 0xFF)
////	{
////		return 2;
////	}	
////	AfterCmdHandle();
//////const uint8_t	RfTxCrc8Arr[9] = {0x08,RF_TX_CRC_8};
////	status = radio_comm_SendCmdGetResp(RfTxCrc8Arr[0], (uint8_t *)(RfTxCrc8Arr+1), 0, 0);// 30
////	if (status != 0xFF)
////	{
////		return 2;
////	}	
////	AfterCmdHandle();
//////const uint8_t	RfRxCrc8Arr[9] = {0x08,RF_RX_CRC_8};
////	status = radio_comm_SendCmdGetResp(RfRxCrc8Arr[0], (uint8_t *)(RfRxCrc8Arr+1), 0, 0);// 30
////	if (status != 0xFF)
////	{
////		return 2;
////	}	
////	AfterCmdHandle();
//////const uint8_t	RfRxFiled8Arr[9] = {0x08,RF_RX_FILED_8};
////	status = radio_comm_SendCmdGetResp(RfRxFiled8Arr[0], (uint8_t *)(RfRxFiled8Arr+1), 0, 0);// 30
////	if (status != 0xFF)
////	{
////		return 2;
////	}	
////	AfterCmdHandle();
//////const uint8_t	RfPropPktCrcConfig5Arr[6] = {0x05,RF_PROP_PKT_CRC_CONFIG_5};
////	status = radio_comm_SendCmdGetResp(RfPropPktCrcConfig5Arr[0], (uint8_t *)(RfPropPktCrcConfig5Arr+1), 0, 0);// 30
////	if (status != 0xFF)
////	{
////		return 2;
////	}	
////	AfterCmdHandle();
//	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//	// Read ITs, clear pending ones
//	si446x_get_int_status(0u, 0u, 0u);
//	
//	return 0;
//}

/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   TRUE / FALSE
 *
 *  @note
 *
 */
U8 gRadio_CheckTransmitted(void)
{
	U8 TEMP_data = 0;
	TEMP_data = SPI_IRQ();
	if (TEMP_data == FALSE)
	{
    /* Read ITs, clear pending ones */
		si446x_get_int_status(0u, 0u, 0u);

    /* check the reason for the IT */
		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void  vRadio_StartTx(U8 channel, U8 len,U8 *pioFixRadioPacket)
{
  /* Reset TX FIFO */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

  // Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);

  /* Fill the TX fifo with datas */
	si446x_write_tx_fifo(len, pioFixRadioPacket);

  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
	si446x_start_tx(channel, 0x30, len);//0x0A  gaojun 2015-09-26
}

/****************************************************************************/
/****RSSI 获取函数*****返回有符号整形数据************************************/
/****************************************************************************/

void Si4463GetRssi(void)
{
	unsigned char Tem_Data = 0;
	
	Si4463_Rssi_Val = 0;
	
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(0x53);
	radio_hal_SpiReadData(1, &Tem_Data);
	radio_hal_SetNsel();
	Si4463_Rssi_Val =(Tem_Data/2) - 130;
}


/*!
 *  Check if Packet received IT flag is pending.
 *
 *  @return   TRUE - Packet successfully received / FALSE - No packet pending.
 *
 *  @note
 *
 */
#if 0
BIT gRadio_CheckReceived(U8 *rx_buf,U8 rcv_len)
{

  if (SPI_IRQ() == 0)
  {

    /* check the reason for the IT */
    if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
    {
      /* Blink once LED2 to show Sync Word detected */
			Si4463GetRssi();
    }

    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);
		
    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {

      /* Packet RX */
      si446x_read_rx_fifo(rcv_len, rx_buf);
	  
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

      return TRUE;
    }

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
    {
      /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
    }
  }

  return FALSE;
}
#endif

#if 0

/*!
 *  Check if Packet received IT flag is pending.
 *
 *  @return   TRUE - Packet successfully received / FALSE - No packet pending.
 *
 *  @note
 *
 */
BIT gRadio_CheckReceived_VariablePacket(void)
{
  if (SPI_IRQ() == 0)
  {
    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);

	if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_BIT)
    {
      /* State change to */
      si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
	
	  /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
      
	  /* State change to */
      si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
    }

    /* check the reason for the IT */
    if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
    {
      /* Blink once LED2 to show Sync Word detected */
  //    vHmi_ChangeLedState(eHmi_Led2_c, eHmi_LedBlinkOnce_c);
    }

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {
      /* Blink once LED3 as CRC OK or not enabled */
 //     vHmi_ChangeLedState(eHmi_Led3_c, eHmi_LedBlinkOnce_c);

      /* Read the length of RX_FIFO */
      si446x_fifo_info(0u);

      /* Packet RX */
      si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &fixRadioPacket[0]);


      return TRUE;
    }

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
    {
      /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
    }
  }

  return FALSE;
}


#endif

/*!
 *  Set Radio to RX mode, fixed packet length.
 *
 *  @param channel Freq. Channel
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel,U8 rcv_len)
{
  // Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);
	
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length according to PH */
	si446x_start_rx(channel, 0u, rcv_len,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );

}

void Rf433Init(void)
{
//	spi_master_init(SPI0, SPI_MODE0, 0);
	Si4463_SpiInit();
	vRadio_Init();
	si446x_part_info();
	if(SPI_IRQ() == 0)
		si446x_get_int_status(0,0,0);
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);	// asa
	Spi0Close();
	Feed_Wdog();
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
	uint8_t cnt = 0;
	
	/* Power Up the radio chip */
    vRadio_PowerUp(); 
	/* Load radio configuration */
	while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
	{
		nrf_delay_ms(10); 
		vRadio_PowerUp();
		cnt++;
		if(cnt >= 3)
			break;
	}
	// Read ITs, clear pending ones  
	si446x_get_int_status(0u, 0u, 0u);
}

