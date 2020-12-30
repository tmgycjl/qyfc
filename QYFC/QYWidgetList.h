#pragma once
#include "QYWindow.h"
#include "qydefine.h"
#include "QYScrollBar.h"
#include <vector>
#include "QYSize.h"
#include "QYWidget.h"
#include "QYIScroll.h"

class QYUI_EXPORT_CLASS QYWidgetList :
	public QYWindow,
	public QYIScroll
{
public:
	QYWidgetList(BOOL bInTab = FALSE);
	virtual ~QYWidgetList();
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	BOOL Create(QYWindow *pParent,QYRect rect);
	QYWidget* Append(QYSize sizeWidget);
	int GetSeparateHeigth(){ return m_ctrlSeparateHeigth; }
	void SetSeparateHeigth(int ctrlSeparateHeigth)
	{
		m_ctrlSeparateHeigth = ctrlSeparateHeigth;
		m_padHeight = m_ctrlSeparateHeigth; 
		m_lastCtrlBottom = m_padHeight;
	}
	void enableScroll(BOOL enable = TRUE);
	std::vector<QYWidget*>* getWidgetList(){ return &m_listItem; }
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	BOOL UpdateLayout();
	void ScrollItem(int off);
	std::vector<QYWidget*> m_listItem;
	int  m_nMaxID;
	int  m_ctrlWidth;
	int  m_ctrlSeparateHeigth;
	int  m_padWidth;
	int  m_padHeight;
	int  m_lastCtrlBottom;
	BOOL m_bFocus;
	BOOL m_bAlternateColorFlag;
	QYRect  m_rectItemList;
	int  m_nMaxTitleTextWidth;
	int  m_nTitleCtrlRance;
	QYRect m_workRect;
	BOOL m_bInTab;
	int  m_Orientation;
	BOOL m_enableScroll = TRUE;
public:;
	void OnTimer(UINT_PTR nIDEvent);
};

