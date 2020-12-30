#pragma once

#include "QYWindow.h"
#include "QYDateTable.h"


class QYUI_EXPORT_CLASS QYDateCtrl :public QYWindow
{
public:
	QYDateCtrl();
	virtual ~QYDateCtrl();
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
	LRESULT OnChar(WPARAM wparam, LPARAM lparam);
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	LRESULT OnKeyDown(WPARAM wparam, LPARAM lparam);
	QYDate GetDate()const;
	void      SetDate(int year, int month, int day);
private:
	enum DATE_FIELD
	{
		DATE_FIELD_YEAR = 0,
		DATE_FIELD_MONTH ,
		DATE_FIELD_DAY,
	};

	void DrawField(const TCHAR *timeText,DATE_FIELD fd,QYDC *pDc,QYRect* rect);
	BOOL PtInField(QYPoint &pt,const TCHAR *timeText, DATE_FIELD fd, QYDC *pDc, QYRect* rect);
	void DrawDateArrow(HDC hDC, QYRect *rect);
	BOOL        m_bFocus;
	BOOL        m_bHot;
	bool	m_bLBtDown;
	int     m_Type;
	int     nMargin;
	TCHAR     m_chYear[5];
	TCHAR     m_chMonth[3];
	TCHAR     m_chDay[3];
	DATE_FIELD m_selField;
	int       m_nClientPadWidth;
	BOOL      m_bBeginEdit;
	BOOL      m_bHotDateButton;
	QYDateTable *m_pDropDateTable;
};

