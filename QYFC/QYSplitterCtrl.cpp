#include "QYSplitterCtrl.h"
#include "QYDC.h"

QYSplitterCtrl::QYSplitterCtrl()
{
	m_bHover = FALSE;
	m_bDown = FALSE;
	m_pairSub = std::make_pair(nullptr, nullptr);
	m_pairView = std::make_pair(nullptr, nullptr);
	m_pView = nullptr;
	m_fX = 0;
	m_fY = 0;
}


QYSplitterCtrl::~QYSplitterCtrl()
{
	SAFE_DESTROY_WINDOW_PTR(m_pairSub.first);
	SAFE_DESTROY_WINDOW_PTR(m_pairSub.second);
}

LRESULT QYSplitterCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//g_ConselLog.Print(0, "message:%04x\n", message);
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{break; }
	case WM_NCDESTROY:break;
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_LBUTTONUP:OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); break;
	default:break;
	}

	return 0L;
}

BOOL QYSplitterCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	QYWindow *pParentWnd,
	int type)
{
	m_Type = type;
	return QYWindow::CreateEx(dwExStyle,
		L"QYSplitterCtrl",
		lpWndName,
		dwStyle,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		pParentWnd);
}

BOOL QYSplitterCtrl::AddSubSplitter(QYSplitterCtrl* first, QYSplitterCtrl* second)
{
	if (nullptr == m_pairSub.first)
	{
		m_pairSub.first = first;
	}

	if (nullptr == m_pairSub.second)
	{
		m_pairSub.second = second;
	}

	return TRUE;
}

BOOL QYSplitterCtrl::AddFirstSubSplitter(QYSplitterCtrl* first)
{
	if (nullptr == m_pairSub.first)
	{
		m_pairSub.first = first;
	}

	return TRUE;
}

BOOL QYSplitterCtrl::AddSecondSubSplitter(QYSplitterCtrl* second)
{
	if (nullptr == m_pairSub.second)
	{
		m_pairSub.second = second;
	}

	return TRUE;
}

void QYSplitterCtrl::SetX(float fX)
{
	m_fX = fX;
}

void QYSplitterCtrl::SetY(float fY)
{
	m_fY = fY;
}


void QYSplitterCtrl::SetView(QYWindow *first, QYWindow *second)
{
	m_pairView.first = first;

	if (nullptr != m_pairView.first && nullptr != m_pairView.first->GetHwnd())
	{
		m_pairView.first->Show(SW_SHOW);
	}

	m_pairView.second = second;
	if (nullptr != m_pairView.second  && nullptr != m_pairView.second->GetHwnd())
	{
		m_pairView.second->Show(SW_SHOW);
	}
}

void QYSplitterCtrl::SetView(QYWindow* pView)
{
	if (nullptr != pView && nullptr != pView->GetHwnd())
	{
		pView->Show(SW_SHOW);
		if (SPLITTER_DOCK_LEFT == m_Type || SPLITTER_DOCK_TOP == m_Type)
		{
			m_pairView.first = pView;
		}
		else
		{
			m_pairView.second = pView;
		}

	}
}

void QYSplitterCtrl::SetFirstView(QYWindow* pView)
{
	pView->Show(SW_SHOW);
	m_pairView.first = pView;
}

void QYSplitterCtrl::SetSecondView(QYWindow* pView)
{
	pView->Show(SW_SHOW);
	m_pairView.second = pView;
}

void QYSplitterCtrl::MoveView(QYRect &first, QYRect &second, int splitterWidth)
{
	if (nullptr != m_pairView.first && nullptr != m_pairView.first->GetHwnd())
	{
		m_pairView.first->MoveWindow(first);
	}
	else if (nullptr != m_pairSub.first)
	{
		MoveView(m_pairSub.first, first, splitterWidth);
	}

	if (nullptr != m_pairView.second && nullptr != m_pairView.second->GetHwnd())
	{
		m_pairView.second->MoveWindow(second);
	}
	else if (nullptr != m_pairSub.second)
	{
		MoveView(m_pairSub.second,second,splitterWidth);
	}
}


