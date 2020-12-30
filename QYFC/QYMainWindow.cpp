#include "QYMainWindow.h"
#include "QYDC.h"
#include "QYLinearBitmap.h"


QYMainWindow::QYMainWindow(void)
	:QYWindow()
{
	QYSizeBox();

	m_bModal = false;
	m_CaptionHeight = 0;
	m_FrameHeight = 0;
	m_NCBorderHeight = 0;
	m_NCBorderWidth = 0;
	m_FrameWidth = 0;
	m_hrestoreRgn = nullptr;
	m_pShadow = nullptr;
	m_hCursor = nullptr;
	m_bNCLBDOWN = FALSE;
	m_rectRestore = QYRect(0);
	m_bZoom = FALSE;
	m_bNCHover = FALSE;
}


QYMainWindow::~QYMainWindow(void)
{
	m_bZoom = FALSE;
	m_Flag = 0;
	m_bGetText = FALSE;
	m_pointLast.x = 0;
	m_pointLast.y = 0;
	m_ButtonHeight = 20;
	m_CaptionHeight = 30;
	m_bNCLBDOWN = FALSE;
	m_bActive = TRUE;
	m_ButtonDownState = bsNone;
	m_ButtonState = bsNone;
	memset(m_Text, 0, sizeof(TCHAR) * MAX_PATH);

	SAFE_DESTROY_WINDOW_PTR(m_pShadow);
}

BOOL QYMainWindow::Create()
{
   // return QYWindow::Create(WS_EX_WINDOWEDGE, L"QYMainWindow", nullptr,  WS_CAPTION |  WS_MAXIMIZEBOX | WS_CLIPCHILDREN);
	UINT style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	style &= ~WS_VISIBLE;
	return QYWindow::Create(0, L"QYMainWindow", nullptr, style);
}

