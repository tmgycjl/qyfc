
#include "QYPicture.h"
#include "QYApp.h"
#include "QYString.h"

void (WINAPI *QYPicture::m_pictureCallback)(void *data, const char *source, BYTE **ppStream, unsigned int *len) = nullptr;
void* QYPicture::m_Data = nullptr;

void QYPicture::SetPictureCallback(void (WINAPI *pictureCallback)(void *data, const char *source, BYTE **ppStream, unsigned int *len), void *data)
{
	m_pictureCallback = pictureCallback;
	m_Data = data;
}

void captureBMP(void *data, int nWidth, int nHeight)
{
	FILE *fp = nullptr;

	char path[MAX_PATH] = { 0 };
	sprintf_s(path, "c:\\work\\face%d.bmp", GetTickCount());

	fopen_s(&fp, path, "wb");

	if (NULL == fp) return;

	BITMAPFILEHEADER bmpFileHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	bmpFileHeader.bfType = MAKEWORD('B', 'M');
	bmpFileHeader.bfSize = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + 3 * nWidth * nHeight;
	bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);
	bmpFileHeader.bfReserved1 = bmpFileHeader.bfReserved2 = 0;

	bmpInfoHeader.biSize = sizeof(bmpInfoHeader);
	bmpInfoHeader.biWidth = nWidth;
	bmpInfoHeader.biHeight = 1 * nHeight;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biSizeImage = 0;
	bmpInfoHeader.biXPelsPerMeter = 0;
	bmpInfoHeader.biYPelsPerMeter = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;

	fwrite(&bmpFileHeader, 1, sizeof(bmpFileHeader), fp);
	fwrite(&bmpInfoHeader, 1, sizeof(bmpInfoHeader), fp);
	fwrite(data, 1, 3 * nWidth*nHeight, fp);
	fclose(fp);
}


int QYPicture::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{

	UINT num = 0;

	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);

	if (size == 0)

	{

		return -1;

	}

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

	if (pImageCodecInfo == NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)

	{

		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)

		{

			*pClsid = pImageCodecInfo[j].Clsid;

			free(pImageCodecInfo);

			return j;

		}

	}

	free(pImageCodecInfo);

	return -1;

}

QYPicture::QYPicture(BYTE statusFlag)
{
	m_pImage = NULL;
	m_statusFlag = statusFlag;
	m_Status = IMAGE_STATUS_NORMAL;
	m_pHoverImage = nullptr;
	m_pPressImage = nullptr;

}

QYPicture::~QYPicture(void)
{
	
	SAFE_DELETE(m_pImage);
	SAFE_DELETE(m_pHoverImage);
	SAFE_DELETE(m_pPressImage);
	if (nullptr != m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = nullptr;
	}
	SAFE_DELETE_ARRAY(m_bitmapData);
}

int QYPicture::GetWidth()
{
	if (nullptr != m_pImage)
	{
		return m_pImage->GetWidth();
	}
	else if (nullptr != m_hBitmap)
	{
		BITMAP bm;
		::GetObject(m_hBitmap, sizeof(BITMAP), &bm);
		return bm.bmWidth;
	}
	
	return 1;
}

int QYPicture::GetHeight()
{
	if (nullptr != m_pImage)
	{
		return m_pImage->GetHeight();
	}
	else if (nullptr != m_hBitmap)
	{
		BITMAP bm;
		::GetObject(m_hBitmap, sizeof(BITMAP), &bm);
		return bm.bmHeight;
	}
	return 1;
	
}

Bitmap* QYPicture::getBitmap()
{
	return m_pImage;
}

BOOL QYPicture::Load(BYTE *pStream, int len)
{
	BOOL res = FALSE;

	if (NULL != m_pImage || pStream == NULL || 0 >= len)
	{
		return FALSE;
	}

	IStream *stream;
	
	HGLOBAL hGblMem = GlobalAlloc(GMEM_FIXED,len);
	BYTE *pByte = (BYTE*)GlobalLock(hGblMem);
	CreateStreamOnHGlobal(hGblMem, TRUE, (LPSTREAM*)&stream);

	ULONG outLen = 0;
	stream->Write(pStream, len, &outLen);

	res = CreateImage(stream);

	GlobalUnlock(hGblMem);
	GlobalFree(hGblMem);
	//stream->Release();

	return res;
}

