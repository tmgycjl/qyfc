#pragma once

#include "QYWindow.h"
#include "QYDateTable.h"


class QYUI_EXPORT_CLASS QYFolderEdit :public QYWindow
{
public:
	QYFolderEdit();
	virtual ~QYFolderEdit();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	LRESULT OnPaint();
	LRESULT OnEraseBackRound(HDC hDC);
	LRESULT LoseFocus();
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	const TCHAR* GetDir();
	void SetDir(LPCTSTR strDir);
private:
	BOOL        m_bFocus;
	BOOL        m_bHot;
	BOOL	  m_bLBtDown;
	int       m_Type;
	int       m_nClientPadWidth;
	BOOL      m_bHotButton;
	QYString  m_strDir;
	QYPicture *m_pButtonBackImage;
};

