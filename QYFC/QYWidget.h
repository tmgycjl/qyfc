#pragma once

#include "QYSize.h"
#include "QYRelative.h"
#include "QYWindow.h"



class QYUI_EXPORT_CLASS QYWidget :
	public QYWindow
{
public:
	QYWidget(QYSize size = 0);
	~QYWidget();
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
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
private:
	QYSize m_Size;
	QYPicture  *m_pImage;
};

