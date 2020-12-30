#include "QYScrollBar.h"
#include "qydc.h"

#if 1


QYScrollBar::QYScrollBar()
{
	m_bHover = FALSE;
	m_bDownScrollBar = FALSE;
	
}


QYScrollBar::~QYScrollBar()
{
}

LRESULT QYScrollBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//g_ConselLog.Print(0, "message:%04x\n", message);
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE;break;}
	case WM_PAINT:{return OnPaint();}break;
	case WM_NCPAINT:{break;}
	case WM_NCDESTROY:{break;}
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;}
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam);}break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_LBUTTONUP:OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;
	default:break;
	}
	return 0L;
}

BOOL QYScrollBar::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd,
	int type)
{
	m_Type = type;
	return QYWindow::Create(dwExStyle,
		L"QYScrollBar",
		lpWndName,
		dwStyle,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
}

LRESULT QYScrollBar::OnMouseMove(UINT nFlags, QYPoint point)
{
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

LRESULT	QYScrollBar::OnMouseLeave(WPARAM, LPARAM)
{
	m_bHover = FALSE;
	Invalidate();

	return TRUE;
}

LRESULT QYScrollBar::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, QY_THEME.LISTBOX_BK_COLOR_NORMAL);

	QYRect rectTemp = rect;

// 	g_ConsoleLog.Print("left:%d,top:%d,right:%d,bottom:%d\n", 
// 		rectTemp.left,
// 		rectTemp.top, 
// 		rectTemp.right,
// 		rectTemp.bottom);
		
		if (SB_VERT == m_Type)
		{
			rectTemp.left += 2;
			rectTemp.right -= 2;
		}
		else
		{
			rectTemp.top += 2;
			rectTemp.bottom -= 2;
		}
	
		if (m_bDownScrollBar)
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.SCROLLBAR_COLOR_SELECT);
		}
		else
		{
			if (m_bHover)
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.SCROLLBAR_COLOR_HOVER);
			}
			else
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.SCROLLBAR_COLOR_NORMAL);
			}
		}


	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYScrollBar::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	
	//	SetCapture();
		m_bDownScrollBar = TRUE;
		m_bHover = FALSE;
		Invalidate();

		QYRect rtWnd;
		GetWindowRect(rtWnd);
		RECT rtParentWnd;
		::GetWindowRect(::GetParent(m_hWnd), &rtParentWnd);
		rtWnd.OffsetRect(-rtParentWnd.left, -rtParentWnd.top);
		point.x += rtWnd.left;
		point.y += rtWnd.top;

		::SendMessage(::GetParent(m_hWnd), WM_LBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
		
	return TRUE;
}

LRESULT QYScrollBar::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//ReleaseCapture();
	if (m_bDownScrollBar)
	{
		m_bDownScrollBar = FALSE;
		Invalidate();
	}

	return TRUE;
}

#endif

QYScrollCtrl::QYScrollCtrl()
{
	m_dockFlag = SB_VERT;
	m_nBarPos = 0;
	m_nRange = 0;
	m_rectBar = 0;
	m_rectScroll = 0;
	m_bHover = FALSE;
	m_bDownScrollBar = FALSE;
	m_nStep = 20;
	_buttonStep = 10;
	m_nMinBarWidth = 20;
	m_pBar = nullptr;
	m_nDownPos = 0;
	m_downArrow = -1;
	m_hoverArrow = -1;
}

QYScrollCtrl::~QYScrollCtrl()
{
}

LRESULT QYScrollCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		(nullptr == m_pBar) ? m_pBar = new QYScrollBar : 0;

		if (nullptr == m_pBar->GetHwnd())
		{
			if (!m_pBar->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, 0, m_hWnd,m_dockFlag))
			{
				return FALSE;
			}
		}
	}
	break;
	case WM_SETCURSOR:
	{
		return TRUE;
	}
	break;
	case WM_ERASEBKGND:
	{
		return TRUE;
		break;
	}
	case WM_PAINT:
	{
		return OnPaint();
	}
	break;
	case WM_NCPAINT:
	{
		break;
	}
	case WM_NCDESTROY:
	{
		SAFE_DESTROY_WINDOW_PTR(m_pBar);
		break;
	}
	case WM_MOUSEMOVE:
	{
		OnMouseMove(wParam, QYPoint(LOSHORT(lParam), HISHORT(lParam)));
	}
	break;
	case WM_MOUSELEAVE:
	{
		OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		OnLButtonDown(wParam, QYPoint(LOSHORT(lParam), HISHORT(lParam)));
	}
	break;
	case WM_LBUTTONUP:
	{
		OnLButtonUp(wParam, QYPoint(LOSHORT(lParam), HISHORT(lParam)));
	}
	break;
	case WM_SIZE:
	{
		return OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
	}
	break;
	default:
	{
		break;
	}
	}
	return 0L;
}

