/********************************************************
 * File		: 标准Base64编码/解码程序C语言实现
 * 		  在 Linux Gcc4 和 Windows VC6 中编译通过
 * Author	: Alan
 * Contact	: www.EasyIDE.cn
 * Update	: 2009-09-01
*/
#ifndef ALAN_BASE64_H
#define ALAN_BASE64_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/********************************************************
 * RETURNMALLOC	: 1, 用动态开辟内存返回
 *                0, 使用参数接收返回值
*/
#define RETURNMALLOC 1

/********************************************************
 * B64		: 标准Base64编码表
*/
static char B64[64] =
{
	'A','B','C','D','E','F','G','H',
	'I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X',
	'Y','Z',
	'a','b','c','d','e','f','g','h',
	'i','j','k','l','m','n','o','p',
	'q','r','s','t','u','v','w','x',
	'y','z',
	'0','1','2','3','4','5','6','7',
	'8','9','+','/'
};

/********************************************************
 * base64_encode: 标准base64编码函数
 * 		  返回值：编码结果字符串指针
 * @s		: 待编码的字符串指针
*/
void base64_encode(unsigned char *s,unsigned char *EncodeDataBuff);
void base64_encode1( unsigned char * bindata, unsigned char * base64, int binlength );

void base64_encode_with_len(unsigned char *s,unsigned char *EncodeDataBuff,unsigned int len);
/********************************************************
 * base64_decode: 标准base64解码函数
 * 		  返回值：解码结果字符串指针
 * @s		: 待解码的字符串指针
*/
unsigned char *protocol_base64_decode(unsigned char *s);

#endif//ALAN_BASE64_H