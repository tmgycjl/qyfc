#pragma once

#include "QYWindow.h"
#include "qysize.h"

class QYUI_EXPORT_CLASS QYSizeBox
{
public:
	QYSizeBox();
	~QYSizeBox();
protected:
	BOOL StartResize(QYWindow *pWnd,QYPoint &point);
	BOOL Resize(QYWindow *pWnd, QYPoint &point);
	BOOL EndResize();
	enum{
		RESIZE_LEFT = 0,
		RESIZE_RIGHT,
		RESIZE_UP,
		RESIZE_DOWN,
		RESIZE_LEFT_UP,
		RESIZE_LEFT_DOWN,
		RESIZE_RIGHT_UP,
		RESIZE_RIGHT_DOWN
	};

	QYPoint    m_LastPoint;
	int      m_nResizeAction;
	BOOL     m_bDownBorder;
	int      m_nSizeBoxWidth;
	QYRect   m_lastWndRect;
	QYSize   m_minSize;
	MINMAXINFO  m_MaxMinInfo;
	HCURSOR m_hCursor;
	
};