LRESULT QYMainWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//g_ConsoleLog.Print(" QYMainWindow::WindowProc message %04x\n", message);
	
	switch (message)
	{
	case WM_CREATE:
	{
		OnCreate((CREATESTRUCT*)lParam);
	}
	break;
	case WM_GETMINMAXINFO:
	{
		auto cX = GetSystemMetrics(SM_CXSCREEN);
		auto cY = GetSystemMetrics(SM_CYSCREEN);

		MINMAXINFO *maxminInfo = (MINMAXINFO*)lParam;
		maxminInfo->ptMaxSize.x = cX + 2 * m_FrameWidth;
		maxminInfo->ptMaxSize.y = cY + 2 * m_FrameHeight;
		maxminInfo->ptMaxTrackSize.x = cX + 2 * m_FrameWidth;
		maxminInfo->ptMaxTrackSize.y = cY + 2 * m_FrameHeight;
		maxminInfo->ptMinTrackSize.x = 800;
		maxminInfo->ptMinTrackSize.y = 450;
	}
	break;
	case WM_ERASEBKGND:
	{
		if (nullptr != m_pShadow)
		{
			QYRect rt;
			GetWindowRect(rt);
			m_pShadow->OnMove(rt);
		}
		return TRUE;
	}
	break;
	case WM_NCDESTROY:break;
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_RESTORE)
		{
			if (nullptr != m_pShadow)
			{
				m_pShadow->Show(SW_SHOW);
			}
		}
		else if (wParam == SC_MAXIMIZE)
		{
			if (nullptr != m_pShadow)
			{
				m_pShadow->Show(SW_HIDE);
			}
		}
		POINT pt;
		GetCursorPos(&pt);
		::ScreenToClient(GetHwnd(), &pt);
		pt.x += m_FrameWidth;
		pt.y += m_FrameHeight;
		QYRect rt;
		GetWindowRect(rt);
		QYPoint pt2(&pt);
		pt2.y += m_CaptionHeight;
		if ((1 && m_rectClose.PtInRect(pt2))
			|| (!m_disableMaxBox && m_rectMax.PtInRect(pt2))
			|| (!m_disableMinBox &&m_rectMin.PtInRect(pt2)))
		{
			return TRUE;
		}
		//return m_bZoom;
	}
	break;
	case WM_CLOSE:PostQuitMessage(0);break;
	case WM_NCPAINT:return OnNcPaint();  break;
	case WM_NCACTIVATE:
	{
		OnNcActivate(wParam);
		break;
		
		if (WA_INACTIVE < wParam)
		{
			return TRUE;
		}
		if (!::IsIconic(m_hWnd))
		{
			return (wParam == 0) ? TRUE : FALSE;
		}
		break;
		lParam = -1;
		int ret = DefWindowProc(m_hWnd, message, wParam, lParam);
		OnNcActivate(wParam);
		if (WA_INACTIVE < wParam)
		{
			//return TRUE;
		}
		//g_ConsoleLog.Print("WM_NCACTIVATE ,hwnd:%d,wParam:%d,lParam:%d,ret:%d\n", m_hWnd, wParam, lParam, ret);
		return ret;
		break;
	}
	case WM_NCHITTEST:
	{
		//return HTCLIENT;
	}
	break;
	case WM_NCLBUTTONUP:return OnNcLButtonUp(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam))); break;
	//case WM_MOUSEMOVE:OnMouseMove(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;
	case WM_NCMOUSEMOVE:return OnNcMouseMove(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;
	case WM_NCMOUSELEAVE:OnNcMouseLeave(wParam, lParam); break;
	case WM_NCLBUTTONDOWN:OnNcLButtonDown(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;
	case WM_WINDOWPOSCHANGED:{OnMove(LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_NCLBUTTONDBLCLK:
	{
		//Zoom(!m_bZoom);

		//OnNcPaint();
		//return TRUE;
	}
	break;
	//case WM_ENTERSIZEMOVE:g_ConsoleLog.Print("IsZoomed %d\n", IsZoomed(m_hWnd)); break;
	case WM_SIZE:case WM_EXITSIZEMOVE:{return OnSize(wParam, LOWORD(lParam), HIWORD(lParam));  }break;
	case WM_SETCURSOR:{return OnSetCursor(wParam, lParam); }break;
	case WM_SHOWWINDOW:
	{
		if (nullptr != m_pShadow &&nullptr != m_pShadow->GetHwnd())
		{
			if (!wParam)
			{
				m_pShadow->Show(SW_HIDE);
			}
			else
			{
				m_pShadow->Show(SW_SHOWNA);
			}
			

		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		OnLButtonDown(wParam, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		PostMessage(WM_NCACTIVATE, TRUE);
	}
	break;
	case WM_TIMER:OnTimer(wParam,lParam); break;
	default: break;
	}

	return 0L;
}

BOOL QYMainWindow::OnCreate(CREATESTRUCT *cs)
{
	DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
	SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));

	
	SetFont((HFONT)(QYApp::m_pMsDefultFont->m_hObject));

	UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
		QYRect WinRC;
		GetWindowRect(WinRC);

	

		m_NCBorderHeight = GetSystemMetrics(SM_CYBORDER);
		m_NCBorderWidth = GetSystemMetrics(SM_CXBORDER);

		m_CaptionHeight = GetSystemMetrics(SM_CYCAPTION);
		m_CloseBoxSize.cy = m_CaptionHeight;
		m_CloseBoxSize.cx = m_CaptionHeight * 3 / 2;
		m_MinBoxSize.cy = m_CaptionHeight;
		m_MinBoxSize.cx = m_CaptionHeight * 3 / 2;
		m_MaxBoxSize.cy = m_CaptionHeight;
		m_MaxBoxSize.cx = m_CaptionHeight * 3 / 2;

		//MoveWindow(WinRC.left, WinRC.top, WinRC.Width(), WinRC.Height() + m_CaptionHeight);

		m_bZoom = IsZoomed(GetHwnd());

		style &= ~WS_SYSMENU;
		//style &= ~WS_SIZEBOX;
		SetWindowLong(GetHwnd(), GWL_STYLE, style);

		m_FrameWidth = 6;
		m_FrameHeight = 6;
		m_disableMaxBox = !(style&WS_MAXIMIZEBOX);
		m_disableMinBox = !(style&WS_MINIMIZEBOX);
		m_nSizeBoxWidth = m_FrameWidth;

		if (m_bShowShadow)
		{
			m_pShadow = new QYWindowShadow;
			m_pShadow->Create(0, this);
		}
		
		OnSize(0, 0, 0);
		OnMove(0, 0);

	return TRUE;
}

LRESULT QYMainWindow::OnExitSize(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

BOOL QYMainWindow::OnNcActivate(BOOL bActive)
{
	//g_ConselLog.Print(0,"hwnd:%d,ncactive:%d\n", GetHwnd(),bActive);
	if (nullptr != m_pShadow)
	{
		QYRect rt;
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	m_bActive = bActive;
	return OnNcPaint();
}

void QYMainWindow::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (m_ButtonState != bsNone)
	{
		m_ButtonState = bsNone;
		OnNcPaint();
	}
}

void QYMainWindow::OnNcLButtonDown(UINT nHitTest, QYPoint point)
{
	m_ButtonDownState = m_ButtonState;

	PostMessage(WM_NCACTIVATE, TRUE);

	if (!m_bNCLBDOWN)
	{
		m_bNCLBDOWN = TRUE;
	}
}

void QYMainWindow::OnNcLButtonDBClick(UINT nHitTest, QYPoint point)
{
	//PostMessage(GetHwnd(),WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM (point.x, point.y));
}

void QYMainWindow::OnLButtonDown(UINT nHitTest, QYPoint point)
{
	QYRect rc;
	GetClientRect(rc);
	//ClientToScreen(hWnd,rc);
	rc.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT;
	if (rc.PtInRect(point))
	{
		//PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}

	//PostMessage(hWnd,WM_NCLBUTTONUP,HTCAPTION,MAKELPARAM (point.x, point.y));
}

BOOL QYMainWindow::OnNcLButtonUp(UINT nHitTest, QYPoint point)
{
	switch (m_ButtonState)
	{
	case bsClose: //关闭窗口
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_HIDE);
		}

		SendMessage(WM_CLOSE, 0, 0);
	}
	break;
	case bsMin:
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_HIDE);
		}

		//SendMessage(WM_SYSCOMMAND, SC_MINIMIZE,0);
		Show(SW_MINIMIZE);

	}
	break;
	case bsMax:Zoom(TRUE); break;
	case bsRes:Zoom(FALSE); break;
	default:
		break;;
	}

	if (m_bNCLBDOWN)
	{
		m_bNCLBDOWN = FALSE;
	}

	m_ButtonDownState = bsNone;

	//OnNcPaint();

	return TRUE;
}



