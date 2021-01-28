#include "QYProgressCtrl.h"
#include "QYDC.h"

QYProgressCtrl::QYProgressCtrl()
{
	m_GrooveBkCor = QY_THEME.PROGRESS_BK_COLOR;
	m_BkCor = RGB(122, 122, 122);
	m_ProcessBkCor = QY_THEME.PROGRESS_PROCESS_COLOR;
	m_nPos = 0;
	m_nRange = 0;
	m_bHover = FALSE;
}


QYProgressCtrl::~QYProgressCtrl()
{
}

BOOL QYProgressCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	return QYWindow::Create(dwExStyle,
		L"QYProgress",
		lpWndName,
		dwStyle,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
}

void   QYProgressCtrl::SetBkColor(COLORREF grooveBkColor, COLORREF progressBkColor)
{
	m_GrooveBkCor = grooveBkColor;
	m_ProcessBkCor = progressBkColor;
	Invalidate();
}

void QYProgressCtrl::setReadonly(bool readOnly)
{
	_readOnly = readOnly;
}

LRESULT QYProgressCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	case WM_PAINT:
	{
		return OnPaint();
	}
	case WM_NCPAINT:
		//return OnNCPaint();
		break;
	case WM_MOUSEMOVE:
	{
		if (_readOnly)
		{
			break;
		}
		return OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	case WM_MOUSELEAVE:
	{
		if (_readOnly)
		{
			break;
		}
		OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (_readOnly)
		{
			break;
		}
		return OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	case WM_LBUTTONUP:
	{
		if (_readOnly)
		{
			break;
		}
		return OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	default:break;
	}
	return 0L;
}


LRESULT QYProgressCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
	if (nullptr != actionCB)
	{
		QYRect rc;
		GetClientRect(rc);
		int pos = m_nRange * point.x / rc.Width();
		QYPropertyList properties;
		properties.addProperty("id", getID());
		properties.addProperty("action", "lbuttondown");
		properties.addProperty("pos", pos);
		actionCB->callback(&properties);
	}

	::SetCursor(LoadCursor(NULL, IDC_HAND));

	return TRUE;
}


LRESULT QYProgressCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	::SetCursor(LoadCursor(NULL, IDC_HAND));

	return TRUE;
}
LRESULT QYProgressCtrl::OnMouseMove(UINT nFlags, QYPoint &point)
{
	if (!_hover)
	{
		_hover = true;
		if (0 == nFlags)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 100;
			tme.hwndTrack = GetHwnd();
			TrackMouseEvent(&tme);
		}

		
		Invalidate();
	}

	::SetCursor(LoadCursor(NULL, IDC_HAND));


	return nFlags;
}


LRESULT QYProgressCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
		if (_hover)
		{
			_hover = false;

			
			Invalidate();
		}
		::SetCursor(LoadCursor(NULL, IDC_ARROW));


	return TRUE;
}




BOOL QYProgressCtrl::OnPaint()
{
	QYPaintDC paintDC(this);

	QYRect		rt;
	GetClientRect(rt);

	QYMemDC dc(&paintDC, rt);

	QYRect rtGroove = rt;
	rtGroove.top = rt.top + 1;
	rtGroove.bottom = rt.bottom - 1;
	QYRect rtProcess = rt;
	if (0 <  m_nRange)
	{
		rtProcess.right = (float)(rt.Width() * m_nPos) / (float)m_nRange;
	}
	else
	{
		rtProcess.right = 0;
	}
	
	rtProcess.top = rtGroove.top + 1;
	rtProcess.bottom = rtGroove.bottom - 1;
	if (rtProcess.top > rtProcess.bottom)
	{
		rtProcess.bottom = rtProcess.top;
	}

	//dc.Rectangle(rt, PROGRESS_BK_COLOR, BUTTON_BORDER_COLOR_NORMAL);
	dc.FillSolidRect(rt, m_GrooveBkCor);
	dc.FillSolidRect(rtProcess, m_ProcessBkCor);

	paintDC.BitBlt(0, 0, rt.Width(), rt.Height(), &dc, 0, 0, SRCCOPY);

	return TRUE;
}


LRESULT QYProgressCtrl::OnNCPaint()
{
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);

	QYPen pen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_NORMAL);
	HPEN oldpen = (HPEN)pDC->SelectObject(&pen);

	pDC->Rectangle(0, 0, rect.right - rect.left, rect.bottom - rect.top,(HBRUSH)::GetStockObject(NULL_BRUSH));

	pDC->SelectObject(oldpen);

	pen.DeleteObject();

	ReleaseDC(pDC);

	return TRUE;
}

void QYProgressCtrl::SetPos(int pos)
{
	m_nPos = pos;
	Invalidate();
}


void QYProgressCtrl::SetRange(int range)
{
	m_nRange = range;
}

int QYProgressCtrl::GetRange()
{
	return m_nRange;
}


int QYProgressCtrl::GetPos()
{
	return m_nPos;
}
