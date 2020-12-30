#pragma once

#include "includedef.h"
#include "QYpicture.h"
#include <functional>
//#include "QYLanguage.h"

class QYDC;
class QYFont;

#include "QYString.h"
#include "QYApp.h"
#include "QYMessageMap.h"
#include "QYRect.h"
#include "QYRelative.h"
#include "QYSize.h"
#include "qycolor.h"

enum
{
	ORIENTATION_VERTICAL = 0,
	ORIENTATION_HORIZONTAL
};




class QYWidget;

class QYUI_EXPORT_CLASS QYWindow : public QYMessageMap
{
public:
	HWND m_hWnd;
	QYWindow(void);
	virtual ~QYWindow(void);
	//QYWindow* GetDesktopWindow(){ return (QYWindow*)::(HWND)GetDesktopWindow(); }
	void CenterWindow(HWND hInsertAfter = HWND_NOTOPMOST);
	QYDC* GetWindowDC();
	BOOL ReleaseDC(QYDC *pDC);
	//virtual int GetWindowText(QYString &strText, int size = MAX_WINDOW_TEXT_LENGTH);
	//virtual int GetWindowText(TCHAR* lpString){ return ::GetWindowText(m_hWnd, lpString, sizeof(lpString)*sizeof(TCHAR) + 1); }
	//virtual BOOL SetWindowText(const TCHAR *lpString){ BOOL ret = ::SetWindowText(m_hWnd, lpString);  Invalidate(); return ret; }
	virtual BOOL SetWindowText(LPCTSTR strText);
	
	BOOL setWindowText(const char *textID);
	BOOL getWindowText(std::string &text);

