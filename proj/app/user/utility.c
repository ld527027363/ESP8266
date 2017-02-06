
#include "types.h"
#include "utility.h"

const unsigned short crc16_table[]=   //
{ 
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned char Byte_Parity_Cal(unsigned char Data)
{
	Data = (Data & 0x0F) ^ (Data >> 4);
	Data = (Data & 0x03) ^ (Data >> 2);
	Data = (Data & 0x01) ^ (Data >> 1);
	return Data;		
}

unsigned char  CheckSum(unsigned char *uBuff, unsigned int uBuffLen) //reentrant
{
	unsigned int i;
	unsigned char uSum = 0;

	for(i=0;i<uBuffLen;i++)
	{
		uSum = uSum + uBuff[i];
	}
	uSum = (~uSum) + 1;
	return uSum;
}

unsigned char  CheckSumEx(unsigned char *uBuff, unsigned int uBuffLen) //reentrant
{
	unsigned int i;
	unsigned char uSum=0;
	
	for(i=0; i<uBuffLen; i++)
	{
		uSum = uSum ^ uBuff[i];
	}
	
	return uSum;
}


unsigned short CalBinCRC16(unsigned short iCrcValue,unsigned char * Data,unsigned short Length)
{
	unsigned char da = 0;
	unsigned char data_temp = 0;
	
	while(Length--!=0)
	{
		da=(unsigned char) (iCrcValue/256);// crc ��λda
		iCrcValue <<= 8;//       crc��λ�Ƶ���λ
		data_temp = *Data;
		iCrcValue ^= crc16_table[da^data_temp];
		Data++;
	}
	return iCrcValue;
}

void crc_ccitt_old(unsigned char * CrcValue,unsigned char * Data,unsigned short Length)
{
    unsigned short crc = 0;
    unsigned char da = 0;
    while (Length-- > 0)
    {
        da=(unsigned char) (crc/256);
        crc <<= 8;
        crc ^= crc16_table[da^*Data];
        Data++;
    }
    crc =  ~crc;
	CrcValue[0] = (unsigned char)(crc >>8);
	CrcValue[1] = (unsigned char)(crc & 0x00FF);
}

void crc_ccitt_upgrade(unsigned char * CrcValue,unsigned char * Data,unsigned short Length)
{
    unsigned short crc = 0;
    unsigned char da = 0;
    while (Length-- > 0)
    {
        da=(unsigned char) (crc/256);
        crc <<= 8;
        crc ^= crc16_table[da^*Data];
        Data++;
    }
    crc =  ~crc;
	CrcValue[0] = (unsigned char)(crc >>8);
	CrcValue[1] = (unsigned char)(crc & 0x00FF);
}

unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc)
{
    unsigned short __crc = crc;
    while (len-- > 0)
    __crc = crc16_table[(__crc >> 8 ^ *q++) & 0xff] ^ (__crc << 8);
    return ~__crc;
}
unsigned short CRC16(unsigned char * Data,unsigned short Length)
{
	unsigned short crc;
    	unsigned char da;
		
    	crc = 0xFFFF;
    	while(Length--!=0)
		{
        	da=(unsigned char) (crc/256);
        	crc <<= 8;
        	crc ^= crc16_table[da^*Data];
        	Data++;
    	}

	return crc;
}


//У������
uint8_t checkCrc16(uint8 * dat, int len)
{
	uint16 crc;
	uint8_t bRet = 0;
	
	
	crc = CRC16(dat,len);
	
	if (crc == ((dat[len]<<8)|dat[len+1]))
	{
		bRet = 1;
	}
	
	return bRet;
}


/////////////////////////////////////////////////////////  
//  
// ??:???? BCD ?  
//  
// ??: int Dec                      ?????????  
//      int length                   BCD ?????  
//  
// ??: unsigned char *Bcd           ???? BCD ?  
//  
// ??: 0  success  
//  
// ??:??? BCD ?????  
//  
//////////////////////////////////////////////////////////  
int DectoBCD(int Dec, unsigned char *Bcd, int length)
{  
     int i;  
     int temp;  
   
     for(i=length-1; i>=0; i--)  
     {  
         temp = Dec%100;  
         Bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);  
         Dec /= 100;  
     }  
   
     return 0;  
} 


