#pragma once
#include "QYWindow.h"

enum enumSplitterDockFlag
{
	SPLITTER_DOCK_LEFT = 0,
	SPLITTER_DOCK_TOP, 
	SPLITTER_DOCK_RIGHT,
	SPLITTER_DOCK_BOTTOM
};


class QYUI_EXPORT_CLASS QYSplitterCtrl :
	public QYWindow
{
public:
	QYSplitterCtrl();
	~QYSplitterCtrl();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		QYWindow *pParentWnd = nullptr,
		int type = SPLITTER_DOCK_LEFT);
	int  GetType(){ return m_Type; }
	void SetX(float fX);
	float  GetX(){ return m_fX; }
	void SetY(float fY);
	float  GetY(){ return m_fY; }
	BOOL AddSubSplitter(QYSplitterCtrl* first, QYSplitterCtrl* second);
	BOOL AddFirstSubSplitter(QYSplitterCtrl* first);
	BOOL AddSecondSubSplitter(QYSplitterCtrl* second);
	void ClearSubSplitter();
	void SetView(QYWindow* first, QYWindow* second);
	void SetView(QYWindow* pView);
	void SetFirstView(QYWindow* pView);
	void SetSecondView(QYWindow* pView);
	void MoveView(QYRect &first, QYRect &second, int splitterWidth);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	void MoveView(QYSplitterCtrl *pSub,QYRect &rect, int splitterWidth);
	BOOL m_bHover;
	BOOL m_bDown;
	int  m_Type;
	float  m_fX;
	float  m_fY;
	QYWindow *m_pView;
	
	 std::pair<QYWindow*, QYWindow*> m_pairView;
	
	std::pair<QYSplitterCtrl*, QYSplitterCtrl*> m_pairSub;
};

