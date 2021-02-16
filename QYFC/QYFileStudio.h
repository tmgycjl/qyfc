#pragma once
#include "qydefine.h"
#include "QYString.h"
#include "QYDateTime.h"
#include <ShlObj.h>

class QYFC_EXPORT_CLASS QYFileStudio
{
public:
	QYFileStudio();
	~QYFileStudio();

	static QYString GetAppPath();
	static BOOL CreateFullPath(LPCTSTR szFullPathName);

	static BOOL IsValidFilePath(LPCTSTR szPathName);
	static BOOL DeleteDirectory(LPCTSTR strDir);
	static BOOL openFileDialog(HWND hWnd, std::string &filePath,  const char *filter, const char *title = nullptr);
	static BOOL browseForFolder(HWND hWnd, std::string &folderName, unsigned int flags, const char *title = "");
	BOOL FindFirst(LPCTSTR szFilePath);
	BOOL FindFirst(std::string  &sFilePath);
	BOOL FindNext();
	BOOL IsEncrypted();
	BOOL IsDot();
	BOOL IsDirectory();
	const TCHAR* GetFileName();
	ULONGLONG GetFileSize();
	QYDateTime GetCreationTime();
	QYDateTime GetLastWriteTime();
private:
	WIN32_FIND_DATA m_FindFileData;
	HANDLE m_hListFile = nullptr;
};

