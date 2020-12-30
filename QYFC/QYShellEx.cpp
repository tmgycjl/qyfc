#include "QYShellEx.h"


QYShellEx::QYShellEx()
{
}


QYShellEx::~QYShellEx()
{
}


HINSTANCE QYShellEx::OpenDir(LPCTSTR szDir, HWND hwnd , LPCWSTR lpOperation , LPCWSTR lpParameters , LPCWSTR lpDirectory , INT nShowCmd )
{
	return ShellExecute(hwnd, lpOperation, szDir, lpParameters, lpDirectory, nShowCmd);
}