BOOL QYScrollCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd,
	int flag )
{
	m_dockFlag = flag;

	return QYWindow::Create(dwExStyle,
		L"QYScrollCtrl",
		lpWndName,
		dwStyle | WS_CLIPCHILDREN,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
}

int  QYScrollCtrl::SetRange(int range)
{
	if (0 == range)
	{
		return 0;
	}

	m_nRange = range;

	if (0 < m_nRange)
	{
		m_pBar->Show(SW_SHOW);
	}
	else
	{
		m_pBar->Show(SW_HIDE);
	}

	return 0;
}

LRESULT QYScrollCtrl::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bHover = TRUE;
		Invalidate();
		//m_pBar->Invalidate();
	}
	else
	{
		if (m_bDownScrollBar)
		{
			if (SB_VERT != m_dockFlag)
			{
				UpdateHorzLayout(point.x- m_LastPoint.x);
			}
			else
			{
				UpdateVertLayout(point.y - m_LastPoint.y);
			}
		}
		
	}

	return TRUE;
}

 LRESULT	QYScrollCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	//g_ConsoleLog.Print("QYScrollCtrl OnMouseleave\n");
	m_bHover = FALSE;
	Invalidate();
//	m_pBar->Invalidate();
	return TRUE;
}

int  QYScrollCtrl::GetAccurateScrollBarHeight(int totalHeight)
{
	if (0 < m_nRange)
	{
		int width = 0;
		int realRange = m_nRange;

		if (totalHeight - realRange > m_nMinBarWidth)
		{
			return (totalHeight - realRange);
		}
		else
		{
			return m_nMinBarWidth;
		}
	}
	else
	{
		return 0;
	}
}


int QYScrollCtrl::step(int offset)
{
	m_nDownPos = m_nBarPos;
	if (SB_VERT == m_dockFlag)
	{
		UpdateVertLayout(offset * (float)(m_rectScroll.Height() - m_nBarHeight) / (float)m_nRange, 0, FALSE);
	}
	else
	{
		UpdateHorzLayout(offset * (float)(m_rectScroll.Width() - m_nBarWidth) / (float)m_nRange, FALSE);
	}

	return m_nBarPos;
}

void QYScrollCtrl::SetPos(int pos)
{ 
	if (SB_VERT == m_dockFlag)
	{
		m_nBarPos=pos * (float)(m_rectScroll.Height() - m_nBarHeight) / (float)m_nRange;
	}
	else
	{
		m_nBarPos=pos * (float)(m_rectScroll.Width() - m_nBarWidth) / (float)m_nRange;
	}


	OnSize(0, 0, 0);
}

int QYScrollCtrl::GetPos()
{
	if (0 >= m_nRange )
	{
		return 0;
	}

	if (SB_VERT == m_dockFlag)
	{
		if (0 >= m_rectScroll.Height() || 0 >= m_nBarHeight)
		{
			return 0;
		}
		return m_nBarPos * (float)m_nRange / (float)(m_rectScroll.Height() - m_nBarHeight);
	}
	else
	{
		if (0 >= m_rectScroll.Width() || 0 >= m_nBarWidth)
		{
			return 0;
		}
		return m_nBarPos * (float)m_nRange / (float)(m_rectScroll.Width() - m_nBarWidth);
	}
	
}

int QYScrollCtrl::setStep(int step)
{
	int lastStep = m_nStep;
	m_nStep = step;
	return lastStep;
}


int QYScrollCtrl::getStep()
{
	return m_nStep;
}

