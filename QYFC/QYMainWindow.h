#pragma once

#include "qydefine.h"
#include "QYWindow.h"
#include "QYDialog.h"
#include "QYSplitterCtrlManager.h"
#include "QYMenu.h"
#include "QYDwm.h"
#include "QYDPI.h"

class QYUI_EXPORT_CLASS QYMainWindow : public QYWindow, public QYSizeBox, public QYSplitterCtrlManager, public QYDwm, public QYDPI
{
public:
	QYMainWindow(void);
	virtual ~QYMainWindow(void);
	
	 BOOL Create();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	 BOOL OnPaint();
	 BOOL PaintDC(QYDC *pDC);
	int ProcNCCalcSize(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL EndDialog(UINT end);
	bool Create(UINT resID, QYWindow *pParentWnd);

	INT_PTR DoModal(UINT resID, QYWindow *pParentWnd = nullptr);
	void DrawDialogText();
	void OnMouseMove(UINT nFlags, QYPoint point);
	BOOL OnNcActivate(BOOL bActive);
	BOOL OnCreate(CREATESTRUCT *cs);
	void OnNcLButtonDown(UINT nHitTest, QYPoint point);
	void OnNcLButtonDBClick(UINT nHitTest, QYPoint point);
	LRESULT OnExitSize(WPARAM wparam, LPARAM lparam);
	LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
	void OnLButtonDown(UINT nHitTest, QYPoint point);
	 BOOL OnNcLButtonUp(UINT nHitTest, QYPoint point);
	 BOOL OnNcMouseMove(UINT nHitTest, QYPoint point);
	 BOOL OnNcMouseLeave(WPARAM wparam, LPARAM lparam);
	void OnTimer(WPARAM wparam, LPARAM lparam);
	virtual BOOL OnNcPaint(int active = 1);
	BOOL NcPaint(QYDC *pDC, QYRect WinRC, int active = 1);
	BOOL OnSize(UINT nType, int cx, int cy);
	BOOL OnMove(int cx, int cy);
	void SetDlgZone();
	void Zoom(BOOL bZoom = TRUE);
protected:
	void InitSysBox(QYRect &WinRC);
	void DrawCloseRect(QYDC *pDC, QYRect &rect, Button_state state = button_state_normal);
	void DrawMaxRect(QYDC *pDC, QYRect &rect, int zoom, Button_state state = button_state_normal);
	void DrawMinRect(QYDC *pDC, QYRect &rect, Button_state state = button_state_normal);
protected:
	QYWindowShadow  *m_pShadow;
	QYMenu      *m_pSystemMenu;
	QYRect      m_rectRestore;
	bool        m_bModal;
	int         m_Flag;
	CButtonState m_ButtonState;
	CButtonState m_ButtonDownState;
	RECT     m_IniRect, m_MinRect, m_MaxRect, m_CloseRect;
	QYRect     m_LTitleRc, m_MTitleRc, m_RTitleRc;
	RECT      m_CaptionRect;
	BOOL         m_IsDrawForm;
	int       m_TitleDrawHeight;
	int       m_NCBorderWidth;
	int       m_NCBorderHeight;
	int       m_FrameWidth;
	int       m_FrameHeight;
	int       m_CaptionHeight;
	COLORREF  m_CapitonColor;	
	int       m_ButtonHeight;
	TCHAR      m_Text[MAX_PATH];
	BOOL	  m_bGetText;
	HBITMAP    m_closeBitmap;
	HBITMAP    m_maxBitmap;
	HBITMAP    m_minBitmap;
	SIZE       m_CloseBoxSize;
	SIZE       m_MaxBoxSize;
	SIZE       m_MinBoxSize;
	QYRect       m_rectClose;
	QYRect       m_rectMax;
	QYRect       m_rectMin;
	BOOL       m_disableMaxBox;
	BOOL       m_disableMinBox;
	BOOL       m_bActive;
	BOOL       m_bNCLBDOWN;
	QYRect       m_rectLast;
	POINT      m_pointLast;
	BOOL       m_bZoom;
	HRGN       m_hrestoreRgn;
	QYRect     m_restoreRect;
};

