#include "QYWindow.h"
#include "QYWidget.h"
#include "QYDC.h"
#include "QYLanguage.h"
#include "QYWidgetList.h"
#include "QYEdit.h"
#include "QYComboBox.h"
#include "QYSwitchButton.h"
#include "QYDateTimeCtrl.h"
#include "QYDateTable.h"
#include "QYIPCtrl.h"
#include "QYFolderEdit.h"
#include "QYStatic.h"
#include "QYToolBar.h"
#include "QYTreeCtrl.h"
#include "QYRectBackground.h"
#include "QYProgressCtrl.h"

std::vector<int> stringSplit(std::string str, std::string pattern)
{
	std::string::size_type pos = 0;
	std::vector<int> result;
	str += pattern;
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			int  s = atoi(str.substr(i, pos - i).c_str());
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}

	return result;
}

std::map<HWND, QYWindow*>  QYWindow::_mapWindow;

QYWindow* QYWindow::getWindowPtr(HWND hWnd)
{
	auto it = _mapWindow.find(hWnd);
	if (it != _mapWindow.end())
	{
		return it->second;
	}

	return nullptr;
}

bool QYWindow::setWindowPtr(HWND hWnd, QYWindow *pWindow)
{
	if (nullptr == pWindow)
	{
		return false;
	}

	std::pair<std::map<HWND, QYWindow*>::iterator, bool> ret = _mapWindow.insert(std::map<HWND, QYWindow*>::value_type(hWnd, pWindow));
	return ret.second;
}

LRESULT CALLBACK QYWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
#if 1
	QYWindow *pWindow = reinterpret_cast<QYWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (NULL != pWindow)
	{
		if ((INT_PTR)TRUE == pWindow->WindowProc(message, wParam, lParam))
		{
// 			if (WM_PAINT == message)
// 			{
// 
// 					UINT style = GetWindowLong(hWnd, GWL_STYLE);
// 					style |= WS_CLIPCHILDREN;
// 					SetWindowLong(hWnd, GWL_STYLE, style);
// 			}
			return (INT_PTR)TRUE;
		}
	}

	switch (message)
	{
	case WM_NCPAINT:return TRUE;
	case WM_ERASEBKGND:return TRUE;
	case WM_GETDLGCODE:
	{
		if (lParam)
		{
			LPMSG lpmsg = (LPMSG)lParam;
			if (lpmsg->message == WM_CHAR)
			{
				return DLGC_WANTCHARS;
			}
		}
	}
	break;
	case WM_NCCREATE:
	{
		if (nullptr == pWindow)
		{
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pWindow = static_cast<QYWindow*>(lpcs->lpCreateParams);
			pWindow->SetHwnd(hWnd);
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pWindow));
			pWindow->WindowProc(message, wParam, lParam);
			return TRUE;
		}
		//SetWindowLong(hWnd, GWLP_USERDATA, (LONG)cs->lpCreateParams);
	}
	break;
	case WM_NCDESTROY:
	{
		LRESULT lRes = ::CallWindowProc(pWindow->m_oldWndProc, hWnd, message, wParam, lParam);
		::SetWindowLongPtr(pWindow->m_hWnd, GWLP_USERDATA, 0L);
		pWindow->SetHwnd(nullptr);
		return lRes;
	}
	break;
	//case WM_SETTEXT: return TRUE; break;
	case WM_NCACTIVATE:
	{
		if (!::IsIconic(hWnd))
		{
			return (wParam == 0) ? TRUE : FALSE;
		}

		break;
		lParam = -1;
		int ret = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	//case WM_NCCALCSIZE:return TRUE;
	default:
	{
		if (nullptr != pWindow && nullptr != pWindow->m_oldWndProc)
		{
			return pWindow->m_oldWndProc(hWnd, message, wParam, lParam);
		}
		else
		{
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	}

#else
	QYWindow* pThis = NULL;
	if (message == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<QYWindow*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	}
	else if (WM_GETDLGCODE == message)
	{

			if (lParam)
			{
				LPMSG lpmsg = (LPMSG)lParam;
				if (lpmsg->message == WM_CHAR)
				{
					return DLGC_WANTCHARS;
				}
			}
	}
	else
	{
		pThis = reinterpret_cast<QYWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (message == WM_NCDESTROY && pThis != NULL)
		{
			LRESULT lRes = ::CallWindowProc(pThis->m_oldWndProc, hWnd, message, wParam, lParam);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			pThis->m_hWnd = NULL;
			return lRes;
		}
	}
	if (pThis != NULL) {
		return pThis->WindowProc(message, wParam, lParam);
	}
	else {
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	
#endif

	
	return 0L;
}

QYWindow::QYWindow(void)
{
	m_hWnd = nullptr;
	m_oldWndProc = nullptr;
	m_pWindowDC = nullptr;
	m_pParent = nullptr;
	m_bHover = FALSE;
	m_hIcon = nullptr;
	m_pFont = new QYFont;
	m_hNotifyMsgHwnd = nullptr;
	m_bShowShadow = FALSE;
	m_nBorderWidth = 0;
	m_nCtrlID = 0;
	m_pParent = nullptr;
	m_CaptionHeight = 0;

	m_FrameHeight = 0;
	m_FrameWidth = 0;
	m_disableMaxBox = TRUE;
	m_disableMinBox = TRUE;
	m_bSizeBox = FALSE;

	if (nullptr != QYApp::m_pDlgFont)
	{
		m_pFont->m_hObject = QYApp::m_pMsDefultFont->m_hObject;
	}
	else
	{
		m_pFont->m_hObject = ::GetStockObject(DEFAULT_GUI_FONT);
	}

	m_colorBk = QY_THEME.CLIENT_COLOR;
	m_colorText = QY_THEME.TEXT_COLOR_NORMAL;
	m_colorSelText = m_colorBk;
}

QYWindow::~QYWindow(void)
{
	for (auto it = m_mapItem.begin(); it != m_mapItem.end(); it++)
	{
		//g_ConsoleLog.Print("key:%s\n", it->first.c_str());
		SAFE_DELETE(it->second);
	}

	m_mapItem.clear();


	SAFE_DELETE(m_pFont);

	if (nullptr != m_hIcon)
	{
		::DestroyIcon(m_hIcon);
		m_hIcon = nullptr;
	}
	SAFE_DESTROY_WINDOW(m_hWnd);
	SAFE_DELETE(m_pWindowDC);
//	SAFE_DELETE(m_pCaptionBkImage);
}


HWND  QYWindow::SetNotifyMsgWindow(HWND hMsgWnd)
{
	HWND last = hMsgWnd;
	m_hNotifyMsgHwnd = hMsgWnd;
	return last;
}

BOOL QYWindow::NotifyMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND desMsgHwnd = (nullptr != m_hNotifyMsgHwnd) ? m_hNotifyMsgHwnd : ::GetParent(m_hWnd);

	return ::SendMessage(m_hNotifyMsgHwnd, message, wparam, lparam);
}

HFONT QYWindow::SetFont(HFONT hFont)
{
	if (nullptr == m_pFont)
	{
		return nullptr;
	}

	HFONT oldfont = (HFONT)m_pFont->m_hObject;
	m_pFont->m_hObject = hFont;

	return oldfont;
}

QYFont* QYWindow::GetFont()
{
	return m_pFont;
}

QYDC* QYWindow::GetWindowDC()
{
	SAFE_DELETE(m_pWindowDC);

	QYDC *pDC = new QYDC(::GetWindowDC(m_hWnd));

	return pDC;
}

BOOL QYWindow::ReleaseDC(QYDC *pDC)
{
	int ret =  ::ReleaseDC(m_hWnd, pDC->GetDC());

	SAFE_DELETE(pDC);

	return ret;
}


QYWindow* QYWindow::SetParent(QYWindow *pWnd)
{
	HWND hWnd = ::SetParent(m_hWnd, pWnd->GetHwnd());

	if (hWnd == pWnd->GetHwnd())
	{
		m_pParent = pWnd;
		return pWnd;
	}
	else
	{
		return nullptr;
	}
}

#if 0


int QYWindow::GetWindowText(QYString &strText, int size)
{
	strText = nullptr;
	int ret = 0;
	if (1024 < size)
	{
		TCHAR *szText = new TCHAR[size];
		ret = ::GetWindowText(m_hWnd, szText, size);

		strText = szText;

		SAFE_DELETE_ARRAY(szText);
	}
	else
	{
		TCHAR szText[1024] = { 0 };
		ret = ::GetWindowText(m_hWnd, szText, size);

		strText = szText;
	}
	


	return ret;
}
#endif

BOOL QYWindow::InvalidateRect(_In_opt_ CONST QYRect *lpRect, _In_ BOOL bErase)
{
	RECT rt;
	if (nullptr != lpRect)
	{
		;
		rt.left = lpRect->left;
		rt.right = lpRect->right;
		rt.bottom = lpRect->bottom;
		rt.top = lpRect->top;
		return ::InvalidateRect(m_hWnd, &rt, bErase);
	}
	else
	{
		return ::InvalidateRect(m_hWnd,nullptr, bErase);
	}
}

BOOL QYWindow::Create(__in DWORD dwExStyle,
	__in_opt LPCWSTR lpClassName,
	__in_opt LPCWSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X ,
	__in int Y,
	__in int nWidth ,
	__in int nHeight ,
	HWND hParentWnd,
	__in_opt HMENU hMenu ,
	__in_opt HINSTANCE hInstance ,
	__in_opt LPVOID lpParam )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance      = (nullptr == hInstance) ? (QYApp::m_hInstance) : hInstance;
	wcex.hIcon			= /*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QYUIDEMO))*/nullptr;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = QYApp::m_bkBrush;
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName  = lpClassName;
	wcex.hIconSm		= /*LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL))*/nullptr;

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QYUIDEMO));
	ATOM atom = RegisterClassEx(&wcex);

	//dwStyle &= ~WS_VISIBLE;
	m_hWnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle,
		X, Y, nWidth, nHeight, hParentWnd, hMenu, wcex.hInstance, this);
	if (nullptr == m_hWnd)
	{
		return FALSE;
	}

	UpdateWindow(m_hWnd);

	m_hNotifyMsgHwnd = hParentWnd;

	return TRUE;
}


