#include "QYDateTimeCtrl.h"
#include "QYDC.h"


QYDateTimeCtrl::QYDateTimeCtrl() :QYWindow()
{
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bLBtDown = false;
	m_bFocus = FALSE;
	m_Flag = DATETIME_FLAG_DATE_TIME;
	m_nClientPadWidth = 5;

	m_pDateCtrl = nullptr;
	m_pTimeCtrl = nullptr;
}

QYDateTimeCtrl::~QYDateTimeCtrl()
{
}


BOOL QYDateTimeCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYDateTimeCtrl",
		lpWndName,
		dwStyle,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);

	if (!ret)
	{
		return FALSE;
	}

	if (DATETIME_FLAG_DATE&m_Flag)
	{
		m_pDateCtrl = new QYDateCtrl;

		if (!m_pDateCtrl->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			return FALSE;
		}
	}

	if (DATETIME_FLAG_TIME&m_Flag)
	{
		m_pTimeCtrl = new QYTimeCtrl;
		if (!m_pTimeCtrl->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			return FALSE;
		}
	}

	GetCurrentDateTime();

	OnSize(0,0);

	return ret;
}

void QYDateTimeCtrl::GetCurrentDateTime()
{
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	if ((DATETIME_FLAG_DATE&m_Flag) && nullptr != m_pDateCtrl)
	{
		m_pDateCtrl->SetDate(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	}

	if ((DATETIME_FLAG_TIME&m_Flag) && nullptr != m_pTimeCtrl)
	{
		m_pTimeCtrl->SetTime(sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	}
}

LRESULT QYDateTimeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
		switch (message)
		{
		case WM_NCDESTROY:
		{
			SAFE_DESTROY_WINDOW_PTR(m_pDateCtrl);
			SAFE_DESTROY_WINDOW_PTR(m_pTimeCtrl);
			break;
		}
		case WM_SIZE:{OnSize(LOWORD(lParam), HIWORD(lParam));}break;
		default:break;
		}

	return 0L;
}


LRESULT QYDateTimeCtrl::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_UP:
	case VK_LEFT:
	{
		
		Invalidate(FALSE);
		
		return TRUE;
	}
	case VK_DOWN:
	case VK_RIGHT:
	{
		Invalidate(FALSE);
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}

LRESULT QYDateTimeCtrl::OnSetFocus()
{
	m_bFocus = TRUE;
	Invalidate();

	return  TRUE;
}

LRESULT QYDateTimeCtrl::OnSize(int cx, int cy)
{
	QYRect rc;
	GetClientRect(rc);
	if ((DATETIME_FLAG_DATE_TIME == m_Flag) && nullptr != m_pDateCtrl && nullptr != m_pTimeCtrl)
	{
		QYRect rcCtrl = rc;
		rcCtrl.right = rcCtrl.right * (float)10 / (float)18;

		if (nullptr != m_pDateCtrl->GetHwnd())
		{
			m_pDateCtrl->MoveWindow(rcCtrl);
		}

		rcCtrl.left = rcCtrl.right;
		rcCtrl.right = rc.right;
		if (nullptr != m_pTimeCtrl->GetHwnd())
		{
			m_pTimeCtrl->MoveWindow(rcCtrl);
		}
	}
	else if ((DATETIME_FLAG_DATE == m_Flag) && nullptr != m_pDateCtrl)
	{
		if (nullptr != m_pDateCtrl->GetHwnd())
		{
			m_pDateCtrl->MoveWindow(rc);
		}
	}
	else if ((DATETIME_FLAG_TIME == m_Flag) && nullptr != m_pTimeCtrl)
	{
		if (nullptr != m_pTimeCtrl->GetHwnd())
		{
			m_pTimeCtrl->MoveWindow(rc);
		}
	}

	return TRUE;
}


LRESULT QYDateTimeCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	return TRUE;
}


LRESULT QYDateTimeCtrl::LoseFocus()
{
	m_bFocus = FALSE;
	//m_selField = (DATETIME_FIELD)-1;
	Invalidate();

	return TRUE;
}


LRESULT QYDateTimeCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHot)
	{
		m_bHot = FALSE;
		//m_selField = (DATETIME_FIELD)-1;
		Invalidate();
	}

	return TRUE;
}

LRESULT QYDateTimeCtrl::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHot )
	{
		m_bHot = TRUE;

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = GetHwnd();
		TrackMouseEvent(&tme);
	}

	return TRUE;
}

LRESULT QYDateTimeCtrl::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	
	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}


LRESULT QYDateTimeCtrl::OnPaint()
{
	QYPaintDC dc(this);
	QYPen PenLine;
	HPEN OldPen;
//	HFONT OldFont;

	COLORREF corPen = RGB(135, 155, 200);

		if (m_bFocus)
		{
			corPen = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
		}
		else
		{
			if (m_bHot)
			{
				corPen = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
			}
			else
			{
				corPen = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
			}
		}
	
	PenLine.CreatePen(PS_SOLID, 1, corPen);
	OldPen = (HPEN)dc.SelectObject(&PenLine);

	QYBrush Brush;
	HBRUSH OldBrush;

	Brush.CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);

	OldBrush = (HBRUSH)dc.SelectObject(&Brush);

	QYRect rc;
	GetClientRect(rc);

	//dc.Rectangle(rc);

	dc.SelectObject(OldBrush);

	dc.SelectObject(OldPen);

	Brush.DeleteObject();
	PenLine.DeleteObject();

	return TRUE;
}



void QYDateTimeCtrl::SetFlag(DATETIME_FLAG flag)
{
	m_Flag = flag;
}

QYDateTime QYDateTimeCtrl::GetDateTime()const
{
	QYDateTime dt;
	
	if ((DATETIME_FLAG_DATE&m_Flag) && nullptr != m_pDateCtrl)
	{
		QYDate date = m_pDateCtrl->GetDate();
		dt.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
	}

	if ((DATETIME_FLAG_TIME&m_Flag) && nullptr != m_pTimeCtrl)
	{
		QYTime time = m_pTimeCtrl->GetTime();
		dt.SetTime(time.GetHour(), time.GetMinute(), time.GetSecond());
	}

	return dt;
}

void QYDateTimeCtrl::SetDateTime(QYDateTime& dt)
{
	if ((DATETIME_FLAG_DATE&m_Flag) && nullptr != m_pDateCtrl)
	{
		m_pDateCtrl->SetDate(dt.GetYear(), dt.GetMonth(), dt.GetDay());
	}

	if ((DATETIME_FLAG_TIME&m_Flag) && nullptr != m_pTimeCtrl)
	{
		m_pTimeCtrl->SetTime(dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	}
}

void QYDateTimeCtrl::SetDateTime(int year, int month, int day, int hour, int mintue, int second)
{
	if ((DATETIME_FLAG_DATE&m_Flag) && nullptr != m_pDateCtrl)
	{
		m_pDateCtrl->SetDate(year, month, day);
	}

	if ((DATETIME_FLAG_TIME&m_Flag) && nullptr != m_pTimeCtrl)
	{
		m_pTimeCtrl->SetTime(hour, mintue, second);
	}
}
