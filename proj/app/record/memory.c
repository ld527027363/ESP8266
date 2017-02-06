
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "memory.h"

unsigned char FlashQueueFlag;

#define FLASH_DATA_VALID_FLAG	0xAA		//д�ڵ�ַ����ֽ�

FLASH_QUEUE TagRecordFlashQueue;
TEMP_FLASH_QUEUE FlashTempQueue;

void Flash_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	Flash_ReadBuf(ReadAddr,pBuffer,NumByteToRead);
}

/************************************************************************************************
	Ѱ�Ҵ���ʼ��ַ��ʼ����һ����0xFF�ĵ�ַ
************************************************************************************************/
uint32_t Flash_FindLastData(uint32_t start_addr)
{
	uint32_t i = 0;
	uint32_t read_addr = RECORD_PARAM_START_ADDR;
	uint8_t  data[16];

	while(1)
	{
		Flash_ReadBuf(read_addr,data,16);		//	dat = Flash_SpiWrite(DUMMY_BYTE);
		if(data[0] == 0xFF)	// �ҵ����²���λ��
		{
			printf(" i = %d \n",i);
			break;
		}
		else if(data[0] != FLASH_DATA_VALID_FLAG)	//if(data != FLASH_DATA_VALID_FLAG)	//��������
		{
			printf(" i = %d \n",i);
			return 0;
		}

		read_addr += 16;

		if(read_addr >= RECORD_PARAM_END_ADDR)		//���һ��λ��Ҳû�ҵ�0xFF
			return i;
		i += 16;;
	}

	return i;
}

void Flash_EraseData(uint32_t start_addr, uint32_t size)
{
	unsigned short i = 0;
	for(i=0; i< size; i++)
	{
		Flash_SectorErase(start_addr+i*FLASH_SEC_SIZE);
	}
}

void FlashQueue_ClearParamFlash(FLASH_QUEUE *p_FlashQueue)
{
	unsigned char i = 0;

	for(i = 0; i < RECORD_PARAM_SEC_NUM; i++)
	{
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SEC_SIZE*i);
	}
	printf("Flash Queue Clear Param Flash\n");
}


