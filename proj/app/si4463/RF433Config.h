/***************************************************************************




****************************************************************************/

const uint8_t RfPwrUpArr[8]= {0x07, RF_POWER_UP};
const uint8_t RfGpioCfgArr[9] = {0x08,RF_GPIO_PIN_CFG};
const uint8_t RfGlobalXoTune1Arr[6] = {0x05, RF_GLOBAL_XO_TUNE_1};
const uint8_t RfGlobalConfig1Arr[6] = {0x05, RF_GLOBAL_CONFIG_1};
const uint8_t RfIntCtlEnable2Arr[7] = {0x06, RF_INT_CTL_ENABLE_2};
const uint8_t RfFrrCtlAMode4Arr[9] = {0x08, RF_FRR_CTL_A_MODE_4};
const uint8_t RfPrembleTxLen9Arr[14] = {0x0D, RF_PREAMBLE_TX_LENGTH_9};
const uint8_t RfSyncConfig5Arr[10] = {0x09, RF_SYNC_CONFIG_5};
const uint8_t RfPktCrcConfig1Arr[6] = {0x05, RF_PKT_CRC_CONFIG_1};
const uint8_t RfPktWhtSeed15_8_4Arr[9] = {0x08, RF_PKT_WHT_SEED_15_8_4};

const uint8_t RfPktLen12Arr[17] = {0x10, RF_PKT_LEN_12};
const uint8_t RfPktField2CrcArr[17]= {0x10, RF_PKT_FIELD_2_CRC_CONFIG_12};
const uint8_t RfPktField5CrcArr [6]= {0x05, RF_PKT_FIELD_5_CRC_CONFIG_1};
const uint8_t  RfModemMode12Arr[17]= {0x10, RF_MODEM_MOD_TYPE_12};
const uint8_t  RfModemFreqdev01Arr[6]= { 0x05, RF_MODEM_FREQ_DEV_0_1};
const uint8_t  RfModecTxRampDelay8Arr[13]= {0x0C, RF_MODEM_TX_RAMP_DELAY_8};
const uint8_t  RfModemBcrOsr19Arr[14]= {0x0D, RF_MODEM_BCR_OSR_1_9};
const uint8_t  RfModemAfcGear7Arr[12]= {0x0B, RF_MODEM_AFC_GEAR_7};
const uint8_t  RfModemAgcCtl1Arr[6]= {0x05, RF_MODEM_AGC_CONTROL_1};
const uint8_t  RfModemAgcWindowArr[13]= {0x0D, RF_MODEM_AGC_WINDOW_SIZE_9};

const uint8_t  RfModemOokCnt18Arr[12]= { 0x0C, RF_MODEM_OOK_CNT1_8};
const uint8_t  RfModemRssiCompArr[6]= {0x05, RF_MODEM_RSSI_COMP_1};
const uint8_t  RfModemClkgenBand1Arr[6]= {0x05, RF_MODEM_CLKGEN_BAND_1};
const uint8_t  RfModemChfltRx1_1Arr[17]= {0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12};
const uint8_t  RfModemChfltRx1_2Arr[17]= {0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12};
const uint8_t  RfModemChfltRx2_1Arr[17]= {0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12};
const uint8_t  RfPaMode4Arr[9]= { 0x08, RF_PA_MODE_4};
const uint8_t  RfSynthPfdcpArr[12]= {0x0B, RF_SYNTH_PFDCP_CPFF_7};
const uint8_t RfMatchValueArr[17]= { 0x10, RF_MATCH_VALUE_1_12};
const uint8_t RfFreqCtlInte8Arr[13]= {0x0C, RF_FREQ_CONTROL_INTE_8};