void QYScrollCtrl::UpdateHorzLayout(int offWidth, BOOL sendToParent)
{
	QYRect rect;
	GetClientRect(rect);
	int left = m_rectScrollArrow[ARROW_TYPE_LEFT].right;
	int lastBarPos = m_nBarPos;
	m_nBarPos = m_nDownPos + offWidth ;
	if (0 < offWidth)
	{
		if (m_nBarPos >= (m_rectScroll.Width() - m_nBarWidth))
		{
			m_nBarPos = m_rectScroll.Width() - m_nBarWidth;
		}
	}
	else
	{
		m_nBarPos = (m_nBarPos <= 0) ? 0 : m_nBarPos;
	}

	if (m_nBarPos != lastBarPos)
	{
		m_rectBar = m_rectScroll;
		m_rectBar.left = left + m_nBarPos;
		m_rectBar.right = m_rectBar.left + m_nBarWidth;

		m_pBar->MoveWindow(m_rectBar);
		InvalidateRect(&m_rectScroll);

		if (sendToParent)
		{
			::SendMessage(::GetParent(m_hWnd), WM_HSCROLL, MAKELONG(SB_THUMBTRACK, 0), (LPARAM)this);
		}
		
	}
}

void QYScrollCtrl::UpdateVertLayout(int offHeight, BOOL sizeChange, BOOL sendToParent)
{
	QYRect rect;
	GetClientRect(rect);
	int top = m_rectScrollArrow[ARROW_TYPE_UP].bottom;
	int lastBarPos = m_nBarPos;
	m_nBarPos = m_nDownPos + offHeight ;
	if (0 < offHeight)
	{
		if (m_nBarPos >= (m_rectScroll.Height() - m_nBarHeight))
		{
			m_nBarPos = m_rectScroll.Height() - m_nBarHeight;
		}
	}
	else
	{
		m_nBarPos = (m_nBarPos <= 0) ? 0 : m_nBarPos;
	}
	
	if (m_nBarPos != lastBarPos)
	{
		m_rectBar = m_rectScroll;
		m_rectBar.top = top + m_nBarPos;
		m_rectBar.bottom = m_rectBar.top + m_nBarHeight;
		
		m_pBar->MoveWindow(m_rectBar);
		InvalidateRect(&m_rectScroll);
		
		if (sendToParent)
		{
			::SendMessage(::GetParent(m_hWnd), WM_VSCROLL, MAKELONG(sizeChange ? SB_PAGEDOWN : SB_THUMBTRACK, 0), (LPARAM)this);
		}
		
	}
}

int  QYScrollCtrl::GetMaxRange()
{
	QYRect rect;
	GetClientRect(rect);

	if (SB_VERT == m_dockFlag)
	{
		m_rectScrollArrow[ARROW_TYPE_UP] = rect;
		m_rectScrollArrow[ARROW_TYPE_UP].bottom = rect.Width();
		m_rectScrollArrow[ARROW_TYPE_DOWN] = rect;
		m_rectScrollArrow[ARROW_TYPE_DOWN].top = rect.bottom - rect.Width();
		m_rectBar = m_rectScroll = rect;
		m_rectScroll.top = m_rectScrollArrow[ARROW_TYPE_UP].bottom;
		m_rectScroll.bottom = m_rectScrollArrow[ARROW_TYPE_DOWN].top;
		return m_rectScroll.Height() - m_nMinBarWidth;
	}
	else
	{
		m_rectScrollArrow[ARROW_TYPE_LEFT] = rect;
		m_rectScrollArrow[ARROW_TYPE_LEFT].right = rect.Height();
		m_rectScrollArrow[ARROW_TYPE_RIGHT] = rect;
		m_rectScrollArrow[ARROW_TYPE_RIGHT].left = rect.right - rect.Height();

		m_rectBar = m_rectScroll = rect;
		m_rectScroll.left = m_rectScrollArrow[ARROW_TYPE_LEFT].right;
		m_rectScroll.right = m_rectScrollArrow[ARROW_TYPE_RIGHT].left;

		return m_rectScroll.Width() - m_nMinBarWidth;
	}
}