/***************************************************************************************
����������β����FLASH�У������������

*******************************************************************************************/
bool FlashQueue_SaveParam(FLASH_QUEUE *p_FlashQueue)
{
	unsigned int addr;
	unsigned char buf[16] = {0};
	unsigned char buf2[32] = {0};
	int j,sector;
	
	bool bRet = false;
	
	if (p_FlashQueue->param_addr_index >= RECORD_PARAM_MAX_CNT)		//�����洢�ﵽ���� ��մ洢����
	{
		FlashQueue_ClearParamFlash(p_FlashQueue);
		p_FlashQueue->param_addr_index = 0;
	}

	addr = p_FlashQueue->flash_addr_param + (p_FlashQueue->param_addr_index)*16;	//��������洢��ַ

	buf[0] = 0xAA;								//�����������
	buf[1] = (p_FlashQueue->cnt)>>16;
	buf[2] = (p_FlashQueue->cnt)>>8;
	buf[3] = (p_FlashQueue->cnt)&0xFF;

	buf[4] = 0xAA;
	buf[5] = (p_FlashQueue->tail_addr)>>16; 
	buf[6] = (p_FlashQueue->tail_addr)>>8; 
	buf[7] = (p_FlashQueue->tail_addr)&0xFF;

	buf[8] = 0xAA;
	buf[9] = (p_FlashQueue->head_addr)>>16;
	buf[10] = (p_FlashQueue->head_addr)>>8;
	buf[11] = (p_FlashQueue->head_addr)&0xFF;
	bRet = Flash_Write_check(buf,addr,16,true);		//�洢����
  
	if (!bRet)
	{
		sprintf((char *)buf2,"SaveParam err:%d\r\n",p_FlashQueue->param_addr_index);
		printf((char *)buf2);
	}

	p_FlashQueue->param_addr_index++;	//ָ����һ����λ

	return bRet;
//	printf(">> Queue tail:%08X,head:%08X,cnt:%04X\r\n",TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr,TagRecordFlashQueue.cnt);	
}
/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////
void FlashQueue_GetParam(FLASH_QUEUE *p_FlashQueue)
{
	uint32_t i;
	uint8_t buf[16];

	i = Flash_FindLastData(p_FlashQueue->flash_addr_param);
	if((i%16)!= 0)	//����16 �������������ݴ�����
	{
		printf(">> Flash 16 Data Error!!!\r\n");
		FlashQueue_ClearParamFlash(p_FlashQueue);
		i = 0;		
	}
	if(i == 0)
	{
		p_FlashQueue->param_addr_index = 0;
		p_FlashQueue->cnt = 0;
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
		Flash_SectorErase(p_FlashQueue->flash_addr_start);
		FlashQueue_ClearParamFlash(p_FlashQueue);
		return;
	}
	
	
	p_FlashQueue->param_addr_index = i/16;

	i += p_FlashQueue->flash_addr_param;	//ָ�������һ�����ݵ���һ����λ��ַ
	i -= 16;							//�����洢��ַi
	Flash_BufferRead(buf, i, 16);
	p_FlashQueue->cnt = 0;//(buf[0]<<24);
	p_FlashQueue->cnt += (buf[1]<<16);
	p_FlashQueue->cnt += (buf[2]<<8);
	p_FlashQueue->cnt += (buf[3]);

	p_FlashQueue->tail_addr = 0;//(buf[0]<<24);
	p_FlashQueue->tail_addr += (buf[5]<<16);
	p_FlashQueue->tail_addr += (buf[6]<<8);
	p_FlashQueue->tail_addr += (buf[7]);	

	p_FlashQueue->head_addr = 0;//(buf[0]<<24);
	p_FlashQueue->head_addr += (buf[9]<<16);
	p_FlashQueue->head_addr += (buf[10]<<8);
	p_FlashQueue->head_addr += (buf[11]);	

	i = p_FlashQueue->tail_addr + (p_FlashQueue->cnt)*(p_FlashQueue->data_len);
	if(i >=  p_FlashQueue->flash_addr_end)		//������Χ�����ͷ
	{
		i -=  p_FlashQueue->flash_addr_end;		//��������
		i += p_FlashQueue->flash_addr_start;		//�ۻص�FLASH��ʼ����ȥ
	}
	if(i != p_FlashQueue->head_addr)										//��֤���ݵ���Ч��
	{
		FlashQueue_ClearParamFlash(p_FlashQueue);
		p_FlashQueue->param_addr_index = 0;
		p_FlashQueue->cnt = 0;
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
		Flash_SectorErase(p_FlashQueue->flash_addr_start);
		
		printf(">> Flash Data Error!!!\r\n");
	}
}

void FlashQueue_ClearQueue(FLASH_QUEUE *p_FlashQueue)
{
	Flash_SectorErase(p_FlashQueue->flash_addr_param);
	Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SEC_SIZE);
	Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SEC_SIZE*2);

	p_FlashQueue->param_addr_index = 0;
	p_FlashQueue->cnt = 0;
	p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	Flash_SectorErase(p_FlashQueue->flash_addr_start);	
}

unsigned char FlashQueue_IsFull(FLASH_QUEUE *p_FlashQueue)
{

	if(p_FlashQueue->cnt >= p_FlashQueue->max_cnt)
		return 1;
	else
		return 0;
}

unsigned char FlashQueue_IsEmpty(FLASH_QUEUE *p_FlashQueue)
{
	if(p_FlashQueue->cnt  == 0)
		return 1;
	else
		return 0;
}

void FlashQueue_EraseTail(FLASH_QUEUE *p_FlashQueue)
{
	unsigned int cnt;
	Flash_SectorErase(p_FlashQueue->tail_addr);		//������β���ڵ�����

	cnt = (FLASH_SEC_SIZE - ((p_FlashQueue->tail_addr)&(FLASH_SEC_SIZE-1)))/(p_FlashQueue->data_len);	//������������
																	
	p_FlashQueue->tail_addr = ((p_FlashQueue->tail_addr)&(~(FLASH_SEC_SIZE-1)))+FLASH_SEC_SIZE;
													//��β��ַ�Ƶ���һ������ʼ��ַ
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;

	p_FlashQueue->cnt -= cnt;

	FlashQueue_SaveParam(p_FlashQueue);
}

