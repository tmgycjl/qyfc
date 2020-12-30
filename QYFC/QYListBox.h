#pragma once
#include "QYWindow.h"
#include "QYScrollBar.h"
#include <vector>

class QYUI_EXPORT_CLASS  QYListBoxItem
{
public:
	QYListBoxItem()
	{
		rect = QYRect(0);
		strText = L"";
		data = 0;
	}

	~QYListBoxItem() { ; }
	QYRect   rect;
	QYString strText;
	DWORD_PTR data;
};


class QYUI_EXPORT_CLASS QYListBox :
	public QYWindow
{
public:
	QYListBox();
	virtual ~QYListBox();

	BOOL DeleteAllItems();
	int AddItem(const TCHAR* strText, DWORD_PTR data = 0);
	int GetItemCount();
	QYScrollCtrl* GetScrollCtrl(){ return m_pScrollBar; }
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
protected:
	std::vector<QYListBoxItem*> m_vectorItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void Clear();
	QYScrollCtrl  *m_pScrollBar;
	int  m_nItemHeight;
	BOOL m_bShowScroll;
	int  m_nScrollWidth;
	int  m_nScrollPos;
	int   m_nStep;
	BOOL m_bDown;
	int  m_nSelItem;
	int  m_nTrackPos;
	int  m_nWheelCount;
	BOOL m_bOpenTrackPosTime;
	int  m_nZDelta;
	QYRect  m_rectItemList;
public:
	virtual LRESULT OnPaint();
	LRESULT OnNCPaint();
	BOOL OnMove(int cx, int cy);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	void OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt);
};