BOOL QYPicture::load(const char *fileName)
{
	BYTE *pStream = nullptr;
	unsigned int len = 0;

	if (nullptr != m_pictureCallback && nullptr != m_Data)
	{
		m_pictureCallback(m_Data, fileName, &pStream, &len);
		if (nullptr == pStream)
		{
			TCHAR wszFileName[MAX_PATH] = { 0 };
			QYString::SafeUTF82W(wszFileName, MAX_PATH, fileName);

			return Load(wszFileName);
		}
		BOOL ret = Load(pStream, len);

		SAFE_DELETE_ARRAY(pStream);

		return ret;
	}
	else
	{
		TCHAR imagePath[MAX_PATH] = { 0 };
		QYString::SafeA2W(imagePath, MAX_PATH, (QYApp::m_imagePath + fileName).c_str());

		return Load(imagePath);
	}

	return FALSE;
}

BOOL QYPicture::Load(LPCTSTR lpFileName)
{
	if (NULL != m_pImage)
	{
		return FALSE;
	}

	m_pImage = new Bitmap(lpFileName);
	
	if (m_statusFlag >= IMAGE_STATUS_HOVER)
	{
		if (!CreateHoverImage())
		{
			return FALSE;
		}
		if (m_statusFlag >= IMAGE_STATUS_PRESS)
		{
			return CreatePressImage();
		}

	}

	return TRUE;
}

BOOL QYPicture::Load(UINT uResID)
{
	BYTE *pStream = nullptr;
	unsigned int len = 0;

	if (nullptr != m_pictureCallback && nullptr != m_Data)
	{
		//m_pictureCallback(m_Data,uResID, &pStream, &len);

		BOOL ret = Load(pStream, len);

		SAFE_DELETE_ARRAY(pStream);

		return ret;
	}

	return FALSE;
}

BOOL QYPicture::Load(UINT uResID, LPCTSTR lpResType, HMODULE hModule)
{
	BOOL res = FALSE;

	if (NULL != m_pImage)
	{
		return FALSE;
	}

	if (lpResType == QY_IMAGE_BMP)
	{
		m_pImage = Bitmap::FromResource(nullptr,MAKEINTRESOURCE(uResID));
	}
	else 
	{
		HRSRC hRsrc = nullptr;
		HINSTANCE  hInst = nullptr;
		if (nullptr != hModule)
		{
			hInst = hModule;
		}
		else
		{
			hInst = GetModuleHandle(QY_MODULE_NAME);
		}

		hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(uResID), lpResType); // type
		if (NULL == hRsrc)
		{
			return FALSE;
		}
		
		DWORD len = SizeofResource(hInst, hRsrc);
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
		if (!lpRsrc)
			return FALSE;

		HGLOBAL hGblMem = GlobalAlloc(GMEM_FIXED,len);

		BYTE *pByte = (BYTE*)GlobalLock(hGblMem);
		memcpy(pByte,lpRsrc,len);
		IStream *stream = nullptr;
		CreateStreamOnHGlobal(hGblMem, TRUE, (LPSTREAM*)&stream);
	
		res = CreateImage(stream);

		GlobalUnlock(hGblMem);
		GlobalFree(hGblMem);
		//stream->Release();
		FreeResource(hRsrc);
		
	}
	
	return res;
}

