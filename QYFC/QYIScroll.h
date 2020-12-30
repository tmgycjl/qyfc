#pragma once
#include "QYScrollBar.h"

class  QYIScroll
{
public:
	QYIScroll();
	virtual ~QYIScroll();

	QYScrollCtrl* GetVScrollCtrl(){ return &m_VScrollBar; }
	QYScrollCtrl* GetHScrollCtrl(){ return &m_HScrollBar; }
protected:
	bool onTimer(QYWindow *pWnd, UINT_PTR nIDEvent);
	bool onVScroll(QYWindow *pWnd, UINT nSBCode);
	bool onHScroll(QYWindow *pWnd, UINT nSBCode);
	LRESULT onMouseWheel(QYWindow *pWnd, UINT nFlags, short zDelta);
	void updateVScrollLayout(QYWindow *pWnd, QYRect &rect,int scrollRange);
	void updateHScrollLayout(QYWindow *pWnd, QYRect &rect, int scrollRange);
	QYScrollCtrl  m_VScrollBar;
	QYScrollCtrl  m_HScrollBar;
	BOOL m_bVShowScroll;
	BOOL m_bHShowScroll;
	int  m_nVScrollWidth;
	int  m_nHScrollHeight;
	long m_nScrollPos;
	int  m_nHScrollPos;
	int  m_nTrackPos;
	int  m_nHTrackPos;
	int  m_nWheelCount;
	BOOL m_bOpenTrackPosTime;
	int  m_nZDelta;
};

