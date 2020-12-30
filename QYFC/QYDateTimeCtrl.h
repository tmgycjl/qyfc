#pragma once

#include "QYWindow.h"
#include "QYDateTime.h"
#include "QYDateCtrl.h"
#include "QYTimeCtrl.h"


enum DATETIME_FLAG
{
	DATETIME_FLAG_DATE = 1,
	DATETIME_FLAG_TIME = 2,
	DATETIME_FLAG_DATE_TIME = 3
};

class QYUI_EXPORT_CLASS QYDateTimeCtrl :public QYWindow
{
public:
	QYDateTimeCtrl();
	virtual ~QYDateTimeCtrl();
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
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	LRESULT OnKeyDown(WPARAM wparam, LPARAM lparam);
	void SetFlag(DATETIME_FLAG flag);
	QYDateTime GetDateTime()const;
	void      SetDateTime(QYDateTime& dt);
	void      SetDateTime(int year,int month,int day,int hour,int mintue,int second);
	void      SetDate(int year, int month, int day);
	void      SetTime(int hour, int mintue, int second);
private:
	void GetCurrentDateTime();
	BOOL        m_bFocus;
	BOOL        m_bHot;
	bool	m_bLBtDown;		// Êó±ê×ó¼ü°´ÏÂ¡£
	int     m_Type;
	DATETIME_FLAG m_Flag;
	int       m_nClientPadWidth;
	QYDateCtrl *m_pDateCtrl;
	QYTimeCtrl *m_pTimeCtrl;
};

