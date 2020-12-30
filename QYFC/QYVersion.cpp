#include "QYVersion.h"

#pragma comment(lib, "version.lib")

QYVersion::QYVersion()
{
}


QYVersion::~QYVersion()
{
}


DWORD QYVersion::GetFileVersion(LPCTSTR lpFile)
{
	DWORD dwVersion = 0;
	TCHAR strModulePath[1024] = { 0 };

	if (nullptr == lpFile)
	{
		::GetModuleFileName(NULL, strModulePath, 1023);
	}
	else
	{
		SafeStrcpy(strModulePath, 1024, lpFile);
	}
	//信息块大小
	DWORD dwFileVersionInfoSize = GetFileVersionInfoSize(strModulePath, NULL);

	if (dwFileVersionInfoSize == 0)
	{
		//没有版本信息
		return 0;
	}

	BYTE *lpData = new BYTE[dwFileVersionInfoSize + 1];
	if (lpData == NULL)
	{
		return 0;
	}

	//获取版本信息
	if (!GetFileVersionInfo(strModulePath, NULL, dwFileVersionInfoSize, lpData))
	{
		delete[] lpData;
		lpData = NULL;
		return 0;
	}

	//查询版本号信息
	UINT unInfoLen = 0;
	VS_FIXEDFILEINFO* vsFileInfo = 0;
	if (!VerQueryValue(lpData, L"\\", (LPVOID*)&vsFileInfo, &unInfoLen))
	{
		delete[] lpData;
		lpData = NULL;
		return 0;
	}

	if (unInfoLen != sizeof(VS_FIXEDFILEINFO))
	{
		delete[] lpData;
		lpData = NULL;
		return 0;
	}
	
	dwVersion = vsFileInfo->dwFileVersionLS & 0xffff;
	dwVersion |= (vsFileInfo->dwFileVersionLS >> 16) << 8;
	dwVersion |= (vsFileInfo->dwFileVersionMS & 0xffff) << 16;
	dwVersion |= (vsFileInfo->dwFileVersionMS >> 16) << 24;

	delete[] lpData;
	lpData = NULL;

	return dwVersion;
}


QYString QYVersion::ConverseVersion(DWORD dwIP)
{
	QYString strIP;
	BYTE byte[4];
	byte[0] = (dwIP >> 24) & 0xff;
	byte[1] = (dwIP >> 16) & 0xff;
	byte[2] = (dwIP >> 8) & 0xff;
	byte[3] = dwIP & 0xff;
	strIP.Format(L"%d.%d.%d.%d", byte[0], byte[1], byte[2], byte[3]);
	return strIP;
}

DWORD QYVersion::ConverseVersion(LPCTSTR pStr)
{
	QYString strIP = pStr;
	BYTE byte[4];
	int nStart, nEnd = -1;

	nStart = nEnd + 1;
	nEnd = strIP.Find('.', nStart);
	byte[3] = SafeA2I(strIP.Mid(nStart, nEnd - nStart));

	nStart = nEnd + 1;
	nEnd = strIP.Find('.', nStart);
	byte[2] = SafeA2I(strIP.Mid(nStart, nEnd - nStart));

	nStart = nEnd + 1;
	nEnd = strIP.Find('.', nStart);
	byte[1] = SafeA2I(strIP.Mid(nStart, nEnd - nStart));

	nStart = nEnd + 1;
	nEnd = strIP.Length();
	byte[0] = SafeA2I(strIP.Mid(nStart, nEnd - nStart));

	DWORD dwIP = (byte[3] << 24) | (byte[2] << 16) | (byte[1] << 8) | byte[0];
	return dwIP;
}