#pragma once
#include "QYWindow.h"
#include "QYImageList.h"
#include "QYSize.h"
#include <list>
#include <vector>
#include "QYDropList.h"
#include "QYToolTip.h"
#include "QYLanguage.h"

enum enumTBTextAlign
{
	TB_TEXT_ALIGN_LEFT = 0,
	TB_TEXT_ALIGN_RIGHT,
	TB_TEXT_ALIGN_TOP,
	TB_TEXT_ALIGN_BOTTOM,
	TB_TEXT_ALIGN_CENTER
};

class QYUI_EXPORT_CLASS  QYToolBarItem
{
public:
	QYToolBarItem(LPCTSTR strText, std::vector<int> *image, int format, DWORD_PTR data)
	{
		reset();
		//this->rect = 0;
		this->strText = strText;
		this->data = data;
		this->format = (nullptr== image) ? TB_TEXT_ALIGN_CENTER : format;
		this->check = 0;
		if (nullptr != image)
		{
			int stat = 0;
			for (auto i = image->begin(); i != image->end() && stat < IMAGE_STATUS_MAX; i++)
			{
				this->image[stat++] = *i;
			}

		}
		//this->strTipText = strTipText;
	}
	QYToolBarItem(const char *text, std::vector<int> *image, int format, DWORD_PTR data)
	{
		reset();
		//this->rect = 0;
		SET_TEXT_ID(textID,text,strText)
		this->data = data;
		this->format = (nullptr == image) ? TB_TEXT_ALIGN_CENTER : format;
		this->check = 0;
		if (nullptr != image)
		{
			int stat = 0;
			for (auto i = image->begin(); i != image->end() && stat < IMAGE_STATUS_MAX; i++)
			{
				this->image[stat++] = *i;
			}

		}
		//this->strTipText = strTipText;
	}
	QYToolBarItem(const char *key,const char *text, std::vector<int> *image, int format)
	{
		reset();
		SET_TEXT_ID(textID, text, strText)
		this->format = (nullptr == image) ? TB_TEXT_ALIGN_CENTER : format;
		this->check = 0;

		if (nullptr != image)
		{
			int stat = 0;
			for (auto i = image->begin(); i != image->end() && stat < IMAGE_STATUS_MAX; i++)
			{
				this->image[stat++] = *i;
			}
			
		}
		
		this->key = key;
		//this->strTipText = strTipText;
	}
	~QYToolBarItem() { ; }
	void reset()
	{
		this->readOnly = 0;

		for (int index = 0; index < IMAGE_STATUS_MAX; index++)
		{
			this->image[index] = -1;
		}
	}

	BYTE    format;
	BYTE    check;
	BYTE    readOnly;
	QYString strText;
	//QYString strTipText;
	int     image[IMAGE_STATUS_MAX] ;
	DWORD_PTR data;
	std::string textID;
	std::string key;
	//QYRect rect;
};


enum enumTBDockStyle
{
	TB_DOCK_HORI = 0,
	TB_DOCK_VERT,
};

class QYUI_EXPORT_CLASS QYToolBar :
	public QYWindow,public QYImageList
{
public:
	QYToolBar();
	virtual ~QYToolBar();
	int InsertItem(int item, const TCHAR* strText, std::vector<int> *image, int format = TB_TEXT_ALIGN_CENTER, DWORD_PTR data = 0);
	int insertItem(int item, const char* text, std::vector<int> *image, int format = TB_TEXT_ALIGN_CENTER, DWORD_PTR data = 0);
	int insertItem(const char *key, const char* text, std::vector<int> *image, int format = TB_TEXT_ALIGN_CENTER);
	BOOL SetItemText(int item, const TCHAR* strText);
	BOOL setItemText(int item, const char* text);
	DWORD_PTR GetItemData(int item);
	std::string getItemKey(int item);
	BOOL DeleteItem(int item);
	BOOL DeleteAllItems();
	int GetItemCount();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	
	int SetItemSize(QYSize size);
	int setSpaceWidth(int spaceWidth);
	QYSize GetItemSize(){ return m_itemSize; };
	void ShowText(BOOL bShowText = TRUE);
	void getItemRect(int item, QYRect &rect);
	BOOL GetCheck(int item);
	BOOL SetCheck(int item,BOOL bCheck = TRUE);
	BOOL getCheck(const char *itemKey);
	BOOL setCheck(const char *itemKey, BOOL bCheck = TRUE);
	BOOL SetReadOnly(int item, BOOL bReadOnly = TRUE);
	void SetItemImage(int item, int image );
	void setItemImage(const char *key, int imageID);
	virtual void updateText();
protected:
	std::list<QYToolBarItem*> m_listItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Clear();
	QYToolBarItem* GetItem(int item);
	QYToolBarItem* getItem(const char *itemKey);
	BOOL m_bHover;
	BOOL m_bDown;
	int  m_nHoverItem;
	int  m_nSelItem;
	int  m_DockStyle;
	int  m_nSeparatePixel;
	QYFont *m_pSelFont;
	QYSize m_itemSize;
	QYSize m_maxImageSize;
	int    m_maxTextX;
	BOOL m_bHasString;
	BOOL m_bShowMoreBtn;
	int  m_nShowItemCount;
	QYSize m_morebtnSize;
	QYPicture *m_pPicMoreButton;
	QYDropList *m_pMoreDropList;
	std::vector<QYToolBarItem*> m_vectorHideItem;
	QYToolTip   *m_pToolTip;
	int    m_padWidth;
	int    m_padHeight;
	int    m_spaceWidth;
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