	LRESULT SendMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
	LRESULT PostMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL Show(int nCmdShow = SW_SHOW);
	BOOL IsWindowVisible(){return ::IsWindowVisible(m_hWnd);}
	 virtual  BOOL Create(__in DWORD dwExStyle,
		__in_opt LPCWSTR lpClassName,
		__in_opt LPCWSTR lpWindowName,
		__in DWORD dwStyle ,
		__in int X = CW_USEDEFAULT,
		__in int Y = 0,
		__in int nWidth = CW_USEDEFAULT,
		__in int nHeight = 0,
		HWND hParentWnd = nullptr,
		__in_opt HMENU hMenu = nullptr,
		__in_opt HINSTANCE hInstance = nullptr,
		__in_opt LPVOID lpParam = nullptr);
	 virtual  BOOL CreateEx(__in DWORD dwExStyle,
		 __in_opt LPCWSTR lpClassName,
		 __in_opt LPCWSTR lpWindowName,
		 __in DWORD dwStyle,
		 __in int X = CW_USEDEFAULT,
		 __in int Y = 0,
		 __in int nWidth = CW_USEDEFAULT,
		 __in int nHeight = 0,
		 QYWindow *pParent = nullptr,
		 __in_opt HMENU hMenu = nullptr,
		 __in_opt HINSTANCE hInstance = nullptr,
		 __in_opt LPVOID lpParam = nullptr);
	
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam) = 0;

	class  QYWidgetItem
	{
	public:
		QYWidgetItem(QY_CONTROL_TYPE ctrlType, QYRelative rel1, QYRelative rel2, QYSize offset1, QYSize offset2)
		{
			m_ctrlType = ctrlType;
			m_rel1 = rel1;
			m_rel2 = rel2;
			m_offset1 = offset1;
			m_offset2 = offset2;
			m_pWnd = nullptr;
			m_inSwallow = false;
		}

		~QYWidgetItem()
		{
			
			if (!m_inSwallow)
			{
				SAFE_DESTROY_WINDOW_PTR(m_pWnd);
			}
		}

		QY_CONTROL_TYPE m_ctrlType;
		QYWindow *m_pWnd;
		QYRelative m_rel1;
		QYRelative m_rel2;
		QYSize m_offset1;
		QYSize m_offset2;
		bool  m_inSwallow;
		std::string m_keySwallow;
	};

	HWND  GetHwnd(){ return m_hWnd; }
	void  SetHwnd(HWND hWnd){  m_hWnd = hWnd; }
	HWND  SetNotifyMsgWindow(HWND hMsgWnd);
	BOOL GetClientRect(QYRect &rect);
	BOOL GetWindowRect(QYRect &rect,QYWindow *pWnd = nullptr);
	BOOL MoveWindow(int x, int y, int width, int height,BOOL bRepain = TRUE);
	BOOL MoveWindow(QYRect rect, BOOL bRepain = TRUE);
	QYWindow* SetParent(QYWindow *pWnd);
	QYWindow* GetParent(){  return m_pParent;}
	BOOL InvalidateRect(_In_opt_ CONST QYRect *lpRect, _In_ BOOL bErase = TRUE);
	int GetDlgCtrlID() { return ::GetDlgCtrlID(m_hWnd); }
	HWND SetCapture(){ return ::SetCapture(m_hWnd); }
	HWND SetFocus(){ return ::SetFocus(m_hWnd); }
	void SetIcon(HICON hIcon){ m_hIcon = hIcon; }
	void SetIcon(UINT uresID){ m_hIcon = ::LoadIcon(QYApp::m_hInstance,MAKEINTRESOURCE(uresID)); }
	BOOL ModifyStyle(UINT remove, UINT add);
	BOOL Invalidate(BOOL bEraze = TRUE){ return ::InvalidateRect(m_hWnd, nullptr, bEraze); }
	BOOL ClientToScreen(__inout QYPoint *point);
	BOOL ClientToWindow(HWND hWnd, __inout QYPoint *point);
	BOOL ClientToScreen(__inout QYRect *rect);
	BOOL ScreenToClient(__inout QYRect *rect);
	BOOL ScreenToClient(__inout QYPoint *point);
	BOOL SetWindowPos( HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags = 0);
	BOOL SetWindowPos( HWND hWndInsertAfter,  QYRect &rect, UINT uFlags = 0);
	BOOL ScrollWindow(int XAmount, int YAmount, QYRect *lpRect, QYRect *lpClipRect);
	UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc = nullptr)
	{ return ::SetTimer(m_hWnd, nIDEvent, uElapse, lpTimerFunc); }
	BOOL KillTimer(UINT_PTR uIDEvent){ return ::KillTimer(m_hWnd, uIDEvent); }
	virtual BOOL SubClassWindow(HWND hWnd);
	BOOL SetWindowProc( INT_PTR WndProc);
	HFONT SetFont(HFONT hFont);
	QYFont* GetFont();
	BOOL DestroyWindow();
	void ShowShadow(BOOL bShow = TRUE);
	void SetBorderWidth(int nBorderWidth){ m_nBorderWidth = nBorderWidth; }
	UINT GetCtrlID()const { return m_nCtrlID; }
	UINT SetCtrlID(UINT nCtrlID); 
	int getBorderWidth(){ return m_nBorderWidth; }
	HWND GetFirstParent();
	void SetBkColor(COLORREF color);
	void SetTextColor(COLORREF color, COLORREF selColor);
	WNDPROC m_oldWndProc = nullptr;
	
	QYWindow* Append(const char *key,
		QY_CONTROL_TYPE ctrlType,
		QYRelative rel1 = 0,
		QYRelative rel2 = 0,
		QYSize offset1 = 0,
		QYSize offset2 = 0,BOOL show = TRUE);
	QYWindow* Append(QYWindow *pWindow, const char *key = "", QY_CONTROL_TYPE ctrlType = QY_CONTROL_TYPE_OTHERS);

	std::map<std::string, QYWidgetItem*>* getMapWidgetItem(){ return &m_mapItem; }

	const char* getTextID(){ return m_textID.c_str(); }
	virtual void updateText();
	
	std::string& getID(){ return m_sID; }
	void setID(const char *id){ m_sID = id; }
	//QYWidget* getWidget(){ return m_pWidget; }
	//void setWidget(QYWidget *pWidget){ m_pWidget = pWidget; }
    QYWindow* getObjectPart(std::string part);
	bool swallowWidget(QYWindow *pWindow,std::string part);
	bool swallowWidget(std::string partWidget, std::string partSwallow);
	bool unswallowWidget(std::string partWidget, std::string partSwallow);
	BOOL loadLayout(const char *source, const char* group);
protected:
	BOOL loadWidget(QYWindow *pWidget, QYXMLTreeNode *pNode);
	QYWindow::QYWidgetItem* getWidgetItem(QYWindow *pWidget, std::string part,std::string &key);
	BOOL OnPaint();
	virtual BOOL onSize(UINT nType, int cx, int cy);
	BOOL NotifyMessage(UINT message, WPARAM wparam, LPARAM lparam);
	QYWindow   *m_pParent;
	QYFont    *m_pFont;
	QYDC      *m_pWindowDC;
	HICON      m_hIcon;
	BOOL       m_bHover;
	BOOL       m_bNCHover;
	BOOL       m_disableMaxBox;
	BOOL       m_disableMinBox;
	BOOL       m_bSizeBox;
	BOOL       m_bActive;
	HWND       m_hNotifyMsgHwnd;
	BOOL       m_bShowShadow;
	BOOL       m_bReDraw;
	int       m_nBorderWidth;
	int       m_CaptionHeight;
	int       m_FrameWidth;
	int       m_FrameHeight;
	UINT       m_nCtrlID;
	COLORREF   m_colorBk;
	COLORREF   m_colorText;
	COLORREF   m_colorSelText;
	std::string m_textID;
	QYString   m_strWindowText;
	//QYWidget   *m_pWidget = nullptr;
	std::string m_sID;
    struct w_funcCB_t m_eventCB;
	std::map<std::string, QYWidgetItem*> m_mapItem;
};
