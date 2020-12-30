#include "QYLinearBitmap.h"
#include "QYDC.h"

static unsigned char g_LinearRGBCaption[] = { 217, 137, 0, 255, 217, 137, 0, 255, 217, 137, 0, 255, 216, 136, 0, 255, 216, 136, 0, 255, 
216, 136, 0, 255, 216, 135, 0, 255, 216, 135, 0, 255, 215, 134, 0, 255, 215, 134, 0, 255, 215, 134, 1, 255, 215, 133, 1, 255, 215, 133, 1, 255,
214, 133, 1, 255, 214, 132, 1, 255, 214, 132, 1, 255, 214, 131, 1, 255, 214, 131, 1, 255, 214, 131, 1, 255, 213, 131, 1, 255, 213, 130, 1, 255, 
213, 130, 1, 255, 213, 130, 1, 255, 213, 129, 1, 255, 213, 129, 1, 255, 212, 129, 1, 255, 212, 128, 1, 255, 212, 128, 1, 255, 212, 128, 1, 255, 
212, 127, 1, 255, 212, 127, 1, 255, 211, 127, 1, 255, 211, 127, 1, 255};

unsigned char  QYLinearBitmap::m_RGBA[MAX_RGB_BYTE] = {0,0,0,0};
unsigned char* QYLinearBitmap::m_LinearRGBArray[MAX_LINEAR_RGB_COUNT] = { 0 };
unsigned int QYLinearBitmap::m_LinearRGBLine[MAX_LINEAR_RGB_COUNT] = { 0 };

QYLinearBitmap::QYLinearBitmap(LINEAR_RGB_ID linearRgbID, int width, int height,BOOL bDialogMode)
{
	m_hBitmap = nullptr;

	int rgbBytes = 4;

	if (0 <= height || 0 <= width)
	{
		if (0 <= linearRgbID && MAX_LINEAR_RGB_COUNT > linearRgbID && nullptr != m_LinearRGBArray[linearRgbID])
		{
			if (width > 0)
			{
				int rgbHeight = 0;
				int defRGBHeight = m_LinearRGBLine[linearRgbID] / rgbBytes;

				if (0 < height && height <= defRGBHeight)
				{
					rgbHeight = height;
				}
				else if (0 == height || height > defRGBHeight)
				{
					rgbHeight = defRGBHeight;
				}

				unsigned char *pData = new unsigned char[rgbHeight * width * rgbBytes];

				if(QY_THEME.CAPTION_BK_GRADIENT)
				{
					for (int i = 0; i < rgbHeight; i++)
					{
						for (int j = 0; j < width; j++)
						{
							memcpy(pData + (i * width * rgbBytes) + (j * rgbBytes), m_LinearRGBArray[linearRgbID] + (i * rgbBytes), rgbBytes);
						}
					}
				}
				else
				{
					COLORREF color = QY_THEME.CAPTION_COLOR;
					if (bDialogMode)
					{
						color = QY_THEME.DLG_CAPTION_COLOR;
					}
					m_RGBA[0] = color & 0xff;
					m_RGBA[1] = (color >> 8) & 0xff;
					m_RGBA[2] = (color >> 16) & 0xff;
					m_RGBA[3] = (color >> 24) & 0xff;

					for (int i = 0; i < rgbHeight; i++)
					{
						for (int j = 0; j < width; j++)
						{
							memcpy(pData + (i * width * rgbBytes) + (j * rgbBytes), m_RGBA, rgbBytes);
						}
					}
				}
				
				m_hBitmap = CreateBitmap(width, rgbHeight, 1, rgbBytes * 8, pData);

				SAFE_DELETE_ARRAY(pData);
			}
			else if (0 != height && 0 == width)
			{

			}
		}

	}
}


QYLinearBitmap::~QYLinearBitmap()
{
	if (nullptr != m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = nullptr;
	}

	if (nullptr != m_hPressBitmap)
	{
		DeleteObject(m_hPressBitmap);
		m_hPressBitmap = nullptr;
	}

	if (nullptr != m_hHoverBitmap)
	{
		DeleteObject(m_hHoverBitmap);
		m_hHoverBitmap = nullptr;
	}
}


const unsigned char*  QYLinearBitmap::GetData(LINEAR_RGB_ID linearRgbID)
{
	if(QY_THEME.CAPTION_BK_GRADIENT)
	{
		if (0 <= linearRgbID && MAX_LINEAR_RGB_COUNT > linearRgbID )
		{
			return m_LinearRGBArray[linearRgbID];
		}
	}
	else
	{
		return m_RGBA;
	}
	
	return nullptr;
}