BOOL QYWindow::CreateEx(__in DWORD dwExStyle,
	__in_opt LPCWSTR lpClassName,
	__in_opt LPCWSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	QYWindow *pParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (nullptr == hInstance) ? (QYApp::m_hInstance) : hInstance;
	wcex.hIcon = /*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QYUIDEMO))*/nullptr;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = QYApp::m_bkBrush;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = lpClassName;
	wcex.hIconSm = /*LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL))*/nullptr;

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QYUIDEMO));
	ATOM atom = RegisterClassEx(&wcex);

	m_hWnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle ,
		X, Y, nWidth, nHeight, pParent->GetHwnd(), hMenu, wcex.hInstance, this);
	if (nullptr == m_hWnd)
	{
		return FALSE;
	}

	UpdateWindow(m_hWnd);

	m_hNotifyMsgHwnd = pParent->GetHwnd();
	m_pParent = pParent;

	return TRUE;
}

BOOL QYWindow::Show(int nCmdShow)
{
	ShowWindow(m_hWnd,nCmdShow);
	UpdateWindow(m_hWnd);
	return TRUE;
}

LRESULT QYWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

BOOL QYWindow::GetClientRect(QYRect &rect)
{
	RECT rt;
	auto ret = ::GetClientRect(m_hWnd, &rt);
	rect = rt;

	if (0 < m_nBorderWidth)
	{
		int n = 0;
	}
	//rect.DeflateRect(m_nBorderWidth, m_nBorderWidth);
	rect.top += m_CaptionHeight;

	if (m_bSizeBox)
	{
		rect.left += m_FrameWidth;
		rect.right -= m_FrameWidth;
		rect.bottom -= m_FrameHeight;
	}
	
	return ret;
}

BOOL QYWindow::GetWindowRect(QYRect &rect, QYWindow *pWnd)
{
	if (nullptr != pWnd)
	{
		auto ret = GetWindowRect(rect);
		if (!ret)
		{
			return ret;
		}

		RECT rt;
		ret = ::GetWindowRect(pWnd->GetHwnd(), &rt);
		rect.OffsetRect(-rt.left,-rt.top);
		return ret;
	}
	else
	{
		RECT rt;
		auto ret = ::GetWindowRect(m_hWnd, &rt);
		rect = rt;
		return ret;

	}
	
}

BOOL QYWindow::ClientToWindow(HWND hWnd, __inout QYPoint *point)
{
	QYRect rtWnd;
	BOOL ret = GetWindowRect(rtWnd);
	if (!ret)
	{
		return ret;
	}

	RECT rtParentWnd;
	ret = ::GetWindowRect(hWnd, &rtParentWnd);
	if (!ret)
	{
		return ret;
	}

	rtWnd.OffsetRect(-rtParentWnd.left, -rtParentWnd.top);
	point->x += rtWnd.left;
	point->y += rtWnd.top;

	return TRUE;
}


