#pragma once
#include "QYWindow.h"

#if 1
 class QYScrollBar :public QYWindow
{
	friend class QYScrollCtrl;
public:
	QYScrollBar();
	virtual ~QYScrollBar();

	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr,
		int type = SB_VERT);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	BOOL m_bHover;
	BOOL m_bDownScrollBar;
	int  m_Type;
};

#endif
 class QYUI_EXPORT_CLASS QYScrollCtrl :
	public QYWindow
{
public:
	QYScrollCtrl();
	virtual ~QYScrollCtrl();

	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr,
		int flag = SB_VERT);

	void SetPos(int pos);
	int  GetPos();
	int setStep(int step);
	int  getStep();
	int step(int offset );
	int SetRange(int range);
	int  GetRange(){ return m_nRange; }
	int  GetMaxRange();
protected:
	enum 
	{
		ARROW_TYPE_UP = 0,
		ARROW_TYPE_DOWN,
		ARROW_TYPE_LEFT,
		ARROW_TYPE_RIGHT,
		ARROW_TYPE_END
	};
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint();
	LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	//int  GetAccurateScrollBarHeight(int totalHeight, int scrollRange, int  &step);
	int  GetAccurateScrollBarHeight(int totalHeight);
	void DrawArrow(HDC hDC, QYRect *rect, int type);
	void UpdateVertLayout(int offHeight,BOOL sizeChange = FALSE,BOOL sendToParent = TRUE);
	void UpdateHorzLayout(int offWidth, BOOL sendToParent = TRUE);
	QYScrollBar *m_pBar;
	int  m_nRange;
	int  m_nWidth;
	int  m_nBarPos;
	int  m_nBarHeight;
	int  m_nBarWidth;
	int  m_nStep;
	int  _buttonStep;
	BOOL m_bDownScrollBar;
	QYRect m_rectScroll;
	QYPoint m_LastPoint;
	int  m_nDownPos;
	QYRect m_rectScrollArrow[ARROW_TYPE_END];
	QYRect m_rectBar;
	int  m_dockFlag;
	int  m_nMinBarWidth;
	int  m_hoverArrow;
	int  m_downArrow;
};