unsigned char FlashQueue_EnQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	bool bRet = false;
	uint32_t addr;
	
	if(FlashQueue_IsFull(p_FlashQueue))
	{
		FlashQueue_EraseTail(p_FlashQueue);
	}
	
	bRet = Flash_Write_check(dat,p_FlashQueue->head_addr,p_FlashQueue->data_len,true);
	
	
	addr = p_FlashQueue->head_addr;
	p_FlashQueue->head_addr += p_FlashQueue->data_len;
	if(p_FlashQueue->head_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;

	if(((p_FlashQueue->head_addr) & (FLASH_SEC_SIZE-1)) == 0)		//����һ���µ�������ʼ
	{
		Flash_SectorErase(p_FlashQueue->head_addr);					//��������
	}

	if (bRet)
	{	
		p_FlashQueue->cnt++;
		
		bRet = FlashQueue_SaveParam(p_FlashQueue);
		
		if(!bRet)
		{
			p_FlashQueue->head_addr = addr;
			p_FlashQueue->cnt--;
			
			printf("EnQueue error! \r\n");
		}
			printf("EnQueue ok! \r\n");
	}
	else
	{
		printf("write error! \r\n");
	}

	return bRet;
}



unsigned char FlashQueue_EnQueueEx(FLASH_QUEUE *p_FlashQueue,unsigned char *dat, uint32_t datLen)
{
	bool bRet = false;
	uint32_t addr;
	uint32_t cnt = 0;
	
	//check data len
	cnt = datLen/p_FlashQueue->data_len;
	if ((datLen%p_FlashQueue->data_len) != 0)
	{
		return bRet;
	}
//	
	
	if(FlashQueue_IsFull(p_FlashQueue)|| (cnt+ p_FlashQueue->cnt) > p_FlashQueue->max_cnt)
	{
		FlashQueue_EraseTail(p_FlashQueue);
	}
	//Flash_PageWrite(dat,p_FlashQueue->head_addr,p_FlashQueue->data_len);
	
	bRet = Flash_Write_check(dat,p_FlashQueue->head_addr,p_FlashQueue->data_len,true);
	
	
		addr = p_FlashQueue->head_addr;
		p_FlashQueue->head_addr += p_FlashQueue->data_len;
		if(p_FlashQueue->head_addr >= p_FlashQueue->flash_addr_end)
			p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;

		if(((p_FlashQueue->head_addr) & (FLASH_SEC_SIZE-1)) == 0)		//����һ���µ�������ʼ
		{
			Flash_SectorErase(p_FlashQueue->head_addr);					//��������
		}
		
    
	if (bRet)
	{	
		p_FlashQueue->cnt++;
		
		bRet = FlashQueue_SaveParam(p_FlashQueue);
		
		if(!bRet)
		{
			p_FlashQueue->head_addr = addr;
			p_FlashQueue->cnt--;
			
			printf("EnQueue error! \r\n");
		}
	}
	else
	{
		printf("write error! \r\n");
	}
	
		

	
//	sprintf(spbuf,"%08X %08X %08X \r\n",\
//	p_FlashQueue->cnt,p_FlashQueue->tail_addr,p_FlashQueue->head_addr);
//	Uart_DebugSendData((uint8_t*)spbuf,32);//asdf
	return bRet;
}



unsigned char FlashQueue_GetTail(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	return 1;
}

unsigned char FlashQueue_DelTail(FLASH_QUEUE *p_FlashQueue)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	FlashQueue_SaveParam(p_FlashQueue);
	return 1;
}

unsigned char FlashQueue_DeQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	FlashQueue_SaveParam(p_FlashQueue);
	return 1;
}

unsigned char FlashQueue_DeQueue_NoSave(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	return 1;
}


#define BIT_FLASH_TAG_CHECKSUM    15
#define LEN_FLASH_TAG_CHECKSUM    15
#define CNT_FLASH_TAG_CHECKESUM   5

