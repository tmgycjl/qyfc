#pragma once

#include "qydefine.h"
#include "QYString.h"
#include <map>

class QYUI_EXPORT_CLASS QYImageManage
{
public:
    static QYImageManage* instance();
	void  getImage(UINT uResID, BYTE **ppStream, unsigned int *len);
public:
	void GenerateImage();
private:
    static QYImageManage* m_pInstance;
	QYString m_strImageDir;
	QYString m_sImageFileName;
	std::map<UINT,UINT64> m_mapIndex;
	static unsigned char m_maskingKey[];
};
