#ifndef _TYPES_H_
#define _TYPES_H_

#include "c_types.h"

#define	MAX_SOCK_NUM		3	/**< Maxmium number of socket  */

#define SERVER_PKT_NONE  	    	0
#define SERVER_PKT_LOGIN  	    1
#define SERVER_PKT_HEART	    	2
#define SERVER_PKT_SYNCBATCH    3
#define SERVER_PKT_UPGRADE      4

#define COMM_PORT_RS485		    	0
#define COMM_PORT_LAN						1
#define COMM_PORT_WIFI					2



typedef struct
{
	unsigned short err_code;
	unsigned short result_code;
}SERVER_REPLY_CODE;

typedef struct
{
	unsigned char tx_rx_state;		//���ͺͽ���״̬
	unsigned char tx_flag;			//������ɱ�־
	unsigned char rx_flag;			//������ɱ�־
	unsigned short timeout_cnt;	//ͨ�ų�ʱ����
}SERVER_COMM_CTRL;

//typedef char int8;

//typedef unsigned char uint8;

//typedef int int16;

//typedef unsigned short uint16;

//typedef long int32;

//typedef unsigned int uint32;

typedef uint8_t			u_char;		/**< 8-bit value */
typedef uint8 			SOCKET;
typedef uint16			u_short;	/**< 16-bit value */
//typedef uint16_t			u_int;		/**< 16-bit value */
typedef unsigned long			u_long;		/**< 32-bit value */

typedef unsigned char u8_t;
typedef unsigned int u32_t;


typedef struct
{
	unsigned char *send_buf;
	unsigned char *rcv_buf;     
	unsigned short rcv_index;               	//�ѽ������
	unsigned short rcv_len;			//Ԥ���ճ���
	unsigned char rcv_state;			//����״̬��ֵ
	unsigned char rcv_all_flag;         	//������ɱ�ʶ
	unsigned char send_flag ;			//���ڷ��ͱ�ʶ
	unsigned char send_state;			//����״̬��ֵ
	unsigned short send_len;			//Ԥ�����ܳ�
	unsigned short send_index;		//�ѷ������
	
}STRUCT_LONG_UART_PAR;



#ifndef BOOL
#define BOOL             uint8
#endif

#ifndef TRUE
#define TRUE             1
#endif
#ifndef FALSE
#define FALSE            (!TRUE)
#endif


#define HANDLE   u32_t


typedef union _un_l2cval 
{
	u_long	lVal;
	u_char	cVal[4];
}un_l2cval;

typedef union _un_i2cval 
{
	u_int	iVal;
	u_char	cVal[2];
}un_i2cval;

#endif		/* _TYPE_H_ */
