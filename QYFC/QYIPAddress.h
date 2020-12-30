#pragma once

#include "QYString.h"

class QYUI_EXPORT_CLASS QYIPAddress
{
public:
	QYIPAddress();
	~QYIPAddress();
	static QYString ConverseIP(DWORD dwIP);
	static DWORD ConverseIP(LPCTSTR pStr);
};



