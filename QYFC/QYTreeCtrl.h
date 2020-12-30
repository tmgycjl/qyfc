#pragma once
#include "QYWindow.h"
#include "QYpicture.h"
#include "QYScrollBar.h"
#include <list>
#include "QYSize.h"
#include "QYImageList.h"
#include "QYToolTip.h"
#include "QYIScroll.h"

class QYUI_EXPORT_CLASS  QYTreeCtrlItem
{
public:
	QYTreeCtrlItem(QYTreeCtrlItem  *parentItem, TCHAR *text, int image = -1, DWORD_PTR data = 0)
	{
		this->id = 0;
		this->strText = text;
		this->data = data;
		this->image = image;
		this->check = FALSE;
		this->expand = 0;
		this->pParentItem = parentItem;
	}
	QYTreeCtrlItem(QYTreeCtrlItem  *parentItem, const char *text, int image = -1, DWORD_PTR data = 0)
	{
		this->id = 0;
		SET_TEXT(strText,text)
		this->data = data;
		this->image = image;
		this->check = FALSE;
		this->expand = 0;
		this->pParentItem = parentItem;
	}
	QYTreeCtrlItem()
	{
		this->id = 0;
		this->strText = L"";
		this->data = 0;
		this->image = -1;
		this->check = FALSE;
		this->expand = 0;
		this->pParentItem = nullptr;
	}
	
	~QYTreeCtrlItem() { ; }
	DWORD id;
	QYTreeCtrlItem *pParentItem;
	QYString strText;
	int      textWidth;
	DWORD_PTR data;
	int image;
	char check;
	char expand;
	std::string textID;
	std::list<QYTreeCtrlItem*> m_listSubItem;
};

class QYUI_EXPORT_CLASS QYTreeCtrl :
	public QYWindow,public QYImageList,public QYIScroll
{
public:
	QYTreeCtrl();
	~QYTreeCtrl();
	BOOL SetRedraw(BOOL bRedraw = TRUE);
	BOOL SetHasButton(BOOL bHasButton = TRUE);
	BOOL SetCheckFlag(BOOL bCheckFlag);
	BOOL SetCheckOnlyFlag(BOOL bCheckOnlyFlag);
	BOOL SetAlwaysHilight(BOOL bAlwaysHilight);
	BOOL SetCheckAll(BOOL bCheckAll);
	BOOL SetCheck(DWORD item, BOOL bCheck);
	BOOL GetCheck(DWORD item);
	BOOL DeleteAllItems();
	BOOL SetItemData(DWORD item, DWORD_PTR data);
	BOOL SetItemText(DWORD item, const TCHAR* strText);
    BOOL setItemText(DWORD item, const char* text);
	DWORD_PTR GetItemData(DWORD item);
	int SetItemHeight(int height);
	DWORD InsertItem(DWORD parentItem, const TCHAR* strText, int image = -1, DWORD_PTR data = 0);
	DWORD insertItem(DWORD parentItem, const char* text, int image = -1, DWORD_PTR data = 0);
	LPCTSTR GetItemText(DWORD item);
	BOOL SetItem(DWORD item, const TCHAR* strText, DWORD_PTR data = 0);
	BOOL SetItemImage(DWORD item, int image);
	QYTreeCtrlItem* FindItem(QYTreeCtrlItem *pParentItem,DWORD item);
	BOOL DeleteItem(DWORD item);
	int GetVisibleItemCount(QYTreeCtrlItem *pParentItem = nullptr);
	int GetChildItemCount(DWORD item);
	DWORD GetSelectItem(){ return m_nSelItem; }
	void selectItem(DWORD item);
	QYScrollCtrl* GetVScrollCtrl(){ return &m_VScrollBar; }
	QYScrollCtrl* GetHScrollCtrl(){ return &m_HScrollBar; }
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	
	BOOL  GetItemRect(DWORD item,QYRect &rect);
	BOOL Expand(DWORD item);
	BOOL Collapse(DWORD item);
	DWORD GetParentItem(DWORD item);
	int GetVisibleIndex(DWORD item);
	int GetItemLevel(DWORD item);
	void SetDragFlag(BOOL bDrag = TRUE){ m_bDragFlag = bDrag; }
	void SetImageFlag(BOOL bImageFlag = TRUE){ m_bImageFlag = bImageFlag; }
	DWORD GetFirstItem(DWORD parentItem);
	DWORD GetNextItem();
	DWORD GetCheckOnlyItem();
protected:
	std::list<QYTreeCtrlItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Clear(QYTreeCtrlItem *pParentItem = nullptr);
	BOOL UpdateLayout();
	void DrawItem(QYDC *pDC, QYRect rect, QYRect &rectItem, QYTreeCtrlItem *pItem = nullptr);
	BOOL DeleteItem(QYTreeCtrlItem *pParentItem, DWORD item);
	QYTreeCtrlItem* GetItemFromIndex(QYTreeCtrlItem *pParentItem, int index, int &findIndex);
	BOOL GetVisibleIndex(QYTreeCtrlItem *pParentItem, DWORD item,int &index);
	int GetItemLevel(QYTreeCtrlItem *pItem);
	BOOL GetMaxVisibleItemWidth(int &width,QYTreeCtrlItem *pParentItem = nullptr);
	QYTreeCtrlItem* GetParentItem(QYTreeCtrlItem* pItem);
	QYTreeCtrlItem* GetItemFromPoint(QYPoint point);
	int GetItemSpan(int textWidth,int level);
	void BeginDrag();
	void EndDrag();
	QYToolTip   *m_pToolTip;
	int  m_nItemHeight;
	BOOL m_bHover;
	BOOL m_bDown;
	DWORD  m_nSelItem;
	DWORD  m_nHoverItem;
	BOOL m_bFocus;
	QYRect  m_rectItemList;
	BOOL m_bCheckFlag;
	BOOL m_bImageFlag;
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
	QYPicture   *m_picExpand;
	QYPicture   *m_picCollapse;
	BOOL m_bHasButton;
	QYSize m_sizeButton;
	QYSize m_sizeImage;
	QYSize m_sizeCheckBox;
	int    m_nClearanceWidth;
	int    m_bDragFlag;
	int    m_bCheckOnlyFlag;
	QYPoint  m_DownPoint;
	BOOL   m_bDraging;
	BOOL   m_bAlwaysHilight;
	std::list<QYTreeCtrlItem*>*  m_pCurrentListItem;
	std::list<QYTreeCtrlItem*>::iterator m_CurrentIt;
	DWORD m_CheckOnlyItem;
public:
	virtual LRESULT OnPaint();
	BOOL OnMove(int cx, int cy);
	virtual LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnRButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnRButtonUp(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	virtual LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	virtual LRESULT OnMouseLeave(WPARAM, LPARAM);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	void OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	void OnHScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar);
	virtual LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt);
	virtual LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
};

