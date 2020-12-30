#pragma once

#include "includedef.h"
#include "QYRect.h"

using namespace Gdiplus;

class QYFC_EXPORT_CLASS QYPicture
{
public:
	QYPicture(BYTE statusFlag = IMAGE_STATUS_NORMAL);
	virtual ~QYPicture(void);
	static void SetPictureCallback(void (WINAPI *pictureCallback)(void *data, const char *source, BYTE **ppStream, unsigned int *len) = nullptr, void *data = nullptr);
public:
	int  GetWidth();
	int  GetHeight();
	Bitmap* getBitmap();
	BOOL Load(LPCTSTR lpFileName);
	BOOL load(const char *fileName);
	BOOL Load(UINT uResID,LPCTSTR lpResType,HMODULE hModule = NULL);
	BOOL Load(BYTE *pStream,int len);
	BOOL LoadRGB(BYTE *pStream, int width, int height, int pixelFormat = QY_RGB);
	BOOL Load(UINT uResID);
	BOOL Save(LPCTSTR strPath, LPCTSTR format = L"image/jpeg");
	void Draw(HDC hDc,int nX,int nY,int nWidth = 0,int nHeight = 0);
	void Draw(HDC hDc, QYRect rectDes);
	void Draw(HDC hDc,
		int nX,
		int nY,
		int nWidth /* = 0 */,
		int nHeight /* = 0 */,
		int nSrcX,
		int nSrcY,
		int nSrcWidth,
		int nSrcHeight);
	void SetHovered();
	void SetPressed();
	
private:
	BOOL CreateImage(IStream *pStream);
	BOOL CreateHoverImage();
	BOOL CreatePressImage();
	static int  GetEncoderClsid(LPCTSTR format, CLSID* pClsid);
#if 1
	Bitmap *m_pImage = nullptr;
	Bitmap *m_pHoverImage = nullptr;
	Bitmap *m_pPressImage = nullptr;
	HBITMAP m_hBitmap = nullptr;
	BYTE m_statusFlag;
	unsigned char *m_bitmapData = nullptr;
#endif
	
	enumImageStatus m_Status;
	static void (WINAPI *m_pictureCallback)(void *data, const char *source, BYTE **ppStream, unsigned int *len);
	static void *m_Data;
};
