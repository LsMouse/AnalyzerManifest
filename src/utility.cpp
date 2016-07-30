#include <fcntl.h>

#include <Windows.h>
#include "utility.h"

#define  LOGBUF_MAX    0x1000
char *g_szlogbuf = NULL;
unsigned int g_nlogusedSize = 0;
unsigned int g_nlogbufSize = 0;
///Base64 编码表
const char base64_code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//crc32实现函数
unsigned int crc32(const unsigned char *buf, int len)
{
	static unsigned int   CRC32[256] = {0};
	unsigned int   crc;
    unsigned int ret = 0xFFFFFFFF;
    int   i, j;
    if( !CRC32[0] )
    {
		for(i = 0;i < 256;i++)
		{
			crc = i;
			for(j = 0;j < 8;j++)
			{
				if(crc & 1)
				{
					crc = (crc >> 1) ^ 0xEDB88320;
				}
				else
				{
					crc = crc >> 1;
				}
			}
			CRC32[i] = crc;
		}
    }
    for(i = 0; i < len;i++)
    {
		ret = CRC32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
    }
	ret = ~ret;
    return ret;
}
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
int Base64_decode(const char *lpIndata,int nInLen,unsigned char *lpOutBuf,int *nSize)
{
    static char base64_value[256] = {0};
	char *result = (char*)malloc(nInLen + 10);
	int j,i;
    int c;
    long val;
	int nret = 0;
	int sum = 0;
	if(!base64_value[0])
	{
		memset(base64_value,0xFF,256);
		for(i = 0;i < 64;i++)
		{
			base64_value[(int) base64_code[i]] = i;
		}
		base64_value['='] = 0;
	}

    if (!lpIndata)
		return 0;
    val = c = 0;
    for (j = 0,i = 0;i < nInLen;i++) {
		unsigned int k = ((unsigned char) *(lpIndata + i)) % 256;
		if (base64_value[k] < 0)
			continue;
		//记录末尾有几个'='号
		if( k == 0x3D)
		{
			sum++;
		}
		val <<= 6;
		val += base64_value[k];
		if (++c < 4)
			continue;
		/* One quantum of four encoding characters/24 bit */
		result[j++] = (val >> 16) & 0xff;	/* High 8 bits */
		result[j++] = (val >> 8) & 0xff;	/* Mid 8 bits */
		result[j++] = val & 0xff;	/* Low 8 bits */
		val = c = 0;
    }
    nret = j - sum;
    if(nret > *nSize)
    {
		free(result);
		return nret;
    }
//    lpOutBuf[nret] = 0;
    if(lpOutBuf != NULL)
    {
		memcpy(lpOutBuf,result,nret);
    }
	free(result);
	nret = j - sum;
    return nret;
}

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
int Base64_encode(unsigned char *data, int len,char *lpOutBuf,int nOutSize)
{
//    static char result[BASE64_RESULT_SZ];

	if (!data)
		return 0;
	int malSize = ((len - 1) / 3 + 1) * 4 + 1;
	char *result = (char*)malloc(malSize);
	memset(result,0,malSize);
    int bits = 0;
    int char_count = 0;
    int out_cnt = 0;


    while (len--) {
		int c = (unsigned char) *data++;
		bits += c;
		char_count++;
		if (char_count == 3) {
			result[out_cnt++] = base64_code[bits >> 18];
			result[out_cnt++] = base64_code[(bits >> 12) & 0x3f];
			result[out_cnt++] = base64_code[(bits >> 6) & 0x3f];
			result[out_cnt++] = base64_code[bits & 0x3f];
			bits = 0;
			char_count = 0;
		} else {
			bits <<= 8;
		}
    }
    if (char_count != 0) {
		bits <<= 16 - (8 * char_count);
		result[out_cnt++] = base64_code[bits >> 18];
		result[out_cnt++] = base64_code[(bits >> 12) & 0x3f];
		if (char_count == 1) {
			result[out_cnt++] = '=';
			result[out_cnt++] = '=';
		} else {
			result[out_cnt++] = base64_code[(bits >> 6) & 0x3f];
			result[out_cnt++] = '=';
		}
    }
    result[out_cnt] = '\0';	/* terminate */
	if(!lpOutBuf)
	{
		free(result);
		return out_cnt;
	}
	if(nOutSize < out_cnt)
	{
		free(result);
		return 0;
	}
	memcpy(lpOutBuf,result,out_cnt);
	free(result);
    return out_cnt;
}

