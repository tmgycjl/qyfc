#pragma once

#include "includedef.h"
#include "QYWindow.h"

class QYUI_EXPORT_CLASS QYCaptionCtrl
{
public:
	QYCaptionCtrl();
	~QYCaptionCtrl();
	void Draw(HDC hDC, int left, int top, int right, int bottom,LPCTSTR strCaption,COLORREF color = RGB(255,204,51));
	void Draw(HDC hDC, QYRect  &rect, LPCTSTR strCaption, COLORREF color);
};

