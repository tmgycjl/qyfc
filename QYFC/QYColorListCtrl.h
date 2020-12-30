#pragma once
#include "QYListCtrl.h"

class QYUI_EXPORT_CLASS  QYColorListCtrlSubItem : public QYListCtrlSubItem
{
public:
	QYColorListCtrlSubItem(TCHAR *text) :QYListCtrlSubItem(text)
	{
		color = -1;
	}
	QYColorListCtrlSubItem() :QYListCtrlSubItem()
	{
		color = -1;
	}

	~QYColorListCtrlSubItem() { ; }
	COLORREF color;
};


class QYUI_EXPORT_CLASS QYColorListCtrl :public QYListCtrl
{
public:
	QYColorListCtrl();
	~QYColorListCtrl();
	BOOL SetItemBkColor(int item, int subitem, COLORREF color);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	virtual LRESULT OnPaint();
	virtual int InsertItem(int item, const TCHAR* strText, int image = -1, DWORD_PTR data = 0);
	virtual int InsertItem(const TCHAR* strText, int image = -1, DWORD_PTR data = 0);
	BOOL SetItem(int item, int subitem, const TCHAR* strText,COLORREF color = -1, DWORD_PTR data = 0);
	BOOL SetItem(int subitem, const TCHAR* strText, COLORREF color = -1, DWORD_PTR data = 0);
private:
};

