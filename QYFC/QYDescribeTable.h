#pragma once
#include "QYWindow.h"
#include <list>
#include "QYScrollBar.h"
#include "QYSize.h"

class QYUI_EXPORT_CLASS  QYDescribeTableItem
{
public:
	QYDescribeTableItem()
	{
		data = 0;
	}
	QYDescribeTableItem(LPCTSTR strField,LPCTSTR strDesc,DWORD_PTR data)
	{
		this->strField = strField;
		this->strDesc = strDesc;
		this->data = data;
	}
	~QYDescribeTableItem() { ; }
	QYString strField;
	QYString strDesc;
	DWORD_PTR data;
	QYSize textSize;
	int height;
};

class QYUI_EXPORT_CLASS QYDescribeTable :
	public QYWindow
{
public:
	QYDescribeTable();
	~QYDescribeTable();
	int InsertItem(int item, const TCHAR* strField = nullptr, const TCHAR* strDesc = nullptr, DWORD_PTR data = 0);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	BOOL SetRedraw(BOOL bRedraw);
protected:
	QYScrollCtrl  *m_pVScrollBar;
	BOOL m_bHover;
	BOOL m_bVShowScroll;
	int  m_nVScrollWidth;
	long  m_nScrollPos;
	int   m_nStep;
	BOOL m_bFocus;
	int  m_nSeparatedWidth;
	QYRect  m_rectItemList;
	int  m_nTrackPos;
	int  m_nHTrackPos;
	int  m_nWheelCount;
	BOOL m_bOpenTrackPosTime;
	int  m_nZDelta;
	BOOL m_bCheckFlag;
	BOOL m_bAlternateColorFlag;
	int  m_nFieldWidth;
	int  m_nTextCap;
	int  m_nRowSpace;
	int  m_nItemHeight;
	BOOL UpdateLayout();
	std::list<QYDescribeTableItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Clear();
	virtual LRESULT OnPaint();
	LRESULT OnSize(UINT nType, int cx, int cy);
	void OnTimer(UINT_PTR nIDEvent);
	void OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt);
};

