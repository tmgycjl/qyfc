#include "QYDialog.h"
#include "QYComboBox.h"
#include "QYDC.h"
#include "QYStatic.h"
#include "QYCheckButton.h"
#include "QYRadioButton.h"
#include "QYComboLBox.h"
#include "QYListBox.h"
#include "QYListCtrl.h"
#include "QYProgressCtrl.h"
#include "QYLinearBitmap.h"
#include "resource.h"
#include "QYWidget.h"

#pragma   comment(lib,"Msimg32.lib")

UINT QYDialog::IDD = 0;

static BOOL CALLBACK RegChildWindow(HWND hWnd, QYWindow *param)
{
	QYDialog *pThis = (QYDialog *)param;

	if (0  < pThis->m_CaptionHeight)
	{
		RECT rect;
		::GetWindowRect(hWnd, &rect);
		RECT rectParent;
		::GetWindowRect(::GetParent(hWnd), &rectParent);
		OffsetRect(&rect, -rectParent.left, -rectParent.top );
		::MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,FALSE);
	}

	QYWindow *pWindow = (QYWindow*)GetWindowLong(hWnd, GWLP_USERDATA);
	if (nullptr != pWindow)
	{
		return TRUE;
	}

	pWindow = pThis->GetDataExchange()->FindCtrl(hWnd,FALSE);

	BOOL bNew = FALSE;
	TCHAR className[255] = { 0 };
	::GetClassName(hWnd, className, 255);
	if (0 == SafeStrcmp(className, CLASS_NAME_BUTTON))
	{
		if (nullptr == pWindow)
		{
			UINT style = GetWindowLong(hWnd, GWL_STYLE);
			if (style&BS_CHECKBOX)
			{
				pWindow = new QYCheckButton;
			}
			else if ((style & 0x0000000f) == BS_AUTORADIOBUTTON)
			{
				pWindow = new QYRadioButton;
			}
			else
			{
				pWindow = new QYButton;
			}
			pWindow->SetFont((HFONT)QYApp::m_pMsDefultFont->m_hObject);
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
			
		}

		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		pWindow->SetHwnd(hWnd);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYButton::ButtonProc);
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_EDIT))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYEdit;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}

		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		pWindow->SetHwnd(hWnd);
		UINT style = GetWindowLong(hWnd, GWL_STYLE);
		if (style&ES_READONLY)
		{
		//	SetWindowLong(hWnd, GWL_STYLE, style);
			((QYEdit*)pWindow)->SetReadOnly(TRUE);
		}
		pWindow->SetFont((HFONT)QYApp::m_pMsDefultFont->m_hObject);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYEdit::EditProc);
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_COMBOBOX))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYComboBox;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}
			SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
			pWindow->SetHwnd(hWnd);
			COMBOBOXINFO ComboBoxInfo;
			ComboBoxInfo.cbSize = sizeof(ComboBoxInfo);
			GetComboBoxInfo(hWnd, &ComboBoxInfo);

			UINT style = GetWindowLong(hWnd, GWL_STYLE);
			if (style&CBS_DROPDOWN)
			{
				((QYComboBox*)pWindow)->SetButtonRect(ComboBoxInfo.rcButton);
				if (nullptr != ComboBoxInfo.hwndItem)
				{
					((QYComboBox*)pWindow)->SetReadOnly(FALSE);

					QYWindow *pEdit = new QYEdit;
					pThis->GetDataExchange()->PushCtrl(ComboBoxInfo.hwndItem, pEdit);
					SetWindowLong(ComboBoxInfo.hwndItem, GWLP_USERDATA, (LONG)pEdit);
					pEdit->SetHwnd(ComboBoxInfo.hwndItem);
					UINT style = GetWindowLong(ComboBoxInfo.hwndItem, GWL_STYLE);
					if (style&ES_READONLY)
					{
						((QYEdit*)pEdit)->SetReadOnly(TRUE);
					}
					((QYEdit*)pEdit)->SetInComboBox();
					pEdit->m_oldWndProc = (WNDPROC)SetWindowLongPtr(ComboBoxInfo.hwndItem, GWLP_WNDPROC, (INT_PTR)QYEdit::EditProc);
				}
				
			}

			pWindow->SetFont((HFONT)QYApp::m_pMsDefultFont->m_hObject);
			pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYComboBox::ComboBoxProc);
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_STATIC))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYStatic;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}
		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		pWindow->SetHwnd(hWnd);
		pWindow->SetFont((HFONT)QYApp::m_pMsDefultFont->m_hObject);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYStatic::StaticProc);
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_LISTBOX))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYListBox;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}
		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		UINT style = GetWindowLong(hWnd, GWL_STYLE);
	
		style &= ~LBS_SORT;
		style &= ~WS_BORDER;
		style |= WS_CLIPCHILDREN;
		SetWindowLong(hWnd, GWL_STYLE, style);
		
		pWindow->SetHwnd(hWnd);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYWindow::WndProc);
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_LISTCTRL))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYListCtrl;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}
		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		pWindow->SetHwnd(hWnd);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYWindow::WndProc);

		QYWindow *pHeaderCtrl = new QYHeaderCtrl;
		if (((QYHeaderCtrl*)pHeaderCtrl)->Create(0,nullptr,nullptr,WS_CHILD,0,pWindow->GetHwnd()))
		{
			((QYListCtrl*)pWindow)->SetHeaderCtrl((QYHeaderCtrl*)pHeaderCtrl);
		}
	}
	else if (0 == SafeStrcmp(className, CLASS_NAME_PROGRESS))
	{
		if (nullptr == pWindow)
		{
			pWindow = new QYProgressCtrl;
			pThis->GetDataExchange()->PushCtrl(hWnd, pWindow);
		}
		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pWindow);
		pWindow->SetHwnd(hWnd);
		UINT style = GetWindowLong(hWnd, GWL_STYLE);
		style &= ~WS_BORDER;
		SetWindowLong(hWnd, GWL_STYLE, style);
		pWindow->m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYWindow::WndProc);
	}

	return TRUE;
}

