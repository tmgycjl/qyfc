#pragma once

#include "qydefine.h"
#include "qyConselLog.H"
#include <vector>
#include <map>
#include "qycolor.h"
#include "QYXML.h"


class QYFont;

extern QYConsoleLog g_ConsoleLog;
class QYDragWindow;
class QYString;
class QYWindow;
class QYWidget;
class QYUI_EXPORT_CLASS QYApp
{
public:
	QYApp();
	virtual BOOL InitInstance(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR    lpCmdLine,
		int       nCmdShow);
	virtual BOOL ExitInstance();
	virtual ~QYApp(void);
	int Exec();
	void SetMainWindow(QYWindow* pWnd){ m_pMainWindow = pWnd; }
	QYWindow* GetMainWindow(){ return m_pMainWindow; }
	HINSTANCE getInstance(){ return m_hInstance; }
	static BOOL SetGlobalTextMetric(TEXTMETRIC  *textMetric);
	static TEXTMETRIC*  GetGlobleTextMetric(){ return &m_textMetric; }
	static QYString GetAppPath();
	static std::string getAppPath();
	static QYString GetModuleFileName();
	static LRESULT CALLBACK QYPopWndMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void StartPopWndMouseHook(HWND hWnd);
	static void StopPopWndMouseHook();
	static LRESULT CALLBACK QYDragMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK QYKeyDownHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void StartDragMouseHook(HWND hWnd);
	static void StopDragMouseHook();
	static void RegistorDropWnd(HWND hWnd);
	static UINT MessageBox(LPCTSTR strCaption, LPCTSTR strMessage, UINT nFlag);
	static void SetTextCallback(const char* (*getTextCB)(const char *msgID));
	static void bindLayoutPath(const char *sPath);
	static std::string m_layoutPath;
	static bool loadTheme(const char *source);
    static  void bindImagePath(const char *sPath);
    static std::string m_imagePath;
	static HWND  m_hPopWindow;
	static HHOOK m_hPopWndMouseHook;
	static HHOOK m_hDragMouseHook;
	static HHOOK m_hKeyDownHook;
	static HWND  m_hDropWindow;
	static HWND  m_hDragWindow;
	static BYTE  *m_pDropData;
	static HINSTANCE  m_hInstance;
	static QYFont   *m_pDlgFont;
	static QYFont   *m_pCaptionFont;
	static QYFont   *m_pMsDefultFont;
	static HBRUSH m_dlg_static_brush;
	static HBRUSH m_dlg_brush ;
	static HBRUSH m_dlg_edit_brush;
	static HBRUSH m_dlg_edit_normal_brush;
	static HBRUSH m_dlg_edit_readonly_brush;
	static HBRUSH m_bkBrush;
	static TEXTMETRIC m_textMetric;
	static QYDragWindow *m_pDragWindow;
	static HWND  m_hDesktopWnd;
	static QYThemeStruct m_ThemeStruct;
	static const char* (*m_getTextCB)(const char *msgID);
private:
	static BOOL loadWidget(QYWidget *pWidget, QYXMLTreeNode *pNode);
	HINSTANCE   m_hPrevInstance;
	LPSTR       m_lpCmdLine;
	int        m_nCmdShow;
	QYWindow   *m_pMainWindow;
protected:

};


#define  QY_THEME     QYApp::m_ThemeStruct