void QYMainWindow::Zoom(BOOL bZoom)
{

#if 1
	if (bZoom && !IsZoomed(m_hWnd))
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_HIDE);
		}

		GetWindowRect(m_rectRestore);

		int x = GetSystemMetrics(SM_CXSCREEN);
		int y = GetSystemMetrics(SM_CYSCREEN);

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		QYRect rtWork = rt;
		rtWork.left -= m_FrameWidth;
		rtWork.right += m_FrameWidth;
		rtWork.bottom += m_FrameHeight;

		//MoveWindow(rtWork);

		UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
		style &= ~WS_SIZEBOX;
		//SetWindowLong(m_hWnd, GWL_STYLE, style);

		m_bZoom = TRUE;
		
		Show(SW_MAXIMIZE);
	}
	else
	{
		if (nullptr != m_pShadow)
		{
			m_pShadow->Show(SW_HIDE);
		}

		//MoveWindow(m_rectRestore);

		UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
		style |= WS_SIZEBOX;
		//SetWindowLong(m_hWnd, GWL_STYLE, style);
		m_bZoom = FALSE;

		Show(SW_RESTORE);
	}

	m_ButtonState = bsNone;
#else
	UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
	if (!(style&WS_MAXIMIZEBOX))
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
		MoveWindow(rect.left - m_BorderWidth, rect.top - m_BorderHeight, rect.Width() + 2 * m_BorderWidth, rect.Height() + 2 * m_BorderHeight);
		m_bZoom = TRUE;
		UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
		style &= ~WS_SIZEBOX;
		//SetWindowLong(GetHwnd(), GWL_STYLE, style);
	}
	else
	{
		QYRect winRect;

		GetWindowRect(winRect);
		MoveWindow(m_restoreRect);
		m_restoreRect = winRect;

		m_bZoom = FALSE;
		UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
		style |= WS_SIZEBOX;
		//SetWindowLong(GetHwnd(), GWL_STYLE, style);
	}
#endif

}

