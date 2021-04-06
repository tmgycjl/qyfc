#include "QYApp.h"
#include "QYDC.h"
#include "QYLinearBitmap.h"
#include "includedef.h"
#include "QYCtrlListDlg.h"
#include "QYDragWindow.h"
#include "QYXML.h"
#include "QYStatic.h"
#include "QYWidgetList.h"
#include "QYDateTimeCtrl.h"
#include "QYToolBar.h"
#include "QYTreeCtrl.h"
#include "QYSwitchButton.h"
#include "QYIPCtrl.h"
ULONG_PTR  m_gdiplusToken;




#ifdef _DEBUG
QYConsoleLog g_ConsoleLog(0);
#else
QYConsoleLog g_ConsoleLog(0);
#endif // _DEBUG

HINSTANCE  QYApp::m_hInstance = nullptr;
TEXTMETRIC QYApp::m_textMetric;


QYFont* QYApp::m_pDlgFont = nullptr;
QYFont* QYApp::m_pCaptionFont = nullptr;
QYFont* QYApp::m_pMsDefultFont = nullptr;
HBRUSH QYApp::m_dlg_static_brush = nullptr;
HBRUSH QYApp::m_dlg_brush = nullptr;
HBRUSH QYApp::m_dlg_edit_brush = nullptr;
HBRUSH QYApp::m_dlg_edit_normal_brush = nullptr;
HBRUSH QYApp::m_dlg_edit_readonly_brush = nullptr;
HBRUSH QYApp::m_bkBrush = nullptr;

const char* (*QYApp::m_getTextCB)(const char *msgID) = nullptr;

QYThemeStruct QYApp::m_ThemeStruct;

static QYFont *s_pFont = nullptr;

HWND  QYApp::m_hPopWindow = nullptr;
HHOOK QYApp::m_hPopWndMouseHook = nullptr;

QYDragWindow *QYApp::m_pDragWindow = nullptr;


LRESULT CALLBACK QYApp::QYPopWndMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		
		if (WM_LBUTTONDOWN == wParam)
		{
			PMSLLHOOKSTRUCT 	ms = (PMSLLHOOKSTRUCT)lParam;

			if (m_hPopWindow != nullptr )
			{
				//g_ConsoleLog.Print("QYPopWndMouseHookProc::OnLButtonDown\n");
				::ScreenToClient(m_hPopWindow, &ms->pt);
				::SendMessage(m_hPopWindow, WM_LBUTTONDOWN, wParam, MAKELONG(ms->pt.x , ms->pt.y ));
				return TRUE;
			}
		}
	}

	return CallNextHookEx(m_hPopWndMouseHook, nCode, wParam, lParam);
}


HWND QYApp::m_hDragWindow = nullptr;
HWND QYApp::m_hDropWindow = nullptr;
BYTE* QYApp::m_pDropData = nullptr;
HHOOK QYApp::m_hDragMouseHook = nullptr;

LRESULT CALLBACK QYApp::QYDragMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		PMSLLHOOKSTRUCT 	ms = (PMSLLHOOKSTRUCT)lParam;
		if (WM_LBUTTONUP== wParam)
		{
			if (nullptr != m_hDropWindow)
			{
				RECT rect;
				GetWindowRect(m_hDropWindow, &rect);
				if (PtInRect(&rect, ms->pt))
				{
					LPARAM ptPos = MAKELONG(ms->pt.x, ms->pt.y);
					PostMessage(m_hDropWindow, QY_WM_DROPDATA, (WPARAM)m_hDragWindow, ptPos);
					(nullptr != m_pDragWindow) ? m_pDragWindow->Show(SW_HIDE) : 0;
					//g_ConsoleLog.Print("send drop message\n");
				}
			}
		}
		else if (WM_MOUSEMOVE == wParam)
		{
			if (nullptr != m_hDropWindow && ::IsWindowVisible(m_hDropWindow) && nullptr != m_hDragWindow)
			{
				RECT rectDrop;
				GetWindowRect(m_hDropWindow, &rectDrop);
				if (PtInRect(&rectDrop, ms->pt) )
				{
					(nullptr != m_pDragWindow) ? m_pDragWindow->TrackPopup(QYPoint(ms->pt.x, ms->pt.y)) : 0;
					PostMessage(m_hDragWindow,WM_SETCURSOR,1, 0);
				}
				else
				{
					(nullptr != m_pDragWindow) ? m_pDragWindow->Show(SW_HIDE) : 0;
					PostMessage(m_hDragWindow, WM_SETCURSOR, 0, 0);
				}
				
			}
		}
	}

	return CallNextHookEx(m_hDragMouseHook, nCode, wParam, lParam);
}

