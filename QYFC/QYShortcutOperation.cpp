
#include <ShlObj.h>
#include "QYShortcutOperation.h"
#include "qydefine.h"
#include "QYString.h"
#include "qydefine.h"

QYShortcutOperation::QYShortcutOperation(void)
{
}

QYShortcutOperation::~QYShortcutOperation(void)
{
}

void QYShortcutOperation::NotifyShell(LONG wEventId, TCHAR *szPath)//路径
{    
	SHChangeNotify( wEventId,SHCNF_FLUSH | SHCNF_PATH,szPath,0);
	
	TCHAR* p;
	for( p=szPath+wcslen(szPath)-1;*p != '\\'; p--);
	*p='\0';
	SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT, SHCNF_FLUSH | SHCNF_PATH,szPath,0);
}

BOOL QYShortcutOperation::CreateLink(LPTSTR szAppPath, LPTSTR szLink)
{
	HRESULT hres ;
	IShellLink * psl ;
	IPersistFile* ppf ;
	
	hres = CoCreateInstance( CLSID_ShellLink, NULL,CLSCTX_INPROC_SERVER, IID_IShellLink,(void **)&psl) ;
	if( FAILED( hres))
		return FALSE ;
	
	psl -> SetPath( szAppPath) ;
	
	psl -> SetHotkey( MAKEWORD( 'R',HOTKEYF_SHIFT |HOTKEYF_CONTROL)) ;
	
	hres = psl -> QueryInterface( IID_IPersistFile,(void**)&ppf) ;
	if( FAILED( hres))
		return FALSE ;
	
	hres = ppf->Save( szLink, STGM_READWRITE) ;
	
	ppf->Release( ) ;
	psl->Release( ) ;
	return TRUE;
}

BOOL QYShortcutOperation::GetDeskTopGroup()
{
	SHGetSpecialFolderPath(HWND_DESKTOP,m_szGroupPath,CSIDL_COMMON_DESKTOPDIRECTORY,FALSE);
	return TRUE;
}

BOOL QYShortcutOperation::CreateItem(LPTSTR szAppPath, LPTSTR szName)
{
	TCHAR szItemPath[MAX_PATH] = {0};
	
	wsprintf(szItemPath, L"%s\\%s.lnk", m_szGroupPath, szName);

	if(!CreateLink(szAppPath, szItemPath))
		return FALSE;

	NotifyShell( SHCNE_CREATE|SHCNE_INTERRUPT, szItemPath) ;

	return TRUE;
}

BOOL QYShortcutOperation::CreateGroup(LPCTSTR szName)
{	
	TCHAR szGroupPath[MAX_PATH] = {0};
	SHGetSpecialFolderPath(HWND_DESKTOP,szGroupPath,CSIDL_COMMON_PROGRAMS,FALSE);
	
	wsprintf(szGroupPath,L"%s\\%s",szGroupPath,szName);
	
	//wsprintf(m_szGroupPath+wcslen(m_szGroupPath),szName);

	if( !CreateDirectory( szGroupPath, NULL) && 183 != GetLastError())
	{
		return FALSE;
	}

	SafeStrcpy(m_szGroupPath,szGroupPath);

	NotifyShell( SHCNE_MKDIR|SHCNE_INTERRUPT,szGroupPath);

	return TRUE;
}

BOOL QYShortcutOperation::DeleteFolder(LPTSTR pszFolder)
{
	SHFILEOPSTRUCT fos ;

	ZeroMemory( &fos, sizeof( fos)) ;
	fos.hwnd = HWND_DESKTOP;
	fos.wFunc = FO_DELETE ;
	fos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION  ;
	fos.pFrom = pszFolder ;

	if( 0 != SHFileOperation( &fos))
		return FALSE ;
	return TRUE;
}

void QYShortcutOperation::DeleteGroup(LPTSTR szPath)
{
	TCHAR szGroupPath[MAX_PATH] = {0};
	SHGetSpecialFolderPath(HWND_DESKTOP,szGroupPath,CSIDL_COMMON_PROGRAMS,FALSE);

	wsprintf(szGroupPath, L"%s\\%s", szGroupPath, szPath);

	SafeStrcpy(m_szGroupPath,szGroupPath);

	DeleteFolder( szGroupPath);
	
	NotifyShell( SHCNE_RMDIR|SHCNE_INTERRUPT, szGroupPath) ;
}

void QYShortcutOperation::DeleteItem(LPTSTR szShortcut)
{
	DeleteLink(szShortcut);
	
	NotifyShell( SHCNE_DELETE|SHCNE_INTERRUPT, szShortcut) ;
}

BOOL QYShortcutOperation::DeleteLink(LPTSTR lpszShortcut)
{
	SHFILEOPSTRUCT fos ;

	ZeroMemory( &fos, sizeof(fos)) ;
	fos.hwnd = HWND_DESKTOP ;
	fos.wFunc = FO_DELETE ;
	fos.pFrom = lpszShortcut;
	fos.pTo = NULL ;
	fos.fFlags = FOF_SILENT | FOF_ALLOWUNDO ;
	
	if( 0 != SHFileOperation( &fos))
		return FALSE ;
	return TRUE ;
}

void QYShortcutOperation::GetMenuStartPath(TCHAR *szMenuStartPath)
{
	SHGetSpecialFolderPath(HWND_DESKTOP,szMenuStartPath,CSIDL_STARTMENU,FALSE);
}

//得到快速启动栏的路径

BOOL QYShortcutOperation::GetIEQuickLaunchPath(TCHAR *pszIEQueickLaunchPath, int bufSize)

