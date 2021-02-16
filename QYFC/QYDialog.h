#pragma once

#include "QYDataExchange.h"
#include "QYWindow.h"
#include "QYButton.h"
#include "QYEdit.h"
#include "QYComboBox.h"
#include "QYWindowShadow.h"
#include "QYSizeBox.h"
#include "QYSplitterCtrlManager.h"
#include "QYDwm.h"
#include "QYDPI.h"

enum CButtonState{ bsNone, bsIni, bsMin, bsMax, bsRes, bsClose };

#define fLeftBand     1
#define fLeftTitle    2
#define fRightTitle   4
#define fMidTitle     8
#define fRightBand    16
#define fBottomBand   32
#define fLeftBottom   64
#define fRightBottom  128
#define fMinButton    256
#define fMaxButton    512
#define fCloseButton  1024
#define fAll          2047


class QYUI_EXPORT_CLASS QYDialog : public QYWindow, public QYSizeBox, public QYSplitterCtrlManager, public QYDwm, public QYDPI
{
public:
	QYDialog();

	virtual ~QYDialog();
	
	static UINT IDD;

	virtual BOOL OnInitDialog();
	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	 BOOL OnPaint();
	BOOL OnNcPaint();
	virtual BOOL Show(int nCmdShow);
	virtual void PaintDC(QYDC *pDC, QYRect WinRC);
	int ProcNCCalcSize(UINT message, WPARAM wParam, LPARAM lParam);
	void Control(QYDataExchange *pDX,int nIDC, QYWindow& rControl);
	BOOL EndDialog(UINT end);
	bool Create(UINT resID,QYWindow *pParentWnd);
	INT_PTR DoModal(UINT resID, QYWindow *pParentWnd = nullptr);
	INT_PTR DoModal( QYWindow *pParentWnd = nullptr);
	void DrawDialogText();
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	 LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
	BOOL OnNcActivate( BOOL bActive);
	void OnLButtonDBclick(UINT nHitTest, QYPoint point);
	LRESULT OnExitSize(WPARAM wparam, LPARAM lparam);
	void OnLButtonDown(UINT nHitTest, QYPoint point);
	void OnLButtonUp(UINT nHitTest, QYPoint point);
	BOOL NcPaint(QYDC *pDC, int active = 1);
	virtual BOOL OnSize(UINT nType, int cx, int cy);
	BOOL OnMove(int cx, int cy);
	void SetDlgZone();
	int  GetFrameWidth();
	int  GetBorderWidth();
	int  GetBorderHeight();
	int  GetFrameHeight();
	QYDataExchange* GetDataExchange(){ return m_pDataExchange; }
	int GetCaptionHeight(){ return m_CaptionHeight; }
	void SetCaptionHeight(int captionHeigth){ m_CaptionHeight = captionHeigth; }
	QYWindow* GetDlgItem(UINT resID);
public:
	friend static BOOL CALLBACK  RegChildWindow(HWND hWnd, QYWindow *param);
	virtual void DoDataExchange(QYDataExchange* pDX);
protected:
	LRESULT OnCtlColor(UINT msg, WPARAM wParam, LPARAM lParam);
	void InitSysBox(QYRect &WinRC);
	void Zoom();
	void DrawCloseRect(QYDC *pDC, QYRect &rect, Button_state state = button_state_normal);
	void DrawMaxRect(QYDC *pDC, QYRect &rect, int zoom, Button_state state = button_state_normal);
	void DrawMinRect(QYDC *pDC, QYRect &rect, Button_state state = button_state_normal);
	void getClientRectFromWindowRect(QYRect &rect);
	QYWindowShadow  *m_pShadow;
	QYDataExchange  *m_pDataExchange;
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
	int       m_ButtonHeight;		
	TCHAR      m_Text[MAX_PATH];
	BOOL	  m_bGetText;		
	HBITMAP    m_closeBitmap;
	HBITMAP    m_maxBitmap;
	HBITMAP    m_minBitmap;
	QYSize       m_CloseBoxSize;
	QYSize       m_MaxBoxSize;
	QYSize       m_MinBoxSize;
	QYRect       m_rectClose;
	QYRect       m_rectMax;
	QYRect       m_rectMin;
	BOOL       m_bNCLBDOWN;
	QYRect       m_rectLast;
	POINT      m_pointLast;
	BOOL       m_bZoom;
	HRGN       m_hrestoreRgn;
	QYRect     m_restoreRect;
    UINT        m_exitCode;
};


