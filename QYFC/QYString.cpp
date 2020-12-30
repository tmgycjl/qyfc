#include "QYString.h"

#define  MAX_FORMAT_STRING_LEN       1024

QYString::QYString()
{
	m_string = new TCHAR[sizeof(TCHAR)];
	ZeroMemory(m_string, sizeof(TCHAR));
}


QYString::~QYString()
{
	SAFE_DELETE_ARRAY(m_string);
}

QYString::QYString( QYString& string)
{
	TCHAR *pTemp = string.Tchar();

	if (nullptr != pTemp)
	{
		int len = (SafeStrlen(pTemp) + 1)*sizeof(TCHAR);
		m_string = new TCHAR[len];
		ZeroMemory(m_string, len);

		SafeStrcpy(m_string, len, pTemp);
	}
	else
	{
		m_string = new TCHAR[sizeof(TCHAR)];
		ZeroMemory(m_string, sizeof(TCHAR));
	}
}

QYString::QYString(const TCHAR *string)
{
	if (nullptr != string)
	{
		int len = (SafeStrlen(string) + 1)*sizeof(TCHAR);
		m_string = new TCHAR[len];
		ZeroMemory(m_string, len);

		SafeStrcpy(m_string, len, string);
	}
	else
	{
		m_string = new TCHAR[sizeof(TCHAR)];
		ZeroMemory(m_string, sizeof(TCHAR));
	}
}


QYString::QYString(const int value)
{
	m_string = new TCHAR[MAX_FORMAT_STRING_LEN];
	SafeSprintf(m_string, MAX_FORMAT_STRING_LEN, L"%d", value);
}

QYString&  QYString::operator = (const TCHAR *string)
{
	if (nullptr != string)
	{
		SAFE_DELETE_ARRAY(m_string);

		int len = (SafeStrlen(string) + 1)*sizeof(TCHAR);
		m_string = new TCHAR[len];

		SafeStrcpy(m_string, len, string);
	}

	return *this;
}

QYString&  QYString::operator = (const int value)
{
	SAFE_DELETE_ARRAY(m_string);
	m_string = new TCHAR[MAX_FORMAT_STRING_LEN];
	SafeSprintf(m_string, MAX_FORMAT_STRING_LEN, L"%d", value);

	return *this;
}

QYString&  QYString::operator = (const float value)
{
	SAFE_DELETE_ARRAY(m_string);
	m_string = new TCHAR[MAX_FORMAT_STRING_LEN];
	SafeSprintf(m_string, MAX_FORMAT_STRING_LEN, L"%f", value);

	return *this;
}

QYString& QYString::operator = ( QYString& string)
{
	TCHAR *pChar = string.Tchar();
	SAFE_DELETE_ARRAY(m_string);

	if (nullptr != pChar)
	{
		SAFE_DELETE_ARRAY(m_string);

        int len = (SafeStrlen(pChar) + 1)*sizeof(TCHAR);
		m_string = new TCHAR[len];

		SafeStrcpy(m_string, len, pChar);
	}

	return *this;
}