BOOL QYPicture::CreateImage(IStream *pStream)
{
	m_pImage = new Bitmap(pStream);
	if (nullptr == m_pImage)
	{
		return FALSE;
	}

	if (m_statusFlag >= IMAGE_STATUS_HOVER)
	{
		if (CreateHoverImage())
		{
			if (m_statusFlag >= IMAGE_STATUS_PRESS)
			{
				return CreatePressImage();
			}
			else
			{
				return TRUE;
			}
			
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL QYPicture::CreateHoverImage()
{
	//return TRUE;
	if (NULL == m_pImage || 0 >= m_pImage->GetWidth())
	{
		return FALSE;
	}
	
	Rect rc(0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
	m_pHoverImage = m_pImage->Clone(rc, PixelFormat32bppARGB);

	//m_pHoverImage = new Bitmap(rc.Width, rc.Height);

	BitmapData* bmpData = new BitmapData;
	
	Status  stat = m_pHoverImage->LockBits(&rc, ImageLockModeWrite, m_pHoverImage->GetPixelFormat(), bmpData);
	if (Status::Ok == stat)
	{

		UINT* tempBuff = (UINT*)bmpData->Scan0;
		for (int i = 0; i < bmpData->Height * bmpData->Width; i++)
		{
			tempBuff[i] |= 0x00444444;
		}
		m_pHoverImage->UnlockBits(bmpData);
	}
	
	delete[] bmpData;

	return TRUE;
}

BOOL QYPicture::CreatePressImage()
{
	//return TRUE;
	if (NULL == m_pImage || 0 >= m_pImage->GetWidth())
	{
		return FALSE;
	}

	Rect rc(0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
	m_pPressImage = m_pImage->Clone(rc, PixelFormat32bppARGB);

	//m_pPressImage = new Bitmap(rc.Width, rc.Height);

	BitmapData* bmpData = new BitmapData;

	Status  stat = m_pPressImage->LockBits(&rc, ImageLockModeWrite, m_pPressImage->GetPixelFormat(), bmpData);
	if (Status::Ok == stat)
	{
		UINT* tempBuff = (UINT*)bmpData->Scan0;
		for (int i = 0; i < bmpData->Height * bmpData->Width; i++)
		{
			tempBuff[i] |= 0x00888888;
		}
		m_pPressImage->UnlockBits(bmpData);
	}

	delete[] bmpData;

	return TRUE;
}

void QYPicture::Draw(HDC hDc,
					  int nX,
					  int nY,
					  int nWidth /* = 0 */,
					  int nHeight /* = 0 */,
					  int nSrcX,
					  int nSrcY,
					  int nSrcWidth,
					  int nSrcHeight)
{
	Bitmap *pBitmap = m_pImage;

	if (IMAGE_STATUS_PRESS == m_Status)
	{
		pBitmap = m_pPressImage;
	}
	else if (IMAGE_STATUS_HOVER == m_Status)
	{
		pBitmap = m_pHoverImage;
	}

	if (NULL == pBitmap)
	{
		return;
	}

	Graphics imgraphics2(hDc);

	Rect rectDes;
	rectDes.X = nX;
	rectDes.Y = nY;
	rectDes.Width = nWidth;
	rectDes.Height = nHeight;
	
	
	if (0 == nSrcWidth && 0 == nSrcHeight)
	{
		nSrcWidth = pBitmap->GetWidth();
		nSrcHeight = pBitmap->GetHeight();
	}

	imgraphics2.DrawImage(pBitmap, rectDes, nSrcX, nSrcY, nSrcWidth, nSrcHeight, UnitPixel);
	m_Status = IMAGE_STATUS_NORMAL;

}

void QYPicture::Draw(HDC hDc, int nX, int nY, int nWidth /* = 0 */, int nHeight /* = 0 */)
{
	Bitmap *pBitmap = m_pImage;

	if (IMAGE_STATUS_PRESS == m_Status)
	{
		pBitmap = m_pPressImage;
	}
	else if (IMAGE_STATUS_HOVER == m_Status)
	{
		pBitmap = m_pHoverImage;
	}

	if (NULL != pBitmap)
	{
		Graphics imgraphics2(hDc);


		if (0 == nWidth || 0 == nHeight)
		{
			imgraphics2.DrawImage(pBitmap, nX, nY);
		}
		else
		{
			imgraphics2.DrawImage(pBitmap, nX, nY, nWidth, nHeight);
		}

		m_Status = IMAGE_STATUS_NORMAL;
	}
	else if (nullptr != m_hBitmap)
	{
		HDC hMemDC = ::CreateCompatibleDC(hDc);
		::SelectObject(hMemDC, m_hBitmap);

		::BitBlt(hDc, nX, nY, nWidth, nHeight, hMemDC, 0, 0,SRCCOPY);
		DeleteDC(hMemDC);
	}
}

void QYPicture::Draw(HDC hDc, QYRect rectDes)
{
	return Draw(hDc,rectDes.left, rectDes.top, rectDes.Width(), rectDes.Height());
}

void QYPicture::SetHovered()
{
	if (m_statusFlag >= IMAGE_STATUS_HOVER)
	{
		m_Status = IMAGE_STATUS_HOVER;
	}
	
}

void QYPicture::SetPressed()
{
	if (m_statusFlag >= IMAGE_STATUS_PRESS)
	{
		m_Status = IMAGE_STATUS_PRESS;
	}
	
}

BOOL QYPicture::LoadRGB(BYTE *pStream, int width, int height, int pixelFormat)
{

	int dstWidth = width;
	int diff = (dstWidth % 4);
	if (0 != diff)
	{
		dstWidth -= diff;
	}
	

	BITMAPINFO bmInfo;
	memset(&bmInfo, 0, sizeof(bmInfo));
	bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);
	bmInfo.bmiHeader.biWidth = dstWidth;
	bmInfo.bmiHeader.biHeight = -1 * height;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 24;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed  = 0;
	bmInfo.bmiHeader.biClrImportant  = 0;
	//bmInfo.bmiColors[0].rgbRed = 255;

	m_bitmapData = new unsigned char[3 * dstWidth * height];
	
#if 0


	int fileHeaderSize = sizeof(BITMAPFILEHEADER) + 2;
	BITMAPFILEHEADER bmpHeader;
	 fileHeaderSize = sizeof(bmpHeader);

	int bmSize = 3 * width * height + fileHeaderSize + sizeof(BITMAPINFOHEADER);
	
	assert(nullptr != bmBuffer);

	BITMAPFILEHEADER *bmpFileHeader = (BITMAPFILEHEADER*)bmBuffer;
	bmpFileHeader->bfType = MAKEWORD('B', 'M');
	bmpFileHeader->bfSize = bmSize;
	bmpFileHeader->bfOffBits = fileHeaderSize + sizeof(BITMAPINFOHEADER);
	bmpFileHeader->bfReserved1 = bmpFileHeader->bfReserved2 = 0;

	BITMAPINFOHEADER *bmpInfoHeader = (BITMAPINFOHEADER*)(bmBuffer + fileHeaderSize);
	bmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader->biWidth = width;
	bmpInfoHeader->biHeight = -1 * height;
	bmpInfoHeader->biPlanes = 1;
	bmpInfoHeader->biBitCount = 24;
	bmpInfoHeader->biCompression = BI_RGB;
	bmpInfoHeader->biSizeImage = 0;
	bmpInfoHeader->biXPelsPerMeter = 0;
	bmpInfoHeader->biYPelsPerMeter = 0;
	bmpInfoHeader->biClrUsed = 0;
	bmpInfoHeader->biClrImportant = 0;

	
#endif
	unsigned char *pData = m_bitmapData;
	unsigned char *srcData = pStream;
	switch (pixelFormat)
	{
	case QY_BGR:
	{
#if 1
		int start = diff / 2;
		
		for(int i = 0; i < height; i++)
		{
			int k = 0;
			for (int j = start; j < width&& k < dstWidth; j++, k++)
			{
				pData[3 * k] = srcData[3 * j + 2];
				pData[3 * k + 1] = srcData[3 * j + 1];
				pData[3 * k + 2] = srcData[3 * j];
			}
			pData += dstWidth * 3;
			srcData += width * 3;
		}
#else
		memcpy(m_bitmapData, pStream, dstWidth*height * 3);
#endif
		
	}
		break;
	case QY_RGBA:
		break;
	case QY_RGB:
		break;
	case QY_BGRA:
		break;
	default:
		break;
	}

	m_pImage = Bitmap::FromBITMAPINFO(&bmInfo, m_bitmapData);
	
	return (nullptr != m_pImage);
}

BOOL QYPicture::Save(LPCTSTR strPath, LPCTSTR format)
{
	if (nullptr != m_pImage)
	{
		CLSID jpegClsid;

		GetEncoderClsid(format, &jpegClsid);
		return (Status::Ok == m_pImage->Save(strPath, &jpegClsid));
	}

	return FALSE;
}