BOOL QYMainWindow::OnNcMouseMove(UINT nHitTest, QYPoint point)
{
	QYRect tempIni, tempMin, tempMax, tempClose, ClientRect;

	GetWindowRect(ClientRect);

	if (!m_bNCHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bNCHover = TRUE;
	}

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
			OnNcPaint();
		}
	}
	else if (tempMax.PtInRect(pt) && !m_disableMaxBox)
	{
		if (m_ButtonState != bsMax && m_ButtonState != bsRes)
		{
			if (!m_bZoom)
			{
				m_ButtonState = bsMax;
			}
			else
			{
				m_ButtonState = bsRes;
			}

			OnNcPaint();
		}
	}
	else if (tempClose.PtInRect(pt))
	{
		if (m_ButtonState != bsClose)
		{
			m_ButtonState = bsClose;
			OnNcPaint();
		}
	}
	else
	{
		if (bsNone != m_ButtonState)
		{
			//g_ConselLog.Print(0,"ncmousemove\n");
			m_ButtonState = bsNone;
			OnNcPaint();
		}
	}
#endif

	return TRUE;
}

BOOL QYMainWindow::OnNcMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (bsNone != m_ButtonState)
	{
		//g_ConselLog.Print(0,"ncmousemove\n");
		m_ButtonState = bsNone;
		OnNcPaint();
	}

	m_bNCHover = FALSE;


	return TRUE;
}

void QYMainWindow::InitSysBox(QYRect &WinRC)
{
	QYRect rectCaption;

	rectCaption.left = m_NCBorderWidth;
	rectCaption.top =  m_NCBorderHeight;
	rectCaption.right = WinRC.Width() - m_NCBorderWidth ;
	rectCaption.bottom = m_CaptionHeight + m_FrameHeight;

	int padRight = (m_bZoom ? m_FrameWidth : 0);
	int padTop = (m_bZoom ? m_FrameHeight : 0);
	m_rectClose.top = rectCaption.top + padTop;
	m_rectClose.right = rectCaption.right - m_NCBorderWidth - padRight;
	m_rectClose.left = m_rectClose.right - m_CloseBoxSize.cx;
	m_rectClose.bottom = rectCaption.bottom;

	m_rectMax.right = m_rectClose.left;
	m_rectMax.left = m_rectMax.right - m_MaxBoxSize.cx;
	m_rectMax.top = m_rectClose.top;
	m_rectMax.bottom = m_rectClose.bottom;

	m_rectMin.right = m_rectMax.left;
	m_rectMin.left = m_rectMin.right - m_MinBoxSize.cx;
	m_rectMin.top = m_rectMax.top;
	m_rectMin.bottom = m_rectMax.bottom;
}

BOOL QYMainWindow::OnMove(int cx, int cy)
{
	if (nullptr != m_pShadow)
	{
		QYRect rt;
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}
	//OnNcActivate(TRUE);

	return TRUE;
}

BOOL QYMainWindow::OnSize(UINT nType, int cx, int cy)
{
	QYRect WinRC, FactRC;
	GetWindowRect(WinRC);
	QYRect clientRect;
	GetClientRect(clientRect);

	m_FrameWidth = (WinRC.Width() - clientRect.Width()) / 2;
	m_FrameHeight = (WinRC.Height() - clientRect.Height() - m_CaptionHeight) / 2;

	if (IsZoomed(m_hWnd))
	{
		m_bZoom = TRUE;
	}
	else
	{
		m_bZoom = FALSE;
	}

	InitSysBox(WinRC);

	if (nullptr != m_pShadow)
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	HRGN hRgn = CreateRectRgn(0, 0, WinRC.Width(), WinRC.Height());

	::SetWindowRgn(m_hWnd, hRgn, TRUE);

	DeleteObject(hRgn);

	QYWindow::onSize(nType, cx, cy);

	UpdateLayout(clientRect);

	OnPaint();

	::ShowWindow(this->m_hWnd, SW_SHOWNA);

	return FALSE;
}