void QYLinearBitmap::InitGlobalLinearRgb()
{
	m_LinearRGBArray[LINEAR_RGB_ID_CAPTION] = g_LinearRGBCaption;
	m_LinearRGBLine[LINEAR_RGB_ID_CAPTION] = sizeof(g_LinearRGBCaption);
}

bool QYLinearBitmap::Load(LPCTSTR filePath, int type)
{
	m_hBitmap = (HBITMAP)::LoadImage(
		NULL,
		filePath,
		IMAGE_BITMAP,
		1, 0,
		LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (nullptr == m_hBitmap)
	{
		return false;
	}

	BITMAP bitmap;

	GetObject(m_hBitmap, sizeof(BITMAP), &bitmap);

	if (0 >= bitmap.bmHeight || 0 >= bitmap.bmWidthBytes)
	{
		return false;
	}

	unsigned int bmSize = bitmap.bmWidthBytes*bitmap.bmHeight;
	
	unsigned char* pBits = new unsigned char[bmSize];

	LONG dl = GetBitmapBits(m_hBitmap, bmSize, pBits);
	if (0 >= dl)
	{
		delete[] pBits;
		return false;
	}

	unsigned char* pData = new unsigned char[bmSize];

	if (type >= IMAGE_STATUS_HOVER)
	{
		for (int i = 0; i < bmSize; i++)
		{
			pData[i] =  0x11;
		}
		m_hHoverBitmap = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, bitmap.bmBitsPixel, pData);
		if (nullptr == m_hHoverBitmap)
		{
			delete[] pBits;
			delete[] pData;
			return false;
		}
	}
	
	
	if (type >= IMAGE_STATUS_PRESS)
	{
		for (int i = 0; i < bmSize; i++)
		{
			pData[i] = pBits[i]|0x22;
		}
		m_hPressBitmap = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, bitmap.bmBitsPixel, pData);
		if (nullptr == m_hPressBitmap)
		{
			delete[] pBits;
			delete[] pData;
			return false;
		}
	}
#if 0

	char* pWriteBuf = new  char[1024*1024];

	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	if (0 >= bitmap.bmHeight || 0 >= bitmap.bmWidthBytes)
	{
		return false;
	}

	DWORD dwSize = bitmap.bmHeight * bitmap.bmWidthBytes;

	unsigned char* pBits = new unsigned char[dwSize];

	LONG dl = GetBitmapBits(hBitmap, dwSize, pBits);
	if (0 >= dl)
	{
		return false;
	}



	FILE *pFile = nullptr;


	fopen_s(&pFile, "c:\\linearRGB.txt", "w");
	int writeLen = 0;
	for (int i = 0; i < dwSize; i += bitmap.bmWidthBytes)
	{
		writeLen += sprintf_s(pWriteBuf + writeLen, 1024, "%d,%d,%d,%d,", pBits[i], pBits[i + 1], pBits[i + 2], pBits[i + 3]);
	}

	fwrite(pWriteBuf, 1, writeLen, pFile);
	fwrite("\r\n", 2, 1, pFile);

	

	fclose(pFile);

	
	delete[] pWriteBuf;

	::DeleteObject(hBitmap);
#endif

	delete[] pData;
	delete[] pBits;
	return true;
}

void QYLinearBitmap::Draw(QYDC *pDC, QYRect *rect)
{
	HDC hdcMem = CreateCompatibleDC(pDC->GetDC());	
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, m_hBitmap);
	::BitBlt(pDC->GetDC(), rect->left, rect->top, rect->Width(), rect->Height(), hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
}

void QYLinearBitmap::BitBlt(HDC hDC, QYRect rect)
{
	HDC hdcMem = CreateCompatibleDC(hDC);

	HBITMAP oldBitmap = nullptr;
	BITMAP bitmap;
	GetObject(m_hBitmap, sizeof(BITMAP), &bitmap);

	if (IMAGE_STATUS_PRESS == m_Status)
	{
		 oldBitmap = (HBITMAP)SelectObject(hdcMem, m_hPressBitmap);
	}
	else if (IMAGE_STATUS_HOVER == m_Status)
	{
		 oldBitmap = (HBITMAP)SelectObject(hdcMem, m_hHoverBitmap);
	}
	else
	{
		 oldBitmap = (HBITMAP)SelectObject(hdcMem, m_hBitmap);
	}

	::StretchBlt(hDC, rect.left, rect.top, rect.Width(), bitmap.bmHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
	
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);

	m_Status = IMAGE_STATUS_NORMAL;
}