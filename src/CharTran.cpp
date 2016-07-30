

//#include "stdafx.h"
#include <windows.h>
#include "CharTran.h"


CCharTran::CCharTran()
{

}

CCharTran::~CCharTran()
{

}

//
// ��������:		Utf8ToUnicode
// ��������:		UTF8�ַ���ת��ΪUNICODE�ַ���
// ��������:		pszUtf8:	UTF8�ַ���
// ��������:		nLen:		UTF8�ַ�������
// ��������:		pszUnicode:	UNICODE�ַ���
// ��������ֵ:		UNICODE�ַ�������
//
int CCharTran::Utf8ToUnicode(const char *pszUtf8, const int nLen, wchar_t *pszUnicode)
{
 	int nUtf8Len = 0;
	int nUnicodeLen = 0;

 	for(nUtf8Len = 0, nUnicodeLen = 0;
		nUtf8Len < nLen;
		nUtf8Len++, nUnicodeLen++)
	{ 
 		nUtf8Len += cUtf8ToUnicode(&pszUtf8[nUtf8Len],&pszUnicode[nUnicodeLen]);
 	}

 	return nUnicodeLen; 
}
//
// ��������:		Utf8ToGb2312
// ��������:		UTF8�ַ���ת��ΪGb2312�ַ���
// ��������:		pszUtf8:	UTF8�ַ���
// ��������:		nLen:		UTF8�ַ�������
// ��������:		pszGb2312:	Gb2312�ַ���
// ��������ֵ:		Gb2312�ַ�������
//
int CCharTran::Utf8ToGb2312(const char *pszUtf8, const int nLen, char *pszGb2312)
{
 	int nUtf8Len = 0;
	int nGb2312Len = 0;
 	WCHAR wcBuf;

 	for(nUtf8Len=0,nGb2312Len=0;
		nUtf8Len<nLen;
		nUtf8Len++,nGb2312Len++)
	{ 
 		if((unsigned char)pszUtf8[nUtf8Len] <= 0x7f)
		{
			// ASCII 0x00 ~ 0x7f 
 			pszGb2312[nGb2312Len] = pszUtf8[nUtf8Len]; 
 		}
		else
		{
 			nUtf8Len += cUtf8ToUnicode(&pszUtf8[nUtf8Len], &wcBuf);
 			nGb2312Len += cUnicodeToGb2312(&wcBuf,&pszGb2312[nGb2312Len]);
 		} 
 	} 
 	return nGb2312Len; 

}

//
// ��������:		UnicodeToUtf8
// ��������:		UNICODE�ַ���ת��ΪUTF8�ַ���
// ��������:		pszUnicode:	UNICODE�ַ���
// ��������:		nLen:		UNICODE�ַ�������
// ��������:		pszUtf8:	UTF8�ַ���
// ��������ֵ:		UTF8�ַ�������
//
int CCharTran::UnicodeToUtf8(const wchar_t *pszUnicode, const int nLen, char *pszUtf8)
{
	int nUnicodeLen = 0;
	int nUtf8Len = 0;

	for (nUnicodeLen = 0, nUtf8Len = 0;
		nUnicodeLen < nLen;
		nUnicodeLen++, nUtf8Len++)
	{
		nUtf8Len += cUnicodeToUtf8(&pszUnicode[nUnicodeLen], &pszUtf8[nUtf8Len]);
	}

	return nUtf8Len;
}

//
// ��������:		UnicodeToGb2312
// ��������:		UNICODE�ַ���ת��ΪGb2312�ַ���
// ��������:		pszUnicode:	UNICODE�ַ���
// ��������:		nLen:		UNICODE�ַ�������
// ��������:		pszGb2312:	Gb2312�ַ���
// ��������ֵ:		Gb2312�ַ�������
//
int CCharTran::UnicodeToGb2312(const wchar_t *pszUnicode, const int nLen, char *pszGb2312)
{
	int nUnicodeLen = 0;
	int nGb2312Len = 0;

	for (nUnicodeLen = 0, nGb2312Len = 0;
		nUnicodeLen < nLen;
		nUnicodeLen++, nGb2312Len++)
	{
		nGb2312Len += cUnicodeToGb2312(&pszUnicode[nUnicodeLen], &pszGb2312[nGb2312Len]);
	}

	return nGb2312Len;
}

//
// ��������:		Gb2312ToUnicode
// ��������:		Gb2312�ַ���ת��ΪUNICODE�ַ���
// ��������:		pszGb2312:	Gb2312�ַ���
// ��������:		nLen:		Gb2312�ַ�������
// ��������:		pszUnicode:	UNICODE�ַ���
// ��������ֵ:		UNICODE�ַ�������
//
int CCharTran::Gb2312ToUnicode(const char *pszGb2312, const int nLen, wchar_t *pszUnicode)
{
	int nGb2312Len = 0;
	int nUnicodeLen = 0;

	for (nUnicodeLen = 0, nGb2312Len = 0;
		nGb2312Len < nLen;
		nUnicodeLen++, nGb2312Len++)
	{
		nGb2312Len += cGb2312ToUnicode(&pszGb2312[nGb2312Len], &pszUnicode[nUnicodeLen]);
	}

	return nUnicodeLen;
}

