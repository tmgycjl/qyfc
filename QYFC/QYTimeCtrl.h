#pragma once

#include "QYWindow.h"
#include "QYDateTime.h"


class QYUI_EXPORT_CLASS QYTimeCtrl :public QYWindow
{
public:
	QYTimeCtrl();
	virtual ~QYTimeCtrl();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	LRESULT OnPaint();
	LRESULT OnEraseBackRound(HDC hDC);
	LRESULT LoseFocus();
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnChar(WPARAM wparam, LPARAM lparam);
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	LRESULT OnKeyDown(WPARAM wparam, LPARAM lparam);
	QYTime& GetTime()const;
	void      SetTime(int hour, int mintue, int second);
private:
	enum TIME_FIELD
	{
		TIME_FIELD_HOUR = 0,
		TIME_FIELD_MINUTE,
		TIME_FIELD_SECOND
	};

	void DrawField(const TCHAR *timeText,TIME_FIELD fd,QYDC *pDc,QYRect* rect);
	BOOL PtInField(QYPoint &pt,const TCHAR *timeText, TIME_FIELD fd, QYDC *pDc, QYRect* rect);
	void DrawTimeArrow(HDC hDC, QYRect *rect,bool bUp = true);
	BOOL        m_bFocus;
	BOOL        m_bHot;
	bool	m_bLBtDown;		// Êó±ê×ó¼ü°´ÏÂ¡£
	int     m_Type;
	int nMargin;
	TCHAR     m_chHour[3];
	TCHAR     m_chMinute[3];
	TCHAR     m_chSecond[3];
	TIME_FIELD m_selField;
	int       m_nClientPadWidth;
	BOOL      m_bBeginEdit;
	BOOL      m_bHotButtonUp;
	BOOL      m_bHotButtonDown;
	BOOL      m_bPressButtonUp;
	BOOL      m_bPressButtonDown;
};

