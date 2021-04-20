#include "QYFileStudio.h"
#include <vector>


QYFileStudio::QYFileStudio()
{
}


QYFileStudio::~QYFileStudio()
{
	if (nullptr != m_hListFile)
	{
		FindClose(m_hListFile);
	}
	
}


QYString QYFileStudio::GetAppPath()
{
	QYString strModulePath;
	TCHAR szModulePath[MAX_PATH] = { 0 };
	DWORD dwResult = ::GetModuleFileName(NULL, szModulePath, MAX_PATH);
	strModulePath = szModulePath;
	strModulePath = strModulePath.Left(strModulePath.RFind('\\') + 1);
	return strModulePath;
}


BOOL QYFileStudio::CreateFullPath(LPCTSTR szFullPathName)
{
	if (NULL == szFullPathName)
		return FALSE;

	std::wstring strFullPathName(szFullPathName);
	char PathSymbol;

	std::wstring::size_type loc = strFullPathName.find('/', 0);
	if (loc != std::string::npos)
	{
		PathSymbol = '/';
	}
	else
	{
		loc = strFullPathName.find('\\', 0);
		if (loc != std::wstring::npos)
		{
			PathSymbol = '\\';
		}
		else
		{

			return FALSE;
		}
	}

	std::wstring strFullPath(strFullPathName, 0, strFullPathName.rfind(PathSymbol, strFullPathName.size() - 1));
	HANDLE hFile;
	WIN32_FIND_DATA finddata;
	int n = sizeof(WIN32_FIND_DATA);

	hFile = ::FindFirstFile(strFullPath.c_str(), &finddata);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFile);

		return TRUE;
	}

	std::vector<std::wstring> vecPath;
	std::wstring tmp;

	for (int i = 0; i < strFullPath.size(); i++)
	{
		if (strFullPath.at(i) != PathSymbol)
		{
			tmp += strFullPath.at(i);
		}
		else
		{
			vecPath.push_back(tmp);
			tmp += PathSymbol;
		}

		if (i == strFullPath.size() - 1)
			vecPath.push_back(tmp);
	}

	DWORD dwAttributes;
	//È¥µôÅÌ·û

	for (int i = 1; i < vecPath.size(); i++)
	{
		dwAttributes = GetFileAttributes(vecPath[i].c_str());
		if (dwAttributes != INVALID_FILE_ATTRIBUTES && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			continue;
		}

		if (!CreateDirectory(vecPath[i].c_str(), NULL))
		{
			return FALSE;
		}

	}

	dwAttributes = GetFileAttributes(strFullPath.c_str());
	return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY);
}


BOOL QYFileStudio::IsValidFilePath(LPCTSTR szPathName)
{
	TCHAR szTestFile[MAX_PATH] = L"test";

	SafeSprintf(szTestFile, L"%s\\%s", szPathName, L"test");

	if (!CreateFullPath(szTestFile))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL QYFileStudio::FindFirst(std::string  &sFilePath)
{
	TCHAR szFilePath[MAX_PATH] = { 0 };
	QYString::SafeA2W(szFilePath, MAX_PATH, sFilePath.c_str());
	return FindFirst(szFilePath);
}

BOOL QYFileStudio::FindFirst(LPCTSTR szFilePath)
{
	m_hListFile = FindFirstFile(szFilePath, &m_FindFileData);
	if (m_hListFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


BOOL QYFileStudio::FindNext()
{
	return FindNextFile(m_hListFile, &m_FindFileData);
}

BOOL QYFileStudio::IsDirectory()
{
	
	return (m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL QYFileStudio::IsDot()
{
	if (0 == SafeStrcmp(L".", m_FindFileData.cFileName) || 0 == SafeStrcmp(L"..", m_FindFileData.cFileName))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL QYFileStudio::IsEncrypted()
{
	return (m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED);
}

const TCHAR* QYFileStudio::GetFileName()
{
	return m_FindFileData.cFileName;
}

ULONGLONG QYFileStudio::GetFileSize()
{
	ULONGLONG fileSize = 0;
	fileSize |= m_FindFileData.nFileSizeHigh;
	fileSize <<= 32;
	fileSize |= m_FindFileData.nFileSizeLow;

	return fileSize;
}

QYDateTime QYFileStudio::GetCreationTime()
{
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_FindFileData.ftCreationTime, &localFileTime);

	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);
	

	return QYDateTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
}

QYDateTime QYFileStudio::GetLastWriteTime()
{
	FILETIME localFileTime;
	FileTimeToLocalFileTime(&m_FindFileData.ftLastWriteTime, &localFileTime);

	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&localFileTime, &sysTime);

	return QYDateTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
}

BOOL QYFileStudio::DeleteDirectory(LPCTSTR strDir)
{
	QYFileStudio tempFind;
	QYString strTempFileFind;
	strTempFileFind.Format(L"%s\\*.*", strDir);
	BOOL bFinded = tempFind.FindFirst(strTempFileFind);
	while (bFinded)
	{
		
		if (!tempFind.IsDot())
		{
			if (tempFind.IsDirectory())
			{
				//DeleteDirectory(tempFind.GetFilePath());
			}
			else
			{
				//DeleteFile(tempFind.GetFilePath());
			}
		}
		bFinded = tempFind.FindNext();
	}
	
	if (!RemoveDirectory(strDir))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL QYFileStudio::browseForFolder(HWND hWnd, std::string &folderName, unsigned int flags, const char *title /* = nullptr */)
{
	char szPathName[MAX_PATH] = { 0 };
	BROWSEINFOA bInfo = { 0 };
	bInfo.hwndOwner = hWnd;
	bInfo.lpszTitle = title;
	bInfo.ulFlags = flags;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolderA(&bInfo);
	if (lpDlist != NULL)
	{
		SHGetPathFromIDListA(lpDlist, szPathName);

		folderName = szPathName;
		return TRUE;
	}

	return FALSE;

}

BOOL QYFileStudio::openFileDialog(HWND hWnd, std::string &filePath, const char *filter, const char *title)
{
	OPENFILENAMEA ofn;
	char szFile[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = title;
	if (GetOpenFileNameA(&ofn))
	{
		filePath = szFile;
		return TRUE;
	}

	return FALSE;
}
