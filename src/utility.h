#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <vector>
#include <string>
using namespace std;
#define LOG_TAG "BackDoor"

//#define  _DEBUG
#define ALIGN(P, ALIGNBYTES)  ( ((unsigned long)P + ALIGNBYTES -1)&~(ALIGNBYTES-1) )  
#define AR_SIZE( a ) sizeof( a ) / sizeof( a[0] )
class AUTO_BUFFER
{
private:
	int m_used_size;
	int m_buff_size;
	int m_alloc_granularity;
	
public:
	AUTO_BUFFER(int init_size, int alloc_granularity = 0)
	{
		if(alloc_granularity == 0)
			m_alloc_granularity = init_size;
		else
			m_alloc_granularity = alloc_granularity;
		
		m_used_size = 0;
		m_buff_size = init_size;
		m_ptr = new char[init_size]; 
	}
	
	~AUTO_BUFFER()
	{
		delete m_ptr;
	}
	
	void Put(char * in_buff, int in_buff_size)
	{
		if(m_used_size + in_buff_size > m_buff_size)
		{
			while(m_buff_size < m_used_size + in_buff_size)
				m_buff_size += m_alloc_granularity;
			
			char * temp_new = new char[m_buff_size];
			char * temp_old = m_ptr;
			
			memcpy(temp_new, temp_old, m_used_size);
			
			delete temp_old;
			m_ptr = temp_new;
		}
		
		memcpy(m_ptr+m_used_size, in_buff, in_buff_size);		
		m_used_size += in_buff_size;
	}
	
	int Size()
	{
		return m_used_size;
	}
	
	char* Get()
	{
		return m_ptr;
	}
	
	char *m_ptr;
};

unsigned int crc32(const unsigned char *buf, int len);
#ifdef _DEBUG
#define LOGD printf
#else
#define LOGD  
#endif
#define ROTATE_LEFT(x, s, n) ((x) << (n)) | ((x) >> ((s) - (n)))		//x有s位则循环左移n
#define ROTATE_RIGHT(x, s, n) ((x) >> (n)) | ((x) << ((s) - (n)))       //x有s位则循环右移n

extern char** environ;
/**************************************************
//
// 函数名称:		Base64_encode
// 函数功能:		Base64算法的编码函数
// 函数参数:		data:		明文数据
// 函数参数:		nlen:		明文数据的长度
// 函数参数:		lpOutBuf:	编码数据缓冲区,如果缓冲区为NULL,那么就返回编码所需要的空间大小
// 函数参数:		nOutSize:	缓冲区大小
// 函数返回值:		编码后的数据长度,如果返回零就表示编码失败
//
************************************************/
int Base64_encode(unsigned char *data, int len,char *lpOutBuf,int nOutSize);
/**************************************************
//
// 函数名称:		Base64_decode
// 函数功能:		Base64算法的解码函数
// 函数参数:		lpIndata:	密文数据
// 函数参数:		nlen:		密文数据的长度
// 函数参数:		lpOutBuf:	解密数据缓冲区,如果此参数传递NULL,就可以返回解密所需的空间大小
// 函数返回值:		解码后的数据长度,如果返回零就表示解密失败
//
************************************************/
int Base64_decode(const char *lpIndata,int nInLen,unsigned char *lpOutBuf,int *nSize);
int readBlockFile(char *path,  char **p);
int printf_log( const char *format, ...);
int  get_log(char ** ppbuf);
void* MALLOC(int len);
void FREE(void* p);
int writeargs(char *path, int argc, char**argv);
int writebuftofile(const char *pPath, unsigned char *pbuf, int len);
int readfiletobuf(char *path,  char **p);
int kmp( unsigned char *matcher, unsigned int mlen, char *pattern, unsigned int plen, int nStartPost);
int ExecDosCmd(char *lpCmd, char **lppOutBuf);
void PrintDump(char *title, char *pbuf, int nlen);
#endif
