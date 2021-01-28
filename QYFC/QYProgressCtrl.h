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
	void setReadonly(bool readOnly = true);
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
	LRESULT OnMouseMove(UINT nFlags, QYPoint &point);
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	int m_nRange;
	int m_nPos;
	BOOL     m_bHover;
	COLORREF m_BkCor;
	COLORREF m_GrooveBkCor;
	COLORREF m_ProcessBkCor;
	bool _readOnly = true;
	bool _hover = false;
};