LRESULT QYScrollCtrl::OnSize(UINT nType, int cx, int cy)
{
	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	if (SB_VERT == m_dockFlag)
	{
		m_rectScrollArrow[ARROW_TYPE_UP] = rect;
		m_rectScrollArrow[ARROW_TYPE_UP].bottom = rect.Width();
		m_rectScrollArrow[ARROW_TYPE_DOWN] = rect;
		m_rectScrollArrow[ARROW_TYPE_DOWN].top = rect.bottom - rect.Width();

		m_rectBar = m_rectScroll = rect;
		m_rectScroll.top = m_rectScrollArrow[ARROW_TYPE_UP].bottom;
		m_rectScroll.bottom = m_rectScrollArrow[ARROW_TYPE_DOWN].top;
	
	    m_nBarHeight = GetAccurateScrollBarHeight(m_rectScroll.Height());

		m_rectBar.top = m_rectScrollArrow[ARROW_TYPE_UP].bottom + m_nBarPos;
		m_rectBar.bottom = m_rectBar.top + m_nBarHeight;

		if (m_rectBar.bottom > m_rectScroll.bottom)
		{
			m_nDownPos = m_nBarPos;
			UpdateVertLayout(m_rectScroll.bottom - m_rectBar.bottom,TRUE);
		}
		else
		{
			InvalidateRect(&m_rectScroll);
			m_pBar->MoveWindow(m_rectBar);
		}
		
	}
	else
	{
		m_rectScrollArrow[ARROW_TYPE_LEFT] = rect;
		m_rectScrollArrow[ARROW_TYPE_LEFT].right = rect.Height();
		m_rectScrollArrow[ARROW_TYPE_RIGHT] = rect;
		m_rectScrollArrow[ARROW_TYPE_RIGHT].left = rect.right - rect.Height();

		m_rectBar = m_rectScroll = rect;
		m_rectScroll.left = m_rectScrollArrow[ARROW_TYPE_LEFT].right;
		m_rectScroll.right = m_rectScrollArrow[ARROW_TYPE_RIGHT].left;

		m_nBarWidth = GetAccurateScrollBarHeight(m_rectScroll.Width());

		m_rectBar.left = m_rectScrollArrow[ARROW_TYPE_LEFT].right + m_nBarPos ;
		m_rectBar.right = m_rectBar.left + m_nBarWidth;

		if (m_rectBar.right > m_rectScroll.right)
		{
			m_nDownPos = m_nBarPos;
			UpdateHorzLayout(m_rectScroll.right - m_rectBar.right);
		}
		else
		{
			InvalidateRect(&m_rectScroll);

			m_pBar->MoveWindow(m_rectBar);
		}
	}

	return TRUE;
}

void QYScrollCtrl::DrawArrow(HDC hDC, QYRect *rect, int type)
{
	
	HPEN hBtnPen = nullptr;

	hBtnPen = CreatePen(PS_SOLID, QY_THEME.SMALL_ARROW_PEN_WIDTH, QY_THEME.SCROLLBAR_ARROW_COLOR_NORMAL);

	//HPEN hHoverBtnPen = CreatePen(PS_SOLID, SMALL_ARROW_PEN_WIDTH, SCROLLBAR_ARROW_COLOR_NORMAL);
	//HPEN hDownBtnPen = CreatePen(PS_SOLID, SMALL_ARROW_PEN_WIDTH, SCROLLBAR_ARROW_COLOR_NORMAL);

	::SelectObject(hDC, hBtnPen);

	RECT rectReal;
	int sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen ) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen ;
	rectReal.right = rectReal.left + sideLen;

	POINT pt;
	if (ARROW_TYPE_UP == type)
	{
		if (m_downArrow == type)
		{
			OffsetRect(&rectReal, 0, -1);
		}

			MoveToEx(hDC, rectReal.left, rectReal.bottom, &pt);
			LineTo(hDC, rectReal.left + sideLen / 2, rectReal.bottom - sideLen / 2);
			LineTo(hDC, rectReal.left + sideLen + 1, rectReal.bottom + 1);
	}
	else if (ARROW_TYPE_DOWN == type)
	{
		if (m_downArrow == type)
		{
			OffsetRect(&rectReal, 0, 1);
		}
			MoveToEx(hDC, rectReal.left, rectReal.top, &pt);
			LineTo(hDC, rectReal.left + sideLen / 2, rectReal.top + sideLen / 2);

			LineTo(hDC, rectReal.left + sideLen + 1, rectReal.top -1);
	}
	else if (ARROW_TYPE_LEFT == type)
	{
		if (m_downArrow == type)
		{
			OffsetRect(&rectReal, -1, 0);
		}

		MoveToEx(hDC, rectReal.right, rectReal.top, &pt);
		LineTo(hDC, rectReal.right - sideLen / 2, rectReal.top + sideLen / 2);

		LineTo(hDC, rectReal.right + 1, rectReal.bottom + 1);
	}
	else if (ARROW_TYPE_RIGHT == type)
	{
		if (m_downArrow == type)
		{
			OffsetRect(&rectReal, 1, 0);
		}
		MoveToEx(hDC, rectReal.left, rectReal.top, &pt);
		LineTo(hDC, rectReal.left + sideLen / 2, rectReal.top + sideLen / 2);

		LineTo(hDC, rectReal.left - 1, rectReal.bottom + 1);
	}

	::DeleteObject(hBtnPen);
}

