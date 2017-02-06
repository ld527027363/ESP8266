
#ifndef __RF433_TASK_H__
#define __RF433_TASK_H__

#define RF433_DEFAULT_TX_CH			26
#define RF433_DEFAULT_RX_CH			6
#define RF433_DEFAULT_DATA_LEN		10
#define RF433_DEFAULT_TIMEOUT_CNT	5

typedef struct
{
	uint8_t tx_ch;
	uint8_t rx_ch;
	uint8_t data_len;
	uint8_t recv;
	uint32_t tiomeout_cnt;
	uint8_t tx_buf[32];
	uint8_t rx_buf[32];
}Rf433_Param_t;


extern Rf433_Param_t Rf433_Param;

void Rf433_StartRx(uint8_t rx_len,uint8_t channel);

void Rf433_CheckRecv(void);

void Rf433_CheckRecv_state(void);

void Rf433_Init(void);

unsigned char gRadio_CheckReceived_test(uint8_t *rx_buf,uint8_t rcv_len);


#endif
