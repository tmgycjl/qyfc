#pragma once
#include "includedef.h"
#include "QYWindow.h"
#include "QYpicture.h"
#include "QYSize.h"
// class Graphics;
// class Image;

#include "QYShadowBorder.h"

class QYUI_EXPORT_CLASS QYWindowShadow : public QYWindow
{
public:
	QYWindowShadow();
	virtual ~QYWindowShadow();
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK WindowShadowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	void OnMove(QYRect &rect);
	void OnSize();
	BOOL OnEraseBkgnd();
	BOOL Create(UINT resID, QYWindow *pParentWnd,int shadowPos = SHADOW_ON_WHOLE,BOOL bSmall = FALSE);
	BOOL Create(UINT resID, HWND  hParentWnd, int shadowPos = SHADOW_ON_WHOLE, BOOL bSmall = FALSE);
private:
	int  m_nShadowPos;
	BOOL m_bSmall;
	QYPicture  *m_picShadow;
};


class QYUI_EXPORT_CLASS QYDragShadow : public QYWindow
{
public:
	QYDragShadow();
	virtual ~QYDragShadow();
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DragShadowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	void TrackPop(QYPoint pt);
	BOOL OnEraseBkgnd();
	BOOL Create(UINT resID, QYWindow *pParentWnd, int shadowPos = SHADOW_ON_WHOLE, BOOL bSmall = FALSE);
	BOOL Create(UINT resID, HWND  hParentWnd, int shadowPos = SHADOW_ON_WHOLE, BOOL bSmall = FALSE);
private:
	QYPicture  *m_pPic;
	QYSize m_sizeWnd;
};


