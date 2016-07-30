// AXML.cpp: implementation of the CAXML class.
//
//////////////////////////////////////////////////////////////////////

#include "AXML.h"
#include "CharTran.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Define Header
typedef struct {
    UINT32 magicnumber;
    UINT32 filesize;
} AXMLHEADER;

// Define the string chunk
 struct STRINGCHUNK{
    UINT32 scSignature;
    UINT32 scSize;
    UINT32 scStringCount;
    UINT32 scStyleCount;
    UINT32 scUNKNOWN;
    UINT32 scStringPoolOffset;
    UINT32 scStylePoolOffset;
	UINT32 scStringOffsets[0];
} ;

typedef struct {
    USHORT uSize;
	WCHAR  swBuf[0];
} STRING_ITEM;

typedef struct {
    UINT32 sncSignature;
    UINT32 sncSize;
    UINT32 sncLineNumber;
    UINT32 sncUNKNOWN;
    UINT32 sncPrefix;
    UINT32 sncUri;
} SNCHUNK;

 struct RESOURCEIDCHUNK{
    UINT32 rcSignature;
    UINT32 rcSize;
    UINT32 rcItem[0];
    
} ;


// Define the Attribute Chunk
struct ATTRIBUTECHUNK{
    UINT32 acNamespaceUri;
    UINT32 acName;
    UINT32 acValueStr;
    UINT32 acType;
    UINT32 acData;
} ;


// Define the Start Tag Chunk
typedef struct {
    UINT32 stcSignature;
    UINT32 stcSize;
    UINT32 stcLineNumber;
    UINT32 stcUNKNOWN;
    UINT32 stcNamespaceUri;
    UINT32 stcName;
    UINT32 stcFlags;
    UINT32 stcAttributeCount;
    UINT32 stcClassAttribute;
} STCHUNK;

// Define the Text Chunk
typedef struct {
    UINT32 tcSignature;
    UINT32 tcSize;
    UINT32 tcLineNumber;
    UINT32 tcUNKNOWN0;
    UINT32 tcName;
    UINT32 tcUNKNOWN1;
    UINT32 tcUNNNOWN2;
} TEXTCHUNK;

CAXML::CAXML(char *pFilePath)
{
	char *pbuf = NULL;
	if(pFilePath)
	{
		readfiletobuf(pFilePath, &m_axml_buf);
	}
}
CAXML::CAXML(char* pBuf, unsigned int nSize)
{
	if(pBuf && *(unsigned int*)(pBuf + 4) == nSize)
	{
		m_axml_buf = (char *)MALLOC(nSize);
		memcpy(m_axml_buf, pBuf, nSize);
	}
}
CAXML::~CAXML()
{
	free(m_axml_buf);
	m_axml_buf = NULL;
}

