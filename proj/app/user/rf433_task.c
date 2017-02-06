

#include "si446x_bsp.h"
#include "rf433_task.h"
#include "memory.h"
#include <string.h>


Rf433_Param_t Rf433_Param;

void Rf433_ParamInit(void)
{

	Rf433_Param.tx_ch = RF433_DEFAULT_TX_CH;
	Rf433_Param.rx_ch = RF433_DEFAULT_RX_CH;
	Rf433_Param.data_len = RF433_DEFAULT_DATA_LEN;
	Rf433_Param.tiomeout_cnt = RF433_DEFAULT_TIMEOUT_CNT;
	memset(Rf433_Param.tx_buf,0,sizeof(Rf433_Param.tx_buf));
	memset(Rf433_Param.rx_buf,0,sizeof(Rf433_Param.rx_buf));
}

void Rf433_TimerInit(void)
{
//	os_timer_disarm(&timer);//鍏抽棴瀹氭椂鍣紝鐩稿綋浜庢竻闆惰鏃跺櫒璁℃暟
//	os_timer_setfn(&timer, (os_timer_func_t *)Rf433_CheckRecv_state, NULL);//鍒濆鍖栧畾鏃跺櫒
//	os_timer_arm(&timer, 1, 1);//寮�濮嬪畾鏃跺櫒璁℃暟,1姣鍚庯紝浼氳皟鐢ㄥ墠闈㈢殑callback鍑芥暟 锛堝悗闈㈢殑0琛ㄧず鍙繍琛屼竴娆� 涓�1琛ㄧず寰幆杩愯锛�
}

uint8_t Rf433_SendData(uint8_t *tx_buf,uint8_t tx_len,uint8_t tx_channel)
{

	vRadio_StartTx(tx_channel, tx_len,tx_buf);
	return 0;
}

void Rf433_StartRx(uint8_t rx_len,uint8_t channel)
{

	si446x_get_int_status(0,0,0);
	vRadio_StartRX(channel,rx_len);
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
}

#if 0
void Rf433_CheckRecv(void)
{
	return;
	uint8_t rx_buf[10] = {0};
	uint8_t tx_buf[10] = {0x80,0x89,0xC3,0x31,0x00,0x20,0x25,0x1C,0x02,0x00};


	if(gRadio_CheckReceived(rx_buf,10) == TRUE)
	{
		//Si4463_Debug_Low();
		Rf433_SendData(tx_buf,10,26);
		Rf433_StartRx(10,26);
		//Si4463_Debug_High();
		printf("rx_buf = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3],rx_buf[4],rx_buf[5],rx_buf[6],rx_buf[7],rx_buf[8],rx_buf[9]);
	}

}
#endif

/*
 *Rf433_CheckRecv_state 鍑芥暟鏈�闀胯�楁椂1.5ms
 *Rf433_CheckRecv_state 鍑芥暟鏈�鐭�楁椂2us
 *Rf433_CheckRecv_state 鑰楁椂涓棿鍊间负260us
 */
void Rf433_CheckRecv_state(void)
{
	int i;
	static int num = 0;
	static uint32_t tick = 0;
	unsigned char ret = 0;
	static uint8_t tmp_buf[1600] = {0};
	uint8_t *p = tmp_buf;
//	uint8_t rx_buf[10] = {0};
	uint8_t rec_ack[10] = {0x80,0x89,0xC3,0x31,0x00,0x20,0x25,0x1C,0x02,0x00};
	for(i = 0; i < 10; i++)
	{
		Rf433_Param.tx_buf[i] = rec_ack[i];
	}

	ret = gRadio_CheckReceived_test(Rf433_Param.rx_buf,Rf433_Param.data_len);

	if( 4 == ret)
	{
		num++;
		//printf("4 recv data \n");
		Rf433_SendData(Rf433_Param.tx_buf,Rf433_Param.data_len,Rf433_Param.tx_ch);
		//printf("rx_buf = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3],rx_buf[4],rx_buf[5],rx_buf[6],rx_buf[7],rx_buf[8],rx_buf[9]);
		for(i = 0; i < 10; i++)
		{
			printf("%02X ",Rf433_Param.rx_buf[i]);
		}
		printf("\n");

		FlashQueue_EnQueue(&TagRecordFlashQueue, Rf433_Param.rx_buf);
		if(num && 0 == num%100)
		{
			for(i = 0; i < 100; i++)
			{
				FlashQueue_DeQueue(&TagRecordFlashQueue,p);
				p += 16;
			}
			#if 0
			for(i = 0; i < sizeof(tmp_buf); i++)
			{
				printf("%2x ",tmp_buf[i]);
				if(i && 15 == i % 16)
					printf("\n");
			}
			printf("\n");
			#endif
		}

	}
	else if(5 == ret)
	{
		Rf433_StartRx(Rf433_Param.data_len,Rf433_Param.rx_ch);
		printf("5 send ok\n");
	}
	else
	{
		//printf("err ret = %d\n",ret);
	}
	tick++;

}

#if 0
void Rf433_Init(void)
{
	Si4463_SpiInit();

	vRadio_Init();
	si446x_part_info();

	if(SPI_IRQ() == 0)
	{
		printf("si446x_get_int_status\n");
		si446x_get_int_status(0,0,0);
	}
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);  // asa
	delay_ms(1000);
	printf("si4463 start rx data\n");
	Rf433_StartRx(10,26);

}
#endif