LRESULT QYScrollCtrl::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	QYBrush Brush(QY_THEME.LISTBOX_BK_COLOR_NORMAL);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	//MemDc.Rectangle(rect);
	MemDc.FillRect(rect, Brush);

	if (SB_VERT == m_dockFlag)
	{
		DrawArrow(MemDc.GetDC(), &m_rectScrollArrow[ARROW_TYPE_DOWN], ARROW_TYPE_DOWN);
		DrawArrow(MemDc.GetDC(), &m_rectScrollArrow[ARROW_TYPE_UP], ARROW_TYPE_UP);
	}
	else
	{
		DrawArrow(MemDc.GetDC(), &m_rectScrollArrow[ARROW_TYPE_LEFT], ARROW_TYPE_LEFT);
		DrawArrow(MemDc.GetDC(), &m_rectScrollArrow[ARROW_TYPE_RIGHT], ARROW_TYPE_RIGHT);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.SelectObject(OldBrush);
	MemDc.SelectObject(OldPen);
	MemDc.DeleteDC();
	PenLine.DeleteObject();
	Brush.DeleteObject();

	return TRUE;
}

LRESULT QYScrollCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	QYRect rect;
	GetClientRect(rect);

	SetCapture();

	if (m_rectBar.PtInRect(point))
	{
		m_bDownScrollBar = TRUE;
		m_LastPoint = point;
		m_nDownPos = m_nBarPos;
		m_pBar->m_bDownScrollBar = TRUE;
		m_pBar->Invalidate();
	}
	else 
	{
		if (SB_VERT == m_dockFlag)
		{
			int nBarHeight = GetAccurateScrollBarHeight(m_rectScroll.Height());
			int type = 0;
			if (m_rectScrollArrow[ARROW_TYPE_UP].PtInRect(point))
			{
				m_downArrow = ARROW_TYPE_UP;
				InvalidateRect(&m_rectScrollArrow[m_downArrow]);
				m_nBarPos -= _buttonStep;
				m_nBarPos = (m_nBarPos < 0) ? 0 : m_nBarPos;
				type = SB_LINEUP;
			}
			else if (m_rectScrollArrow[ARROW_TYPE_DOWN].PtInRect(point))
			{
				m_downArrow = ARROW_TYPE_DOWN;
				InvalidateRect(&m_rectScrollArrow[m_downArrow]);

				m_nBarPos += _buttonStep;
				if (m_nBarPos > (m_rectScroll.Height() - m_nBarHeight))
				{
					m_nBarPos = m_rectScroll.Height() - m_nBarHeight;
				}
				type = SB_LINEDOWN;
			}
			else if (point.y < m_rectBar.top)
			{
				type = SB_PAGEUP;
				m_nBarPos = (point.y - m_rectScrollArrow[ARROW_TYPE_UP].bottom) ;
				m_nBarPos = (m_nBarPos <= 0) ? 0 : m_nBarPos;
			}
			else if (point.y > m_rectBar.bottom)
			{
				type = SB_PAGEDOWN;
				m_nBarPos = (point.y  - m_rectBar.Height() - m_rectScrollArrow[ARROW_TYPE_UP].bottom) ;
				if (m_nBarPos >= (m_rectScroll.Height() - m_nBarHeight))
				{
					m_nBarPos = m_rectScroll.Height() - m_nBarHeight;
				}
			}
			else
			{
				return TRUE;
			}

			int top = m_rectScrollArrow[ARROW_TYPE_UP].bottom;
			m_rectBar = m_rectScroll;
			m_rectBar.top = top + m_nBarPos ;
			m_rectBar.bottom = m_rectBar.top + m_nBarHeight;
			m_pBar->MoveWindow(m_rectBar);
			::SendMessage(::GetParent(m_hWnd), WM_VSCROLL, MAKELONG(type, 0), (LPARAM)this);
		}
		else
		{
			int nBarWidth = GetAccurateScrollBarHeight(m_rectScroll.Width());
			int type = 0;
			if (m_rectScrollArrow[ARROW_TYPE_LEFT].PtInRect(point))
			{
				m_downArrow = ARROW_TYPE_LEFT;
				InvalidateRect(&m_rectScrollArrow[m_downArrow]);
				m_nBarPos -= _buttonStep;
				m_nBarPos = (m_nBarPos < 0) ? 0 : m_nBarPos ;
				type = SB_LINELEFT;
			}
			else if (m_rectScrollArrow[ARROW_TYPE_RIGHT].PtInRect(point))
			{
				m_downArrow = ARROW_TYPE_RIGHT;
				InvalidateRect(&m_rectScrollArrow[m_downArrow]);
				m_nBarPos += _buttonStep;
				if (m_nBarPos >= (m_rectScroll.Width() - m_nBarWidth))
				{
					m_nBarPos = m_rectScroll.Width() - m_nBarWidth;
				}
				type = SB_LINERIGHT;
			}
			else if (point.y < m_rectBar.top)
			{
				type = SB_PAGELEFT;
				m_nBarPos = (point.y - m_rectScrollArrow[ARROW_TYPE_LEFT].bottom) / m_nStep;
				m_nBarPos = (m_nBarPos <= 0) ? 0 : m_nBarPos;
			}
			else if (point.y > m_rectBar.bottom)
			{
				type = SB_PAGERIGHT;
				m_nBarPos = (point.y - m_rectBar.Width() - m_rectScrollArrow[ARROW_TYPE_RIGHT].bottom) / m_nStep;
				if (m_nBarPos >= (m_rectScroll.Width() - m_nBarWidth))
				{
					m_nBarPos = m_rectScroll.Width() - m_nBarWidth;
				}
			}
			else
			{
				return TRUE;
			}

			int left = m_rectScrollArrow[ARROW_TYPE_LEFT].right;
			m_rectBar = m_rectScroll;
			m_rectBar.left = left + m_nBarPos;
			m_rectBar.right = m_rectBar.left + m_nBarWidth;
			m_pBar->MoveWindow(m_rectBar);
			::SendMessage(::GetParent(m_hWnd), WM_HSCROLL, MAKELONG(type, m_nBarPos), (LPARAM)this);
		}
	}
	
	return TRUE;
}

LRESULT QYScrollCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	if (m_bDownScrollBar)
	{
		m_bDownScrollBar = FALSE;
		
		m_pBar->SendMessage(WM_LBUTTONUP, nFlags, MAKELONG(point.x - m_rectBar.left, point.y - m_rectBar.top));
		if (SB_VERT == m_dockFlag)
		{
			::SendMessage(::GetParent(m_hWnd), WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, m_nBarPos), (LPARAM)this);
		}
		else if (SB_HORZ == m_dockFlag)
		{
			::SendMessage(::GetParent(m_hWnd), WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, m_nBarPos), (LPARAM)this);
		}
	}

	if (-1 != m_downArrow)
	{
		int last = m_downArrow;
		m_downArrow = -1;
		InvalidateRect(&m_rectScrollArrow[last]);
	}

	return TRUE;
}