string CAXML::GetItemString(int nIndex)
{
	string retstr;
	STRINGCHUNK *snck = (STRINGCHUNK *)(m_axml_buf + 8);
	if ((0x8+snck->scStringPoolOffset + snck->scStringOffsets[nIndex]) <  (0x8+ snck->scSize)) 
    {
        STRING_ITEM * strItem = (STRING_ITEM *)(m_axml_buf + 0x8+snck->scStringPoolOffset + snck->scStringOffsets[ nIndex ]);
		if(strItem->uSize > 0)
		{
			char *pstrbuf = (char *)MALLOC(strItem->uSize * 2);
			CCharTran cctn;
			cctn.UnicodeToGb2312(strItem->swBuf, strItem->uSize, (char*)pstrbuf);
			retstr = string(pstrbuf);
			free(pstrbuf);
		}
	}
	return retstr;
}
//
// 函数名称:		FindItemString
// 函数功能:		查找字符串 索引
// 函数参数:		inStr:		查找的字符串
// 函数返回值:		成功 返回新字符串的索引   失败返回-1
//
int CAXML::FindItemString(string instr)
{
	string str ;
	STRINGCHUNK *snck = GetStringChunk();
	int i = 0;
	for(i = 0; i < snck->scStringCount; i++)
	{
		str = GetItemString(i);
		if(  str.length() > 0 && str == instr)
		{
			return i;
		}	
	}
	return -1;
}
//
// 函数名称:		AddAXMLString
// 函数功能:		添加字符串
// 函数参数:		inStr:		新的字符串
// 函数返回值:		成功 返回新字符串的索引   失败返回-1
//
int CAXML::AddAXMLString(string inStr)
{
	int iret = -1;
	char *pNewBuf = (char *)MALLOC(GetAXMLSize() * 2 + inStr.length() * 2);
	if(NULL == pNewBuf)
	{
		return iret;
	}
	memcpy(pNewBuf, m_axml_buf, GetAXMLSize());
	STRINGCHUNK *snck = (STRINGCHUNK *)(pNewBuf + sizeof(AXMLHEADER));
	memcpy(pNewBuf + 0x8+snck->scStringPoolOffset + 4, m_axml_buf +  0x8+snck->scStringPoolOffset, snck->scSize - snck->scStringPoolOffset);
	snck->scStringPoolOffset += 4;
	int nnewoxmlpost = snck->scSize + 4;		
	CCharTran chtr;
	WCHAR *pwbuf = (WCHAR *)MALLOC(inStr.length() * 2 + 4);
	if(NULL == pwbuf)
	{
		free(pNewBuf);
		return iret;
	}
	STRING_ITEM *pstritem = NULL;
	int nswlen = chtr.Gb2312ToUnicode((const char*)inStr.c_str(), inStr.length(), pwbuf);
	pstritem = (STRING_ITEM *)MALLOC(nswlen * 2 + 4);
	pstritem->uSize = nswlen;
	memcpy(pstritem->swBuf, pwbuf, nswlen * 2);
	memcpy(pNewBuf + nnewoxmlpost + 8, pstritem, pstritem->uSize * 2 + 4);
	snck->scStringOffsets[snck->scStringCount] = nnewoxmlpost - snck->scStringPoolOffset;
	iret = snck->scStringCount;
	snck->scStringCount++;
	nnewoxmlpost += pstritem->uSize * 2 + 4;
	free(pstritem);
	
	nnewoxmlpost = ALIGN(nnewoxmlpost, 4);
	memcpy(pNewBuf + nnewoxmlpost + 8, m_axml_buf + snck->scSize + 8, GetAXMLSize() - 8 -  snck->scSize);
	*(unsigned int*)(pNewBuf + 4) += nnewoxmlpost - snck->scSize;
	snck->scSize = nnewoxmlpost;
	snck->scStylePoolOffset = snck->scStylePoolOffset ? nnewoxmlpost: 0;
	free(m_axml_buf);	//释放原来空间
	m_axml_buf = pNewBuf;
	return iret;
}

//
// 函数名称:		AlterItemString
// 函数功能:		替换字符串
// 函数参数:		nIndex:		AXML 中的索引
// 函数参数:		inStr:		新的字符串
// 函数返回值:		成功 返回TRUE   失败返回FALSE
//
BOOL CAXML::AlterItemString(unsigned int nIndex, string inStr)
{
	if(nIndex >= GetItemStringCount())
	{
		return FALSE;
	}
	char *pNewBuf = (char *)MALLOC(GetAXMLSize() * 2 + inStr.length() * 2);
	if(NULL == pNewBuf)
	{
		return FALSE;
	}
	memcpy(pNewBuf, m_axml_buf, GetAXMLSize());
	STRINGCHUNK *snck = GetStringChunk();
	int nnewoxmlpost = snck->scSize;		
	CCharTran chtr;
	WCHAR *pwbuf = (WCHAR *)MALLOC(inStr.length() * 2 + 4);
	STRING_ITEM *pstritem = NULL;
	int nswlen = chtr.Gb2312ToUnicode((const char*)inStr.c_str(), inStr.length(), pwbuf);
	pstritem = (STRING_ITEM *)MALLOC(nswlen * 2 + 4);
	pstritem->uSize = nswlen;
	memcpy(pstritem->swBuf, pwbuf, nswlen * 2);
	memcpy(pNewBuf + nnewoxmlpost + 8, pstritem, pstritem->uSize * 2 + 4);
	snck->scStringOffsets[nIndex] = nnewoxmlpost - snck->scStringPoolOffset;
	nnewoxmlpost += pstritem->uSize * 2 + 4;
	free(pstritem);
	
	nnewoxmlpost = ALIGN(nnewoxmlpost, 4);
	memcpy(pNewBuf + nnewoxmlpost + 8, m_axml_buf + snck->scSize + 8, GetAXMLSize() - 8 -  snck->scSize);
	*(unsigned int*)(pNewBuf + 4) += nnewoxmlpost - snck->scSize;
	snck->scSize = nnewoxmlpost;
	snck->scStylePoolOffset = snck->scStylePoolOffset ? nnewoxmlpost: 0;
	free(m_axml_buf);
	m_axml_buf = pNewBuf;
	return TRUE;
}
  
