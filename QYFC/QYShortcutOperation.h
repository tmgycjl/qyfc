#pragma once


#include "qydefine.h"
enum
{
	LINK_DESKTOP = 0,
	LINK_START_MENU
};

class QYFC_EXPORT_CLASS QYShortcutOperation
{
public:
	QYShortcutOperation(void);
	~QYShortcutOperation(void);
public:
	BOOL CreateGroup(LPCTSTR szName);
	BOOL GetDeskTopGroup();
	BOOL CreateItem(LPTSTR szAppPath,LPTSTR szName);
	void DeleteGroup(LPTSTR szPath);
	void DeleteItem(LPTSTR szShortcut);
	void GetMenuStartPath(TCHAR *szMenuStartPath);
	BOOL CreateFileShortcut(LPCTSTR lpszFileName,
		LPCTSTR lpszLnkFileDir,
		LPCTSTR lpszLnkFileName,
		LPCTSTR lpszWorkDir,
		WORD wHotkey,
		LPCTSTR lpszDescription,
		int iShowCmd = SW_SHOWNORMAL);
	static BOOL GetSpecialPath(TCHAR *pszProgramsPath, int csidl);
	static BOOL GetIEQuickLaunchPath(TCHAR *pszIEQueickLaunchPath, int bufSize);
	static BOOL CreateDesktopShotCut(LPCTSTR strName, LPCTSTR strSourcePath);
private:
	BOOL DeleteFolder( LPTSTR pszFolder);
	BOOL DeleteLink( LPTSTR lpszShortcut);
	void NotifyShell(LONG wEventId, TCHAR *szPath);
	BOOL CreateLink ( LPTSTR szAppPath, LPTSTR szLink);
private:
	TCHAR   m_szGroupPath[MAX_PATH];
};
