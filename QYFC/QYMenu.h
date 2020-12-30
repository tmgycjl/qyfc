#pragma once

#include <vector>
#include "QYWindow.h"
#include "QYWindowShadow.h"

class QYUI_EXPORT_CLASS  QYMenuItem
{
public:
	QYMenuItem()
	{
		strText = L"";
		data = 0;
		id = 0;
		enable = true;
		check = false;
		pSubMenu = nullptr;
	}

	~QYMenuItem() { ; }
	UINT     id;
	bool     enable;
	bool     check;
	QYString strText;
	std::string key;
	DWORD_PTR data;
	QYWindow *pSubMenu;
};


enum 
{
	TPMENU_LEFT_BOTTOM = 0,
	TPMENU_LEFT_TOP,
	TPMENU_RIGHT_BOTTOM,
	TPMENU_RIGHT_TOP
};

class QYUI_EXPORT_CLASS QYMenu :public QYWindow
{
public:
	QYMenu(QYWindow *pSounrceWnd = nullptr);
	virtual ~QYMenu();
	virtual BOOL Show(int nCmdShow);
	int AddItem(UINT id,const TCHAR* strText, DWORD_PTR data = 0);
	int addItem(UINT id, const char* text, DWORD_PTR data = 0);
	int addItem(std::string key, const char* text);
	QYWindow* AddSubMenu(UINT id);
	QYWindow* addSubMenu(std::string key);
	bool EnableItem(UINT id,bool enable);
	bool enableItem(std::string key, bool enable);
	void SetItemText(UINT id, const TCHAR* strText);
	void setItemText(UINT id, const char* text);
	void setItemText(std::string key, const char* text);
	bool CheckItem(UINT id,bool check);
	bool checkItem(std::string key, bool check);
	void SetItemHeight(int itemHeight);
	void TrackPopup(QYPoint point, int width, int pos, QYWindow *pWnd,bool show = false);
	BOOL Create();
	QYWindow* SetParentMenu(QYWindow* pParentMenu);
	QYWindow* GetParentMenu(){ return m_pParentMenu; }
	QYWindow* getSubMenu(std::string key);
	BOOL loadMenu(const char *source, const char* group);
	void onCloseWindow(WPARAM wParam, LPARAM lParam);
protected:
	std::vector<QYMenuItem*> m_vectorItem;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	BOOL loadMenu(QYXMLTreeNode *pNode);
	void Clear();
	void DrawGuideArrow(HDC hDC, QYRect *rect, bool sel);
	QYWindowShadow *m_pShadow;
	int  m_nItemHeight;
	int  m_nSeparateHeight;
	BOOL m_bHover;
	int  m_nSelItem;
	int  m_nCursel;
	int  m_DefaultHeight;
	int  m_padHeight;
	int  m_padWidth;
	QYWindow *m_pSourceWnd;
	QYPicture *m_pCheckImage;
	QYWindow  *m_pParentMenu;
public:
	virtual LRESULT OnPaint();
	BOOL OnMove(int cx, int cy);
	 LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	 LRESULT OnSize(UINT nType, int cx, int cy);
	 LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	 LRESULT OnMouseLeave(WPARAM, LPARAM);
	 LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	 void  OnSubMenuHided();
};

