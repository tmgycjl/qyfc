#pragma once
#include "QYWindow.h"
#include "QYSize.h"

class QYUI_EXPORT_CLASS QYDragWindow :
	public QYWindow
{
public:
	QYDragWindow();
	~QYDragWindow();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	void TrackPopup(QYPoint point);
protected:
	 LRESULT OnPaint();
	  LRESULT OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	QYWindow  *m_pSourceWnd;
	QYSize m_sizeWnd;
	QYPicture *m_pPic;
};

