

#ifndef _CHAR_TRAN_H_
#define _CHAR_TRAN_H_

class CCharTran
{
public:
	CCharTran();
	~CCharTran();
public:
	int Utf8ToUnicode(const char *pszUtf8, const int nLen, wchar_t *pszUnicode);
	int Utf8ToGb2312(const char *pszUtf8, const int nLen, char *pszGb2312);

	int UnicodeToUtf8(const wchar_t *pszUnicode, const int nLen, char *pszUtf8);
	int UnicodeToGb2312(const wchar_t *pszUnicode, const int nLen, char *pszGb2312);

	int Gb2312ToUtf8(const char *pszGb2312, const int nLen, char *pszUtf8);
	int Gb2312ToUnicode(const char *pszGb2312, const int nLen, wchar_t *pszUnicode);
protected:
private:
	int cUtf8ToUnicode(const char *pszUtf8, wchar_t *pszUnicode);
	int cUtf8ToGb2312(const char *pszUtf8, char *pszGb2312);

	int cUnicodeToUtf8(const wchar_t *pszUnicode, char *pszUtf8);
	int cUnicodeToGb2312(const wchar_t *pszUnicode, char *pszGb2312);

	int cGb2312ToUtf8(const char *pszGb2312, char *pszUtf8);
	int cGb2312ToUnicode(const char *pszGb2312, wchar_t *pszUnicode);
};

#endif