// 返回读取到数据的长度
int readBlockFile(char *path,  char **p)
{
	FILE *fr = fopen(path, "rb");
	int len = 0x1000;
	if(NULL == fr)
	{
		return 0;
	}
	*p = ( char *)malloc(len + 1);
	memset(*p, 0, len + 1);
	int iret = fread(*p, 1, len, fr);
	fclose(fr);
	return iret;
}
void* MALLOC(int len)
{
	void *p = malloc(len);
	if(p)
	{
		memset(p, 0, len );
	}
	return p;
}
void FREE(void* p)
{
	if(p)
	{
		free(p);
		p = NULL;
	}
}
int printf_log( const char *format, ...)
{
	int cnt = 0;
	char szbuf[1024] = {0};
	if(NULL == g_szlogbuf)
	{
		g_szlogbuf = (char*)MALLOC(LOGBUF_MAX);
		g_nlogbufSize = LOGBUF_MAX;
	}
	va_list   args; 
	va_start(args,format); 
	cnt = vsprintf(szbuf, format, args);
	if(g_szlogbuf && cnt + g_nlogusedSize < g_nlogbufSize )
	{
		strncat(g_szlogbuf, szbuf, g_nlogbufSize);
		g_nlogusedSize += cnt;
	}
	else if(g_szlogbuf)
	{
		char * ptmp = (char*) MALLOC(g_nlogbufSize + LOGBUF_MAX);
		memcpy(ptmp, g_szlogbuf, g_nlogusedSize + 1);
		FREE(g_szlogbuf);
		g_szlogbuf = ptmp;
		g_nlogbufSize += LOGBUF_MAX;
		strncat(g_szlogbuf, szbuf, g_nlogbufSize);
		g_nlogusedSize += cnt;
	}
	else
	{
		cnt = 0;
	}
	va_end(args);
	return cnt;
}
int  get_log(char ** ppbuf)
{
	int sum = 0;
	if(g_szlogbuf)
	{
		*ppbuf = (char*)MALLOC(g_nlogusedSize + 1);
		memcpy(*ppbuf, g_szlogbuf, g_nlogusedSize + 1);
		sum = g_nlogusedSize;
		FREE(g_szlogbuf);
		g_szlogbuf = NULL;
		g_nlogbufSize = 0;
		g_nlogusedSize = 0;
	}
	return sum;
}

void PrintDump(char *title, char *pbuf, int nlen)
{
	if(title)
	{
		printf("%s:", title);
	}
	int i= 0;
	for(i = 0; i < nlen; i++)
	{
		printf("%02X", (unsigned char)pbuf[i]);
	}
	printf("\n");
}



//将 buf 数据写入到文件中
int writebuftofile(const char *pPath, unsigned char *pbuf, int len)
{
	int iret = 0;
	FILE *fw = fopen(pPath, "wb");
	if(NULL == fw)
	{
		return 0;
	}
	iret = fwrite(pbuf, 1, len, fw);
	fclose(fw);
	return iret;
}

int readfiletobuf(char *path,  char **p)
{
	FILE *fr = fopen(path, "rb");
	int len = 0;
	if(NULL == fr)
	{
		return 0;
	}
	fseek(fr, 0, SEEK_END);
	len = ftell(fr);
	fseek(fr, 0, SEEK_SET);
	*p = ( char *)malloc(len + 1);
	memset(*p, 0, len + 1);
	int iret = fread(*p, 1, len, fr);
	fclose(fr);
	return iret;
}


///////////////////////////////////////////////个人修改增强版支持模糊匹配
///////////支持 模糊匹配
void getnext_bin(int sub[], int subSize, int next[])
{
	// 得到next数据,其实本质是自身KMP匹配
	//	printf("sub bin array : ");
	int i,j;
	i = 0;
	j = -1;
	next[0] = -1;
	//printf("%d", next[i]);
	while(i+1 < subSize)
	{
		if(j==-1 || sub[i]==sub[j] || sub[j] == -1)
		{
			++i;
			++j;
#if 1
			if (sub[i] != sub[j])
			{
				next[i] = j;
			}
			else
			{
				next[i] = next[j];
			}

#else
			next[i] = j;
#endif
			//printf(", %d",next[i]);
		}
		else 
		{
			j = next[j];
		}
	}
	//printf("\n");
}