INT_PTR CALLBACK QYDialog::DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//return FALSE;
	UNREFERENCED_PARAMETER(lParam);
	
	QYDialog *pWindow = (QYDialog*)GetWindowLong(hWnd, GWLP_USERDATA);
	if (NULL != pWindow)
	{
		if ((INT_PTR)TRUE == pWindow->WindowProc(message, wParam, lParam))
		{
			return (INT_PTR)TRUE;
		}
	}

	switch (message)
	{
	case WM_PAINT:return TRUE;
	case WM_NCPAINT:return TRUE;
	case WM_GETDLGCODE:
	{
		return DLGC_WANTCHARS;
	}
	break;
	case WM_CTLCOLORDLG:
	{
		return (LRESULT)QYApp::m_bkBrush;
	}
	break;
	case WM_CTLCOLOREDIT:
	{
		UINT style = GetWindowLong(hWnd, GWL_STYLE);
		if (style&ES_READONLY)
		{
			return (LRESULT)QYApp::m_dlg_edit_readonly_brush;
		}
		else
		{
			return (LRESULT)QYApp::m_dlg_edit_brush;
		}
	}
	break;
	case WM_CTLCOLORSTATIC:{::SetBkMode((HDC)wParam, TRANSPARENT);return (LRESULT)QYApp::m_dlg_static_brush;}break;
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORBTN:{::SetBkColor((HDC)wParam, QY_THEME.BUTTON_BK_COLOR_NORMAL); return (LRESULT)QYApp::m_dlg_edit_brush; }break;
	case WM_ERASEBKGND:return TRUE;break;
	case WM_INITDIALOG:
	{
		QYDialog *pDlg = (QYDialog *)lParam;
		if (NULL != pDlg)
		{
			pDlg->m_hWnd = hWnd;
			if (pDlg->m_bModal)
			{
				SetWindowLong(pDlg->m_hWnd, GWLP_USERDATA, (LONG)pDlg);
			}

			pDlg->OnInitDialog();
		}
	}
	break;
	}

	return (INT_PTR)FALSE;
}

QYDialog::QYDialog() :QYWindow()
{
	QYSizeBox();

	m_pDataExchange = nullptr;
	m_bModal        = false;
	m_FrameHeight = 0;
	m_NCBorderHeight = 0;
	m_NCBorderWidth = 0;
	m_FrameWidth  = 0;
	m_hrestoreRgn = nullptr;
	m_bShowShadow = TRUE;
	m_pShadow     =  nullptr;
	m_hCursor = nullptr;
	m_bZoom = FALSE;
	m_Flag = 0;
	m_bGetText = FALSE;
	m_pointLast.x = 0;
	m_pointLast.y = 0;
	m_ButtonHeight = 20;
	m_bNCLBDOWN = FALSE;
	m_bActive = TRUE;
	m_ButtonDownState = bsNone;
	m_ButtonState = bsNone;
	memset(m_Text, 0, sizeof(TCHAR) * 260);
	m_CaptionHeight = QY_THEME.DEFAULT_CAPTION_HEIGHT;
}

QYDialog::~QYDialog()
{
	if (nullptr != m_pDataExchange)
	{
		m_pDataExchange->Clear();
		delete m_pDataExchange;
	}

	SAFE_DESTROY_WINDOW_PTR(m_pShadow);
}