BOOL QYWindow::ScreenToClient(__inout QYPoint *point)
{
	POINT pt;
	pt.x = point->x;
	pt.y = point->y;

	BOOL ret = ::ScreenToClient(m_hWnd, &pt);
	if (ret)
	{
		*point = pt;
		return TRUE;
	}

	return FALSE;
}


BOOL QYWindow::ScreenToClient(__inout QYRect *rect)
{
	POINT pt;
	pt.x = rect->left;
	pt.y = rect->top;

	BOOL ret = ::ScreenToClient(m_hWnd, &pt);
	if (ret)
	{
		int offsetX = pt.x - rect->left;
		int offsetY = pt.y - rect->top;

		rect->OffsetRect(offsetX, offsetY);

		return TRUE;
	}

	return FALSE;
}


BOOL QYWindow::ClientToScreen(__inout QYPoint *point)
{
	POINT pt;
	pt.x = point->x;
	pt.y = point->y;

	BOOL ret = ::ClientToScreen(m_hWnd, &pt);
	if (ret)
	{
		*point = pt;
		return TRUE;
	}

	return FALSE;
}


BOOL QYWindow::ClientToScreen(__inout QYRect *rect)
{
	POINT pt1;
	POINT pt2;
	pt2.x = pt1.x = rect->left;
	pt2.y = pt1.y = rect->top;

	
	BOOL ret = ::ClientToScreen(m_hWnd, &pt2);
	if (ret)
	{
		rect->OffsetRect(pt2.x - pt1.x, pt2.y - pt1.y);
		return TRUE;
	}

	return FALSE;
}


BOOL QYWindow::MoveWindow(int x, int y, int width, int height, BOOL bRepain )
{
	return ::MoveWindow(m_hWnd, x, y, width, height, bRepain);
}

BOOL QYWindow::MoveWindow(QYRect rect, BOOL bRepain )
{
	return ::MoveWindow(m_hWnd, rect.left, rect.top, rect.Width(), rect.Height(), bRepain);
}

void QYWindow::CenterWindow(HWND hInsertAfter)
{
	auto cX = GetSystemMetrics(SM_CXFULLSCREEN);
	auto cY = GetSystemMetrics(SM_CYFULLSCREEN);

	QYRect rt;
	GetWindowRect(rt);
	::SetWindowPos(m_hWnd, hInsertAfter, (cX - rt.Width()) / 2, (cY - rt.Height()) / 2, rt.Width(), rt.Height(), 0);
	//MoveWindow((cX - rt.Width()) / 2, (cY - rt.Height()) / 2, rt.Width(), rt.Height());
}

LRESULT QYWindow::SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(m_hWnd, Msg, wParam, lParam);
}

LRESULT QYWindow::PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return ::PostMessage(m_hWnd, Msg, wParam, lParam);
}

BOOL QYWindow::ModifyStyle(UINT remove, UINT add)
{
	if (add&WS_SIZEBOX)
	{
		m_bSizeBox = TRUE; 
		m_FrameWidth = 3;
		m_FrameHeight = 3;
		add &= ~WS_SIZEBOX;
	}

	if (remove&WS_SIZEBOX)
	{
		m_bSizeBox = FALSE; 
	}

	if (add&WS_MAXIMIZEBOX)
	{
		m_disableMaxBox = FALSE; 
		add &= ~WS_MAXIMIZEBOX;
	}

	if (remove&WS_MAXIMIZEBOX)
	{
		m_disableMaxBox = TRUE;
	}


	if (add&WS_MINIMIZEBOX)
	{
		m_disableMinBox = FALSE;
	}

	if (remove&WS_MINIMIZEBOX)
	{
		m_disableMinBox = TRUE; 
		
	}

	UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
	style |= add;
	style &= ~remove;
	LONG ret =  SetWindowLong(GetHwnd(), GWL_STYLE, style);

	
	return ret;
}

BOOL QYWindow::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL QYWindow::SetWindowPos(HWND hWndInsertAfter, QYRect &rect, UINT uFlags)
{
	return ::SetWindowPos(m_hWnd, hWndInsertAfter, rect.left, rect.top, rect.Width(), rect.Height(), uFlags);
}


BOOL QYWindow::ScrollWindow(int XAmount, int YAmount, QYRect *lpRect, QYRect *lpClipRect)
{
	RECT rect;
	if (nullptr != lpRect)
	{
		rect.left = lpRect->left;
		rect.right = lpRect->right;
		rect.bottom = lpRect->bottom;
		rect.top = lpRect->top;
	}
	else
	{
		rect.top = rect.bottom = rect.right = rect.left = 0;
	}
	
	RECT ClipRect;
	if (nullptr != lpClipRect)
	{
		ClipRect.left = lpClipRect->left;
		ClipRect.right = lpClipRect->right;
		ClipRect.bottom = lpClipRect->bottom;
		ClipRect.top = lpClipRect->top;
	}
	else
	{
		return ::ScrollWindow(m_hWnd, XAmount, YAmount, &rect, nullptr);
	}
	
	
	return ::ScrollWindow(m_hWnd, XAmount, YAmount, &rect, &ClipRect);
}

BOOL QYWindow::SubClassWindow(HWND hWnd)
{
	m_hWnd = hWnd;
	SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)this);
	m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (INT_PTR)WndProc);

	return TRUE;
}

BOOL QYWindow::SetWindowProc(INT_PTR WndProc)
{
	return (nullptr != (m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, WndProc)));
}

BOOL QYWindow::DestroyWindow()
{
	return ::DestroyWindow(m_hWnd);
}


void QYWindow::ShowShadow(BOOL bShow)
{
	m_bShowShadow = bShow;
}

UINT QYWindow::SetCtrlID(UINT nCtrlID)
{ 
	UINT oldCtrlID = m_nCtrlID;
	m_nCtrlID = nCtrlID;

	return oldCtrlID;
}

HWND QYWindow::GetFirstParent()
{
	HWND hParentWnd = nullptr;
	HWND hLastParentWnd = nullptr;
	while (nullptr != (hParentWnd = ::GetParent(m_hWnd)))
	{
		hLastParentWnd = hParentWnd;
	}

	return hLastParentWnd;
}

