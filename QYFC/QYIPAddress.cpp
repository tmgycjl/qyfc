#include "QYIPAddress.h"


QYIPAddress::QYIPAddress()
{
}


QYIPAddress::~QYIPAddress()
{
}


QYString QYIPAddress::ConverseIP(DWORD dwIP)
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

DWORD QYIPAddress::ConverseIP(LPCTSTR pStr)
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