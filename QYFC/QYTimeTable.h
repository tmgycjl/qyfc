#pragma once
#include "QYWindow.h"
#include <list>
#include "QYDateTime.h"
#include "QYpicture.h"
#include <vector>

class QYUI_EXPORT_CLASS  QYTimeTableItem
{
public:
	QYTimeTableItem(LPCTSTR strText, DWORD_PTR data)
	{
		this->strText = strText;
		this->data = data;

		memset(minArray, 0, MINIUTE_IN_DAY);
	}

	~QYTimeTableItem() { ; }
	char minArray[MINIUTE_IN_DAY];
	std::vector<unsigned long> vectorValidPos;
	QYString strText;
	DWORD_PTR data;
};


class QYUI_EXPORT_CLASS QYTimeTable :
	public QYWindow
{
public:
	QYTimeTable();
	~QYTimeTable();
	int InsertItem(int item,const TCHAR* strText = L"", DWORD_PTR data = 0);
	void PutInTimeframe(int item,unsigned int sTime,unsigned int eTime);
	DWORD_PTR GetItemData(int item);
	int SetItemHeight(const int height);
	void SetEditImage(UINT imageID);
	void SetEditImage(LPCTSTR imageName);
	int GetItemCount();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	QYTimeTableItem* GetItem(int item);
	void ClearTime();
protected:
	std::list<QYTimeTableItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Clear();
	//BOOL m_bHover;
	BOOL m_bDown;
	//int  m_nHoverItem;
	int  m_nSelItem;
	bool m_bDownEditButton;
	int  m_nItemHeight;
	//int  m_nSeparatePixel;
	QYFont *m_pTimeFont;
	int m_nHeaderHeight;
	int m_textWdith;
	int m_OriginalHeaderHeight;
	int m_OriginalTextWdith;
	int m_textHeight;
	int m_subItemWidth;
	int m_leaveWidth;
	COLORREF  m_validTimeColor;
	QYPicture *m_pEditPic;
public:
	virtual LRESULT OnPaint();
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
};