LRESULT QYDialog::OnCtlColor(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

int QYDialog::ProcNCCalcSize( UINT message, WPARAM wParam, LPARAM lParam)
{
	 m_FrameWidth = 1; /*左右边框的厚度*/
	 m_FrameHeight = 1; /*下边框的厚度*/
	int nTHight = 30; /*标题栏的高度*/
	NCCALCSIZE_PARAMS * p;
	RECT * rc;

	RECT aRect;
	RECT bRect;
	RECT bcRect;

	if (wParam == TRUE)
	{
		p = (NCCALCSIZE_PARAMS *)lParam; /*矩形是B A AC，目标是改成BC B A*/

		CopyRect(&aRect, &p->rgrc[0]);
		CopyRect(&bRect, &p->rgrc[1]);

		bcRect.left = aRect.left + m_FrameWidth;
		bcRect.top = aRect.top + nTHight;
		bcRect.right = aRect.right - m_FrameWidth;
		bcRect.bottom = aRect.bottom - m_FrameHeight;

		CopyRect(&p->rgrc[2], &bcRect);

		wParam = FALSE;
	}
	else
	{
		rc = (RECT *)lParam;
		rc->left = rc->left + m_FrameWidth;
		rc->top = rc->top + nTHight;
		rc->right = rc->right - m_FrameWidth;
		rc->bottom = rc->bottom - m_FrameHeight;
	}
	return GetLastError();
}

LRESULT QYDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//g_ConsoleLog.Print("message:%04x\n", msg.message);
	switch (message)
	{
	case WM_CREATE:
	{
		OnInitDialog();
	}
		break;
	case WM_ACTIVATEAPP:
	{
		//g_ConsoleLog.Print("QYDialog::WM_ACTIVATEAPP\n");
		return TRUE;
	}
	break;
	case WM_GETMINMAXINFO:
	{
		auto cX = GetSystemMetrics(SM_CXSCREEN);
		auto cY = GetSystemMetrics(SM_CYSCREEN);

		MINMAXINFO *maxminInfo = (MINMAXINFO*)lParam;
		maxminInfo->ptMaxSize.x = cX + 2 * m_FrameWidth;
		maxminInfo->ptMaxSize.y = cY + 2 * m_FrameHeight;
		maxminInfo->ptMaxTrackSize.x = cX + 2*m_FrameWidth;
		maxminInfo->ptMaxTrackSize.y = cY + 2*m_FrameHeight;
		maxminInfo->ptMinTrackSize.x = 400;
		maxminInfo->ptMinTrackSize.y = 300;
	}
	break;
    case WM_CLOSE:
	{
#if 0
		EndDialog(IDCANCEL);
#else
        if (0 == wParam)
        {
            wParam = IDCANCEL;
        }
		m_exitCode = wParam;
#endif
		
	} 
	break;
	case WM_NCPAINT:return OnNcPaint();break;
	case WM_PAINT:{return OnPaint();}break;
	case WM_NCACTIVATE:{OnNcActivate(wParam);break;}
	case WM_MOUSELEAVE:{return OnMouseLeave(wParam, lParam);}break;
	case WM_MOUSEMOVE:{OnMouseMove(wParam, QYPoint(LOSHORT(lParam), HISHORT(lParam)));break;}
	case WM_WINDOWPOSCHANGED:{OnMove(LOWORD(lParam), HIWORD(lParam));}break;
	case WM_NCLBUTTONDBLCLK:{Zoom();return TRUE;}break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam));}break;
	case WM_LBUTTONDBLCLK:OnLButtonDBclick(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;
	case WM_LBUTTONDOWN:{OnLButtonDown(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
    case WM_LBUTTONUP:{OnLButtonUp(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_SETCURSOR:{return OnSetCursor(wParam, lParam);}break;
	case WM_SHOWWINDOW:{(nullptr != m_pShadow && nullptr != m_pShadow->GetHwnd()) ? m_pShadow->Show(wParam) : 0;}break;
	default:break;
	}

	return 0L;
}

void QYDialog::DoDataExchange(QYDataExchange* pDX)
{
	
}

void QYDialog::Control(QYDataExchange *pDX, int nIDC, QYWindow& rControl)
{
	auto *pControlWnd = &rControl;
	auto hWnd = ::GetDlgItem(GetHwnd(), nIDC);
	if (nullptr != hWnd)
	{
		m_pDataExchange->PushCtrl(hWnd, pControlWnd,FALSE);
	}
}

#include "QYDC.h"
#include "QYComboLBox.h"


BOOL QYDialog::OnInitDialog()
{
	DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
	SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));

	m_pDataExchange = new QYDataExchange;

	DoDataExchange(m_pDataExchange);

	SetFont((HFONT)(QYApp::m_pMsDefultFont->m_hObject));

	SetBkColor(QY_THEME.CLIENT_COLOR);

	UINT style = GetWindowLong(m_hWnd, GWL_STYLE);

	if (style&WS_CHILD)
	{
		m_bZoom = FALSE;

		style |= WS_CLIPCHILDREN;
		//style &= ~WS_SIZEBOX;
		//style &= ~WS_CAPTION;
		SetWindowLong(GetHwnd(), GWL_STYLE, style);

		m_NCBorderHeight = 0;
		m_NCBorderWidth = 0;
		m_CaptionHeight = 0;
		m_FrameWidth = 0;
		m_FrameHeight =0;
		m_MinBoxSize = m_MaxBoxSize = m_CloseBoxSize = 0;

		m_disableMaxBox = m_disableMinBox = TRUE;
	}
	else if (style&WS_POPUP)
	{
		QYRect WinRC;
		GetWindowRect(WinRC);

		m_NCBorderHeight = GetSystemMetrics(SM_CYBORDER);
		m_NCBorderWidth = GetSystemMetrics(SM_CXBORDER);
		m_CaptionHeight = (0 == m_CaptionHeight) ? QY_THEME.DIALOG_CAPTION_HEIGHT : m_CaptionHeight;
		m_CloseBoxSize.cy = m_CaptionHeight;
		m_CloseBoxSize.cx = m_CaptionHeight * 3 / 2;
		m_MinBoxSize.cy = m_CaptionHeight;
		m_MinBoxSize.cx = m_CaptionHeight * 3 / 2;
		m_MaxBoxSize.cy = m_CaptionHeight;
		m_MaxBoxSize.cx = m_CaptionHeight * 3 / 2;


		MoveWindow(WinRC.left, WinRC.top, WinRC.Width(), WinRC.Height() + m_CaptionHeight);

		m_bZoom = IsZoomed(GetHwnd());

		style &= ~WS_SYSMENU;
		//style &= ~WS_SIZEBOX;
		style |= WS_CLIPCHILDREN;
		//style &= ~WS_CAPTION;
		SetWindowLong(GetHwnd(), GWL_STYLE, style);

		//m_disableMaxBox = !(style&WS_MAXIMIZEBOX);
		//m_disableMinBox = !(style&WS_MINIMIZEBOX);
		//m_bSizeBox = (style&WS_SIZEBOX);

		m_nSizeBoxWidth = m_FrameWidth;
#if 0
		m_pCaptionBkImage = new QYPicture;
		m_pCaptionBkImage->Load(IDP_CAPTION_BK,QY_IMAGE_PNG);

		m_CaptionHeight = m_pCaptionBkImage->GetHeight();
		if (DIALOG_CAPTION_HEIGHT<m_CaptionHeight)
		{
			m_CaptionHeight = DIALOG_CAPTION_HEIGHT;
		}
#endif
#if 0


		GetWindowRect(WinRC);
		QYRect clientRect;
		GetClientRect(clientRect);
		

		ClientToScreen(&clientRect);
		clientRect.OffsetRect(-WinRC.left, -WinRC.top);

		HRGN hRgn = CreateRectRgn(clientRect.left,
			clientRect.top,
			clientRect.right,
			clientRect.bottom);
		//CombineRgn(hRgn, hSubRgn, hRgn, RGN_XOR);
		::SetWindowRgn(m_hWnd, hRgn, TRUE);

		DeleteObject(hRgn);

		InitSysBox(clientRect);
#endif
		if (m_bShowShadow)
		{
			m_pShadow = new QYWindowShadow;
			m_pShadow->Create(0, this);
			//m_pShadow->Show(SW_SHOW);
		}
		
		//OnSize(0, 0, 0);
		//OnMove(0, 0);
		
	}
	//SetActiveWindow(m_hWnd);
	
#if 0
	
	SetWindowLong(this->GetHwnd(), GWL_EXSTYLE, \
		GetWindowLong(this->GetHwnd(), GWL_EXSTYLE) ^ 0x80000);

	SetLayeredWindowAttributes(this->GetHwnd(), QY_THEME.CLIENT_COLOR, 122, 1);
#endif

	//EnumChildWindows(GetHwnd(), (WNDENUMPROC)RegChildWindow, (LPARAM)this);


	//CenterWindow();

	//UpdateWindow(m_hWnd);

	return TRUE;
}


BOOL QYDialog::Show(int nCmdShow)
{
	(nullptr == m_pShadow) ? 0 : m_pShadow->Show(nCmdShow);
	return QYWindow::Show(nCmdShow);
}

BOOL QYDialog::EndDialog(UINT end)
{
	(nullptr == m_pShadow) ? 0 : m_pShadow->Show(SW_HIDE);
#if 0
	return ::EndDialog(GetHwnd(), end);
#else
	
	PostMessage(WM_CLOSE,end);
	return TRUE;
#endif
}

bool QYDialog::Create(UINT resID, QYWindow *pParentWnd)
{
	if (nullptr != m_hWnd)
	{
		return false;
	}
#if 0
	DialogBox(QYApp::m_hInstance, MAKEINTRESOURCE(resID), pParentWnd->GetHwnd(), (DLGPROC)DialogProc);
#else
	if (nullptr == (m_hWnd = CreateDialogParam(QYApp::m_hInstance, 
		MAKEINTRESOURCE(resID), 
		(nullptr == pParentWnd) ? nullptr: pParentWnd->GetHwnd(), 
		(DLGPROC)DialogProc, 
		(LPARAM)this)))
	{
		return false;
	}

	SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)this);
