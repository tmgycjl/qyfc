// muxer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <string>

#include "../ffplay/ffplay.h"

#pragma comment(lib,"ffplay")



#define  WRITE_PICTURE    0

#if  WRITE_PICTURE
#include "../MyMediaFile/MyMediaFile.h"


#if _DEBUG
#pragma  comment(lib,"MyMediaFile-d")
#else
#pragma  comment(lib,"MyMediaFile")
#endif

#endif

int _tmain(int argc, _TCHAR* argv[])
{
	
#if WRITE_PICTURE
	char szFileName[MAX_PATH] = { 0 };
	char szFilePath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA findFileData;
	char baseDir[] = "F:\\media\\person movie\\JPEG\\";
	//char baseDir[] = "c:\\";
	strcat_s(szFilePath, baseDir);
	strcat_s(szFilePath, "*.jpg*");
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


		std::string outPath = "F:\\media\\person movie\\pchp\\";
		outPath += findFileData.cFileName;
		outPath += ".pchp";

		myMediaPicWrite(szFileName, outPath.c_str());

	} while (FindNextFile(hFind, &findFileData) );
#else


	ffplayStartUp(nullptr);

	char szFileName[MAX_PATH] = { 0 };
	char szFilePath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA findFileData;
	char baseDir[] = "c:\\outfile\\";
	//char baseDir[] = "c:\\";
	strcat_s(szFilePath, baseDir);
	strcat_s(szFilePath, "*.flv*");
	HANDLE hFind = ::FindFirstFile(szFilePath, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return 0;
	}

	int fileCount = 5;
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

		printf("open file :%s,fileCount:%d\n", szFileName, --fileCount);
#if 0
		std::string newName = baseDir;
		newName += std::to_string(fileCount);
		newName += ".mp4";
		
#endif



		std::string outPath = "F:\\media\\person movie\\pch\\";
		outPath += findFileData.cFileName;
		outPath += ".pch";

		if (0 != ffplayWriteMyMediaFile(szFileName, outPath.c_str()))
		{
			fileCount++;
		}

	} while (FindNextFile(hFind, &findFileData) && fileCount > 0);

#endif
	return 0;
}