HHOOK QYApp::m_hKeyDownHook = nullptr;


LRESULT CALLBACK QYApp::QYKeyDownHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (VK_MENU == wParam || VK_ESCAPE == wParam)
		{
			if (nullptr != m_hPopWindow)
			{
				PostMessage(m_hPopWindow, QY_WM_CLOSE_POP_WINDOW, 0, 0);
			}
		}
	}

	return CallNextHookEx(m_hKeyDownHook, nCode, wParam, lParam);
}


QYApp::QYApp()
{
	m_textMetric.tmWeight = FW_NORMAL;
	m_textMetric.tmHeight = QY_THEME.MS_YAHEI_FONT_SIZE;
	m_textMetric.tmDescent = 4;
}


BOOL QYApp::InitInstance(HINSTANCE hInstance,
			  HINSTANCE hPrevInstance,
			  LPTSTR    lpCmdLine,
			  int       nCmdShow)
{
	m_hInstance     = hInstance;
	m_hPrevInstance = hPrevInstance;

	m_nCmdShow      = nCmdShow;

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_pDlgFont = new QYFont;
	m_pDlgFont->CreateFont(m_ThemeStruct.DLG_FONT_SIZE, 0, FW_NORMAL, m_ThemeStruct.DLG_FONT_TYPE);
	m_pMsDefultFont = new QYFont;
	m_pMsDefultFont->CreateFont(m_textMetric.tmHeight, 0, m_textMetric.tmWeight, m_ThemeStruct.MS_YAHEI_FONT_TYPE);
	m_pCaptionFont = new QYFont;
	m_pCaptionFont->CreateFont(m_textMetric.tmHeight, 0, m_textMetric.tmWeight, m_ThemeStruct.CAPTION_FONT_TYPE);
	m_dlg_static_brush = ::CreateSolidBrush(m_ThemeStruct.CLIENT_COLOR);
	m_dlg_edit_brush = ::CreateSolidBrush(m_ThemeStruct.EDIT_BK_COLOR_NORMAL);
	m_dlg_edit_readonly_brush = ::CreateSolidBrush(m_ThemeStruct.EDIT_BK_COLOR_READONLY);
	m_bkBrush = ::CreateSolidBrush(m_ThemeStruct.CLIENT_COLOR);

	QYLinearBitmap::InitGlobalLinearRgb();

	m_hKeyDownHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)QYKeyDownHookProc, m_hInstance, GetCurrentThreadId());

	
	//StartPopWndMouseHook(NULL);

	return TRUE;
}

BOOL QYApp::ExitInstance()
{
	DeleteObject(m_dlg_edit_readonly_brush);
	DeleteObject(m_dlg_edit_brush);
	DeleteObject(m_dlg_static_brush);
	
	StopPopWndMouseHook();

	StopDragMouseHook();

	if (nullptr != m_hKeyDownHook)
	{
		::UnhookWindowsHookEx(m_hKeyDownHook);
		m_hKeyDownHook = nullptr;
	}

	SAFE_DESTROY_WINDOW_PTR(m_pDragWindow);

	if (nullptr != m_pDlgFont)
	{
		m_pDlgFont->DeleteObject();
		SAFE_DELETE(m_pDlgFont);
	}
	if (nullptr != m_pCaptionFont)
	{
		m_pCaptionFont->DeleteObject();
		SAFE_DELETE(m_pCaptionFont);
	}
	if (nullptr != m_pMsDefultFont)
	{
		m_pMsDefultFont->DeleteObject();
		SAFE_DELETE(m_pMsDefultFont);
	}

	if (0 != m_gdiplusToken)
	{
		GdiplusShutdown(m_gdiplusToken);
	}

	QYToolTip *p = QYToolTip::getInstance();
	SAFE_DESTROY_WINDOW_PTR(p);

	return TRUE;
}