//
// ��������:		Gb2312ToUtf8
// ��������:		Gb2312�ַ���ת��ΪUtf8�ַ���
// ��������:		pszGb2312:	Gb2312�ַ���
// ��������:		nLen:		Gb2312�ַ�������
// ��������:		pszUtf8:	Utf8�ַ���
// ��������ֵ:		Utf8�ַ�������
//
int CCharTran::Gb2312ToUtf8(const char *pszGb2312, const int nLen, char *pszUtf8)
{
	int nGb2312Len = 0;
	int nUtf8Len = 0;
	WCHAR wcBuf;

	for (nUtf8Len = 0, nGb2312Len = 0;
		nGb2312Len < nLen;
		nUtf8Len++, nGb2312Len++)
	{
		if ((unsigned char)pszGb2312[nGb2312Len] <= 0x7f)
		{
			pszUtf8[nUtf8Len] = pszGb2312[nGb2312Len];
		}
		else
		{
			nGb2312Len += cGb2312ToUnicode(&pszGb2312[nGb2312Len], &wcBuf);
			nUtf8Len += cUnicodeToUtf8(&wcBuf, &pszUtf8[nUtf8Len]);
		}

	}

	return nUtf8Len;
}

// --------- UTF-8תUnicode������ ---------------- 
int CCharTran::cUtf8ToUnicode(const char *pszUtf8, wchar_t *pszUnicode)
{
	int nRet = 0;
	char *pzsTmpUicode = (char *)pszUnicode;
	unsigned char cIn = (unsigned char)pszUtf8[0];
	if(cIn <= 0x7f)
	{ 
		// ASCII 0x00 ~ 0x7f
		pzsTmpUicode[0] = pszUtf8[0];
	}
	else if(cIn <= 0xdf)
	{
		pzsTmpUicode[0] = (pszUtf8[0]<<6)|(pszUtf8[1]&0x3f);
		pzsTmpUicode[1] = (pszUtf8[0]>>2)&0x0f;
		nRet = 1;
	}
	else if(cIn <= 0xef)
	{
		pzsTmpUicode[0] = (pszUtf8[1]<<6)|(pszUtf8[2]&0x3f);
		pzsTmpUicode[1] = (pszUtf8[0]<<4)|((pszUtf8[1]>>2)&0x0f);
		nRet = 2;
	}
	else if(cIn <= 0xf7)
	{
		pzsTmpUicode[0] = (pszUtf8[2]<<6)|(pszUtf8[3]&0x3f);
		pzsTmpUicode[1] = (pszUtf8[1]<<4)|((pszUtf8[2]>>2)&0x0f);
		pzsTmpUicode[2] = ((pszUtf8[0]<<2)&0x1c)|((pszUtf8[1]>>4)&0x03);
		nRet = 3;
	}
	return nRet;
} 

// ---------- UnicodeתUTF-8������ ------------------ 
int CCharTran::cUnicodeToUtf8(const wchar_t *pszUnicode, char *pszUtf8)
{ 
	int nRet = 0; 
	unsigned char* pszTmpUicode = (unsigned char *)pszUnicode; 
	if((unsigned short)pszUnicode[0] <= 0x7f)
	{
		// ASCII  0x00 ~ 0x7f 
		pszUtf8[0] = (char)pszTmpUicode[0]; 
	}
	else if((unsigned short)pszUnicode[0] <= 0x7ff)
	{
		// 0x080 ~ 0x7ff 
		pszUtf8[0] = 0xc0|(pszTmpUicode[1]<<2)|(pszTmpUicode[0]>>6); 
		pszUtf8[1] = 0x80|(pszTmpUicode[0]&0x3f); 
		nRet = 1; 
	}
	else
	{
		// 0x0800 ~ 0xFFFF 
		pszUtf8[0] = 0xe0|(pszTmpUicode[1]>>4); 
		pszUtf8[1] = 0x80|((pszTmpUicode[1]&0x0f)<<2)|(pszTmpUicode[0]>>6); 
		pszUtf8[2] = 0x80|(pszTmpUicode[0]&0x3f); 
		nRet = 2; 
	} 
	return nRet; 
} 
 
 // ----------- UnicodeתGB2312������ ------------------ 
int CCharTran::cUnicodeToGb2312(const wchar_t *pszUnicode, char *pszGb2312)
{ 
 	int nRet = 0; 
 	if((unsigned short)pszUnicode[0]<= 0x7f)
	{
		// ASCII  0x00 ~ 0x7f 
 		pszGb2312[0] = (char)pszUnicode[0]; 
 	}
	else
	{ 
 		::WideCharToMultiByte(CP_ACP,0,pszUnicode,1,pszGb2312,sizeof(WCHAR),NULL,NULL); 
 		nRet = 1; 
 	}

 	return nRet;
 } 
 
 // ----------- GB2312תUnicode������ ------------------ 
int CCharTran::cGb2312ToUnicode(const char *pszGb2312, wchar_t *pszUnicode)
{ 
 	int nRet = 0; 
 	if((unsigned char)pszGb2312[0] <= 0x7f)
	{
		// ASCII  0x00 ~ 0x7f 
 		pszUnicode[0] = (WCHAR)pszGb2312[0]; 
 	}
	else
	{ 
 		::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pszGb2312,2,pszUnicode,1); 
 		nRet = 1; 
 	}

 	return nRet; 
 } 
