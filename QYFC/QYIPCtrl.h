#pragma once

#include "QYWindow.h"


class QYUI_EXPORT_CLASS QYIPCtrl :public QYWindow
{
public:
	QYIPCtrl();
	virtual ~QYIPCtrl();
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
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnChar(WPARAM wparam, LPARAM lparam);
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	LRESULT OnKeyDown(WPARAM wparam, LPARAM lparam);
	DWORD  GetValue() const;
	void  GetValue(QYString& ip);
	void  getValue(std::string& ip);
	void   SetValue(LPCTSTR ip);
	void   setValue(const char *ip);
	void   SetValue(DWORD ip);
private:
	void DrawField(const TCHAR *timeText,int  fd,QYDC *pDc,QYRect* rect);
	BOOL PtInField(QYPoint &pt, const TCHAR *timeText, int fd, QYDC *pDc, QYRect* rect);
	BOOL        m_bFocus;
	BOOL        m_bHot;
	bool	m_bLBtDown;
	int     m_Type;
	int      m_selField;
	int       m_nClientPadWidth;
	BOOL      m_bBeginEdit;
	BOOL      m_bHotButtonUp;
	BOOL      m_bHotButtonDown;
	BOOL      m_bPressButtonUp;
	BOOL      m_bPressButtonDown;
	int       m_nIPBlockWidth;
	int       m_nIPPointWidth;
	TCHAR     m_chIP[MAX_IPV4_BLOCK_COUNT][MAX_IPV4_ADDRESS_BLOCK_SIZE];
};

