#pragma once

#include "qydefine.h"
#include <map>
#include <vector>

class QYUI_EXPORT_CLASS QYImageManager
{
public:
	~QYImageManager();
	static QYImageManager* instance();
	static void  WINAPI QYPictureCallback(void *data, const char *source, BYTE **ppStream, unsigned int *len);
public:
	void packImages(std::string &imageDir, std::string &dstFileName);
	void loadImages(std::string &imageFile);
	void  OnPictureCallback(const char *source, BYTE **ppStream, unsigned int *len);
private:
	void findImage(LPCTSTR w_imageDir, FILE *outFile, std::vector<std::pair<std::string, UINT64>> *vectorIndex);
	static QYImageManager* m_pInstance;
	DWORD m_dwLanguage;
	TCHAR m_strImageDir[MAX_PATH];
	std::string m_sImageFileName;
	std::map<std::string, UINT64> m_mapIndex;
	static unsigned char m_maskingKey[];
};

