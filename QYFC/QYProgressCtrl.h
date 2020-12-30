#pragma once
#include "QYWindow.h"
class QYFC_EXPORT_CLASS QYProgressCtrl :
	public QYWindow
{
public:
	QYProgressCtrl();
	~QYProgressCtrl();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	 BOOL OnPaint();
	 LRESULT OnNCPaint();
public:
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd);
	void  SetBkColor(COLORREF grooveBkColor, COLORREF progressBkColor);
	int GetPos();
	void SetPos(int pos);
	void SetRange(int range);
	int GetRange();
private:
	int m_nRange;
	int m_nPos;
	BOOL     m_bHover;
	COLORREF m_BkCor;
	COLORREF m_GrooveBkCor;
	COLORREF m_ProcessBkCor;
};