int kmp_bin(unsigned char main[], int mainSize, int sub[], int subSize, int next[])
{
	// 返回s在m中的第一个数据的下标
	int i,j;
	i = 0;
	j = 0;
	int nIndex = -1;
	while(i < mainSize)
	{
		if(j==-1 || ((int)main[i] & 0xFF)==sub[j] || sub[j] == -1)
		{
			++i;
			++j;
			if(j == subSize)
			{
				nIndex = (i-j);
				break;
			}
		}
		else
		{
			j = next[j];
		}
	}
	return nIndex;
}
/////////////////////////////////////
// 函数名称:kmp
// 函数功能:二进制kmp 搜索算法
// 函数参数:matcher:被搜索的地址
// 函数参数:mlen:搜索空间
// 函数参数:pattern: 查找的16进制字符串
// 函数参数:plen:关键字长度
// 函数参数:nStartPost: 缓冲区的起始位置
// 函数返回值:成功返回索引,否则-1
//////////////////////////////////////////
int kmp( unsigned char *matcher, unsigned int mlen, char *pattern, unsigned int plen, int nStartPost)
{
	int iRet = 0;
	int nLen = plen / 2;
	int *lpNext = (int *) malloc( (nLen + 1) * sizeof(int));
	int *lpSub = (int *) malloc( (nLen + 1) * sizeof(int));
	int i = 0;
	memset(lpNext, 0, (nLen + 1) * sizeof(int));
	memset(lpSub, 0, (nLen + 1) * sizeof(int));
	for(i = 0; i < nLen; i ++)
	{
		if(pattern[i * 2] == '?')
		{
			lpSub[i] = -1;
		}
		else
		{
			sscanf(pattern + i * 2, "%02X", lpSub + i);
		}
	}
	getnext_bin(lpSub, nLen, lpNext);
	iRet = kmp_bin(matcher + nStartPost, mlen - nStartPost, lpSub, nLen, lpNext);
	free(lpNext);
	free(lpSub);
	if(iRet < 0)
	{
		return iRet;
	}
	else
	{
		return iRet + nStartPost;
	}
	
}


///////////////////////////////////////////
//
// 函数名称:		ExecDosCmd
// 函数功能:		执行CMD命令
// 函数参数:		lpCmd:			要执行的命令
// 函数参数:		lppOutBuf:		返回命令执行之后的指针
// 函数返回值:		成功返回缓冲区数据大小, 否则为-1
//
////////////////////////////////////
int ExecDosCmd(char *lpCmd, char **lppOutBuf = NULL)
{    
	
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;
	int nRetLen = 0;
	DWORD nBufSize = 0;
	
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
	{
		return -1;
	} 
	char command[1024];    //长达1K的命令行，够用了吧
	strcpy(command,"Cmd.exe /C ");
	strcat(command, lpCmd);
	STARTUPINFO si;
	PROCESS_INFORMATION pi; 
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si); 
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN
	if (!CreateProcess(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return -1;
	}
	CloseHandle(hWrite);
	
	char buffer[4096] = {0};          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。
	DWORD bytesRead; 
	nBufSize = 4096;
	if(lppOutBuf)
	{
		*lppOutBuf = (char *)malloc(4096);
		memset(*lppOutBuf, 0, 4096);
		while (true) 
		{
			if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)
				break;
			//buffer中就是执行的结果，可以保存到文本，也可以直接输出
			if(nBufSize > (nRetLen + bytesRead) )
			{
				memcpy(*lppOutBuf + nRetLen, buffer, bytesRead);
			}
			else
			{
				nBufSize += 4096 + bytesRead;
				char *lpTmp = (char*)malloc(nBufSize);
				memset(lpTmp, 0, nBufSize);
				memcpy(lpTmp, *lppOutBuf, nRetLen);
				free(*lppOutBuf);
				*lppOutBuf = lpTmp;
				memcpy(*lppOutBuf + nRetLen, buffer, bytesRead);
				
			}
			nRetLen += bytesRead; 
			
		}
	}
	else
	{
		while (true) 
		{
			if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)
				break;
		}
	}
	
	CloseHandle(hRead); 
	return nRetLen;
}
