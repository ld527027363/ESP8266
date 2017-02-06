

#include "si4463_spi.h"
void nrf_delay_ms(uint16 ms)
{
	while(ms--)
	{
		os_delay_us(1000);
	}
}

void Delay_10us(uint16 num)
{
	while( num--)
		{
			os_delay_us(10);
		}
}

void Spi_Delay(uint32_t cnt)
{
	uint32_t i = 0;

	while(cnt--)
	{
		for(i = 0; i < 10; i++);
	}
}

void Si4463_SpiInit(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);

	GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));
	GPIO_AS_INPUT(4);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(12));
	GPIO_AS_INPUT(12);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);

	gpio16_output_conf();
	gpio16_output_set(0);
}

void Si4463_Spi_WriteByte(uint8 data)
{
	signed char i;
//	Si4463_Debug_High();

	SI4463_SPI_CLK_LOW();
	for (i=7; i>=0; i--)
	{
	   if(data&(1<<i))
	   {
		   SI4463_SPI_MOSI_HIGH();
	   }
	   else
	   {
		   SI4463_SPI_MOSI_LOW();
	   }
	   SI4463_SPI_CLK_HIGH();
	   Spi_Delay(SI4463_SPI_DELAY_US_CNT);

	   SI4463_SPI_CLK_LOW();
	   Spi_Delay(SI4463_SPI_DELAY_US_CNT);
	}
	SI4463_SPI_MOSI_LOW();
	SI4463_SPI_CLK_LOW();
//	Si4463_Debug_Low();
}

uint8 Si4463_Spi_ReadByte(void)
{
	int i;
	uint8 r = 0;
	//Si4463_Debug_High();
	SI4463_SPI_CLK_LOW();
	SI4463_SPI_MOSI_HIGH(); // 读取数据时拉高TX？
	Spi_Delay(SI4463_SPI_DELAY_US_CNT);
	for (i=7; i>=0; i--)
	{
		SI4463_SPI_CLK_HIGH();
		Spi_Delay(SI4463_SPI_DELAY_US_CNT);

		if(SI4463_SPI_GET_MISO_STATUS())
		{
			r |= 1 << i;
		}

		SI4463_SPI_CLK_LOW();
		Spi_Delay(SI4463_SPI_DELAY_US_CNT);
	}
	SI4463_SPI_MOSI_LOW();  // 读取完数据拉低TX？
	SI4463_SPI_CLK_LOW();

	Spi_Delay(SI4463_SPI_DELAY_US_CNT);
	//Si4463_Debug_Low();


	return r;
}


void Si4463_Spi_WriteBuf(uint8 *data_buf,uint8 data_len)
{
	uint8 i = 0;

	for(i = 0; i < data_len; i++)
	{
		Si4463_Spi_WriteByte(data_buf[i]);
	}

}

uint8 Si4463_IRQ_GetStatus(void)		//RF433_IRQ_STATUS()
{
	return GPIO_INPUT_GET(GPIO_ID_PIN(4));
}


void Si4463_CS_High(void)
{
	gpio16_output_set(1);
}


void Si4463_CS_Low(void)
{
	gpio16_output_set(0);
}


void Si4463_SDN_High(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
}


void Si4463_SDN_Low(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
}


void Si4463_Debug_High(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 1);
}


void Si4463_Debug_Low(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 0);
}

typedef struct Rf433_BufferData
{
    uint8	  buf[10];
    uint32    bufferlength;
    int       rssi;
}Rf433Buffer;

typedef Rf433Buffer* Rf433buffer;

static Rf433Buffer rf433buffer = {{0},0,0};

uint32 Si4463_RF433_Write(void* Buffer,uint32 BufferLength)
{
	uint8_t tx_buf[16] = {0};
	uint8_t rx_buf[16] = {0};
    Rf433buffer userdata = &rf433buffer;
    uint32_t t;

//    Si4463_SpiInit();	//spi_master_init(SPI0, SPI_MODE0, 0);

    tx_buf[0] = 0x20;

    tx_buf[1] = 1;
    tx_buf[2] = 2;
    tx_buf[3] = 3;
    tx_buf[4] = 4;

    tx_buf[5] = 0x51;
    tx_buf[6] = 0x20;
    tx_buf[7] = 0x20;
    tx_buf[8] = 0xd4;
    tx_buf[9] = 0x2f;

    printf("vRadio_StartTx 1 \n ");
    vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,tx_buf);

   // printf("Si4463_RF433_Write :vRadio_StartTx done \n ");

    t = 50;
    while(--t)
    {

        if(SPI_IRQ() == 0)
            break;
        delay_ms(1);
    }
    si446x_get_int_status(0,0,0);

    if(t == 0)
    {

        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
        //Si4463_SDN_High();	//Spi0Close();
        printf("RF_ERROR_SEND_ERR\n");
        return RF_ERROR_SEND_ERR;
    }

    vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
    t = 50;
    si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
    memset(userdata->buf,0,sizeof(userdata->buf));

    while(--t)
    {
#if 1
        if(gRadio_CheckReceived(rx_buf,RF_RX_LEN,&userdata->rssi) == TRUE)
#else
        memset(buffA,0,sizeof(buffA));
        if(gRadio_CheckReceived(buffA,RF_RX_LEN) == TRUE)
#endif
        {

            userdata->bufferlength = RF_RX_LEN;
            break;
        }
        delay_ms(1);
    }

    si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
//    Si4463_SDN_High();//Spi0Close();
    if(t == 0)
    {

        printf("RF_ERROR_RCV_ERR\n");
        return RF_ERROR_RCV_ERR;
    }
    printf("Si4463_RF433_Write :All done **************************************************************** \n\n ");
    return BufferLength;

}

/*uint8_t Rf433SendData(void)
{
	uint32_t t;
	uint8_t tx_buf[10] = {0,1,2,3,4,0x51,6,7,0x8A,0x5E};
	uint8_t rx_buf[10] = {0};

	Si4463_Debug_High();
	vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,tx_buf);
	t = 50000;
	while(--t)
	{
		if(SPI_IRQ() == 0)
			break;
		os_delay_us(1);
	}
	Si4463_Debug_Low();
	si446x_get_int_status(0,0,0);
	if(t == 0)
	{
		si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);  // asa
		printf("tx err \n");
		return RF_ERROR_SEND_ERR;
	}
//	printf("vRadio_StartRX \n");
	taskENTER_CRITICAL();
	vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
	Si4463_Debug_High();

	t = 35000;
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

	Si4463_Debug_Low();
	while(--t)
	{
		if(gRadio_CheckReceived(rx_buf,RF_RX_LEN) == TRUE)
		{
			printf("rx_buf = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3],rx_buf[4],rx_buf[5],rx_buf[6],rx_buf[7],rx_buf[8],rx_buf[9]);
			break;
		}
	//	os_delay_us(1);
	}
	Si4463_Debug_High();
	taskEXIT_CRITICAL();
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);  // asa
//	Spi0Close();
	if(t == 0)
	{
		printf("rx err t = %d\n",t);
		Si4463_Debug_Low();
		return 0;
	}

	printf("rx ok \n");
	Si4463_Debug_Low();
	//Rf433RcvDataHandle();
	return 0;
}
*/
