// AXML.h: interface for the CAXML class.
//
//////////////////////////////////////////////////////////////////////
#define DEXLOG(...)		{printf(__VA_ARGS__);printf("\n");}



#if !defined(AFX_AXML_H__7B891D97_5B06_4A04_BE82_AA6D4E0603E2__INCLUDED_)
#define AFX_AXML_H__7B891D97_5B06_4A04_BE82_AA6D4E0603E2__INCLUDED_
#include <windows.h>
#include <string>
#include "utility.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct RESOURCEIDCHUNK;
struct STRINGCHUNK;
class CAXML
{
public:
	string GetPkgName();
	string GetApplicationEntry();
	BOOL DelApplicationEntry();
	BOOL SaveAXMLFile(const char *path);
	BOOL versionCode(char *pClassName);
	BOOL SetSDK(char *pClassName);
	BOOL SetApplicationEntry(char *pClassName);

	char* GetAXMLBuffer();
	RESOURCEIDCHUNK* GetResourceidChunk();
	unsigned int GetAXMLSize();
	STRINGCHUNK * GetStringChunk();
	int FindSTCUNKOffset(const char *stcName);
	unsigned int GetItemStringCount();
	void PrintAllItemString();
	BOOL AlterItemString(unsigned int nIndex, string inStr);
	int AddAXMLString(string inStr);
	int FindItemString(string instr);
	string GetItemString(int nIndex);
	
	CAXML(char *pFilePath);
	CAXML(char* pBuf, unsigned int nSize);
	virtual ~CAXML();
private:
	char * m_axml_buf;

};


#endif // !defined(AFX_AXML_H__7B891D97_5B06_4A04_BE82_AA6D4E0603E2__INCLUDED_)
