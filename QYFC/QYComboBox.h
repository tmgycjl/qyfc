#pragma once
#include "QYWindow.h"
#include "QYDropList.h"


class QYUI_EXPORT_CLASS QYComboBox :public QYWindow
{
public:
	QYComboBox();
	virtual ~QYComboBox();

	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);

	static LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int AddString(LPCTSTR buf, DWORD_PTR data = NULL);
    int addString(const char *key, const char *text,DWORD_PTR data = 0);
	DWORD_PTR GetLBData(int item);
    BOOL DeleteAll();
	void SetItemHeight(int itemHeight);
	void SetReadOnly(BOOL bReadOnly = TRUE);
	int  GetCount();
	void SetString(LPCTSTR szText){ m_strText = szText; Invalidate(); }
	void SetButtonRect(RECT rcButton);
	void SetDropList(QYDropList *pDropList);
	int  GetCurSel();
	BOOL SetCurSel(int nCursel);
	QYString GetString();
	QYDropList* GetLBPtr(){ return m_pDropList; }
private:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	int         m_Flag;
	BOOL        m_bFocus;
	BOOL        m_bHot;
	BOOL        m_bReadOnly;
	QYDropList  *m_pDropList;
	QYString  m_strText;
	QYRect      m_rectButton;
public:
	LRESULT OnPaint();
	LRESULT OnSerFocus();
	LRESULT OnNCPaint();
	LRESULT LoseFocus();
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnMouseMove(UINT nFlags, QYPoint &point);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	virtual BOOL SubClassWindow(HWND hWnd);
private:
	void DrawArrow(HDC hDC, QYRect *rect);
};