void QYWindow::SetBkColor(COLORREF color)
{
	m_colorBk = color;
}

void QYWindow::SetTextColor(COLORREF color, COLORREF selColor)
{
	m_colorText = color;
	m_colorSelText = selColor;
}


BOOL QYWindow::SetWindowText(LPCTSTR strText)
{
	m_strWindowText = strText;

    ::SetWindowText(m_hWnd, m_strWindowText);

	Invalidate();

	return TRUE;
}


BOOL QYWindow::setWindowText(const char *textID)
{
	if (nullptr == textID)
	{
		return FALSE;
	}

	m_textID = textID;

	const char *text = QYLanguage::instance()->getText(m_textID.c_str());

	int dstSize = (strlen(text) + 1)*sizeof(TCHAR);
	TCHAR *dstText = new TCHAR[dstSize];
	memset(dstText, 0, dstSize);

	QYString::SafeUTF82W(dstText, dstSize, text);
   
	m_strWindowText = dstText;
    ::SetWindowText(m_hWnd, dstText);
	delete[] dstText;
    
	SendMessage(WM_NCPAINT, 0, 0);

	Invalidate();

	return TRUE;
}

BOOL QYWindow::getWindowText(std::string &text)
{
	if (MAX_WINDOW_TEXT_LENGTH - 1 < m_strWindowText.Length()*sizeof(TCHAR))
	{
		return FALSE;
	}
	char dstText[MAX_WINDOW_TEXT_LENGTH] = { 0 };

	QYString::SafeW2AUTF8(dstText, MAX_WINDOW_TEXT_LENGTH, m_strWindowText);

	text = dstText;

	return TRUE;
}

void QYWindow::updateText()
{
	setWindowText(getTextID());
}

BOOL QYWindow::onSize(UINT nType, int cx, int cy)
{
	if (SIZE_MINIMIZED == nType)
	{
		return TRUE;
	}
	if (0 < m_mapItem.size())
	{
		QYRect rect;
		GetClientRect(rect);
	
		QYRect rectItem;
		for (auto it = m_mapItem.begin(); it != m_mapItem.end(); it++)
		{
			QYWidgetItem *pItem = it->second;

			rectItem.left = rect.left + (pItem->m_rel1.m_X * rect.Width());
			rectItem.left += pItem->m_offset1.cx;

			rectItem.right = rect.left + (pItem->m_rel2.m_X * rect.Width());
			rectItem.right += pItem->m_offset2.cx;

			rectItem.top = rect.top + (pItem->m_rel1.m_Y * rect.Height());
			rectItem.top += pItem->m_offset1.cy;

			rectItem.bottom = rect.top + (pItem->m_rel2.m_Y * rect.Height());
			rectItem.bottom += pItem->m_offset2.cy;

			if (rectItem.Width() > 0 && rectItem.Height() > 0&& nullptr != pItem->m_pWnd)
			{
				pItem->m_pWnd->MoveWindow(rectItem);
			}
		}
	}
	
	return TRUE;
}

QYWindow::QYWidgetItem* QYWindow::getWidgetItem(QYWindow *pWidget, std::string part, std::string &key)
{
	std::map<std::string, QYWidgetItem*> *pMapWidgetItem = nullptr;
	if (nullptr == pWidget)
	{
		pMapWidgetItem = &m_mapItem;
	}
	else
	{
		pMapWidgetItem = pWidget->getMapWidgetItem();
	}

	for (auto it = pMapWidgetItem->begin(); it != pMapWidgetItem->end(); it++)
	{
		if (QY_CONTROL_TYPE_WIDGETLIST == it->second->m_ctrlType)
		{
			if (it->first == part)
			{
				key = it->first;
				return it->second;
			}
			else
			{
				auto *listWidget = ((QYWidgetList*)it->second->m_pWnd)->getWidgetList();
				for (QYWidget *pWidget : *listWidget)
				{
					QYWidgetItem *pItem = getWidgetItem(pWidget, part, key);
					if (nullptr != pItem)
					{
						return pItem;
					}
				}
			}

		}
		else if (QY_CONTROL_TYPE_WIDGET == it->second->m_ctrlType)
		{
			if (it->first == part)
			{
				key = it->first;
				return it->second;
			}
			else
			{
				QYWidgetItem *pItem = getWidgetItem(it->second->m_pWnd, part,key);
				if (nullptr != pItem)
				{
					return pItem;
				}
			}

		}
		else if (it->first == part)
		{
			key = it->first;
			return it->second;
		}
	}

	return nullptr;
}

QYWindow* QYWindow::getObjectPart(std::string part)
{
	std::string key;
	QYWidgetItem *pItem = getWidgetItem(nullptr,part,key);
	if (nullptr != pItem)
	{
		return pItem->m_pWnd;
	}
	return nullptr;

}

bool QYWindow::swallowWidget(QYWindow *pWindow, std::string part)
{
	std::string key;
	QYWidgetItem *pItem = getWidgetItem(nullptr, part, key);
	if (nullptr != pItem)
	{
		if (nullptr != pWindow)
		{
			pWindow->setID(key.c_str());
		}

		pItem->m_pWnd = pWindow;
		return true;
	}
	return false;
}

bool QYWindow::swallowWidget(std::string partWidget, std::string partSwallow)
{
	std::string key;
	QYWidgetItem *pWidgetItem = getWidgetItem(nullptr, partWidget, key);
	if (nullptr != pWidgetItem)
	{
		if (pWidgetItem->m_inSwallow)
		{
			if (pWidgetItem->m_keySwallow == partSwallow)
			{
				return true;
			}

			unswallowWidget(partWidget, pWidgetItem->m_keySwallow);
		}
		

		QYWidgetItem *pSwallowItem = getWidgetItem(nullptr, partSwallow, key);
		if (nullptr != pSwallowItem)
		{
			pSwallowItem->m_pWnd = pWidgetItem->m_pWnd;
			pWidgetItem->m_inSwallow = true;
			pWidgetItem->m_keySwallow = partSwallow;
			HWND hParentWnd = ::GetParent(pSwallowItem->m_pWnd->GetHwnd());
			if (nullptr != hParentWnd)
			{
				::SendMessage(hParentWnd, WM_SIZE, 0, 0);
			}
			
			return true;
		}
	}

	return false;
}