BOOL QYString::operator == (const TCHAR *string)
{
	if (0 == SafeStrcmp(m_string, string))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
BOOL QYString::operator == ( QYString& string)
{
	TCHAR *pChar = string.Tchar();

	if (0 == SafeStrcmp(m_string,pChar))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

BOOL QYString::operator != (QYString& string)
{
	TCHAR *pChar = string.Tchar();

	if (0 != SafeStrcmp(m_string, pChar))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

QYString QYString::operator + (TCHAR* string)
{
	int size = SafeStrlen(m_string) + SafeStrlen(string) + sizeof(TCHAR);
	TCHAR *tempStr = new TCHAR[size];
	SafeStrcpy(tempStr, size, m_string);
	SafeStrcat(tempStr, size,string);

	QYString strTemp = tempStr;
	SAFE_DELETE_ARRAY(tempStr);
	return strTemp;
}

QYString QYString::operator + (QYString& string)
{
	int size = SafeStrlen(m_string) + string.Length() + sizeof(TCHAR);
	TCHAR *tempStr = new TCHAR[size];
	SafeStrcpy(tempStr, size, m_string);
	SafeStrcat(tempStr, size, string);

	QYString strTemp = tempStr;
	SAFE_DELETE_ARRAY(tempStr);
	return strTemp;
}


QYString& QYString::operator += (const TCHAR * string)
{
	Insert(Length(), (TCHAR*)string);
	return *this;
}


QYString& QYString::operator += (QYString& string)
{
	Insert(Length(), string.Tchar());

	return *this;
}

TCHAR* QYString::operator &()
{
	return this->m_string;
}

TCHAR& QYString::operator [] (int index)
{
	return m_string[index];
}

QYString::operator const TCHAR *()const
{
	return this->m_string;
}
int QYString::Length()
{
	if (nullptr != m_string)
	{
		return SafeStrlen(m_string); 
	}

	return 0;
}


void QYString::Clear()
{
	SAFE_DELETE_ARRAY(m_string);
}

int  QYString::Delete(int start, int count )
{
	if (nullptr != m_string)
	{
		auto len = (SafeStrlen(m_string) + 1)*sizeof(TCHAR);
		auto *temp = new TCHAR[len];
		SafeStrcpy(temp, len, m_string);
		ZeroMemory(m_string, len);

		memcpy(m_string, temp, start*sizeof(TCHAR));
		memcpy(m_string + start, temp + start + count, (SafeStrlen(temp) - start - count)*sizeof(TCHAR));

		SAFE_DELETE_ARRAY(temp);
	}
	
	return 0;
}

QYString QYString::Right(int count)
{
	auto len = (count + 1)*sizeof(TCHAR);
	auto *temp = new TCHAR[len];
	ZeroMemory(temp, len );

	int start = SafeStrlen(m_string) - count;

	memcpy(temp, m_string + start, count * sizeof(TCHAR));

	QYString retstring = temp;
	SAFE_DELETE_ARRAY(temp);

	return retstring;
}

QYString QYString::Left(int count)
{
	auto len = (count + 1)*sizeof(TCHAR);
	auto *temp = new TCHAR[len];
	ZeroMemory(temp, len);

	int start = SafeStrlen(m_string) - count;

	memcpy(temp, m_string, count * sizeof(TCHAR));

	QYString retstring = temp;
	SAFE_DELETE_ARRAY(temp);

	return retstring;
}

QYString  QYString::Mid(int start, int count)
{
	if (nullptr != m_string)
	{
		if (-1 == count)
		{
			auto len = (SafeStrlen(m_string) + 1)*sizeof(TCHAR);
			auto *temp = new TCHAR[len];
			ZeroMemory(temp, len );

			memcpy(temp, m_string + start, Length() * sizeof(TCHAR));

			QYString retstring = temp;
			SAFE_DELETE_ARRAY(temp);

			return retstring;
		}
		else
		{
			auto len = (SafeStrlen(m_string) + 1)*sizeof(TCHAR);
			auto *temp = new TCHAR[len];
			ZeroMemory(temp, len);

			memcpy(temp, m_string + start, count * sizeof(TCHAR));

			QYString retstring = temp;
			SAFE_DELETE_ARRAY(temp);

			return retstring;
		}
		
	}

	return L"";
}

void QYString::Format(const TCHAR *format, ...)
{
	SAFE_DELETE_ARRAY(m_string);

	//if (nullptr != m_string)
	{
		m_string = new TCHAR[MAX_FORMAT_STRING_LEN];

		va_list ap;
		va_start(ap, format);
		vswprintf_s(m_string, MAX_FORMAT_STRING_LEN, format, ap);
		va_end(ap);
	}
}

int QYString::Find(TCHAR ch, int start)
{
	if (nullptr == m_string)
	{
		return -1;
	}

	TCHAR *p = SafeStrChr(m_string + start, ch);
	if (nullptr != p)
	{
		return (p - m_string);
	}

	return -1;
}

int QYString::RFind(TCHAR ch)
{
	if (nullptr == m_string)
	{
		return -1;
	}

	TCHAR *p = SafeStrrChr(m_string, ch);
	if (nullptr != p)
	{
		return (p - m_string);
	}

	return -1;
}

int QYString::Insert(int start, TCHAR *string)
{
	auto len = (SafeStrlen(m_string) + 1 + SafeStrlen(string)) * sizeof(TCHAR);
	auto *pTemp = new TCHAR[len];

	ZeroMemory(pTemp, len);
	
	memcpy(pTemp, m_string, (start)* sizeof(TCHAR));
	auto *p = pTemp + SafeStrlen(pTemp);
	memcpy(p, string, SafeStrlen(string)* sizeof(TCHAR));
	memcpy(p + SafeStrlen(string), m_string + start, (SafeStrlen(m_string) - start )* sizeof(TCHAR));
	SAFE_DELETE_ARRAY(m_string);

	m_string = new TCHAR[len];
	SafeStrcpy(m_string, len, pTemp);

	SAFE_DELETE_ARRAY(pTemp);

	return  0;
}

BOOL QYString::Insert(int start, TCHAR chr)
{
	if (nullptr == m_string)
	{
		return FALSE;
	}
	auto len = (SafeStrlen(m_string) + 2) * sizeof(TCHAR);
	auto *pTemp = new TCHAR[len];

	ZeroMemory(pTemp, len);

	memcpy(pTemp, m_string, (start ) * sizeof(TCHAR));
	auto *p = pTemp + SafeStrlen(pTemp);
	*p = chr;
	if (start > SafeStrlen(m_string))
	{
		//assert(FALSE);
		return FALSE;
	}

	memcpy(p + 1, m_string + start, (SafeStrlen(m_string) - start)* sizeof(TCHAR));
	SAFE_DELETE_ARRAY(m_string);

	m_string = new TCHAR[len];
	SafeStrcpy(m_string, len, pTemp);

	SAFE_DELETE_ARRAY(pTemp);

	return  TRUE;
}

BOOL QYString::SafeW2A(char *dest, int destlen, const wchar_t *source)
{
	int   nLen = wcslen(source) + 1;
	if (destlen < nLen)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_ACP, 0, source, nLen, dest, 2 * nLen, NULL, NULL);
	return TRUE;
}

BOOL QYString::SafeA2W(wchar_t *dest, int destlen, const char *source)
{
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, source, -1, NULL, 0);

	if (destlen < dwNum)
	{
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, 0, source, -1, dest, dwNum);
	return TRUE;
}

BOOL QYString::SafeW2AUTF8(char *dest, int destlen, const TCHAR *source)
{
	if (nullptr == source)
	{
		return FALSE;
	}

    int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)source, -1, NULL, 0, NULL, NULL);
   // char *pElementText = new char[iTextLen + 1];
    //memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
   
   // std::string strReturn(pElementText);
   // delete[] pElementText;

	//int   nLen = wcslen(source) + 1;
    if (destlen < iTextLen)
	{
		return FALSE;
	}

    ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)source, -1, dest, iTextLen, NULL, NULL);

	return TRUE;
}

BOOL QYString::SafeUTF82W(TCHAR *dest, int destlen, const char *source)
{
	if (nullptr == source)
	{
		return FALSE;
	}
	DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);

	if (destlen < dwNum)
	{
		return FALSE;
	}

    MultiByteToWideChar(CP_UTF8, 0, source, -1, dest, destlen);
	return TRUE;
}

int QYString::GetUTF8StrLen(TCHAR *str, int len)
{
	int realLen = 0;
	for (int i = 0; i < len; i++)
	{
		if (str[i] > 127)
		{
			realLen += 3;
		}
		else
		{
			realLen++;
		}
	}

	return realLen;
}

BOOL QYString::IsChineseChar(LPCTSTR str)
{
	int len = SafeStrlen(str);
	int realLen = 0;
	for (int i = 0; i < len; i++)
	{
		if (str[i] > 127)
		{
			return TRUE;
		}
	}

	return FALSE;
}