#endif

	return true;
}

INT_PTR QYDialog::DoModal(UINT resID, QYWindow *pParentWnd)
{
	//DialogBox(QYApp::m_hInstance, MAKEINTRESOURCE(resID), (nullptr == pParentWnd) ? nullptr : pParentWnd->GetHwnd(), DialogProc);
	m_bModal = true;
#if 0
	if (nullptr == (m_hWnd = CreateDialogParam(QYApp::m_hInstance,
		MAKEINTRESOURCE(resID),
		(nullptr == pParentWnd) ? nullptr : pParentWnd->GetHwnd(),
		(DLGPROC)DialogProc,
		(LPARAM)this)))
	{
		return false;
	}

	MSG msg;

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{ 
		if (!TranslateAccelerator(msg.hwnd, nullptr, &msg)) 
		{
			if (IsDialogMessage(msg.hwnd, &msg)) 
			{
				
				::SendMessage(m_hWnd, msg.message, msg.wParam, msg.lParam);
			}
			else 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
	}
}

#else
	return DialogBoxParam(QYApp::m_hInstance,
		MAKEINTRESOURCE(resID),
		(nullptr == pParentWnd) ? nullptr : pParentWnd->GetHwnd(),
		(DLGPROC)DialogProc,
		(LPARAM)this);
#endif
	
	
}