bool QYWindow::unswallowWidget(std::string partWidget, std::string partSwallow)
{
	std::string key;
	QYWidgetItem *pWidgetItem = getWidgetItem(nullptr, partWidget, key);
	if (nullptr != pWidgetItem)
	{
		QYWidgetItem *pSwallowItem = getWidgetItem(nullptr, partSwallow, key);
		if (nullptr != pSwallowItem)
		{
			pSwallowItem->m_pWnd = nullptr;
			pWidgetItem->m_inSwallow = false;
			pWidgetItem->m_keySwallow = "";
			return true;
		}
	}

	return false;
}

QYWindow* QYWindow::Append(const char *key, 
	QY_CONTROL_TYPE ctrlType,
	QYRelative rel1,
	QYRelative rel2, 
	QYSize offset1,
	QYSize offset2, 
	BOOL show)
{
	auto *pitem = new QYWidgetItem(ctrlType, rel1, rel2, offset1, offset2);
	assert(nullptr != pitem);

	UINT style = show ? WS_CHILD | WS_VISIBLE : WS_CHILD;

	switch (ctrlType)
	{
	case QY_CONTROL_TYPE_FOLDER_LISTCTRL:
	{
		pitem->m_pWnd = new QYListCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYListCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_EDIT:
	{
		pitem->m_pWnd = new QYEdit;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYEdit*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_FOLDER_EDIT:
	{
		pitem->m_pWnd = new QYFolderEdit;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYFolderEdit*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_IPADDRESS:
	{
		pitem->m_pWnd = new QYIPCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYIPCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_DATETABLE:
	{
		pitem->m_pWnd = new QYDateTable;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}


		if (!((QYDateTable*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_LISTCTRL:
	{
		pitem->m_pWnd = new QYListCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}


		if (!((QYListCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_DATETIME:
	{
		pitem->m_pWnd = new QYDateTimeCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYDateTimeCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_COMBOBOX:
	{
		pitem->m_pWnd = new QYComboBox;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYComboBox*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_BUTTON:
	{
		pitem->m_pWnd = new QYButton;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYButton*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_STATIC:
	{
		pitem->m_pWnd = new QYStatic;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYStatic*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_SWITCH_BUTTON:
	{
		pitem->m_pWnd = new QYSwitchButton;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYSwitchButton*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_WIDGETLIST:
	{
		pitem->m_pWnd = new QYWidgetList;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYWidgetList*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_WIDGET:
	{
		pitem->m_pWnd = new QYWidget;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYWidget*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_RADIO_BUTTON:
		return nullptr;
	case QY_CONTROL_TYPE_TOOLBAR:
	{
		pitem->m_pWnd = new QYToolBar;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYToolBar*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_TREE:
	{
		pitem->m_pWnd = new QYTreeCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYTreeCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_RECT:
	{
		pitem->m_pWnd = new QYRectBackground;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYRectBackground*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
		break;
	case QY_CONTROL_TYPE_PROCESS:
	{
		pitem->m_pWnd = new QYProgressCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYProgressCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, style, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
		break;
	case QY_CONTROL_TYPE_SWALLOW:
	{
		//nothing to do
	}
	break;
	default:
	{
		SAFE_DELETE(pitem);
		return nullptr;
	}
	}

	if (nullptr != pitem->m_pWnd)
	{
		pitem->m_pWnd->setID(key);
	}

	m_mapItem.insert(std::map<std::string, QYWidgetItem*>::value_type(std::string(key), pitem));

	return pitem->m_pWnd;
}

QYWindow* QYWindow::Append(QYWindow *pWindow, const char *key, QY_CONTROL_TYPE ctrlType)
{
	if (nullptr == pWindow)
	{
		return nullptr;
	}

	auto *pitem = new QYWidgetItem(ctrlType, 0, 0, 0, 0);
	assert(nullptr != pitem);

	pitem->m_pWnd = pWindow;
	pitem->m_pWnd->setID(key);

	m_mapItem.insert(std::map<std::string, QYWidgetItem*>::value_type(std::string(key), pitem));

	return pitem->m_pWnd;
}

BOOL QYWindow::loadWidget(QYWindow *pWidget, QYXMLTreeNode *pNode)
{
	auto childrenList = pNode->getChildrenList();
	for (QYXMLTreeNode* p : *childrenList)
	{
		std::string key = p->getAttribute("id");
		if (!key.empty())
		{
			QYRelative rel1 = QYRelative(0, 0);
			QYRelative rel2 = QYRelative(0, 0);
			QYSize offset1 = QYSize(0, 0);
			QYSize offset2 = QYSize(0, 0);
			bool rel1_is_exsit = false;
			bool rel2_is_exsit = false;
			bool offset1_is_exsit = false;
			bool offset2_is_exsit = false;

			std::string s_rel1 = p->getAttribute("rel1");
			if (!s_rel1.empty())
			{
				sscanf_s(s_rel1.c_str(), "%lf,%lf", &rel1.m_X, &rel1.m_Y);
				rel1_is_exsit = true;
			}

			std::string s_rel2 = p->getAttribute("rel2");
			if (!s_rel2.empty())
			{
				sscanf_s(s_rel2.c_str(), "%lf,%lf", &rel2.m_X, &rel2.m_Y);
				rel2_is_exsit = true;
			}

	
			auto pMapWidgetItem = pWidget->getMapWidgetItem();
			if (!rel1_is_exsit)
			{
				std::string s_relto = p->getAttribute("relto");
				if (!s_relto.empty())
				{
					auto it = pMapWidgetItem->find(s_relto);
					if (it != pMapWidgetItem->end())
					{
						rel1 = it->second->m_rel1;
						rel2 = it->second->m_rel2;
						offset1 += it->second->m_offset1;
						offset2 += it->second->m_offset2;
					}

					rel1_is_exsit = true;
				}

			}

			if (!rel1_is_exsit)
			{
				int posCount = 0;
				std::string s_leftto = p->getAttribute("leftto");
				if (!s_leftto.empty())
				{
					std::string  tokey;
					std::string  topos;
					int spacePos = s_leftto.find(' ');
					tokey.append(s_leftto.c_str(), spacePos);
					topos.append(s_leftto.c_str() + spacePos + 1, s_leftto.length() - spacePos - 1);
					if ("GROUP" == tokey)
					{
						rel1.m_X = 0;
						offset1.cx = 0;
					}
					else
					{
						auto it = pMapWidgetItem->find(tokey);
						if (it != pMapWidgetItem->end())
						{
							if ("LEFT" == topos)
							{
								rel1.m_X = it->second->m_rel1.m_X;
								 offset1.cx += it->second->m_offset1.cx;
							}
							else if ("RIGHT" == topos)
							{
								rel1.m_X = it->second->m_rel2.m_X;
								offset2.cx = offset1.cx += it->second->m_offset2.cx;
							}

						}
					}

					posCount++;
				}

				std::string s_topto = p->getAttribute("topto");
				if (!s_topto.empty())
				{
					std::string  tokey;
					std::string  topos;
					int spacePos = s_topto.find(' ');
					tokey.append(s_topto.c_str(), spacePos);
					topos.append(s_topto.c_str() + spacePos + 1, s_topto.length() - spacePos - 1);
					if ("GROUP" == tokey)
					{
						rel1.m_Y = 0;
						offset1.cy = 0;
					}
					else
					{
						auto it = pMapWidgetItem->find(tokey);
						if (it != pMapWidgetItem->end())
						{
							if ("TOP" == topos)
							{
								rel1.m_Y = it->second->m_rel1.m_Y;
								offset2.cy = offset1.cy += it->second->m_offset1.cy;
							}
							else if ("BOTTOM" == topos)
							{
								rel1.m_Y = it->second->m_rel2.m_Y;
								offset2.cy = offset1.cy += it->second->m_offset2.cy;
							}

						}
					}

					posCount++;
				}

				std::string s_rightto = p->getAttribute("rightto");
				if (!s_rightto.empty())
				{
					std::string  tokey;
					std::string  topos;
					int spacePos = s_rightto.find(' ');
					tokey.append(s_rightto.c_str(), spacePos);
					topos.append(s_rightto.c_str() + spacePos + 1, s_rightto.length() - spacePos - 1);
					if ("GROUP" == tokey)
					{
						rel2.m_X = 1;
						offset2.cx = 0;
					}
					else
					{
						auto it = pMapWidgetItem->find(tokey);
						if (it != pMapWidgetItem->end())
						{
							if ("LEFT" == topos)
							{
								 rel2.m_X = it->second->m_rel1.m_X;
								offset2.cx += offset1.cx = it->second->m_offset1.cx;
							}
							else if ("RIGHT" == topos)
							{
								rel2.m_X = it->second->m_rel2.m_X;
								offset2.cx += it->second->m_offset2.cx;
							}

						}
					}
					posCount++;
				}

				std::string s_bottomto = p->getAttribute("bottomto");
				if (!s_bottomto.empty())
				{
					std::string  tokey;
					std::string  topos;
					int spacePos = s_bottomto.find(' ');
					tokey.append(s_bottomto.c_str(), spacePos);
					topos.append(s_bottomto.c_str() + spacePos + 1, s_bottomto.length() - spacePos - 1);
					if ("GROUP" == tokey)
					{
						rel2.m_Y = 1;
						offset2.cy = 0;
					}
					else
					{
						auto it = pMapWidgetItem->find(tokey);
						if (it != pMapWidgetItem->end())
						{
							if ("TOP" == topos)
							{
								rel1.m_Y = it->second->m_rel1.m_Y;
								offset2.cy += it->second->m_offset1.cy;
							}
							else if ("BOTTOM" == topos)
							{
								rel2.m_Y = it->second->m_rel2.m_Y;
								offset2.cy = it->second->m_offset2.cy;
							}

						}
					}
					posCount++;
				}

				if (4 > posCount)
				{
					//lose position set
				}

				std::string s_margin = p->getAttribute("margin");
				if (!s_margin.empty())
				{
					int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					sscanf_s(s_margin.c_str(), "%d,%d,%d,%d", &x1, &y1, &x2, &y2);
					offset1.cx += x1;
					offset1.cy += y1;
					offset2.cx -= x2;
					offset2.cy -= y2;
				}

			}

			std::string s_offset1 = p->getAttribute("offset1");
			if (!s_offset1.empty())
			{
				QYSize offset12 = 0;
				sscanf_s(s_offset1.c_str(), "%d,%d", &offset12.cx, &offset12.cy);
				offset1.cx += offset12.cx;
				offset1.cy += offset12.cy;
				offset1_is_exsit = true;
			}

			std::string s_offset2 = p->getAttribute("offset2");
			if (!s_offset2.empty())
			{
				QYSize offset22 = 0;
				sscanf_s(s_offset2.c_str(), "%d,%d", &offset22.cx, &offset22.cy);
				offset2.cx += offset22.cx;
				offset2.cy += offset22.cy;
				offset2_is_exsit = true;
			}

			COLORREF bkColor = QY_THEME.CLIENT_COLOR;
			std::string s_color = p->getAttribute("color");
			if (!s_color.empty())
			{
				int r = 0;
				int g = 0;
				int b = 0;
				if (3 == sscanf_s(s_color.c_str(), "%d,%d,%d", &r, &g, &b))
				{
					bkColor = RGB(r, g, b);
				}

			}

			if ("static" == p->name)
			{
				QYStatic *pStatic = (QYStatic*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_STATIC, rel1, rel2, offset1, offset2);

				if (nullptr != pStatic)
				{
					pStatic->setWindowText(p->getAttribute("text").c_str());

					pStatic->SetBkColor(bkColor);

					int align = DT_LEFT;
					std::string s_align = p->getAttribute("align");
					if (!s_align.empty())
					{
						if ("left" == s_align)
						{
							align = DT_LEFT;
						}
						else if ("right" == s_align)
						{
							align = DT_RIGHT;
						}
						else if ("top" == s_align)
						{
							align = DT_TOP;
						}
						else if ("bottom" == s_align)
						{
							align = DT_BOTTOM;
						}
						else if ("center" == s_align)
						{
							align = DT_CENTER;
						}
					}
					pStatic->SetTextFormat(0, align);
				}
			}
			else if ("datetime" == p->name)
			{
				QYDateTimeCtrl *pDateTime = (QYDateTimeCtrl*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_DATETIME, rel1, rel2, offset1, offset2);
				if (nullptr != pDateTime)
				{
					std::string sType = p->getAttribute("type");
					if (sType == "date")
					{
						pDateTime->SetFlag(DATETIME_FLAG_DATE);
					}
					else if (sType == "time")
					{
						pDateTime->SetFlag(DATETIME_FLAG_TIME);
					}
					else if (sType == "date_time")
					{
						pDateTime->SetFlag(DATETIME_FLAG_DATE_TIME);
					}
				}
			}
			else if ("button" == p->name)
			{
				QYButton *pButton = (QYButton*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_BUTTON, rel1, rel2, offset1, offset2);
				if (nullptr != pButton)
				{
					pButton->setWindowText(p->getAttribute("text").c_str());

					int imageState = atoi(p->getAttribute("image_state").c_str());
					if (0 < imageState )
					{
						if (3 < imageState)
						{
							imageState = 3;
						}
					}
					else
					{
						imageState = 3;
					}
					pButton->setImage(p->getAttribute("image").c_str(), imageState);

					pButton->SetBkColor(bkColor);
				}
			}
            else if ("folderedit" == p->name)
            {
                QYFolderEdit *pFolderEdit = (QYFolderEdit*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_FOLDER_EDIT, rel1, rel2, offset1, offset2);
                if (nullptr != pFolderEdit)
                {
                    //nothing to do
                }
            }
            else if ("combobox" == p->name)
            {
                QYComboBox *pComboBox = (QYComboBox*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_COMBOBOX, rel1, rel2, offset1, offset2);
                if (nullptr != pComboBox)
                {
                    auto cbElemList = p->getChildrenList();
                    for (QYXMLTreeNode* pCBElem : *cbElemList)
                    {
                        if ("image" == pCBElem->name)
                        {
                            //add image list
                        }
                        else if ("item" == pCBElem->name)
                        {
                            int align = DT_RIGHT;
                            std::string s_align = pCBElem->getAttribute("align");
                            if (!s_align.empty())
                            {
                                if ("left" == s_align)
                                {
                                    align = TB_TEXT_ALIGN_LEFT;
                                }
                                else if ("right" == s_align)
                                {
                                    align = TB_TEXT_ALIGN_RIGHT;
                                }
                                else if ("top" == s_align)
                                {
                                    align = TB_TEXT_ALIGN_TOP;
                                }
                                else if ("bottom" == s_align)
                                {
                                    align = TB_TEXT_ALIGN_BOTTOM;
                                }
                                else if ("center" == s_align)
                                {
                                    align = TB_TEXT_ALIGN_CENTER;
                                }
                            }
                            pComboBox->addString(pCBElem->getAttribute("id").c_str(), pCBElem->getAttribute("text").c_str());
                        }
                    }
                }
            }
			else if ("listctrl" == p->name)
			{
				QYListCtrl *pListCtrl = (QYListCtrl*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_LISTCTRL, rel1, rel2, offset1, offset2);
				if (nullptr != pListCtrl)
				{
					pListCtrl->SetCheckFlag((p->getAttribute("check_flag") == "true") ? TRUE : FALSE);

					auto listElemList = p->getChildrenList();
					int column = 0;
					for (QYXMLTreeNode* pListElem : *listElemList)
					{
						if ("column" == pListElem->name)
						{                          
							enumSortType sortType = SORT_TYPE_NONE;
							std::string s_sortType = pListElem->getAttribute("sort_type");
							if (!s_sortType.empty())
							{
								if ("number" == s_sortType)
								{
									sortType = SORT_TYPE_NUM;
								}
								else if ("text" == s_sortType)
								{
									sortType = SORT_TYPE_TEXT;
								}
								else if ("date" == s_sortType)
								{
									sortType = SORT_TYPE_DATE;
								}
								else if ("time" == s_sortType)
								{
									sortType = SORT_TYPE_TIME;
								}
								else if ("ip" == s_sortType)
								{
									sortType = SORT_TYPE_IP;
								}
								else if ("float" == s_sortType)
								{
									sortType = SORT_TYPE_FLOAT;
								}
							}


							UINT format = DT_LEFT;
							std::string s_format = pListElem->getAttribute("format");
							if (!s_format.empty())
							{
								if ("left" == s_format)
								{
									format = DT_LEFT;
								}
								else if ("center" == s_format)
								{
									format = DT_CENTER;
								}
								else if ("right" == s_format)
								{
									format = DT_RIGHT;
								}
							}

							pListCtrl->insertColumn(column++,
								pListElem->getAttribute("text").c_str(),
								atoi(pListElem->getAttribute("width").c_str()),
								format,
								sortType);


						}
					}
				}
			}
			else if ("toolbar" == p->name)
			{
				QYToolBar *pToolbar = (QYToolBar*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_TOOLBAR, rel1, rel2, offset1, offset2);
				if (nullptr != pToolbar)
				{
					pToolbar->ShowText((p->getAttribute("show_text") == "true") ? TRUE : FALSE);

					pToolbar->SetBorderWidth(atoi(p->getAttribute("border").c_str()));

					pToolbar->SetBkColor(bkColor);

					auto toolbarElemList = p->getChildrenList();
					for (QYXMLTreeNode* pToolbarElem : *toolbarElemList)
					{
						if ("image" == pToolbarElem->name)
						{
							pToolbar->AddImage(atoi(pToolbarElem->getAttribute("id").c_str()),
								pToolbarElem->getAttribute("source").c_str());
						}
						else if ("item" == pToolbarElem->name)
						{
							int align = DT_RIGHT;
							std::string s_align = pToolbarElem->getAttribute("align");
							if (!s_align.empty())
							{
								if ("left" == s_align)
								{
									align = TB_TEXT_ALIGN_LEFT;
								}
								else if ("right" == s_align)
								{
									align = TB_TEXT_ALIGN_RIGHT;
								}
								else if ("top" == s_align)
								{
									align = TB_TEXT_ALIGN_TOP;
								}
								else if ("bottom" == s_align)
								{
									align = TB_TEXT_ALIGN_BOTTOM;
								}
								else if ("center" == s_align)
								{
									align = TB_TEXT_ALIGN_CENTER;
								}
							}

							std::vector<int> image = stringSplit(pToolbarElem->getAttribute("image"),",");

							pToolbar->insertItem(pToolbarElem->getAttribute("id").c_str(),
								pToolbarElem->getAttribute("text").c_str(),
								&image, align);
						}
					}
				}


			}
			else if ("tree" == p->name)
			{
				QYTreeCtrl *pTree = (QYTreeCtrl*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_TREE, rel1, rel2, offset1, offset2);
				if (nullptr != pTree)
				{
					pTree->SetBorderWidth(atoi(p->getAttribute("border").c_str()));
					pTree->SetAlwaysHilight((p->getAttribute("always_hilight") == "true") ? TRUE : FALSE);
					pTree->SetItemHeight(atoi(p->getAttribute("item_height").c_str()));
					pTree->SetDragFlag((p->getAttribute("drag") == "true") ? TRUE : FALSE);

					auto treeElemList = p->getChildrenList();

					for (QYXMLTreeNode* pTreeElem : *treeElemList)
					{
						if ("image" == pTreeElem->name)
						{
							pTree->AddImage(atoi(pTreeElem->getAttribute("id").c_str()), pTreeElem->getAttribute("source").c_str());
						}

					}
				}
			}
			else if ("edit" == p->name)
			{
				QYEdit *pEdit = (QYEdit*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_EDIT, rel1, rel2, offset1, offset2);
				if (nullptr != pEdit)
				{
					pEdit->SetReadOnly(p->getAttribute("readonly") == "true");
					pEdit->SetPasswordFlag(p->getAttribute("password") == "true");
				}
			}
			else if ("switch" == p->name)
			{
				QYSwitchButton *pSwitch = (QYSwitchButton*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_SWITCH_BUTTON, rel1, rel2, offset1, offset2);
				if (nullptr != pSwitch)
				{
					pSwitch->SetCheck(FALSE);
				}
			}
			else if ("ip" == p->name)
			{
				QYIPCtrl *pIPCtrl = (QYIPCtrl*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_IPADDRESS, rel1, rel2, offset1, offset2);
				if (nullptr != pIPCtrl)
				{
					//pIPCtrl->setValue("192.168.1.1");
				}
			}
			else if ("widget" == p->name)
			{
				QYWidget *pWidgetSub = (QYWidget*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_WIDGET, rel1, rel2, offset1, offset2);
				if (nullptr != pWidgetSub)
				{
					std::string s_color = p->getAttribute("color");
					if (!s_color.empty())
					{
						int r = 0;
						int g = 0;
						int b = 0;
						if (3 == sscanf_s(s_color.c_str(), "%d,%d,%d", &r, &g, &b))
						{
							pWidgetSub->SetBkColor(RGB(r, g, b));
						}

					}

					std::string s_image = p->getAttribute("image");
					if (!s_image.empty())
					{
						pWidgetSub->SetImage(s_image.c_str());
					}

					loadWidget(pWidgetSub, p);
				}
			}
			else if ("widgetlist" == p->name)
			{
				QYWidgetList *pWidgetList = (QYWidgetList*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_WIDGETLIST, rel1, rel2, offset1, offset2);
				if (nullptr != pWidgetList)
				{
					auto widgetElemList = p->getChildrenList();
					for (QYXMLTreeNode* pWidgetElem : *widgetElemList)
					{
						if ("widget" == pWidgetElem->name)
						{
							QYSize size;
							std::string s_size = pWidgetElem->getAttribute("size");
							if (!s_size.empty())
							{
								sscanf_s(s_size.c_str(), "%d,%d", &size.cx, &size.cy);
							}
							QYWidget *pWidgetListWidget = pWidgetList->Append(size);
							if (nullptr != pWidgetListWidget)
							{
								loadWidget(pWidgetListWidget, pWidgetElem);
							}
						}

					}
				}
			}
			else if ("datetable" == p->name)
			{
				QYDateTable *pDateTable = (QYDateTable*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_DATETABLE, rel1, rel2, offset1, offset2);
			}
			else if ("process" == p->name)
			{
				QYProgressCtrl *pProcess = (QYProgressCtrl*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_PROCESS, rel1, rel2, offset1, offset2);
				if (nullptr != pProcess)
				{
					std::string s_color = p->getAttribute("color");
					if (!s_color.empty())
					{
						int r = 0;
						int g = 0;
						int b = 0;
						int r1 = 0;
						int g1 = 0;
						int b1 = 0;
						if (6 == sscanf_s(s_color.c_str(), "%d,%d,%d,%d,%d,%d", &r, &g, &b, &r1, &g1, &b1))
						{
							pProcess->SetBkColor(RGB(r, g, b), RGB(r1, g1, b1));
						}

					}
				}
			}
			else if ("rect" == p->name)
			{
				QYRectBackground *pRect = (QYRectBackground*)pWidget->Append(key.c_str(), QY_CONTROL_TYPE_RECT, rel1, rel2, offset1, offset2);
				if (nullptr != pRect)
				{
					std::string s_color = p->getAttribute("color");
					if (!s_color.empty())
					{
						int r = 0;
						int g = 0;
						int b = 0;
						if (3 == sscanf_s(s_color.c_str(), "%d,%d,%d", &r, &g, &b))
						{
							pRect->SetBkColor(RGB(r, g, b));
						}

					}

					std::string s_image = p->getAttribute("image");
					if (!s_image.empty())
					{
						pRect->SetImage(s_image.c_str());
					}
				}
			}
			else if ("swallow" == p->name)
			{
				pWidget->Append(key.c_str(), QY_CONTROL_TYPE_SWALLOW, rel1, rel2, offset1, offset2);
			}
		}


	}

	return TRUE;
}

BOOL QYWindow::loadLayout(const char *source, const char* group)
{
	FILE *pFile = nullptr;
	std::string layoutPath = QYApp::m_layoutPath + source;
	QYXML xml;
	if (xml.Load(layoutPath.c_str()))
	{
		auto pRoot = xml.GetRoot();
		auto groupElemList = pRoot->getChildrenList();
		for (auto pGroupElem : *groupElemList)
		{
			if ("UI" == pGroupElem->name)
			{
				groupElemList = pGroupElem->getChildrenList();
				for (auto pGroupElem : *groupElemList)
				{
					if ("group" == pGroupElem->name && group == pGroupElem->getAttribute("id"))
					{
						setWindowText(pGroupElem->getAttribute("text").c_str());
						QYSize size;
						std::string s_size = pGroupElem->getAttribute("size");
						if (!s_size.empty())
						{
							sscanf_s(s_size.c_str(), "%d,%d", &size.cx, &size.cy);
						}


						std::string s_color = pGroupElem->getAttribute("color");
						if (!s_color.empty())
						{
							int r = 0;
							int g = 0;
							int b = 0;
							if (3 == sscanf_s(s_color.c_str(), "%d,%d,%d", &r, &g, &b))
							{
								SetBkColor(RGB(r, g, b));
							}

						}

						loadWidget(this, pGroupElem);

						std::string s_type = pGroupElem->getAttribute("type");
						if (s_type.empty() || "child" != s_type)
						{
							SetWindowPos(nullptr, 0, 0, size.cx, size.cy, SWP_NOMOVE);
							CenterWindow();
						}

						return TRUE;
					}
				}
			}

			return FALSE;
		}
		
		
	}

	return FALSE;
}