void QYSplitterCtrl::MoveView(QYSplitterCtrl *pSub, QYRect &rect, int splitterWidth)
{
	QYRect rectSplitter = rect, firstSub = rect, secondSub = rect;

	int type = pSub->GetType();
	float fX = pSub->GetX();
	float fY = pSub->GetY();

	if (SPLITTER_DOCK_LEFT == type)
	{
		if (0 < fX  )
		{
			if (1 >= fX)
			{
				firstSub.right = rect.left + ((rect.Width() - splitterWidth)* fX);
			}
			else
			{
				firstSub.right = rect.left + fX ;
			}
			
		}
		else if (0 > fX)
		{
			if (-1 >= fX)
			{
				firstSub.right = rect.right - splitterWidth + fX;
			}
			else
			{
				firstSub.right = rect.right + (rect.Width() - splitterWidth)*  fX;
			}
		}
		else 
		{
			firstSub.right = rect.right - splitterWidth;
		}

		

		rectSplitter.left = firstSub.right;
		rectSplitter.right = rectSplitter.left + splitterWidth;

		secondSub.left = rectSplitter.right;
	}
	else if (SPLITTER_DOCK_TOP == type)
	{
		if (0 < fY)
		{
			if (1 >= fY)
			{
				firstSub.bottom = rect.top + ((rect.Height() - splitterWidth)* fY);
			}
			else
			{
				firstSub.bottom = rect.top + fY;
			}

		}
		else if (0 > fY)
		{
			if (-1 >= fY)
			{
				firstSub.bottom = rect.bottom - splitterWidth + fY;
			}
			else
			{
				firstSub.bottom = rect.bottom + (rect.Height() - splitterWidth)*  fY;
			}
		}
		else
		{
			firstSub.bottom = rect.bottom  - splitterWidth;
		}


		rectSplitter.top = firstSub.bottom;
		rectSplitter.bottom = rectSplitter.top + splitterWidth;

		secondSub.top = rectSplitter.bottom;
	}
	else if(SPLITTER_DOCK_RIGHT == type)
	{

	}
	else
	{
		if (0 < fY)
		{
			if (1 >= fY)
			{
				secondSub.top = rect.bottom - ((rect.Height() - splitterWidth)* fY);
			}
			else
			{
				secondSub.top = rect.bottom - fY;
			}

		}
		else if (0 > fY)
		{
			if (-1 >= fY)
			{
				secondSub.top = rect.top + splitterWidth - fY;
			}
			else
			{
				secondSub.top = rect.top + splitterWidth  - (rect.Height() - splitterWidth)*  fY;
			}
		}
		else
		{
			secondSub.top = rect.top + splitterWidth;
		}

	
		rectSplitter.bottom = secondSub.top;
		rectSplitter.top = rectSplitter.bottom - splitterWidth;

		firstSub.bottom = rectSplitter.top;
	}
	pSub->MoveWindow(rectSplitter);
	pSub->MoveView(firstSub, secondSub, splitterWidth);
}

void QYSplitterCtrl::ClearSubSplitter()
{
	if (nullptr != m_pairSub.first)
	{
		m_pairSub.first->ClearSubSplitter();
		SAFE_DESTROY_WINDOW_PTR(m_pairSub.first);
	}

	if (nullptr != m_pairView.first && nullptr != m_pairView.first->GetHwnd())
	{
		m_pairView.first->Show(SW_HIDE);
	}
	if (nullptr != m_pairSub.second)
	{
		m_pairSub.second->ClearSubSplitter();
		SAFE_DESTROY_WINDOW_PTR(m_pairSub.second);

		
	}

	if (nullptr != m_pairView.second && nullptr != m_pairView.second->GetHwnd())
	{
		m_pairView.second->Show(SW_HIDE);
	}
}

LRESULT QYSplitterCtrl::OnMouseMove(UINT nFlags, QYPoint point)
{
	//g_ConsoleLog.Print("QYScrollBar OnMouseMove\n");
	if (!m_bHover)
	{
		//SetFocus();
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 10;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bHover = TRUE;
		Invalidate();
	}

	return TRUE;
}

LRESULT	QYSplitterCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	m_bHover = FALSE;
	Invalidate();

	return TRUE;
}

LRESULT QYSplitterCtrl::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, QY_THEME.SPLITTER_COLOR_NORMAL);

	QYRect rectTemp = rect;

	if (m_bDown)
	{
		MemDc.FillSolidRect(rectTemp, QY_THEME.SPLITTER_COLOR_DOWN);
	}
	else
	{
		if (m_bHover)
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.SPLITTER_COLOR_HOVER);
		}
		else
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.SPLITTER_COLOR_NORMAL);
		}
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYSplitterCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bDown)
	{
		m_bDown = TRUE;
		Invalidate();
	}

	return TRUE;
}

LRESULT QYSplitterCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//ReleaseCapture();
	if (m_bDown)
	{
		m_bDown = FALSE;
		Invalidate();
	}

	return TRUE;
}