//
// 函数名称:		PrintAllItemString
// 函数功能:		打印所有 索引和字符串
// 函数返回值:		无
//
void CAXML::PrintAllItemString()
{
	STRINGCHUNK *snck = GetStringChunk();
	int i = 0;
	for(i = 0; i < snck->scStringCount; i++)
	{
		if ((0x8+snck->scStringPoolOffset + snck->scStringOffsets[ i ]) <  (0x8+ snck->scSize)) 
        {
            string strtmp = GetItemString(i);
			printf("index: 0x%X, buf: %s\n", i, strtmp.c_str());
		}
	}
}

unsigned int CAXML::GetItemStringCount()
{
	STRINGCHUNK *snck = GetStringChunk();
	return snck->scStringCount;
}

//
// 函数名称:		FindSTCUNKOffset
// 函数功能:		查找 指定的 STCUNK
// 函数参数:		inStr:		STCUNK的名字
// 函数返回值:		成功 返回文件偏移   失败返回 -1
//
int CAXML::FindSTCUNKOffset(const char *stcName)
{
	STRINGCHUNK *snck = GetStringChunk();
	RESOURCEIDCHUNK *resour = GetResourceidChunk();
	int npost = sizeof(AXMLHEADER) + snck->scSize + resour->rcSize + sizeof(SNCHUNK);
	int nSize = GetAXMLSize();
	while(npost < nSize)
	{
		if(0x100102 == *(unsigned int*)(m_axml_buf + npost))
		{
			STCHUNK *startTagChunk = (STCHUNK*)(m_axml_buf + npost);
			string strName = GetItemString(startTagChunk->stcName);
			if( strName.length() > 0 && !strcmp(strName.c_str(), stcName))
			{
				break;
			}
			npost += startTagChunk->stcSize;
		}
		else if(0x100104 == *(unsigned int*)(m_axml_buf + npost))
		{
			TEXTCHUNK *textChunk = (TEXTCHUNK*)(m_axml_buf + npost);
			npost += textChunk->tcSize;
		}
		else
		{
			npost += 0x18;
		}
	}		
	if(npost == nSize)
	{
		return -1;
	}
	return npost;
}

STRINGCHUNK * CAXML::GetStringChunk()
{
	return (STRINGCHUNK *)(m_axml_buf + sizeof(AXMLHEADER));
}

unsigned int CAXML::GetAXMLSize()
{
	AXMLHEADER * head = (AXMLHEADER *)m_axml_buf;
	return head->filesize;
}

RESOURCEIDCHUNK* CAXML::GetResourceidChunk()
{
	STRINGCHUNK *snck = GetStringChunk();
	return (RESOURCEIDCHUNK *)(m_axml_buf + sizeof(AXMLHEADER) + snck->scSize);
}

