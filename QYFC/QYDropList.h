#pragma once

#include <vector>
#include "QYWindow.h"
#include "QYWindowShadow.h"
#include "QYScrollBar.h"
#include "QYButton.h"
#include "QYIScroll.h"
class QYUI_EXPORT_CLASS  QYDropListItem
{
public:
	 QYDropListItem()
	{
		rect = QYRect(0);
		strText = L"";
		data = 0;
	}

	 ~QYDropListItem() {; }
	QYRect   rect;
	QYString strText;
	DWORD_PTR data;
    std::string textID;
    std::string key;
};


enum 
{
	TPDL_LEFT_BOTTOM = 0,
	TPDL_LEFT_TOP,
	TPDL_RIGHT_BOTTOM,
	TPDL_RIGHT_TOP
};

class QYUI_EXPORT_CLASS QYDropList :public QYWindow, public QYIScroll
{
public:
	QYDropList(QYWindow *pSounrceWnd = nullptr);
	virtual ~QYDropList();
	virtual BOOL Show(int nCmdShow);
	BOOL SetCurSel(int cusor);
	int GetCurSel();
	int GetCurSel(LPCTSTR strText);
	QYString GetCuselString(int nCursel);
	BOOL DeleteAllItems();
	int AddItem(const TCHAR* strText, DWORD_PTR data);
	int addItem(const char *key, const char* text, DWORD_PTR data = 0);
	DWORD_PTR GetItemData(int item);
	int FindData(DWORD_PTR data);
	void SetItemHeight(int itemHeight);
	int  GetItemCount();
	void TrackPopup(QYPoint point, int width, int pos, QYWindow *pWnd);
//	QYScrollCtrl* GetScrollCtrl(){ return m_pScrollBar; }
	 BOOL Create(DWORD dwExStyle, 
		LPCTSTR lpClassName,
		LPCTSTR lpWndName, 
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	 void SetDeleteFlag(BOOL bDelete = TRUE) { m_bDeleted = bDelete; }
protected:
	std::vector<QYDropListItem*> m_vectorItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void Clear();
	BOOL UpdateLayout();
	void DrawDeleteRect(QYDC *pDC, QYRect &rect);
	QYWindow  *m_pSourceWnd;
	QYWindowShadow *m_pShadow;
	int  m_nItemHeight;
	BOOL m_bHover;
	int  m_nSelItem;
	int  m_nCursel;
	int  m_DefaultHeight;
	BOOL m_bDownScrollBar;
	QYRect m_rectItem;
	QYPoint m_LastPoint;
	int   m_nStep;
	BOOL  m_bDeleteRectDown;
	BOOL  m_bDeleteRectHot;
	Button_state  m_deleteBtnState;
	BOOL  m_bDeleted;
	int   m_nMinVisibleItems;
public:
	virtual LRESULT OnPaint();
	BOOL OnMove(int cx, int cy);
	 LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	 LRESULT OnSize(UINT nType, int cx, int cy);
	 LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	 LRESULT OnMouseLeave(WPARAM, LPARAM);
	 LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
};