void QYMainWindow::DrawCloseRect(QYDC *pDC, QYRect &rect, Button_state state)
{
#if 1
	QYBrush brush;
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
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
		//brush.CreateSolidBrush(CLOSE_BOX_BK_COLOR_NORMAL);
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
	//pDC->Rectangle(rectReal);
	QYPoint pt;

	pDC->MoveToEx(rectReal.left + 1, rectReal.top + 1, &pt);
	pDC->LineTo(rectReal.left + sideLen, rectReal.top + sideLen);
	pDC->MoveToEx(rectReal.left + 1, rectReal.top + sideLen - 1, &pt);
	pDC->LineTo(rectReal.left + sideLen, rectReal.top);

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();
	brush.DeleteObject();
#endif
}

void QYMainWindow::DrawMaxRect(QYDC *pDC, QYRect &rect, int zoom, Button_state state)
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
		//brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
		//brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
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

		pDC->Rectangle(rectReal.left - 2, rectReal.top + 2, rectReal.right - 2, rectReal.bottom + 2);
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

void QYMainWindow::DrawMinRect(QYDC *pDC, QYRect &rect, Button_state state)
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
		//brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, QY_THEME.SYS_BOX_PEN_COLOR_DISABLE);
	}
	else
	{
	//	brush.CreateSolidBrush(SYS_BOX_BK_COLOR_NORMAL);
		hSysBtnPen.CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, !m_bActive ? QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE : QY_THEME.SYS_BOX_PEN_COLOR_NORMAL_ACTIVE);
	}

	HPEN oldpen = (HPEN)pDC->SelectObject(&hSysBtnPen);

	pDC->FillRect(rect, brush);

	QYPoint pt;

	int sideLen = min(rect.Height() * 2 / 4, (rect.Width() - 2) * 2 / 4);
	pDC->MoveToEx(rect.left + (rect.Width() - sideLen) / 2,
		rect.top + rect.Height() * 1 / 2,
		&pt);
	pDC->LineTo(rect.right - (rect.Width() - sideLen) / 2, rect.top + rect.Height() / 2);

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();
	brush.DeleteObject();
#endif
}

HBRUSH m_GdiBrush;

BOOL QYMainWindow::OnPaint()
{
	QYPaintDC dc(this);
	QYRect Clientrect, captionRect;
	GetClientRect(Clientrect);

	QYBrush hClientBrush(QY_THEME.CLIENT_COLOR);

	dc.FillRect(Clientrect, hClientBrush);

	hClientBrush.DeleteObject();

	return TRUE;
}

BOOL QYMainWindow::NcPaint(QYDC *pDC, QYRect WinRC,int active)
{
	QYBrush NullBrush(GetStockObject(NULL_BRUSH));
	auto oldbrush = (HBRUSH)pDC->SelectObject(&NullBrush);

	QYPen BorderPen(PS_SOLID, m_NCBorderWidth, m_bActive ? QY_THEME.BORDER_PEN_COLOR_ACTIVE : QY_THEME.BORDER_PEN_COLOR_ACTIVE);
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
	rectCaption.bottom = m_CaptionHeight + m_FrameHeight;

	QYBrush CaptionBrush(QY_THEME.CAPTION_COLOR);
	pDC->FillRect(rectCaption, CaptionBrush);

	QYLinearBitmap linearBitmap(LINEAR_RGB_ID_CAPTION, rectCaption.Width(), rectCaption.Height());

	linearBitmap.Draw(pDC, &rectCaption);

	COLORREF corBorder = QY_THEME.BORDER_PEN_COLOR_ACTIVE;
	const unsigned char *pData = QYLinearBitmap::GetData(LINEAR_RGB_ID_CAPTION);
	if (nullptr != pData)
	{
		if (QY_THEME.CAPTION_BK_GRADIENT)
		{
			pData += 4 * rectCaption.Height();
		}

		corBorder = RGB(*(pData + 2), *(pData + 1), *pData);
	}

	QYBrush hBorderBrush(corBorder);

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

	int padTop = (m_bZoom ? m_FrameHeight : 0);
	int padLeft = (m_bZoom ? m_FrameWidth : 0);
	if (nullptr != m_hIcon)
	{
		ICONINFO icoInfo;
		GetIconInfo(m_hIcon, &icoInfo);
		pDC->DrawIconEx(rectCaption.left + padLeft/2 + (rectCaption.Height() - icoInfo.xHotspot) / 2,
			rectCaption.top + padTop + (rectCaption.Height() - padTop - icoInfo.yHotspot) / 2,
			m_hIcon,
			icoInfo.xHotspot,
			icoInfo.yHotspot);

		::DeleteObject(icoInfo.hbmColor);
		::DeleteObject(icoInfo.hbmMask);

		rectCaption.left += padLeft / 2 + icoInfo.xHotspot + 2;
	}

	

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_bActive ? QY_THEME.CAPTION_TEXT_COLOR_ACTIVE : QY_THEME.CAPTION_TEXT_COLOR_ACTIVE);

	rectCaption.left += m_FrameWidth;
	rectCaption.top += padTop;
	//rectCaption.top += m_FrameWidth;
	pDC->DrawText(m_strWindowText, m_strWindowText.Length(), &rectCaption, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);

	CaptionBrush.DeleteObject();
	BorderPen.DeleteObject();
	
	QYRect rectBorder = 0;
	rectBorder.top = rectCaption.bottom;
	rectBorder.left = WinRC.left + m_NCBorderWidth;
	rectBorder.right = WinRC.left + m_FrameWidth;
	rectBorder.bottom = WinRC.bottom - m_NCBorderHeight;

	pDC->FillRect(rectBorder, hBorderBrush);

	rectBorder.right = WinRC.right - m_NCBorderWidth;
	rectBorder.left = WinRC.right - m_FrameWidth;

	pDC->FillRect(rectBorder, hBorderBrush);

	rectBorder.bottom = WinRC.bottom - m_NCBorderHeight;
	rectBorder.top = rectBorder.bottom - m_FrameHeight;
	rectBorder.left = WinRC.left  + m_FrameWidth;
	rectBorder.right = WinRC.right  - m_FrameWidth;

	pDC->FillRect(rectBorder, hBorderBrush);

	hBorderBrush.DeleteObject();

	return TRUE;
}