{
	LPITEMIDLIST  ppidl;

	if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &ppidl) == S_OK)
	{
		BOOL flag = SHGetPathFromIDList(ppidl, pszIEQueickLaunchPath);

		SafeStrcat(pszIEQueickLaunchPath, bufSize, L"\\Microsoft\\Internet Explorer\\Quick Launch");

		CoTaskMemFree(ppidl);

		return flag;

	}
	return FALSE;
}

//得到 开始->程序组 的路径
BOOL QYShortcutOperation::GetSpecialPath(TCHAR *pszProgramsPath,int csidl)
{
	LPITEMIDLIST  ppidl;
	if (SHGetSpecialFolderLocation(NULL, csidl, &ppidl) == S_OK)
	{
		BOOL flag = SHGetPathFromIDList(ppidl, pszProgramsPath);

		CoTaskMemFree(ppidl);

		return flag;

	}

	return FALSE;

}

/*

函数功能：对指定文件在指定的目录下创建其快捷方式
函数参数：
lpszFileName    指定文件，为NULL表示当前进程的EXE文件。
lpszLnkFileDir  指定目录，不能为NULL。
lpszLnkFileName 快捷方式名称，为NULL表示EXE文件名。
wHotkey         为0表示不设置快捷键
pszDescription  备注
iShowCmd        运行方式，默认为常规窗口

*/

BOOL QYShortcutOperation::CreateFileShortcut(LPCTSTR lpszFileName,
	LPCTSTR lpszLnkFileDir,
	LPCTSTR lpszLnkFileName,
	LPCTSTR lpszWorkDir,
	WORD wHotkey, 
	LPCTSTR lpszDescription, 
	int iShowCmd )
{
	if (lpszLnkFileDir == NULL)

		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink对象指针
	IPersistFile   *ppf; //IPersisFil对象指针

	//创建IShellLink对象
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);

	if (FAILED(hr))

		return FALSE;

	//从IShellLink对象中获取IPersistFile接口

	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);

	if (FAILED(hr))

	{

		pLink->Release();

		return FALSE;

	}

	//目标

	TCHAR *pgmFile = nullptr;
	_get_wpgmptr(&pgmFile);

	if (lpszFileName == NULL)

		pLink->SetPath((TCHAR*)pgmFile);

	else

		pLink->SetPath((TCHAR*)lpszFileName);

	//工作目录

	if (lpszWorkDir != NULL)

		pLink->SetPath((TCHAR*)lpszWorkDir);



	//快捷键

	if (wHotkey != 0)

		pLink->SetHotkey(wHotkey);



	//备注

	if (lpszDescription != NULL)

		pLink->SetDescription(lpszDescription);



	//显示方式

	pLink->SetShowCmd(iShowCmd);
	//快捷方式的路径 + 名称

	TCHAR szBuffer[MAX_PATH];

	if (lpszLnkFileName != NULL) //指定了快捷方式的名称

		SafeSprintf(szBuffer, MAX_PATH, L"%s\\%s", (TCHAR*)lpszLnkFileDir, (TCHAR*)lpszLnkFileName);

	else

	{
		//没有指定名称，就从取指定文件的文件名作为快捷方式名称。

		TCHAR *pstr;

		if (lpszFileName != NULL)

			pstr = SafeStrChr((TCHAR*)lpszFileName, '\\');

		else

			pstr = SafeStrChr(pgmFile, '\\');

		if (pstr == NULL)

		{

			ppf->Release();

			pLink->Release();

			return FALSE;

		}

		//注意后缀名要从.exe改为.lnk

		SafeSprintf(szBuffer, MAX_PATH,L"%s\\%s", (TCHAR*)lpszLnkFileDir, pstr);

		int nLen = SafeStrlen(szBuffer);

		szBuffer[nLen - 3] = 'l';

		szBuffer[nLen - 2] = 'n';

		szBuffer[nLen - 1] = 'k';

	}

	//保存快捷方式到指定目录下

	WCHAR  wsz[MAX_PATH];  //定义Unicode字符串

	//MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(wsz, TRUE);

	ppf->Release();

	pLink->Release();

	return SUCCEEDED(hr);
}

BOOL QYShortcutOperation::CreateDesktopShotCut(LPCTSTR strName, LPCTSTR strSourcePath)
{
	int i;
	TCHAR Path[MAX_PATH + 1];
	QYString strDestDir;
	i = CSIDL_DESKTOPDIRECTORY;
	LPITEMIDLIST pidl;
	LPMALLOC pShell;
	if (SUCCEEDED(SHGetMalloc(&pShell)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, i, &pidl)))
		{
			if (!SHGetPathFromIDList(pidl, Path))
			{
				pShell->Free(pidl);
				return FALSE;
			}
			pShell->Release();
			strDestDir.Format(L"%s", Path);
			strDestDir += L"\\";
			strDestDir += strName;//设置桌面快捷方式的名字
			strDestDir += L".lnk";
			IShellLink* psl;
			if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl)))
			{
				psl->SetPath(strSourcePath);
				psl->SetWorkingDirectory(strSourcePath);   
				IPersistFile* ppf;
				if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (LPVOID*)
					&ppf)))
				{
//					WCHAR wsz[MAX_PATH];
					//MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, &strDestDir, -1, wsz, MAX_PATH);
					if (SUCCEEDED(ppf->Save(&strDestDir, TRUE)))
					{
						ppf->Release();
						psl->Release();
						return TRUE;
					}
					else
					{
						ppf->Release();
						psl->Release();
						return FALSE;
					}
				}
				else
				{
					ppf->Release();
					psl->Release();
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}