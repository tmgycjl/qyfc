// muxer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#include "../ffplay/ffplay.h"

#pragma comment(lib,"ffplay")


int _tmain(int argc, _TCHAR* argv[])
{

	ffplayStartUp(nullptr);

	char szFileName[MAX_PATH] = { 0 };
	char szFilePath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA findFileData;
	char baseDir[] = "Z:/Downloads/";

	strcat_s(szFilePath, baseDir);
	strcat_s(szFilePath, "*.mp4*");
	HANDLE hFind = ::FindFirstFile(szFilePath, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return 0;
	}

	int fileCount = 0;
	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		memset(szFileName, 0, sizeof(szFileName));
		strncpy_s(szFileName, baseDir, strlen(baseDir));
		strcat_s(szFileName, findFileData.cFileName);

		unsigned int startTs = 0;

		printf("open file :%s,fileCount:%d\n", szFileName, ++fileCount);

		ffplayWriteMyMediaFile(szFileName);

	} while (FindNextFile(hFind, &findFileData));


	return 0;
}

