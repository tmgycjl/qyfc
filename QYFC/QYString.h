#pragma once

using namespace std;

#include <string>

#include "qydefine.h"

class QYUI_EXPORT_CLASS QYString
{
public:
	QYString();
	QYString(const TCHAR *string);
	QYString(const int value);
	QYString( QYString& string);
	~QYString();
	 TCHAR* Tchar() { return m_string; }
	 int Length();
	 void Clear();
	QYString& operator = (const TCHAR *string);
	QYString& operator = (  QYString& string);
    QYString& operator = (const int value);
	QYString& operator = (const float value);
	BOOL operator == (const TCHAR *string);
	
	BOOL operator == ( QYString& string);
	BOOL operator != (QYString& string);

	  QYString operator + (TCHAR* string);
	QYString& operator += (const TCHAR * string);
	 QYString operator + ( QYString& string);
	QYString& operator += (QYString& string);
	TCHAR& operator [] (int index);
	TCHAR* operator & ();
	operator const TCHAR*()const;

	int  Delete(int start, int count = 1);
	QYString  Mid(int start, int count = -1);
	int  Insert(int start,TCHAR *string);
	BOOL  Insert(int start, TCHAR chr);
	QYString Right(int count);
	QYString Left(int count);

	void Format(const TCHAR *format, ...);

	int Find(TCHAR ch,int start = 0);
	int RFind(TCHAR ch);

	static BOOL SafeW2A(char *dest, int destlen, const wchar_t *source);
	static BOOL SafeA2W(wchar_t *dest, int destlen, const char *source);
	static BOOL SafeW2AUTF8(char *dest, int destlen, const TCHAR *source);
	static BOOL SafeUTF82W(TCHAR *dest, int destlen, const char *source);
	static int GetUTF8StrLen(TCHAR *str, int len);
	static BOOL IsChineseChar(LPCTSTR str);
private:
	
#if 0
#ifdef UNICODE
	wstring m_string;
#else
	string m_string;
#endif
#else
	TCHAR   *m_string;
#endif

};