INT_PTR QYDialog::DoModal(QYWindow *pParentWnd)
{
#if 0
    return DoModal(IDD_DIALOG_MODAL,pParentWnd);
#else
    m_bModal = true;

    HWND hParentWnd = nullptr;
    if (nullptr != pParentWnd)
    {
        hParentWnd = pParentWnd->GetHwnd();
    }

    if (!QYWindow::Create(0, L"QYDialog", nullptr, WS_POPUP  , 0, 0, 0, 0, hParentWnd))
    {
        return 0;
    }

    assert(::IsWindow(m_hWnd));

    Show(SW_SHOWNORMAL);

	if (nullptr != hParentWnd)
	{
		::EnableWindow(hParentWnd, FALSE);
	}
   
    MSG msg = { 0 };

    m_exitCode = 0;
    while (0 == m_exitCode &&::GetMessage(&msg, nullptr, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

	if (nullptr != hParentWnd)
	{
		::EnableWindow(hParentWnd, TRUE);
		::SetFocus(hParentWnd);
	}
   
    if (msg.message == WM_QUIT)
    {
        ::PostQuitMessage(msg.wParam);
    }

    return m_exitCode;
#endif
	
}

LRESULT QYDialog::OnExitSize( WPARAM wparam, LPARAM lparam)
{
	return 0;
}

int  QYDialog::GetFrameWidth()
{
	return m_FrameWidth;
}
int  QYDialog::GetFrameHeight()
{
	return m_FrameHeight;
}

int   QYDialog::GetBorderWidth()
{
	return m_NCBorderWidth;
}

int QYDialog::GetBorderHeight()
{
	return m_NCBorderHeight;
}

BOOL QYDialog::OnNcActivate( BOOL bActive)
{
	g_ConsoleLog.Print("ncactive:%d\n",bActive);
	m_bActive = bActive;
	//OnPaint();
	Invalidate();

	return TRUE;
}

LRESULT QYDialog::OnMouseLeave(WPARAM, LPARAM)
{
	m_bHover = FALSE;

	if (bsNone != m_ButtonState)
	{
		m_ButtonState = bsNone;
		InvalidateRect(&m_rectClose);
		InvalidateRect(&m_rectMin);
		InvalidateRect(&m_rectMax);
	}

	m_hCursor = LoadCursor(NULL, IDC_ARROW);
	PostMessage(WM_SETCURSOR, 0, 0);

	return TRUE;
}

LRESULT QYDialog::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 10;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bHover = TRUE;
		InvalidateRect(&m_rectClose);
		InvalidateRect(&m_rectMin);
		InvalidateRect(&m_rectMax);
	}

	QYRect tempIni, tempMin, tempMax, tempClose, ClientRect;

	QYRect clientRect;
	GetWindowRect(clientRect);

	QYPoint pt;
	pt.x = point.x - ClientRect.left;
	pt.y = point.y - ClientRect.top;

	tempMin = m_rectMin;
	tempMax = m_rectMax;
	tempClose = m_rectClose;
#if 1
	if (tempMin.PtInRect(pt) && !m_disableMinBox)
	{
		if (m_ButtonState != bsMin)
		{
			m_ButtonState = bsMin;
			InvalidateRect(&m_rectClose);
			InvalidateRect(&m_rectMin);
			InvalidateRect(&m_rectMax);
		}
	}
	else if (tempMax.PtInRect(pt) && !m_disableMaxBox)
	{
		if (m_ButtonState != bsMax && m_ButtonState != bsRes)
		{
			if (m_bZoom)
			{
				m_ButtonState = bsMax;
			}
			else
			{
				m_ButtonState = bsRes;
			}

			InvalidateRect(&m_rectClose);
			InvalidateRect(&m_rectMin);
			InvalidateRect(&m_rectMax);
		}
	}
	else if (tempClose.PtInRect(pt))
	{
		if (m_ButtonState != bsClose)
		{
			m_ButtonState = bsClose;
			InvalidateRect(&m_rectClose);
			InvalidateRect(&m_rectMin);
			InvalidateRect(&m_rectMax);
		}
	}
	else
	{
		if (bsNone != m_ButtonState)
		{
			//g_ConselLog.Print(0,"ncmousemove\n");
			m_ButtonState = bsNone;
			InvalidateRect(&m_rectClose);
			InvalidateRect(&m_rectMin);
			InvalidateRect(&m_rectMax);
		}
	}
#endif

	if (m_bSizeBox)
	{
		Resize(this,point);			
	}

	return TRUE;
}


void QYDialog::OnLButtonDBclick(UINT nHitTest, QYPoint point)
{
	if ((!m_rectClose.PtInRect(point) && !m_rectMax.PtInRect(point) && !m_rectMin.PtInRect(point))
		|| (m_disableMaxBox && m_rectMax.PtInRect(point))
		|| (m_disableMinBox && m_rectMin.PtInRect(point)))
	{
		QYRect rc;
		getClientRectFromWindowRect(rc);
		rc.top = m_bSizeBox ? m_nSizeBoxWidth : 0;
		rc.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT;
		if (rc.PtInRect(point))
		{
			Zoom();
		}
	}
}

void QYDialog::OnLButtonDown(UINT nHitTest, QYPoint point)
{
	  if (m_bSizeBox)
	  {
		 // if (!m_bNCLBDOWN)
		  {
			  m_bNCLBDOWN = TRUE;
			  StartResize(this,point);
		  }
	  }

	  if ((!m_rectClose.PtInRect(point) && !m_rectMax.PtInRect(point) && !m_rectMin.PtInRect(point))
		  || (m_disableMaxBox && m_rectMax.PtInRect(point))
		  || (m_disableMinBox && m_rectMin.PtInRect(point)) )
	  {
		  QYRect rc;
		  getClientRectFromWindowRect(rc);
		  //ClientToScreen(hWnd,rc);
		  rc.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT;

		  rc.top = m_bSizeBox ? m_nSizeBoxWidth :0;
		  if (rc.PtInRect(point) && !m_bZoom && !m_bDownBorder)
		  {
			  PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
		  }
	  }
	  m_ButtonDownState = m_ButtonState;
	  InvalidateRect(&m_rectClose);
	  InvalidateRect(&m_rectMax);
	  InvalidateRect(&m_rectMin);
}

