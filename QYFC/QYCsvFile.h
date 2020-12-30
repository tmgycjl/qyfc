#pragma once

#include "qydefine.h"
#include "QYString.h"

class QYUI_EXPORT_CLASS QYCsvFile
{
public:
	QYCsvFile();
	~QYCsvFile();
	enum
	{
		modeWrite = 0,
		modeRead,
		modeAppend
	};
	BOOL Open(LPCTSTR strFileName, int nMode);
	int  Write(LPCTSTR strValue);
	int  Read(QYString &strText);
	QYString ReadLine();

	void Close(){ SAFE_CLOSE_FILE(m_pFile); }
	unsigned long GetLength();
	unsigned long GetSeekPos();

	int  Write(int nValue);
	int  WriteEndl();
	int  WriteComma();
	void GotoEndl();
	BOOL IsEndl(){ return m_bIsEndl; }
private:
	char  *m_pBuf;
	int    m_nMaxBuff;
	FILE   *m_pFile;
	BOOL   m_bIsEndl;
};

