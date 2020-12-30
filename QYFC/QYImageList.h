#pragma once

#include "qydefine.h"
#include <map>
#include "QYpicture.h"



class QYUI_EXPORT_CLASS QYImageList
{
public:
	QYImageList();
	virtual ~QYImageList();
	std::map<int, QYPicture*>  m_mapImage;
	//int AddImage(int image, UINT resID, BOOL bSubImage = FALSE,HMODULE hModule =nullptr);
    int AddImage(const char* imageFilePath, BOOL bSubImage = FALSE);
	int AddImage(int image, LPCTSTR imageFilePath, BOOL bSubImage = FALSE);
    int AddImage(int image, const char* imageFilePath, BOOL bSubImage = FALSE);
	int AddImage(int image, UINT resID, BOOL bSubImage = FALSE);
	QYPicture* GetImage(int image);
};

