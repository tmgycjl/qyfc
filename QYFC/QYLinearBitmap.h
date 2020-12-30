#pragma once

#include "QYString.h"
#include "QYApp.h"
#include "QYMessageMap.h"
#include "QYRect.h"
#include "qycolor.h"
#include "qydefine.h"

#define  MAX_RGB_BYTE    4
#define  MAX_LINEAR_RGB_COUNT  128

class  QYDC;

enum LINEAR_RGB_ID
{
	LINEAR_RGB_ID_CAPTION = 0,
};

class QYUI_EXPORT_CLASS QYLinearBitmap
{
public:
	QYLinearBitmap()
	{
		m_hBitmap = nullptr;
	}
	QYLinearBitmap(LINEAR_RGB_ID linearRgbID, int width, int height, BOOL bDialogMode = FALSE);
	~QYLinearBitmap();

	bool Load(LPCTSTR filePath, int type = IMAGE_STATUS_NORMAL);

	static void InitGlobalLinearRgb();
	void Draw(QYDC *pDC,QYRect *rect);
	void BitBlt(HDC hDC, QYRect rect);
	void press();
	void setHovered()
	{

			m_Status = IMAGE_STATUS_HOVER;
		

	}

	void setPressed()
	{

			m_Status = IMAGE_STATUS_PRESS;


	}

	static const unsigned char* GetData(LINEAR_RGB_ID linearRgbID);
private:
	static unsigned char m_RGBA[MAX_RGB_BYTE];
	static unsigned char* m_LinearRGBArray[MAX_LINEAR_RGB_COUNT];
	static unsigned int m_LinearRGBLine[MAX_LINEAR_RGB_COUNT];
	HBITMAP  m_hBitmap;
	HBITMAP  m_hHoverBitmap;
	HBITMAP  m_hPressBitmap;
	int m_Status = IMAGE_STATUS_NORMAL;
};

