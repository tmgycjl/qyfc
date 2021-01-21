// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma  once


#include "qydefine.h"

class QYFC_EXPORT_CLASS QYIniFile
{
public:
	QYIniFile();
	QYIniFile(LPCTSTR file);
	virtual ~QYIniFile();

	void Write_int(LPCTSTR lpAppName, LPCTSTR lpKeyName, int key);
	void Write_str(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString);
	LPTSTR Get_str(LPCTSTR lpAppName, LPCTSTR lpKeyName,LPCTSTR  lpDefault);
	int  Get_int(LPCTSTR lpAppName,  LPCTSTR lpKeyName,int nDefault);
	void SetFileName(LPCTSTR file);	
	LPTSTR GetFileName();

protected:
	LPCTSTR  filename;
	TCHAR temp[1024];
};

