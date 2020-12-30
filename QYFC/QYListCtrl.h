#pragma once
#include "QYWindow.h"
#include "QYScrollBar.h"
#include "QYHeaderCtrl.h"
#include "QYPagePane.h"
#include "QYpicture.h"
#include <list>
#include "QYIScroll.h"


class QYUI_EXPORT_CLASS LISTCTRL_SORT_PARAM
{
public:
	LISTCTRL_SORT_PARAM()
	{
		type = SORT_TYPE_NONE;
		column = 0;
	}
	~LISTCTRL_SORT_PARAM(){ ; }
	int type;
	int column;
protected:
private:
};

class QYUI_EXPORT_CLASS  QYListCtrlSubItem
{
public:
	QYListCtrlSubItem(TCHAR *text)
	{
		strText = text;
		left = 0;
		right = 0;
		data = 0;
	}
	QYListCtrlSubItem(const char*text)
	{
		SET_TEXT(strText, text)
		left = 0;
		right = 0;
		data = 0;
	}
	QYListCtrlSubItem()
	{
		strText = L"";
		left = 0;
		right = 0;
		data = 0;
	}

	~QYListCtrlSubItem() { ; }

	
	QYString strText;
	int left;
	int right;
	DWORD_PTR data;
};

class QYUI_EXPORT_CLASS  QYListCtrlItem
{
public:
	QYListCtrlItem()
	{
		check = 0;
		data = 0;
	}

	~QYListCtrlItem()
	{
		for (auto it = m_listSubItem.begin(); it != m_listSubItem.end(); it++)
		{
			SAFE_DELETE(*it);
		}

		m_listSubItem.clear();
	}

	//bool less_num(const QYListCtrlItem *item1, const QYListCtrlItem *item2);
	bool operator < (const QYListCtrlItem &m)const {
		return true;
	}
	std::list<QYListCtrlSubItem*> m_listSubItem;
	DWORD_PTR data;
	char check;
};


class QYUI_EXPORT_CLASS QYListCtrl :public QYWindow , public QYIScroll
{
public:
	QYListCtrl();
	virtual ~QYListCtrl();
	BOOL SetRedraw(BOOL bRedraw = TRUE);
	void SetHeaderCtrl(QYHeaderCtrl *pHeaderCtrl);
	BOOL SetAlternateColorFlag(BOOL bAlternateColorFlag);
	BOOL SetCheckFlag(BOOL bCheckFlag);
	BOOL SetPageFlag(BOOL bPageFlag,int visibleItemCount = 1000);
	BOOL SetSortColmun(int colmun,int sortType);
	BOOL SetCheckAll(BOOL bCheckAll);
	BOOL IsCheckAll(); 
	BOOL SetCheck(int item, BOOL bCheck);
	BOOL GetCheck(int item);
	int  GetCheckItems(int *itemArray);
	BOOL SetCheck( BOOL bCheck);
	BOOL GetCheck();
	BOOL DeleteAllItems();
	BOOL SetItemData(int item, DWORD_PTR data);
	BOOL SetItemData(DWORD_PTR data);
	DWORD_PTR GetItemData(int item);
	int  GetItemDatas(int *itemArray, int items, DWORD_PTR *data);
	BOOL SetSubItemData(int item, int subitem, DWORD_PTR data);
	BOOL SetSubItemData(int subitem, DWORD_PTR data);
	DWORD_PTR GetSubItemData(int item, int subitem );
	int InsertColumn(int column, const TCHAR* strText, 
		int width = 0, int format = DT_LEFT, 
		enumSortType sortType = SORT_TYPE_NONE, 
		DWORD_PTR data = 0);
    int insertColumn(int column, const char* text,
        int width = 0, int format = DT_LEFT,
        enumSortType sortType = SORT_TYPE_NONE,
        DWORD_PTR data = 0);
	int InsertColumn(const TCHAR* strText, 
		int width, 
		int format = DT_LEFT, 
		enumSortType sortType = SORT_TYPE_NONE, 
		DWORD_PTR data = 0);
	int insertColumn(const char* text,
        int width,
        int format = DT_LEFT,
        enumSortType sortType = SORT_TYPE_NONE,
        DWORD_PTR data = 0);
	BOOL SetColumnText(int column, const TCHAR* strText);
	TCHAR* GetColumnText(int column);
	BOOL SetColumnWidth(int column, int width);
	void SortItems(int column, int  ascend);
	BOOL DeleteColumn(int column);
	BOOL DeleteAllColumns();
	int GetColumnCount();
	int SetItemHeight(int height);
	BOOL GetItemRect(int item,int subitem,QYRect &rect);
	virtual int InsertItem(int item, const TCHAR* strText, int image = -1 , DWORD_PTR data = 0);
	virtual int InsertItem(const TCHAR* strText, int image = -1, DWORD_PTR data = 0);
	virtual int insertItem(int item, const char* text, int image = -1, DWORD_PTR data = 0);
	virtual int insertItem(const char* text, int image = -1, DWORD_PTR data = 0);
	BOOL SetItem(int item, int subitem, const TCHAR* strText, DWORD_PTR data = 0);
	BOOL setItem(int item, int subitem, const char* text, DWORD_PTR data = 0);
	LPCTSTR GetItemText(int item, int subitem);
	BOOL SetItem(int subitem, const TCHAR* strText,DWORD_PTR data = 0);
	BOOL setItem(int subitem, const char* text, DWORD_PTR data = 0);
	BOOL setItemText(int item,int subitem, const char* text);
	BOOL DeleteItem(int startItem, int count);
	BOOL DeleteItem(int item);
	int GetItemCount();
	int GetSelectItem(){ return m_nSelItem; }
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
    void setAlwaysSelected(BOOL alwaysSelected = TRUE);
	static LISTCTRL_SORT_PARAM listctrl_sort_param;
protected:
	static unsigned int m_nMaxItemCount;
	std::list<QYListCtrlItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Clear();
	BOOL UpdateLayout();
	QYListCtrlSubItem* FindSubItem(int item, int subitem);
	QYListCtrlItem* FindItem(int item);
	void OnColmunWidthChanged(int colmun);
	BOOL Less(LPCTSTR strText1, LPCTSTR strText2, enumSortType sortType);
	static bool less_num(const QYListCtrlItem  *item1, const QYListCtrlItem *item2);
	static bool large_num(const QYListCtrlItem  *item1, const QYListCtrlItem *item2);
	QYHeaderCtrl  *m_pHeaderCtrl;
	QYPagePane    *m_pPagePane;
	int  m_nItemHeight;
	BOOL m_bHover;
	BOOL m_bDown;
	int  m_nSelItem;
	int  m_nHoverItem;
	int  m_nLastHoverItem;
	BOOL m_bFocus;
	int  m_nSeparatedWidth;
	int  m_nHeaderHeight;
	int  m_nPagePaneHeight;
	QYRect  m_rectItemList;
	QYListCtrlItem  *m_pLastItem;
	BOOL m_bCheckFlag;
	BOOL m_bAlternateColorFlag;
	BOOL m_bShowPagePane;
	unsigned int  m_nVisibleItemCount;
	int  m_nFirstVisibleItem;
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
    BOOL  m_alwaysSelected;
public:
	virtual LRESULT OnPaint();
	BOOL OnMove(int cx, int cy);
	virtual LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnRButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnRButtonUp(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	virtual LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	virtual LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnClickColmun(WPARAM wparam,LPARAM lparam);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	void OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	void OnHScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	virtual LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt);
	virtual LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
};

