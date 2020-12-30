#pragma once
#include "QYWindow.h"


class QYUI_EXPORT_CLASS QYRectBackground :
	public QYWindow
{
public:
	QYRectBackground(QYSize size = 0);
	~QYRectBackground();
	QYSize GetSize(){ return m_Size; }
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);

	BOOL SetImage(const char* strImageFileName);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
private:
	QYSize m_Size;
	QYPicture  *m_pImage;
};