BOOL QYMainWindow::PaintDC(QYDC *pDC)
{
	QYRect WinRC, FactRC;
	GetWindowRect(WinRC);

	QYBrush NullBrush(GetStockObject(NULL_BRUSH));
	auto oldbrush = (HBRUSH)pDC->SelectObject(&NullBrush);

	QYPen BorderPen(PS_SOLID, m_NCBorderWidth, m_bActive ? QY_THEME.BORDER_PEN_COLOR_ACTIVE : QY_THEME.BORDER_PEN_COLOR_ACTIVE);
	auto oldpen = (HPEN)pDC->SelectObject(&BorderPen);

	pDC->Rectangle(0, 0, WinRC.Width(), WinRC.Height());

	QYRect rectCaption;
	rectCaption.left = m_NCBorderWidth;
	rectCaption.top = m_NCBorderHeight;
	rectCaption.right = WinRC.Width() - m_NCBorderWidth;
	rectCaption.bottom = m_CaptionHeight + m_FrameHeight;

	QYBrush CaptionBrush(QY_THEME.CAPTION_COLOR);
	pDC->FillRect(rectCaption, CaptionBrush);

	QYBrush ClientBrush(QY_THEME.CLIENT_COLOR);
	QYRect rectLeft;

	if (QY_THEME.DIALOG_CAPTION_HEIGHT > m_CaptionHeight)
	{
		rectLeft.left = m_NCBorderWidth;
		rectLeft.right = m_FrameWidth + rectLeft.left;
		rectLeft.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT + m_FrameHeight;
		rectLeft.top = rectCaption.bottom;
		pDC->FillRect(rectLeft, CaptionBrush);
	}

	rectLeft.left = m_NCBorderWidth;
	rectLeft.right = m_FrameWidth;
	rectLeft.top = m_FrameHeight + ((QY_THEME.DIALOG_CAPTION_HEIGHT > m_CaptionHeight) ? QY_THEME.DIALOG_CAPTION_HEIGHT : m_CaptionHeight);
	rectLeft.bottom = WinRC.Height() - m_NCBorderHeight;

	pDC->FillRect(rectLeft, ClientBrush);

	QYRect rectRight;

	if (QY_THEME.DIALOG_CAPTION_HEIGHT > m_CaptionHeight)
	{
		rectRight.right = WinRC.Width() - m_NCBorderWidth;
		rectRight.left = rectRight.right - m_FrameWidth;
		rectRight.bottom = QY_THEME.DIALOG_CAPTION_HEIGHT + m_FrameHeight;
		rectRight.top = rectCaption.bottom;
		pDC->FillRect(rectRight, CaptionBrush);
	}

	rectRight.right = WinRC.Width() - m_NCBorderWidth;
	rectRight.left = WinRC.Width() - m_FrameWidth;
	rectRight.top = m_FrameHeight + ((QY_THEME.DIALOG_CAPTION_HEIGHT > m_CaptionHeight) ? QY_THEME.DIALOG_CAPTION_HEIGHT : m_CaptionHeight);
	rectRight.bottom = WinRC.Height() - m_NCBorderHeight;

	pDC->FillRect(rectRight, ClientBrush);

	QYRect rectBottom;
	rectBottom.left = m_NCBorderWidth;
	rectBottom.right = WinRC.Width() - m_NCBorderWidth;
	rectBottom.top = WinRC.Height() - m_FrameHeight;
	rectBottom.bottom = WinRC.Height() - m_NCBorderHeight;

	pDC->FillRect(rectBottom, ClientBrush);

	CaptionBrush.DeleteObject();
	ClientBrush.DeleteObject();

	Button_state state2 = button_state_normal;
	if (m_ButtonState == m_ButtonDownState && m_ButtonState == bsClose)
	{
		state2 = button_state_down;
	}
	else
	{
		if (bsClose == m_ButtonState)
		{
			state2 = button_state_hot;
		}
		else
		{
			state2 = button_state_normal;
		}
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
			if (m_ButtonState == m_ButtonDownState&&  m_ButtonState == bsMax)
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
				if (bsMin == m_ButtonState)
				{
					state2 = button_state_hot;
				}
				else
				{
					state2 = button_state_normal;
				}
			}
		}
		DrawMinRect(pDC, m_rectMin, state2);
	}

	if (nullptr != m_hIcon)
	{
		ICONINFO icoInfo;
		GetIconInfo(m_hIcon, &icoInfo);
		pDC->DrawIconEx(rectCaption.left + (rectCaption.Height() - icoInfo.xHotspot) / 2,
			rectCaption.left + (rectCaption.Height() - icoInfo.yHotspot) / 2,
			m_hIcon,
			icoInfo.xHotspot,
			icoInfo.yHotspot);

		::DeleteObject(icoInfo.hbmColor);
		::DeleteObject(icoInfo.hbmMask);

		rectCaption.left += icoInfo.xHotspot + 2;
	}

	HFONT oldFont = (HFONT)pDC->SelectObject(m_pFont);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_bActive ? QY_THEME.CAPTION_TEXT_COLOR_ACTIVE : QY_THEME.CAPTION_TEXT_COLOR_ACTIVE);

	rectCaption.left += m_FrameWidth;
	pDC->DrawText(m_strWindowText, m_strWindowText.Length(), &rectCaption, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);

	BorderPen.DeleteObject();
	pDC->SelectObject(oldFont);


	ReleaseDC(pDC);
	return TRUE;
}