void QYDialog::OnLButtonUp(UINT nHitTest, QYPoint point)
{
	if (m_bNCLBDOWN)
	{
		m_bNCLBDOWN = FALSE;
		BOOL downBorder = m_bDownBorder;
		EndResize();
		if (downBorder)
		{
			return;
		}
	}

	switch (m_ButtonState)
	{
	case bsClose: 
	{
		nullptr != m_pShadow ? m_pShadow->Show(SW_HIDE) : 0;
		
		SendMessage( WM_CLOSE, 0, 0);
		//return;
	}
	break;
	case bsMin:
	{
		nullptr != m_pShadow ? m_pShadow->Show(SW_HIDE) : 0;
		
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE,0);
	}
	break;
	case bsMax:
	case bsRes:
	{
		Zoom();
	}
	break;
	default:
		break;;
	}

	m_ButtonDownState = bsNone;
	m_ButtonState = bsNone;

	//OnPaint();
}

void QYDialog::Zoom()
{
	
#if 0
	UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
	if (!(style&WS_SIZEBOX))
	{
		return;
	}
	if (IsZoomed(GetHwnd()))
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_SHOW);
			
		}

		m_bZoom = FALSE;
		Show(SW_RESTORE);
	}
	else
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_HIDE);
		}

		m_bZoom = TRUE;
		Show(SW_MAXIMIZE);
	}
#else
	UINT  style = GetWindowLong(GetHwnd(), GWL_STYLE);
	if (m_disableMaxBox)
	{
		return;
	}

	if (!m_bZoom)
	{
		auto cX = GetSystemMetrics(SM_CXFULLSCREEN);
		auto cY = GetSystemMetrics(SM_CYSCREEN);

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, FALSE);

		QYRect rect;
		rect = rt;
		GetWindowRect(m_restoreRect);

		//(nullptr != m_pShadow) ? m_pShadow->Show(SW_HIDE) : 0;
#if 0

		SetWindowPos(nullptr, rect.left - m_FrameWidth,
			rect.top - m_FrameHeight,
			rect.Width() + 2 * (m_FrameWidth + m_NCBorderWidth),
			rect.Height() + 2 * (m_FrameWidth + m_NCBorderWidth));
#else

		SetWindowPos(HWND_NOTOPMOST, rect.left, rect.top, rect.Width(), rect.Height());
#endif

	

		m_bZoom = TRUE;
		m_bSizeBox = FALSE;
	}
	else
	{
		QYRect winRect;

		m_bSizeBox = TRUE;

		SetWindowPos(nullptr, m_restoreRect);

		//(nullptr != m_pShadow) ? m_pShadow->Show(SW_SHOW) : 0;

		m_bZoom = FALSE;
		
	}
#endif
	
	Invalidate();
}

void QYDialog::InitSysBox(QYRect &WinRC)
{
	QYRect rectCaption;

	rectCaption.left = m_NCBorderWidth;
	rectCaption.top = m_NCBorderHeight;
	rectCaption.right = WinRC.Width() - m_NCBorderWidth;
	rectCaption.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT + m_NCBorderWidth;

	m_rectClose.top = rectCaption.top;
	m_rectClose.right = rectCaption.right  ;
	m_rectClose.left = m_rectClose.right - m_CloseBoxSize.cx;
	m_rectClose.bottom = rectCaption.bottom;

	m_rectMax.left = m_rectClose.left - m_MaxBoxSize.cx;
	m_rectMax.top = m_rectClose.top;
	m_rectMax.right = m_rectClose.left;
	m_rectMax.bottom = m_rectClose.bottom;

	m_rectMin.left = m_rectMax.left - m_MinBoxSize.cx;
	m_rectMin.top = m_rectMax.top;
	m_rectMin.right = m_rectMax.left;
	m_rectMin.bottom = m_rectMax.bottom;
}

BOOL QYDialog::OnMove(int cx, int cy)
{
	if (nullptr != m_pShadow)
	{
		QYRect clientRect;
		getClientRectFromWindowRect(clientRect);

		ClientToScreen(&clientRect);
		m_pShadow->OnMove(clientRect);
	}

	return TRUE;
}

BOOL QYDialog::OnSize(UINT nType, int cx, int cy)
{
	if (SIZE_MINIMIZED == nType)
	{
		return TRUE;
	}

	QYRect WinRC, FactRC;
	GetWindowRect(WinRC);

	QYRect clientRect(0,0,WinRC.Width(),WinRC.Height());

	//GetClientRect(clientRect);

	QYRect realClientRect = clientRect;

	InitSysBox(clientRect);
	
	ClientToScreen(&clientRect);
	nullptr != m_pShadow ? m_pShadow->OnMove(WinRC) : 0;

// 	clientRect.OffsetRect(-WinRC.left, -WinRC.top);
// 	HRGN hRgn = CreateRectRgn(clientRect.left , clientRect.top , clientRect.right, clientRect.bottom);
// 	::SetWindowRgn(m_hWnd, hRgn, TRUE);
// 	DeleteObject(hRgn);

	GetClientRect(realClientRect);

	UpdateLayout(realClientRect);

	QYWindow::onSize(nType, cx, cy);

 	OnPaint();

	return FALSE;
}