//У��flash���� ʧ�ܶ�ξ��ض� 
bool FlashQueue_CheckTagData(FLASH_QUEUE *p_FlashQueue, uint8_t *dat, int nTryTime)
{
	bool bRet = false;
	int i = 0;
	
	nTryTime = (nTryTime > CNT_FLASH_TAG_CHECKESUM) ? CNT_FLASH_TAG_CHECKESUM : nTryTime;
	
	for (i=0 ;i<nTryTime; i++)
	{
		if (dat[BIT_FLASH_TAG_CHECKSUM]  == CheckSum(dat,15))
		{
			bRet = true;
			break;
		}
		FlashQueue_GetTagData(p_FlashQueue, dat);
	}
	
	//��ζ�ȡʧ��
	if (i == nTryTime)
	{
		//do thing
		
	}
	
	return bRet;
}


unsigned char FlashQueue_GetTagData(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	//printf("FlashQueue_IsEmpty\n");
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;

	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);

	return 1;
}

//hcp 2015-10-21
void FlashQueue_FinTagGet(FLASH_QUEUE *p_FlashQueue)
{
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
}

//hcp �޸� copy ����  temp flash ��ز���
void FlashQueue_Copy(TEMP_FLASH_QUEUE * p_FlashQueueDis,FLASH_QUEUE *p_FlashQueueSrc)
{
	p_FlashQueueDis->flash_addr_start 	= p_FlashQueueSrc->flash_addr_start;
	p_FlashQueueDis->flash_addr_end 		= p_FlashQueueSrc->flash_addr_end;
	p_FlashQueueDis->flash_addr_param 	= p_FlashQueueSrc->flash_addr_param;
	p_FlashQueueDis->data_len 			= p_FlashQueueSrc->data_len;
	p_FlashQueueDis->max_cnt 			= p_FlashQueueSrc->max_cnt;
	p_FlashQueueDis->cnt 				= p_FlashQueueSrc->cnt;
	p_FlashQueueDis->tail_addr 			= p_FlashQueueSrc->tail_addr;
	p_FlashQueueDis->head_addr 			= p_FlashQueueSrc->head_addr;
	p_FlashQueueDis->param_addr_index	= p_FlashQueueSrc->param_addr_index;
	
	//��ر�Ǳ��� hcp 2015-10-17
	p_FlashQueueDis->bk_cnt = p_FlashQueueSrc->cnt;
	p_FlashQueueDis->bk_tail_addr = p_FlashQueueSrc->tail_addr;
	
}

void Flash_TagRecordQueueInit(void)
{
	TagRecordFlashQueue.flash_addr_start = RECORD_START_ADDR;
	TagRecordFlashQueue.flash_addr_end = RECORD_START_ADDR + RECORD_SIZE;
	TagRecordFlashQueue.flash_addr_param = RECORD_PARAM_START_ADDR;
	TagRecordFlashQueue.data_len = RECORD_UNIT_LEN;
	TagRecordFlashQueue.max_cnt = (RECORD_SIZE-FLASH_SEC_SIZE)/RECORD_UNIT_LEN;
	FlashQueue_GetParam(&TagRecordFlashQueue);
	printf(">> Queue tail:%08X,head:%08X,cnt:%04X\r\n",TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr,TagRecordFlashQueue.cnt);
}

void Flash_PrintTagRecordQueueInit(FLASH_QUEUE *p_FlashQueue)
{
	p_FlashQueue->flash_addr_start = RECORD_START_ADDR;
	p_FlashQueue->flash_addr_end = RECORD_START_ADDR + RECORD_SIZE;
	p_FlashQueue->flash_addr_param = RECORD_PARAM_START_ADDR;
	p_FlashQueue->data_len = RECORD_UNIT_LEN;
	p_FlashQueue->max_cnt = (RECORD_SIZE-FLASH_SEC_SIZE)/RECORD_UNIT_LEN;
	p_FlashQueue->cnt = TagRecordFlashQueue.max_cnt;
	p_FlashQueue->tail_addr = 0;
	p_FlashQueue->head_addr = 0;
}



