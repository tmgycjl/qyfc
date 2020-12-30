#pragma once
#include "QYWindow.h"
#include "qydefine.h"
#include "QYScrollBar.h"
#include <list>
#include "QYSize.h"

class QYUI_EXPORT_CLASS  QYCtrlListItem
{
public:
	QYCtrlListItem(int nID, QY_CONTROL_TYPE ctrlType, LPCTSTR strTitle, int ctrlHeight)
	{
		m_nID = nID;
		m_nCtrlType = ctrlType;
		m_strTitle  = strTitle;
		m_Size.cy = ctrlHeight;
		m_pWnd = nullptr;
	}

	~QYCtrlListItem()
	{
		SAFE_DESTROY_WINDOW_PTR(m_pWnd);
	}
	int               m_nID;
	QY_CONTROL_TYPE   m_nCtrlType;
	QYWindow *m_pWnd;
	QYString m_strTitle;
	QYSize      m_Size;
};


class QYUI_EXPORT_CLASS QYCtrlList :
	public QYWindow
{
public:
	QYCtrlList(BOOL bInTab = FALSE);
	virtual ~QYCtrlList();
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	BOOL Create(QYWindow *pParent,QYRect rect);
	int  SetCtrlWidth(int ctrlWidth);
	QYWindow* AddCtrl(QY_CONTROL_TYPE ctrlType, LPCTSTR strTitle = L"", int ctrlHeight = QY_THEME.DEFAULT_CTRL_HEIGHT);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	BOOL UpdateLayout();
	void ScrollItem(int off);
	std::list<QYCtrlListItem*> m_listItem;
	int  m_nMaxID;
	int  m_ctrlWidth;
	int  m_ctrlSeparateHeigth;
	int  m_padWidth;
	int  m_padHeight;
	int  m_lastCtrlBottom;
	QYScrollCtrl m_VScrollBar;
	BOOL m_bVShowScroll;
	int  m_nVScrollWidth;
	long  m_nScrollPos;
	int   m_nStep;
	BOOL m_bFocus;
	int  m_nTrackPos;
	int  m_nWheelCount;
	BOOL m_bOpenTrackPosTime;
	int  m_nZDelta;
	BOOL m_bAlternateColorFlag;
	QYRect  m_rectItemList;
	int  m_nMaxTitleTextWidth;
	//int  m_nMaxCtrlWidth;
	int  m_nTitleCtrlRance;
	QYRect m_workRect;
	BOOL m_bInTab;
public:
	void OnTimer(UINT_PTR nIDEvent);
	void OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt);
};

