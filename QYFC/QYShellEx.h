#pragma once

#include "qydefine.h"

class QYFC_EXPORT_CLASS QYShellEx
{
public:
	QYShellEx();
	~QYShellEx();
	static HINSTANCE OpenDir(LPCTSTR szDir, __in_opt HWND hwnd = nullptr, LPCWSTR lpOperation = nullptr, LPCWSTR lpParameters = nullptr, LPCWSTR lpDirectory = nullptr, INT nShowCmd = SW_SHOWNORMAL);
};

