#pragma once

#include "QYButton.h"
#include "QYpicture.h"
#include "QYSize.h"
class QYUI_EXPORT_CLASS QYSwitchButton :
	public QYButton
{
public:
	QYSwitchButton();
	virtual ~QYSwitchButton();//
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	LRESULT OnNCPaint();
	BOOL SetCheck(BOOL bCheck = TRUE);
	BOOL GetCheck();
	QYSize& GetBackImageSize()const;
protected:
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
	BOOL         m_bCheck;
};