QYApp::~QYApp(void)
{
	
}

BOOL QYApp::SetGlobalTextMetric(TEXTMETRIC  *textMetric)
{
	memcpy(&m_textMetric, textMetric, sizeof(TEXTMETRIC));

	return TRUE;
}

int QYApp::Exec()
{//
	MSG msg;
	//HACCEL hAccelTable;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int) msg.wParam;
}

QYString QYApp::GetModuleFileName()
{
	TCHAR szModuleName[MAX_PATH] = { 0 };
	DWORD dwResult = ::GetModuleFileName(NULL, szModuleName, MAX_PATH);
	QYString  strModuleName = szModuleName;
	//strModuleName = strModuleName.Right(strModuleName.RFind('.') + 1);

	return strModuleName;
}

QYString QYApp::GetAppPath()
{
	TCHAR szModulePath[MAX_PATH] = { 0 };
	DWORD dwResult = ::GetModuleFileName(NULL, szModulePath, MAX_PATH);
	QYString  strModulePath = szModulePath;
	strModulePath = strModulePath.Left(strModulePath.RFind('\\') + 1);

	return strModulePath;
}

std::string QYApp::getAppPath()
{
	TCHAR wszModulePath[MAX_PATH] = { 0 };
	DWORD dwResult = ::GetModuleFileName(NULL, wszModulePath, MAX_PATH);
	char szModulePath[MAX_PATH] = { 0 };
	QYString::SafeW2A(szModulePath, MAX_PATH, wszModulePath);
	std::string sModulePath = szModulePath;
	std::string dstPath;
	dstPath.append(sModulePath.c_str(), sModulePath.rfind('\\') + 1);
	return dstPath;
}


void QYApp::StartPopWndMouseHook(HWND hWnd)
{
	if (nullptr == m_hPopWindow && nullptr == m_hPopWndMouseHook)
	{
		m_hPopWindow = hWnd;
		m_hPopWndMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)QYPopWndMouseHookProc, m_hInstance, 0);
	}
	
}

void QYApp::StopPopWndMouseHook()
{
	if (nullptr != m_hPopWndMouseHook)
	{
		::UnhookWindowsHookEx(m_hPopWndMouseHook);
		m_hPopWndMouseHook = nullptr;
		m_hPopWindow = nullptr;
	}
}

void QYApp::RegistorDropWnd(HWND hWnd)
{
	m_hDropWindow = hWnd;
}

void QYApp::StartDragMouseHook(HWND hWnd)
{
	StopDragMouseHook();

	m_hDragWindow = hWnd;

	if (nullptr == m_pDragWindow)
	{
		m_pDragWindow = new QYDragWindow;
		m_pDragWindow->Create(0, nullptr, nullptr, WS_CHILD , 0, GetDesktopWindow());
	}
	
	m_hDragMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)QYDragMouseHookProc, m_hInstance, 0);

	//g_ConsoleLog.Print("StartDragMouseHook\n");
}

void QYApp::StopDragMouseHook()
{
	if (nullptr != m_hDragMouseHook)
	{
		::UnhookWindowsHookEx(m_hDragMouseHook);
		m_hDragMouseHook = nullptr;
		m_hDragWindow = nullptr;
		//g_ConsoleLog.Print("StopDragMouseHook\n");
	}

	if (nullptr != m_hKeyDownHook)
	{
		::UnhookWindowsHookEx(m_hKeyDownHook);
		m_hKeyDownHook = nullptr;
	}
}

void QYApp::SetTextCallback(const char* (*getTextCB)(const char *msgID))
{
	m_getTextCB = getTextCB;
}


std::string QYApp::m_layoutPath;


void QYApp::bindLayoutPath(const char *sPath)
{
	m_layoutPath = sPath;
}