void QYDialog::DrawCloseRect(QYDC *pDC, QYRect &rect, Button_state state )
{
#if 1
	QYBrush brush ;
	QYPen  hSysBtnPen;

	if (button_state_hot == state)
	{
		brush.CreateSolidBrush(QY_THEME.CLOSE_BOX_BK_COLOR_HOT);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_down == state)
	{
		brush.CreateSolidBrush(QY_THEME.CLOSE_BOX_BK_COLOR_DOWN);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_disable == state)
	{
		//brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
		//brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, !m_bActive ? QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE : QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE);
	}

	HPEN oldpen = (HPEN)pDC->SelectObject(&hSysBtnPen);
	pDC->FillRect(rect, brush);
	

	QYRect rectReal;
	int sideLen = min(rect.Height() * 1 / 2, (rect.Width() - 2) * 1 / 2);
	rectReal.top = (rect.Height() - sideLen) / 2 + rect.top;
	rectReal.left = (rect.Width() - sideLen) / 2 + rect.left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen;

	QYPoint pt;

	pDC->MoveToEx(rectReal.left + 1, rectReal.top + 1, &pt);
	pDC->LineTo(rectReal.left + sideLen, rectReal.top + sideLen);
	pDC->MoveToEx(rectReal.left + 1, rectReal.top + sideLen - 1, &pt);
	pDC->LineTo( rectReal.left + sideLen, rectReal.top);

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();
	brush.DeleteObject();
#endif
}

void QYDialog::DrawMaxRect(QYDC *pDC, QYRect &rect, int zoom, Button_state state )
{
#if 1
	QYBrush brush;
	QYPen  hSysBtnPen;

	if (button_state_hot == state)
	{
		brush.CreateSolidBrush(QY_THEME.SYS_BOX_BK_COLOR_HOT);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_down == state)
	{
		brush.CreateSolidBrush(QY_THEME.SYS_BOX_BK_COLOR_DOWN);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_disable == state)
	{
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, !m_bActive ? QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE : QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE);
	}

	HPEN oldpen = (HPEN)pDC->SelectObject(&hSysBtnPen);

	pDC->FillRect(rect, brush);

	QYRect rectReal;

	if (m_bZoom)
	{
		int sideLen = min((rect.Height()) * 2 / 4, (rect.Width() - 2) * 2 / 4);
		rectReal.top = (rect.Height() - sideLen) / 2 + rect.top + sideLen / 8;
		rectReal.left = (rect.Width() - sideLen) / 2 + rect.left;
		rectReal.bottom = rectReal.top + sideLen - sideLen / 4;
		rectReal.right = rectReal.left + sideLen;
		pDC->Rectangle(rectReal.left + 1, rectReal.top - 1, rectReal.right + 1, rectReal.bottom - 1);

		pDC->Rectangle( rectReal.left - 2, rectReal.top + 2, rectReal.right - 2, rectReal.bottom + 2);
	}
	else
	{
		int sideLen = min(rect.Height() * 2 / 4, (rect.Width() - 2) * 2 / 4);
		rectReal.top = (rect.Height() - sideLen) / 2 + rect.top + sideLen / 8;
		rectReal.left = (rect.Width() - sideLen) / 2 + rect.left;
		rectReal.bottom = rectReal.top + sideLen - sideLen / 4;
		rectReal.right = rectReal.left + sideLen;
		pDC->Rectangle(rectReal.left, rectReal.top, rectReal.right, rectReal.bottom);
	}

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();
	brush.DeleteObject();
#endif
}

void QYDialog::DrawMinRect(QYDC *pDC, QYRect &rect, Button_state state )
{
#if 1
	QYBrush brush;
	QYPen  hSysBtnPen;

	if (button_state_hot == state)
	{
		brush.CreateSolidBrush(QY_THEME.SYS_BOX_BK_COLOR_HOT);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_down == state)
	{
		brush.CreateSolidBrush(QY_THEME.SYS_BOX_BK_COLOR_DOWN);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_HOT);
	}
	else if (button_state_disable == state)
	{
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
		brush.GetBrush(NULL_BRUSH);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, !m_bActive ? QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE : QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE);
	}

	HPEN oldpen = (HPEN)pDC->SelectObject(&hSysBtnPen);

	pDC->FillRect(rect, brush);

	QYPoint pt;

	int sideLen = min(rect.Height() * 2 / 4, (rect.Width() - 2) * 2 / 4);
	pDC->MoveToEx(rect.left + (rect.Width() - sideLen) / 2,
		rect.top + rect.Height() * 1 / 2,
		&pt);
	pDC->LineTo(rect.right - (rect.Width() - sideLen) / 2,rect.top + rect.Height() / 2);

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();
	brush.DeleteObject();
#endif
}