char* CAXML::GetAXMLBuffer()
{
	return m_axml_buf;
}
//
BOOL CAXML::versionCode(char *pClassName) {
	int i = 0;
	int nClassesNameItemStrIdex = AddAXMLString(string(pClassName));
	if (nClassesNameItemStrIdex < 0)
	{
		return FALSE;
	}
	int nOffset = FindSTCUNKOffset("manifest");
	if (nOffset < 0 || nOffset > GetAXMLSize())
	{
		return FALSE;
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for (i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{
		char *pName = NULL;
		if (attrs[i].acType == 0x3000008)
		{
			string strName = GetItemString(attrs[i].acName);
			if (strName == "versionCode")
			{
				break;
			}
		}
	}
	if (i < startTagChunkApp->stcAttributeCount)
	{
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData = nClassesNameItemStrIdex;
	}
	else
	{
		///......、
		bool find = false;
		for (i = 0; i < startTagChunkApp->stcAttributeCount; i++)
		{
			if (attrs[i].acName == FindItemString("versionCode")) {
				find = true;
				attrs[i].acValueStr = nClassesNameItemStrIdex;
				attrs[i].acData = nClassesNameItemStrIdex;
				return TRUE;
			}
		}
		for (i = 0; i < startTagChunkApp->stcAttributeCount; i++)
		{
			if (attrs[i].acType == 0x12000008 || attrs[i].acType == 0x3000008)
			{
				
				break;
					

			}
		}
		startTagChunkApp->stcSize += sizeof(ATTRIBUTECHUNK);
		startTagChunkApp->stcAttributeCount++;
		nOffset += i * sizeof(ATTRIBUTECHUNK);
		memmove(m_axml_buf + nOffset + sizeof(ATTRIBUTECHUNK), m_axml_buf + nOffset, GetAXMLSize() - nOffset);
		*(unsigned int*)(m_axml_buf + 4) += sizeof(ATTRIBUTECHUNK);		//修改 AXML 文件大小
		attrs[i].acName = FindItemString("versionCode");
		attrs[i].acType = 0x3000008;
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData = nClassesNameItemStrIdex;
	}
	return TRUE;
}

//
BOOL CAXML::SetSDK(char *pClassName) 
{
	int i = 0;
	int nClassesNameItemStrIdex = AddAXMLString(string(pClassName));
	if (nClassesNameItemStrIdex < 0)
	{
		return FALSE;
	}
	int nOffset = FindSTCUNKOffset("manifest");
	if (nOffset < 0 || nOffset > GetAXMLSize())
	{
		return FALSE;
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for (i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{
		char *pName = NULL;
		if (attrs[i].acType == 0x3000008)
		{
			string strName = GetItemString(attrs[i].acName);
			if (strName == "platformBuildVersionCode")
			{
				break;
			}
		}
	}
	if (i < startTagChunkApp->stcAttributeCount)
	{
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData = nClassesNameItemStrIdex;
	}
	else
	{
		///......
		for (i = 0; i < startTagChunkApp->stcAttributeCount; i++)
		{
			if (attrs[i].acType == 0x12000008 || attrs[i].acType == 0x3000008)
			{
				break;
			}
		}
		startTagChunkApp->stcSize += sizeof(ATTRIBUTECHUNK);
		startTagChunkApp->stcAttributeCount++;
		nOffset += i * sizeof(ATTRIBUTECHUNK);
		memmove(m_axml_buf + nOffset + sizeof(ATTRIBUTECHUNK), m_axml_buf + nOffset, GetAXMLSize() - nOffset);
		*(unsigned int*)(m_axml_buf + 4) += sizeof(ATTRIBUTECHUNK);		//修改 AXML 文件大小
		attrs[i].acName = FindItemString("platformBuildVersionCode");
		attrs[i].acType = 0x3000008;
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData = nClassesNameItemStrIdex;
	}
	return TRUE;
}

//
// 函数名称:		SetApplicationEntry
// 函数功能:		设置 Application 入口
// 函数参数:		pClassName:		入口类名
// 函数返回值:		成功 返回TRUE   失败返回FALSE
//

BOOL CAXML::SetApplicationEntry(char *pClassName)
{
	int i = 0;
	int nClassesNameItemStrIdex = AddAXMLString(string(pClassName));
	if(nClassesNameItemStrIdex < 0)
	{
		return FALSE;
	}
	int nOffset = FindSTCUNKOffset("application");
	if(nOffset < 0 || nOffset > GetAXMLSize())
	{
		return FALSE;
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for(i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{	
		char *pName = NULL; 
		if( attrs[i].acType == 0x3000008)
		{
			string strName = GetItemString(attrs[i].acName);
			if(strName == "name")
			{
				break;
			}
		}
	}
	if(i < startTagChunkApp->stcAttributeCount)
	{
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData = nClassesNameItemStrIdex;
	}
	else
	{
		///......
		for(i = 0; i < startTagChunkApp->stcAttributeCount; i++)
		{
			if(attrs[i].acType == 0x12000008 || attrs[i].acType == 0x3000008)
			{
				break;
			}
		}
		startTagChunkApp->stcSize += sizeof(ATTRIBUTECHUNK);
		startTagChunkApp->stcAttributeCount++;
		nOffset += i * sizeof(ATTRIBUTECHUNK);
		memmove(m_axml_buf + nOffset + sizeof(ATTRIBUTECHUNK), m_axml_buf + nOffset, GetAXMLSize() - nOffset);
		*(unsigned int*)(m_axml_buf + 4) += sizeof(ATTRIBUTECHUNK);		//修改 AXML 文件大小
		attrs[i].acName = FindItemString("name");
		attrs[i].acType = 0x3000008;
		attrs[i].acValueStr = nClassesNameItemStrIdex;
		attrs[i].acData =nClassesNameItemStrIdex;
	}
	return TRUE;
}

//
// 函数名称:		DelApplicationEntry
// 函数功能:		删除 Application 入口
// 函数参数:		无
// 函数返回值:		成功 TRUE  失败 FALSE
//
BOOL CAXML::DelApplicationEntry()
{
	int i = 0;
	int nOffset = FindSTCUNKOffset("application");
	if(nOffset < 0 || nOffset > GetAXMLSize())
	{
		return FALSE;
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for(i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{	
		char *pName = NULL; 
		if( attrs[i].acType == 0x3000008)
		{
			string strName = GetItemString(attrs[i].acName);
			if(strName == "name")
			{
				break;
			}
		}
	}
	if(i < startTagChunkApp->stcAttributeCount)
	{
		startTagChunkApp->stcSize -= sizeof(ATTRIBUTECHUNK);
		startTagChunkApp->stcAttributeCount--;
		nOffset += i * sizeof(ATTRIBUTECHUNK);
		memmove(m_axml_buf + nOffset, m_axml_buf + nOffset + sizeof(ATTRIBUTECHUNK), GetAXMLSize() - nOffset - sizeof(ATTRIBUTECHUNK));
		*(unsigned int*)(m_axml_buf + 4) -= sizeof(ATTRIBUTECHUNK);		//修改 AXML 文件大小
		return TRUE;
	}
	
	return FALSE;
}

//
// 函数名称:		GetApplicationEntry
// 函数功能:		获取 Application 入口
// 函数参数:		无
// 函数返回值:		成功 返回接口类名
//
string CAXML::GetApplicationEntry()
{
	int i = 0;
	int nOffset = FindSTCUNKOffset("application");
	if(nOffset < 0 || nOffset > GetAXMLSize())
	{
		return "";
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for(i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{	
		char *pName = NULL; 
		if( attrs[i].acType == 0x3000008)
		{
			string strName = GetItemString(attrs[i].acName);
			if(strName == "name")
			{
				break;
			}
		}
	}
	if(i < startTagChunkApp->stcAttributeCount)
	{
		return GetItemString(attrs[i].acValueStr);
	}
	return "";
}

//
// 函数名称:		SaveAXMLFile
// 函数功能:		保存成文件
// 函数参数:		path:		文件路径
// 函数返回值:		成功 返回TRUE   失败返回FALSE
//

BOOL CAXML::SaveAXMLFile(const char *path)
{
	return writebuftofile(path, (unsigned char*)m_axml_buf, GetAXMLSize()) > 0;
}



//
// 函数名称:		GetPkgName
// 函数功能:		获取 AXML 文件中的包名
// 函数参数:		无
// 函数返回值:		成功 返回包名
//
string CAXML::GetPkgName()
{
	int i = 0;
	int nOffset = FindSTCUNKOffset("manifest");
	if(nOffset < 0 || nOffset > GetAXMLSize())
	{
		return "";
	}
	STCHUNK *startTagChunkApp = (STCHUNK*)(m_axml_buf + nOffset);
	nOffset += sizeof(STCHUNK);
	ATTRIBUTECHUNK *attrs = (ATTRIBUTECHUNK *)(m_axml_buf + nOffset);
	for(i = 0; i < startTagChunkApp->stcAttributeCount; i++)
	{	
		char *pName = NULL; 
		if( attrs[i].acType == 0x3000008 && attrs[i].acNamespaceUri == 0xFFFFFFFF)
		{
			string strName = GetItemString(attrs[i].acName);
			if(strName == "package")
			{
				break;
			}
		}
	}
	if(i < startTagChunkApp->stcAttributeCount)
	{
		return GetItemString(attrs[i].acValueStr);
	}
	return "";
}