void QYMainWindow::SetDlgZone()
{
	OnNcPaint();
}

BOOL QYMainWindow::OnNcPaint(int active)
{

	QYDC *pDc = GetWindowDC();

	
	QYRect WinRC;
	GetWindowRect(WinRC);
	WinRC.right = WinRC.Width();
	WinRC.left = 0;
	WinRC.bottom = WinRC.Height();
	WinRC.top = 0;

	QYRect rectClip = WinRC;
	int ret = ExcludeClipRect(pDc->GetDC(),
		rectClip.left + m_FrameWidth,
		rectClip.top + m_CaptionHeight + m_FrameHeight,
		rectClip.right - m_FrameWidth,
		rectClip.bottom - m_FrameHeight);
	QYMemDC memDC(pDc,WinRC);

	HFONT oldFont = (HFONT)memDC.SelectObject(m_pFont);

	NcPaint(&memDC, WinRC);

	memDC.SelectObject(oldFont);

	pDc->BitBlt(0, 0, WinRC.Width(), WinRC.Height(), &memDC, 0, 0, SRCCOPY);


	ReleaseDC(pDc);

	return TRUE;
}

LRESULT QYMainWindow::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	if (nullptr != m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void  QYMainWindow::OnTimer(WPARAM wparam, LPARAM lparam)
{
	
}