unsigned char BCD2HEX(unsigned char bcd_data)    //BCD??HEX???     
{   
    unsigned char temp;   
    temp=(bcd_data/16*10 + (bcd_data%16));   
    return temp;   
}

unsigned char HEX2BCD(unsigned char hex_data)    //HEX??BCD???      
{   
    unsigned char temp;   
    temp=(hex_data/10*16 + (hex_data%10));
//temp = 	hex_data;
    return temp;   
}  


#if (EN_FUN_TIMEOUT)

//////   ����Ӧ ��ʱ ����
typedef struct timeout_s{
	u32_t min;        //min timeout
	u32_t max;				//max timeout
	u32_t cur;				//current timeout
	u32_t lst;				//last timeout
	u32_t *pTick;     //ϵͳʱ��
	float dev;    		//�������� Ӱ�� cur
	u32_t tick;        //��ʱ����
	timeout_mode_t mode;//��ʱӰ������
	u32_t mode_cnt;		//���ʱ
	u32_t param;       //���Ӳ���
}timeout_t;


#define CONF_TIMEOUT_NUM			3

#define CONF_TIMEOUT_REC      10

u32_t    s_timeout_rec[CONF_TIMEOUT_REC] = {0};
u32_t    s_timeout_rec_cnt = 0;


#define TIMEOUT_DEV_INIT      (1.5f)


#define  TIMEOUT_BACK_PERT    (1.5f)
#define  TIMEOUT_ADVAN_PERT   (0.8f)


// handle  get set check
#define timeout_check_handle(hdl) (hdl<CONF_TIMEOUT_NUM?TRUE:FALSE)
#define set_cur_hdl(hdl) do{\
g_timeout_cur_hdl = hdl;\
}while(0);

#define get_cur_timeout() (g_timeout_cur_hdl)
#define get_timout_pt(hdl) (&s_timeout_info[hdl])


static struct timeout_s s_timeout_info[CONF_TIMEOUT_NUM] = {0};
HANDLE g_timeout_cur_hdl = 0;


u32_t get_max(u32_t *pDat, u32_t num)
{
	u32_t i=0;
	u32_t ret = 0;
	
	for(i=0; i<num; i++)
	{
		if (pDat[i] > ret)
		{
			ret = pDat[i];
		}
	}
	
	return ret;
}

/*
*  ��ʼ��   mode:
*/
HANDLE  timeout_init(u32_t * pTick, u32_t min, u32_t max, timeout_mode_t mode)
{
	HANDLE hRet = 0xff;
	int i = 0;
	
	for (i=0; i<CONF_TIMEOUT_NUM; i++)
	{
		//maxΪ0ʱ δ��ռ��
		if (s_timeout_info[i].max == 0)
		{
			s_timeout_info[i].pTick = pTick;
			s_timeout_info[i].min = min;
			s_timeout_info[i].max = max;
			s_timeout_info[i].dev = TIMEOUT_DEV_INIT;
			s_timeout_info[i].mode = mode;
			s_timeout_info[i].cur = 0;
			
			
			set_cur_hdl(i);
			hRet = i;
			break;
		}
		
	}
	
	return hRet;
}



