#pragma once
#include "QYWindow.h"
#include "QYEdit.h"
#include "QYpicture.h"

class QYUI_EXPORT_CLASS QYPagePane :public QYWindow
{
public:
	QYPagePane();
	~QYPagePane();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	BOOL SetButtonText(int button, LPCTSTR strText);
	int SetPage(int page);
	int  GetPage(){ return m_nPage; }
	int SetPageRange(int pageRange);
	int GetPageRange(){ return m_nTotalPages; }
	enum 
	{
		PAGE_BUTTON_LAST = 0,
		PAGE_BUTTON_NEXT,
		PAGE_BUTTON_FIRST,
		PAGE_BUTTON_END,
		PAGE_BUTTON_MAX
	};
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int  m_nSeparatedWidth;
	int  m_selButton;
	int  m_hotButton;
	QYRect m_rectButton[PAGE_BUTTON_MAX];
	QYString m_strButtonText[PAGE_BUTTON_MAX];
	unsigned int m_buttonCmd[PAGE_BUTTON_MAX];
	QYPicture *m_picPage;
	QYRect m_rectStatic;
	QYEdit m_Edit;
	int    m_nButtonHeight;
	int    m_nButtonWidth;
	int    m_nEditWidth;
	int    m_nStaticWidth;
	int    m_nTotalPages;
	int    m_nPage;
public:
	virtual LRESULT OnPaint();
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
};