#if 0
void Rf433Init(void)
{
	Si4463_SpiInit();

	vRadio_Init();

	if(SPI_IRQ() == 0)
		si446x_get_int_status(0,0,0);
	si446x_part_info();
//	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
	vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
}
#endif

void Rf433_Init(void)
{
	//uint8_t rx_buf[16] = {0};

	Si4463_SpiInit();
	Rf433_ParamInit();

	do
	{
		//Si4463_Debug_High();
		vRadio_Init();

		if(SPI_IRQ() == 0)
		{
			printf("si446x_get_int_status\n");
			si446x_get_int_status(0,0,0);
		}
		si446x_part_info();
		//si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);  // asa
		delay_ms(500);

		Rf433_StartRx(Rf433_Param.data_len,Rf433_Param.rx_ch);

		//si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
		delay_ms(20);

	}while(SPI_IRQ() == 0);
}

#if 0
uint8_t Rf433_SendRecData(uint8_t *p_data)
{
	uint32_t t= 50000;
	uint8_t rx_buf[10] = {0};

	if(p_data)
	{

		vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,p_data);

		while(--t)
		{
			if(SPI_IRQ() == 0)
				break;
			//os_delay_us(1);
		}
		si446x_get_int_status(0,0,0);
		if(t == 0)
		{
			si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
			printf("tx err t = %d\n",t);
			return RF_ERROR_SEND_ERR;
		}
	}

	vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);

	t = 35000;
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

	while(--t)
	{
		if(gRadio_CheckReceived(rx_buf,RF_RX_LEN) == TRUE)
		{
			printf("rx_buf = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3],rx_buf[4],rx_buf[5],rx_buf[6],rx_buf[7],rx_buf[8],rx_buf[9]);
			break;
		}

	}

	if(t == 0)
	{
		printf("rx err t = %d\n",t);
		return 0;
	}
	printf("rx ok \n");

	Rf433_IntHandle();
	return 0;
}
#endif

#if 0
void Rf433_IntHandle(void)
{
	uint8_t tx_buf[10] = {0x80,0x89,0xC3,0x31,0x00,0x20,0x25,0x1C,0x02,0x00};

	vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,tx_buf);
	*/

	/*status = gRadio_CheckReceived(rx_buf,RF_RX_LEN);
	if(status == 1)
	{
		Rf433SendTimeoutCnt = 0;	//脥拢脰鹿路垄脣脥鲁卢脢卤录脝脢媒
		if(Sys_Test_Mode)
			vRadio_StartRX(TEST_MODE_RF433_RX_CH,RF_RX_LEN);
		else
			vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
		si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
	}
	else if(status == 2)
	{
		Rf433_RcvDataHandle(rx_buf);
		if(TestReq == 0x0f)
		{
			test_rf_recv(rx_buf);
		}

	}
	*/
}
#endif

void Rf433_Int_Handle(void)
{
	unsigned char rx_buf[32]={0};
	unsigned char status;
//	unsigned short tim;
//	char sendbuf[64];

	status = gRadio_CheckReceived(rx_buf,10);
	if(status == 1)
	{
		//vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
		//si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
	}
	else if(status == 2)	//陆脫脢脮IRQ
	{
		//Rf433_RcvDataHandle(rx_buf);
	}
}


unsigned char gRadio_CheckReceived_test(uint8_t *rx_buf,uint8_t rcv_len)
{

 if (SPI_IRQ() == 0)
  {
    /* check the reason for the IT */
    if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
    {
      /* Blink once LED2 to show Sync Word detected */
			Si4463GetRssi();
			//printf("si4463 get rssi\n");
    }

    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_BIT)
	{
		//printf("si4463 send ok \n");
		return 5;
	}

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {

      /* Packet RX */
      si446x_read_rx_fifo(rcv_len, rx_buf);

      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

	  //printf("si4463 get  data\n");
      return 4;
    }

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
    {
      /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
    }
  }
	//Si4463_Debug_High();
  return 0;
}