/*
*  ����  ����cnt �ӳٳ�ʱ
*/
void  timeout_start(HANDLE hdl, u32_t cnt)
{
	timeout_t *pT = 0;
	u32_t tmp = 0;
	
	if (timeout_check_handle(hdl))
	{
		
		pT = get_timout_pt(hdl);
		
		switch(pT->mode)
		{
			case timeout_mode_null:  //û��Ӱ�����ӣ��ʺϳ�ʱ�Ƚ��ȶ�
				if (pT->lst > 0) 
				{
					pT->cur = pT->lst*pT->dev;
				}
				else  //��һ��
				{
					pT->cur = cnt;
				}
			
				break;
				
			case timeout_mode_max:    //�����������ֵ 
				if (pT->lst > 0) 
				{
					pT->cur = pT->lst *  TIMEOUT_BACK_PERT;
					tmp = get_max(s_timeout_rec,CONF_TIMEOUT_REC)*TIMEOUT_BACK_PERT;
					if (tmp > pT->min)
					pT->cur = pT->cur>tmp ?  pT->cur : tmp;
				}
				else  //��һ��
				{
					pT->cur = pT->min;
				}
			
				break;
			
			case timeout_mode_cnt:   //����Ԥ�г�ʱʱ��
				if (pT->lst > 0) 
				{
					pT->cur = pT->lst*cnt*pT->dev/pT->lst;
				}
				else  //��һ��
				{
					pT->cur = cnt;
				}
				
				pT->mode_cnt = cnt;
				break;
					
			default:
				
				break;
		}
		
		pT->tick = *(pT->pTick);
		
		set_cur_hdl(hdl);
	}
}




/*
*  ֹͣ  ���� �Ƿ���ʵ��ʱ
*/
void  timeout_stop(HANDLE hdl, BOOL bTimeout)
{
	u32_t tmp = 0;
	timeout_t *pT = 0;
	
	
	if (timeout_check_handle(hdl))
	{
		
		pT = get_timout_pt(hdl);
		tmp = (*(pT->pTick)- pT->tick);
		
		if (bTimeout)  // ��ʱ
		{
			if (pT->cur*TIMEOUT_BACK_PERT < pT->max)
			{	
				pT->lst = pT->cur*TIMEOUT_BACK_PERT;
				
				if (pT->mode != timeout_mode_max)
						pT->dev *=TIMEOUT_BACK_PERT;			
			}
		}
		else
		{
			// �ж��Ƿ���Ҫ���˻�ǰ��
			
			
			/* �洢��¼ */
			s_timeout_rec_cnt = (s_timeout_rec_cnt>=CONF_TIMEOUT_REC)?0:s_timeout_rec_cnt;
			s_timeout_rec[s_timeout_rec_cnt++] = tmp;
			
			if (tmp > pT->cur)
			{
				//����
				if (tmp*TIMEOUT_BACK_PERT < pT->max)
				{
					pT->lst = tmp;
					pT->dev *=TIMEOUT_BACK_PERT;
				}
			}
			else
			{
					//ǰ��
				if (tmp*TIMEOUT_ADVAN_PERT > pT->min)
				{
					
					if (pT->mode == timeout_mode_max)
					{	
						tmp = get_max(s_timeout_rec,CONF_TIMEOUT_REC);
						pT->lst = tmp ;	
						if (pT->lst < pT->cur)
						{
							pT->lst = (pT->cur - (pT->cur - pT->lst)/2);
							
						}
						
						pT->dev *=TIMEOUT_ADVAN_PERT;						
					}
					else
					{
						pT->lst = tmp;		
						pT->dev *=TIMEOUT_ADVAN_PERT;
					}
				}
			}
			
		}
		
		if (pT->lst*pT->dev > pT->max)
		{
			pT->lst = pT->max/TIMEOUT_BACK_PERT;
		}
		
		if (pT->lst*pT->dev < pT->min)
		{
			pT->lst = pT->min;
		}
		
	}
	
}

/*
*  ��ȡ��ʱֵ
*/
u32_t  timeout_get(HANDLE hdl)
{
	
#if (EN_TST_TIMEOUT)
	char buf[32] = {0};
	sprintf(buf, "\r\n timeout: %d dev: %f max:%d\r\n ", get_timout_pt(hdl)->cur, get_timout_pt(hdl)->dev,get_max(s_timeout_rec,CONF_TIMEOUT_REC));
	Printf(buf);
#endif
	
	return get_timout_pt(hdl)->cur;

	
}

#endif

