#pragma once
#include "QYWindow.h"
#include "QYScrollBar.h"
#include "QYpicture.h"
#include <list>
#include "QYLanguage.h"

enum enumSortType
{
	SORT_TYPE_NONE = 0,
	SORT_TYPE_NUM ,
	SORT_TYPE_FLOAT ,
	SORT_TYPE_TEXT ,
	SORT_TYPE_IP,
	SORT_TYPE_DATE,
	SORT_TYPE_TIME
};

class QYUI_EXPORT_CLASS  QYHeaderCtrlItem
{
public:
	QYHeaderCtrlItem()
	{
		this->format = DT_LEFT;
		//m_itemType = HEADERCTRL_ITEM_TYPE_COLUMN;
		left = 0;
		right = 0;
		strText = L"";
		data = 0;
		sortType = 0;
		ascend = 0xff;
	}
	QYHeaderCtrlItem(LPCTSTR strText, BYTE format = DT_LEFT, enumSortType sortType = SORT_TYPE_NONE, DWORD_PTR data = 0)
	{
		//m_itemType = HEADERCTRL_ITEM_TYPE_COLUMN;
		this->format = (255 > format) ? format : DT_LEFT;

		this->left = 0;
		this->right = 0;
		this->strText = strText;
		this->data = data;
		this->sortType = sortType;
		this->ascend = 0xff;
	}
    QYHeaderCtrlItem(const char  *text, BYTE format = DT_LEFT, enumSortType sortType = SORT_TYPE_NONE, DWORD_PTR data = 0)
    {
        //m_itemType = HEADERCTRL_ITEM_TYPE_COLUMN;
        this->format = (255 > format) ? format : DT_LEFT;

        this->left = 0;
        this->right = 0;
        setTextID(text);
        this->data = data;
        this->sortType = sortType;
        this->ascend = 0xff;
    }
	~QYHeaderCtrlItem() { ; }
    void setTextID(const char *textID)
    {
		this->textID = textID;

        const char *text = QYLanguage::instance()->getText(this->textID.c_str());
            int dstSize = (strlen(text) + 1)*sizeof(TCHAR);
            TCHAR *dstText = new TCHAR[dstSize];
            memset(dstText, 0, dstSize);

            QYString::SafeUTF82W(dstText, dstSize, text);

            strText = dstText;
            delete[] dstText;
        
    }

	BYTE    format;
	BYTE  sortType;
	BYTE  ascend;
	int left;
	int right;
	QYString strText;
    std::string textID;
	DWORD_PTR data;
};

class QYUI_EXPORT_CLASS QYHeaderCtrl :public QYWindow
{
	friend class QYListCtrl;
	friend class QYButtonListCtrl;
public:
	QYHeaderCtrl();
	virtual ~QYHeaderCtrl();
	//int InsertItem(const TCHAR* strText, int width, int format, enumSortType sortType = SORT_TYPE_NONE, DWORD_PTR data = 0);
	int InsertItem(int item, const TCHAR* strText, int width, int format, enumSortType sortType = SORT_TYPE_NONE, DWORD_PTR data = 0);
    int insertItem(int item, const char* text, int width, int format, enumSortType sortType = SORT_TYPE_NONE, DWORD_PTR data = 0);
	BOOL SetItemText(int item,const TCHAR* strText);
	BOOL SetItemWidth(int item, const int width);
	void SetCheckFlag(BOOL bCheckFlag);
	void SetCheckAll(BOOL bCheckAll);
	BOOL DeleteItem(int item);
	BOOL DeleteAllItems();
	int GetItemCount();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	QYHeaderCtrlItem* GetItem(int item);
	void updateText();
protected:
	std::list<QYHeaderCtrlItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int GetLastItemRect(QYRect &rect);
	void Clear();
	void DrawArrow(HDC hDC, QYRect *rect, BOOL ascend);
	BOOL m_bHover;
	BOOL m_bDown;
	int  m_nHoverItem;
	int  m_nSelItem;
	int  m_nSeparatedWidth;
	int  m_nLastItem;
	BOOL m_bReDraw;
	int  m_nScrollPos;
	BOOL m_bCheckFlag;
	BOOL m_bCheckAll;
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
	BOOL m_bChangingWidth;
	HCURSOR m_hCursor;
	QYPoint  m_LastPoint;
	int  m_nLastWidth;
	int  m_nRealWidth;
public:
	virtual LRESULT OnPaint();
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
};