void QYDialog::PaintDC(QYDC *pDC, QYRect WinRC)
{
	QYBrush hClientBrush(QY_THEME.CLIENT_COLOR);
	QYBrush NullBrush(GetStockObject(NULL_BRUSH));
	auto oldbrush = (HBRUSH)pDC->SelectObject(&NullBrush);

	QYPen BorderPen(PS_SOLID, m_NCBorderWidth, m_bActive ? QY_THEME.DLG_BORDER_PEN_COLOR_ACTIVE : QY_THEME.DLG_BORDER_PEN_COLOR_ACTIVE);
	auto oldpen = (HPEN)pDC->SelectObject(&BorderPen);

	auto borderWidth = 0;

	if (!m_bShowShadow)
	{
		pDC->Rectangle(0, 0, WinRC.Width(), WinRC.Height());
		borderWidth = m_NCBorderWidth;
	}

	QYRect rectCaption;
	rectCaption.left = borderWidth;
	rectCaption.top = borderWidth;
	rectCaption.right = WinRC.right - borderWidth;
	rectCaption.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT +borderWidth;

	QYBrush CaptionBrush(QY_THEME.DLG_CAPTION_COLOR);
	//pDC->FillRect(rectCaption, CaptionBrush);

	QYLinearBitmap linearBitmap(LINEAR_RGB_ID_CAPTION, rectCaption.Width(), 0,TRUE);
	linearBitmap.Draw(pDC, &rectCaption);

	Button_state state2 = button_state_normal;
	if (m_ButtonState == m_ButtonDownState && m_ButtonState == bsClose)
	{
		state2 = button_state_down;
	}
	else
	{
		state2 = (bsClose == m_ButtonState) ? button_state_hot : button_state_normal;
	}

	DrawCloseRect(pDC, m_rectClose, state2);

	if (!m_disableMaxBox || !m_disableMinBox)
	{
		if (m_disableMaxBox)
		{
			state2 = button_state_disable;
		}
		else
		{
			if (m_ButtonState == m_ButtonDownState &&  m_ButtonState == bsMax)
			{
				state2 = button_state_down;
			}
			else
			{
				if (bsMax == m_ButtonState || bsRes == m_ButtonState)
				{
					state2 = button_state_hot;
				}
				else
				{
					state2 = button_state_normal;
				}
			}

		}

		DrawMaxRect(pDC, m_rectMax, !m_bZoom, state2);

		if (m_disableMinBox)
		{
			state2 = button_state_disable;
		}
		else
		{
			if (m_ButtonState == m_ButtonDownState&& m_ButtonState == bsMin)
			{
				state2 = button_state_down;
			}
			else
			{
				state2 = (bsMin == m_ButtonState) ? button_state_hot : button_state_normal;
			}
		}
		DrawMinRect(pDC, m_rectMin, state2);
	}

	QYRect rectText = rectCaption;
	if (nullptr != m_hIcon)
	{
		ICONINFO icoInfo;
		GetIconInfo(m_hIcon, &icoInfo);
		int leftIcon = rectCaption.left + (rectCaption.Height() - icoInfo.xHotspot) / 2;
		int topIcon = rectCaption.top + (rectCaption.Height() - icoInfo.xHotspot) / 2;

		pDC->DrawIconEx(leftIcon,topIcon,m_hIcon,icoInfo.xHotspot,icoInfo.yHotspot);

		::DeleteObject(icoInfo.hbmColor);
		::DeleteObject(icoInfo.hbmMask);

		rectText.left = leftIcon + icoInfo.xHotspot ;
	}

	HFONT oldFont = (HFONT)pDC->SelectObject(m_pFont);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_bActive ? QY_THEME.CAPTION_TEXT_COLOR_ACTIVE : QY_THEME.CAPTION_TEXT_COLOR_ACTIVE);

	rectText.left += 10;
	pDC->DrawText(m_strWindowText, m_strWindowText.Length(), &rectText, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);

	CaptionBrush.DeleteObject();
	BorderPen.DeleteObject();
	pDC->SelectObject(oldFont);

	WinRC.top = rectCaption.bottom;
	WinRC.left += borderWidth;
	WinRC.right -= borderWidth;
	WinRC.bottom -= borderWidth;

	pDC->FillRect(WinRC, hClientBrush);

	hClientBrush.DeleteObject();
}

BOOL QYDialog::OnNcPaint()
{
	return TRUE;
	QYPaintDC dc(this);

	QYRect WinRC;
	getClientRectFromWindowRect(WinRC);

	QYMemDC memDC(&dc, WinRC);

	PaintDC(&memDC, WinRC);

	dc.BitBlt(0, 0, WinRC.Width(), WinRC.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();

	return TRUE;
}

BOOL QYDialog::OnPaint()
{
	QYPaintDC dc(this);
	
	QYRect WinRC;
	getClientRectFromWindowRect(WinRC);

	QYMemDC memDC(&dc, WinRC);

	PaintDC(&memDC, WinRC);

	dc.BitBlt(0, 0, WinRC.Width(), WinRC.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();

	return TRUE;
}

LRESULT QYDialog::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	if (nullptr != m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return FALSE;
}

QYWindow* QYDialog::GetDlgItem(UINT resID)
{
	HWND hWnd = ::GetDlgItem(m_hWnd, resID);
	if (nullptr == hWnd)
	{
		return nullptr;
	}

	assert(nullptr != m_pDataExchange);
	QYWindow *pWnd = m_pDataExchange->FindCtrl(hWnd);
	if (nullptr == pWnd)
	{
		pWnd = m_pDataExchange->FindCtrl(hWnd,FALSE);
	}
	return pWnd;
}

void QYDialog::getClientRectFromWindowRect(QYRect &rect)
{
	QYRect WinRC;
	GetWindowRect(WinRC);

	rect = QYRect(0, 0, WinRC.Width(), WinRC.Height());
}