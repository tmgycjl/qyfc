  // IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////


#include "QYIniFile.h"
#include "QYString.h"

QYIniFile::QYIniFile()
{
	filename=NULL;
}
QYIniFile::QYIniFile(LPCTSTR file)
{	
	filename = NULL;	
	if(file != NULL)
	{
		SafeStrcpy(temp, sizeof(temp)/sizeof(TCHAR), file);
		filename = (LPTSTR)temp;
	}
}

QYIniFile::~QYIniFile()
{

}

int QYIniFile::Get_int(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nDefault)
{
	if(filename==NULL)
	{
		return nDefault;
	}
	return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, filename);
}

LPTSTR QYIniFile::Get_str(LPCTSTR lpAppName, LPCTSTR  lpKeyName, LPCTSTR  lpDefault)
{
	static TCHAR temp[MAX_PATH];
	DWORD nSize = MAX_PATH;
	if(filename == NULL)
	{
		return NULL;
	}	
	GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, (LPTSTR)temp, nSize, filename);
	return (LPTSTR)temp;
}

void QYIniFile::Write_str(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString)
{
	if(filename == NULL)
	{
		return ;
	}
	WritePrivateProfileString(lpAppName,lpKeyName,lpString,filename);
}

void QYIniFile::Write_int(LPCTSTR lpAppName, LPCTSTR lpKeyName, int key)
{
	if(filename == NULL)
	{
		return ;
	}
	QYString str(key);
	WritePrivateProfileString(lpAppName, lpKeyName, str, filename);
}

void QYIniFile::SetFileName(LPCTSTR file)
{
	if(file!=NULL)
	{
		SafeStrcpy(temp, sizeof(temp)/sizeof(TCHAR), file);
		filename = (LPTSTR)temp;
	}
}

LPTSTR QYIniFile::GetFileName()
{
	SafeStrcpy(temp, sizeof(temp)/sizeof(TCHAR), filename);
	return (LPTSTR)temp;
}
