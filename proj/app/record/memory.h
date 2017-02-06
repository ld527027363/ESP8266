#ifndef __MEMORY_H
#define __MEMORY_H


#include "esp_common.h"
#include "flash.h"


#define RECORD_PARAM_UNIT_LEN			(16)
#define RECORD_UNIT_LEN					(16)

//��¼�����洢����
#define RECORD_PARAM_START_ADDR			(FLASH_USER_DATA_START_ADDR)
#define RECORD_PARAM_SIZE				(32*1024)
#define RECORD_PARAM_SEC_NUM			(RECORD_PARAM_SIZE/FLASH_SEC_SIZE)
#define RECORD_PARAM_END_ADDR			(RECORD_PARAM_START_ADDR + RECORD_PARAM_SIZE)
#define RECORD_PARAM_MAX_CNT			(RECORD_PARAM_SIZE/RECORD_PARAM_UNIT_LEN)


//��¼�洢����
#define RECORD_START_ADDR				(RECORD_PARAM_END_ADDR)
#define RECORD_SIZE						(1024*1024)
#define RECORD_SEC_NUM					(RECORD_PARAM_SIZE/FLASH_SEC_SIZE)
#define RECORD_END_ADDR					(RECORD_PARAM_START_ADDR + RECORD_PARAM_SIZE)
#define RECORD_MAX_CNT					(RECORD_SIZE/RECORD_UNIT_LEN)

#define RECORD_PARAM_FLAG 				(0xAA55AA55)


typedef struct
{	
	uint32_t flash_addr_start;		//�洢��ʼ��ַ����������������
	uint32_t flash_addr_end;		//�洢��ֹ��ַ����������������
	uint32_t flash_addr_param;		//��Ŷ��в����ĵ�ַ
	uint8_t data_len;				//��λ���ݳ���
	uint32_t max_cnt;				//���Ԫ�����ܿռ�/ ���ݳ���- ��ȫ������
	uint32_t cnt;					//��ǰԪ�ظ���
	uint32_t tail_addr;			//��β��ַ
	uint32_t head_addr;			//��ͷ��ַ
	uint32_t param_addr_index;		//�����洢��������ÿ��һ�Σ���1�����1023
	
}FLASH_QUEUE;


typedef struct
{	
	uint32_t flash_addr_start;		//�洢��ʼ��ַ����������������
	uint32_t flash_addr_end;		//�洢��ֹ��ַ����������������
	uint32_t flash_addr_param;		//��Ŷ��в����ĵ�ַ
	uint8_t data_len;				//��λ���ݳ���
	uint32_t max_cnt;				//���Ԫ�����ܿռ�/ ���ݳ���- ��ȫ������
	uint32_t cnt;					//��ǰԪ�ظ���
	uint32_t tail_addr;			//��β��ַ
	uint32_t head_addr;			//��ͷ��ַ
	uint32_t param_addr_index;		//�����洢��������ÿ��һ�Σ���1�����1023
	uint32_t bk_tail_addr;			//���ݶ�β��ַ
	uint32_t bk_cnt;					//���ݵ�ǰԪ�ظ���
}TEMP_FLASH_QUEUE;


void FlashQueue_ClearParamFlash(FLASH_QUEUE *p_FlashQueue);
bool FlashQueue_SaveParam(FLASH_QUEUE *p_FlashQueue);
void FlashQueue_GetParam(FLASH_QUEUE *p_FlashQueue);
unsigned char FlashQueue_EnQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
unsigned char FlashQueue_GetTail(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
unsigned char FlashQueue_DelTail(FLASH_QUEUE *p_FlashQueue);
void Flash_TagRecordClear(void);
void Flash_TagRecordQueueInit(void);
void Flash_PrintTagRecordQueueInit(FLASH_QUEUE *p_FlashQueue);
unsigned char FlashQueue_GetTagData(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
void Flash_EraseData(uint32_t start_addr, uint32_t size);
unsigned char FlashQueue_DeQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
void FlashQueue_ClearQueue(FLASH_QUEUE *p_FlashQueue);
bool FlashQueue_CheckTagData(FLASH_QUEUE *p_FlashQueue, uint8 *dat, int nTryTime);
void FlashQueue_Copy(TEMP_FLASH_QUEUE * p_FlashQueueDis,FLASH_QUEUE *p_FlashQueueSrc);
extern FLASH_QUEUE TagRecordFlashQueue;
extern TEMP_FLASH_QUEUE FlashTempQueue;
extern unsigned char FlashQueueFlag;
unsigned char FlashQueue_IsEmpty(FLASH_QUEUE *p_FlashQueue);

void FlashQueue_FinTagGet(FLASH_QUEUE *p_FlashQueue);

unsigned char FlashQueue_DeQueue_NoSave(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
 
#endif 


