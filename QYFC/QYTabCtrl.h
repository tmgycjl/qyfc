#pragma once
#include "QYWindow.h"
#include <list>

class QYUI_EXPORT_CLASS  QYTabCtrlItem
{
public:
	QYTabCtrlItem(QYWindow *pWnd,LPCTSTR strText,DWORD_PTR data)
	{
		this->rect = 0;
		this->strText = strText;
		this->data = data;
		this->format = DT_CENTER;
		this->pWnd = pWnd;
	}
	~QYTabCtrlItem() { ; }
	BYTE    format;
	QYString strText;
	DWORD_PTR data;
	QYWindow *pWnd;
	QYRect rect;
};

enum enumTabDockPos
{
	TAB_DOCK_LEFT = 0,
	TAB_DOCK_TOP, 
	TAB_DOCK_RIGHT, 
	TAB_DOCK_BOTTOM
};

class QYUI_EXPORT_CLASS QYTabCtrl :public QYWindow
{
public:
	QYTabCtrl();
	virtual ~QYTabCtrl();
	int InsertItem(int item, QYWindow *pWnd,const TCHAR* strText = L"", int width = 0, int format = 256, DWORD_PTR data = 0);
	BOOL SetItemText(int item, const TCHAR* strText);
	BOOL SetItemText(QYWindow *pWnd, const TCHAR* strText);
	DWORD_PTR GetItemData(int item);
	int GetSelectItem();
	int SetItemHeight(const int height);
	BOOL DeleteItem(int item);
	BOOL DeleteAllItems();
	int GetItemCount();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	QYTabCtrlItem* GetItem(int item);
	QYWindow* SelectItem(QYWindow *pWnd);
	QYWindow* SelectItem(int selItem);
	void setDockPos(enumTabDockPos dockPos){ m_TabDockPos = dockPos; }
protected:
	std::list<QYTabCtrlItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int GetLastItemRect(QYRect &rect);
	void Clear();
	BOOL GetItemRectFromIndex(int index,QYRect rectWnd,QYRect &rect);
	BOOL m_bHover;
	BOOL m_bDown;
	int  m_nHoverItem;
	int  m_nSelItem;
	int  m_TabDockPos;
	int  m_nItemHeight;
	int  m_nSeparatePixel;
	QYFont *m_pSelFont;
public:
	virtual LRESULT OnPaint();
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
};
