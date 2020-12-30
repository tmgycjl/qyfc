#include "QYCsvFile.h"


QYCsvFile::QYCsvFile(void)
{
	m_nMaxBuff = 4096;;
	m_pFile = NULL;
	m_pBuf = nullptr;
	m_bIsEndl = FALSE;
}

QYCsvFile::~QYCsvFile(void)
{
	SAFE_CLOSE_FILE(m_pFile);

	SAFE_DELETE_ARRAY(m_pBuf);
}

BOOL QYCsvFile::Open(LPCTSTR strFileName, int nMode)
{
	SAFE_CLOSE_FILE(m_pFile);

	char filename[MAX_PATH] = { 0 };
	QYString::SafeW2A(filename, MAX_PATH - 1, strFileName);

	char mode[10] = { 0 };

	if (modeWrite == nMode)
	{
		sprintf_s(mode, 9, "w");
	}
	else if (modeRead == nMode)
	{
		sprintf_s(mode, 9, "r");
	}
	else if (modeAppend == nMode)
	{
		sprintf_s(mode, 9, "a");
	}
	if (0 != fopen_s(&m_pFile, filename, mode))
	{
		return FALSE;
	}

	m_pBuf = new char[m_nMaxBuff];

	return TRUE;
}

int QYCsvFile::Write(LPCTSTR strValue)
{
	int nSize = SafeStrlen(strValue) * sizeof(TCHAR) - 1;
	if (nSize > m_nMaxBuff)
	{
		return 0;
	}

	QYString::SafeW2A(m_pBuf, m_nMaxBuff - 1, strValue);

	return fwrite(m_pBuf, 1, strlen(m_pBuf), m_pFile);
}

int QYCsvFile::Read(QYString &strText)
{
	memset(m_pBuf, 0, m_nMaxBuff);

	TCHAR outBuf[1024];
	int readLen = 0;
	int ret = 0;
	BOOL bEndl = FALSE;
	while (1024 > readLen)
	{
		char *pRead = &m_pBuf[readLen];
		ret = fread(pRead, sizeof(char), 1, m_pFile);
		if (ret == sizeof(char))
		{
			if (',' == *pRead)
			{
				*pRead = '\0';
				break;
			}
			else if ('\n' == *pRead)
			{
				bEndl = TRUE;
				*pRead = '\0';
				break;
			}
		}
		else
		{
			return -1;
		}

		readLen += ret;
	}

	QYString::SafeA2W(outBuf, 1023, m_pBuf);
	strText = outBuf;

	return bEndl ? 1 : 0;
}

QYString QYCsvFile::ReadLine()
{
	memset(m_pBuf, 0, m_nMaxBuff);

	QYString strRet;
	TCHAR outBuf[4096];
	int readLen = 0;
	int ret = 0;
	while (1024 > readLen)
	{
		char *pRead = &m_pBuf[readLen];
		ret = fread(pRead, sizeof(char), 1, m_pFile);
		if (ret == sizeof(char))
		{
			if ('\n' == *pRead)
			{
				*pRead = '\0';
				break;
			}
		}
		else
		{
			return L"";
		}

		readLen += ret;
	}

	QYString::SafeA2W(outBuf, 1023, m_pBuf);

	return strRet = outBuf;
}