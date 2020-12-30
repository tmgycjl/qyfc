#pragma once

#include "qydefine.h"
#include "QYString.h"

class QYUI_EXPORT_CLASS QYVersion
{
public:
	QYVersion();
	~QYVersion();
	static DWORD GetFileVersion(LPCTSTR lpFile);
	static QYString ConverseVersion(DWORD dwIP);
	static DWORD ConverseVersion(LPCTSTR pStr);
};