std::string QYApp::m_imagePath;
void QYApp::bindImagePath(const char *sPath)
{
    m_imagePath = sPath;
}


bool QYApp::loadTheme(const char *source)
{
	FILE *pFile = nullptr;
	std::string layoutPath = m_layoutPath + source;
	QYXML xml;
	if (xml.Load(layoutPath.c_str()))
	{
		QYXMLTreeNode *pRoot = xml.GetRoot();
		auto themeElemList = pRoot->getChildrenList();

		for (QYXMLTreeNode* themeElem : *themeElemList)
		{
			int r = 0;
			int g = 0;
			int b = 0;

			if ("DLG_FONT_TYPE" == themeElem->name)
			{
				TCHAR temp[1024] = { 0 };
				QYString::SafeUTF82W(temp, 1024, themeElem->data.c_str());
				SafeStrcpy(QY_THEME.DLG_FONT_TYPE, temp);
			}
			else if ("MS_YAHEI_FONT_TYPE" == themeElem->name)
			{
				TCHAR temp[1024] = { 0 };
				QYString::SafeUTF82W(temp, 1024, themeElem->data.c_str());
				SafeStrcpy(QY_THEME.MS_YAHEI_FONT_TYPE, temp);
			}
			else if ("CAPTION_FONT_TYPE" == themeElem->name)
			{
				TCHAR temp[1024] = { 0 };
				QYString::SafeUTF82W(temp, 1024, themeElem->data.c_str());
				SafeStrcpy(QY_THEME.CAPTION_FONT_TYPE, temp);
			}
			else if ("CAPTION_FONT_TYPE_2" == themeElem->name)
			{
				TCHAR temp[1024] = { 0 };
				QYString::SafeUTF82W(temp, 1024, themeElem->data.c_str());
				SafeStrcpy(QY_THEME.CAPTION_FONT_TYPE_2, temp);
			}
			else if ("CAPTION_BK_GRADIENT" == themeElem->name)
			{
				BOOL temp = FALSE;
				if ("true" == themeElem->data)
				{
					temp = TRUE;
				}
				else
				{
					temp = FALSE;
				}
				QY_THEME.CAPTION_BK_GRADIENT = temp;
			}
			else if ("DEFAULT_FONT_WIDTH" == themeElem->name)
			{
				QY_THEME.DEFAULT_FONT_WIDTH = atoi(themeElem->data.c_str());
			}
			else if ("CAPTION_FONT_WIDTH" == themeElem->name)
			{
				QY_THEME.CAPTION_FONT_WIDTH = atoi(themeElem->data.c_str());
			}
			else if ("CAPTION_FONT_HEIGHT" == themeElem->name)
			{
				QY_THEME.CAPTION_FONT_HEIGHT = atoi(themeElem->data.c_str());
			}
			else if ("BUTTON_FONT_SIZE" == themeElem->name)
			{
				QY_THEME.BUTTON_FONT_SIZE = atoi(themeElem->data.c_str());
			}
			else if ("DLG_FONT_SIZE" == themeElem->name)
			{
				QY_THEME.DLG_FONT_SIZE = atoi(themeElem->data.c_str());
			}
			else if ("TITLE_FONT_SIZE" == themeElem->name)
			{
				QY_THEME.TITLE_FONT_SIZE = atoi(themeElem->data.c_str());
			}
			else if ("CAPTION_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(),"%d,%d,%d",&r,&g,&b))
				{
					QY_THEME.CAPTION_COLOR = RGB(r, g, b);
				}
			}
			else if ("DLG_CAPTION_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DLG_CAPTION_COLOR = RGB(r, g, b);
				}
			}
			else if ("CLIENT_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CLIENT_COLOR = RGB(r, g, b);
				}
			}
			else if ("DLG_CLIENT_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DLG_CLIENT_COLOR = RGB(r, g, b);
				}
			}
			else if ("BORDER_PEN_COLOR_ACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BORDER_PEN_COLOR_ACTIVE = RGB(r, g, b);
				}
			}
			else if ("BORDER_PEN_COLOR_UNACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BORDER_PEN_COLOR_UNACTIVE = RGB(r, g, b);
				}
			}
			else if ("DLG_BORDER_PEN_COLOR_ACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DLG_BORDER_PEN_COLOR_ACTIVE = RGB(r, g, b);
				}
			}
			else if ("DLG_BORDER_PEN_COLOR_UNACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DLG_BORDER_PEN_COLOR_UNACTIVE = RGB(r, g, b);
				}
			}
			else if ("SPLITTER_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SPLITTER_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("SPLITTER_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SPLITTER_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("SPLITTER_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SPLITTER_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("CAPTION_TEXT_COLOR_ACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CAPTION_TEXT_COLOR_ACTIVE = RGB(r, g, b);
				}
			}
			else if ("CAPTION_TEXT_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CAPTION_TEXT_COLOR = RGB(r, g, b);
				}
			}
			else if ("DLG_BUTTON_PANE_BK_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DLG_BUTTON_PANE_BK_COLOR = RGB(r, g, b);
				}
			}
			else if ("BUTTON_TEXT_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_TEXT_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("BUTTON_TEXT_COLOR_DISABLE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_TEXT_COLOR_DISABLE = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_FOCUS" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_FOCUS = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_DISABLE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_DISABLE = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BORDER_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BORDER_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BORDER_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BORDER_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BORDER_COLOR_FOCUS" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BORDER_COLOR_FOCUS = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BORDER_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BORDER_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BORDER_COLOR_DISABLE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BORDER_COLOR_DISABLE = RGB(r, g, b);
				}
			}
			else if ("EDIT_SELECT_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_SELECT_COLOR = RGB(r, g, b);
				}
			}
			else if ("EDIT_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("EDIT_BK_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BK_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("EDIT_BK_COLOR_FOCUS" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BK_COLOR_FOCUS = RGB(r, g, b);
				}
			}
			else if ("EDIT_BK_COLOR_READONLY" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BK_COLOR_READONLY = RGB(r, g, b);
				}
			}
			else if ("EDIT_BORDER_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BORDER_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("CLOSE_BOX_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CLOSE_BOX_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("CLOSE_BOX_BK_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CLOSE_BOX_BK_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("CLOSE_BOX_BK_COLOR_HOT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.CLOSE_BOX_BK_COLOR_HOT = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_PEN_COLOR_HOT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_PEN_COLOR_HOT = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_PEN_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_PEN_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_PEN_COLOR_NORMAL_ACTIVE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_PEN_WIDTH" == themeElem->name)
			{
				QY_THEME.SYS_BOX_PEN_WIDTH = atoi(themeElem->data.c_str());
			}
			else if ("SYS_BOX_PEN_COLOR_DISABLE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_PEN_COLOR_DISABLE = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_BK_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_BK_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("SYS_BOX_BK_COLOR_HOT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SYS_BOX_BK_COLOR_HOT = RGB(r, g, b);
				}
			}
			else if ("EDIT_BORDER_COLOR_FOCUS" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BORDER_COLOR_FOCUS = RGB(r, g, b);
				}
			}
			else if ("EDIT_BORDER_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.EDIT_BORDER_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("COMBOBOX_ARROW_PEN_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.COMBOBOX_ARROW_PEN_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("COMBOBOX_ARROW_BK_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.COMBOBOX_ARROW_BK_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("COMBOBOX_ARROW_BK_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.COMBOBOX_ARROW_BK_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BK_COLOR_NORMAL2" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BK_COLOR_NORMAL2 = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BK_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BK_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BK_COLOR_SELECT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BK_COLOR_SELECT = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BK_COLOR_LOSE_FOCUS" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BK_COLOR_LOSE_FOCUS = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("LISTCTRL_SEPARATED_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTCTRL_SEPARATED_COLOR = RGB(r, g, b);
				}
			}
			else if ("DROPLIST_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DROPLIST_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("LISTBOX_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTBOX_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("LISTBOX_BK_COLOR_SELECT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTBOX_BK_COLOR_SELECT = RGB(r, g, b);
				}
			}
			else if ("LISTBOX_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.LISTBOX_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("MENU_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.MENU_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("MENU_BK_COLOR_SELECT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.MENU_BK_COLOR_SELECT = RGB(r, g, b);
				}
			}
			else if ("TOOLTIP_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TOOLTIP_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("TOOLTIP_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TOOLTIP_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("HEADERCTRL_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.HEADERCTRL_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("HEADERCTRL_BK_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.HEADERCTRL_BK_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("HEADERCTRL_BK_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.HEADERCTRL_BK_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("HEADERCTRL_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.HEADERCTRL_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("HEADERCTRL_SEPARATED_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.HEADERCTRL_SEPARATED_COLOR = RGB(r, g, b);
				}
			}
			else if ("PAGEPANE_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.PAGEPANE_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("TAB_BOTTOM_SPLIT_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_BOTTOM_SPLIT_COLOR = RGB(r, g, b);
				}
			}
			else if ("TAB_BK_COLOR_HOT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_BK_COLOR_HOT = RGB(r, g, b);
				}
			}
			else if ("TAB_BK_COLOR_SEL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_BK_COLOR_SEL = RGB(r, g, b);
				}
			}
			else if ("TAB_BK_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_BK_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("BUTTON_BK_COLOR_SEL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.BUTTON_BK_COLOR_SEL = RGB(r, g, b);
				}
			}
			else if ("ITEM_BK_COLOR_HOT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.ITEM_BK_COLOR_HOT = RGB(r, g, b);
				}
			}
			else if ("ITEM_BK_COLOR_SEL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.ITEM_BK_COLOR_SEL = RGB(r, g, b);
				}
			}
			else if ("ITEM_BORBER_COLOR_SEL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.ITEM_BORBER_COLOR_SEL = RGB(r, g, b);
				}
			}
			else if ("DATETABLE_CURRENT_DAY_BK_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DATETABLE_CURRENT_DAY_BK_COLOR = RGB(r, g, b);
				}
			}
			else if ("DATETABLE_CURRENT_DAY_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DATETABLE_CURRENT_DAY_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("DATETABLE_MARK_DAY_BK_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DATETABLE_CURRENT_DAY_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("DATETABLE_HOVER_DAY_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DATETABLE_CURRENT_DAY_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("DATETABLE_CHECK_DAY_BORDER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.DATETABLE_CURRENT_DAY_BORDER_COLOR = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_COLOR_SELECT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_COLOR_SELECT = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_ARROW_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_ARROW_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_ARROW_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_ARROW_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("SCROLLBAR_ARROW_COLOR_DOWN" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.SCROLLBAR_ARROW_COLOR_DOWN = RGB(r, g, b);
				}
			}
			else if ("TIMETABLE_HEADER_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TIMETABLE_HEADER_COLOR = RGB(r, g, b);
				}
			}
			else if ("TIMETABLE_ITEM_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TIMETABLE_ITEM_COLOR = RGB(r, g, b);
				}
			}
			else if ("PROGRESS_BK_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.PROGRESS_BK_COLOR = RGB(r, g, b);
				}
			}
			else if ("PROGRESS_PROCESS_COLOR" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.PROGRESS_PROCESS_COLOR = RGB(r, g, b);
				}
			}
			else if ("TEXT_COLOR_NORMAL" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_TEXT_COLOR_NORMAL = QY_THEME.TEXT_COLOR_NORMAL = RGB(r, g, b);
				}
			}
			else if ("TEXT_COLOR_HOVER" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TEXT_COLOR_HOVER = RGB(r, g, b);
				}
			}
			else if ("TEXT_COLOR_SELECT" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TAB_TEXT_COLOR_SELECT = QY_THEME.TEXT_COLOR_SELECT = RGB(r, g, b);
				}
			}
			else if ("TEXT_COLOR_DISABLE" == themeElem->name)
			{
				if (3 == sscanf_s(themeElem->data.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					QY_THEME.TEXT_COLOR_DISABLE = RGB(r, g, b);
				}
			}
		}

		return true;
	}

	return false;
}
