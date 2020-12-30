#pragma once

#include "QYWindow.h"
class QYUI_EXPORT_CLASS QYStatic : public QYWindow
{
public:
	QYStatic();
	virtual ~QYStatic();
	static LRESULT CALLBACK StaticProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	void SetTextFormat(UINT removeFormat,UINT addFormat);
private:
	UINT m_textFormat;
	
};

