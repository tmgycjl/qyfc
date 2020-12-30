#include "QYTimeCtrl.h"
#include "QYDC.h"


QYTimeCtrl::QYTimeCtrl() :QYWindow()
{
	nMargin = 2;
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bLBtDown = false;
	m_bFocus = FALSE;
	m_selField = TIME_FIELD_HOUR;
	m_nClientPadWidth = 5;
	m_bBeginEdit = FALSE;
	m_bHotButtonUp = FALSE;
	m_bHotButtonDown = FALSE;
	m_nBorderWidth = 1;
	m_bPressButtonUp = FALSE;
	m_bPressButtonDown = FALSE;
}

QYTimeCtrl::~QYTimeCtrl()
{
}


BOOL QYTimeCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYTimeCtrl",
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

	OnSize(0,0);

	return ret;
}

LRESULT QYTimeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
		switch (message)
		{
		case WM_CHAR:{OnChar(wParam, lParam);}break;
		case WM_RBUTTONUP:return TRUE;break;
		case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_LBUTTONDBLCLK:{OnLButtonDBClick(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
		case WM_ERASEBKGND:
		case WM_PAINT:{return OnPaint();break;}
		case WM_MOUSEMOVE:{return OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam)));}break;
		case WM_MOUSELEAVE:{return OnMouseLeave(wParam, lParam);}
		case WM_SETFOCUS:{return  OnSetFocus();}break;
		case WM_SIZE:{OnSize(LOWORD(lParam), HIWORD(lParam));}break;
		case WM_KILLFOCUS:{LoseFocus();}break;
		case WM_KEYDOWN:{OnKeyDown(wParam, lParam);}break;
		default:{break;}
		}

	return 0L;
}


LRESULT QYTimeCtrl::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_UP:
	case VK_LEFT:
	{
		if (TIME_FIELD_HOUR < (int)m_selField)
		{
			m_selField = (TIME_FIELD)((int)m_selField - 1);
		}
		else
		{
			m_selField = TIME_FIELD_SECOND;
		}
		Invalidate(FALSE);
		
		return TRUE;
	}
	case VK_DOWN:
	case VK_RIGHT:
	{
		if (TIME_FIELD_SECOND > m_selField)
		{
			m_selField = (TIME_FIELD)((int)m_selField + 1);
		}
		else
		{
			m_selField = TIME_FIELD_HOUR;
		}

		Invalidate(FALSE);
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}

LRESULT QYTimeCtrl::OnSetFocus()
{
	m_bFocus = TRUE;
	Invalidate();

	return  TRUE;
}

LRESULT QYTimeCtrl::OnSize(int cx, int cy)
{
	return TRUE;
}


LRESULT QYTimeCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	//::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_ITEM_DBCLICK, m_nSelItem, 0);

	return OnLButtonDown(nFlags, point);
}


LRESULT QYTimeCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	::SetFocus(m_hWnd);

	QYRect rcTimeField;
	GetClientRect(rcTimeField);
	rcTimeField.OffsetRect(m_nClientPadWidth, 0);

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);
	BOOL bFind = FALSE;

	if (!bFind 
		&& (PtInField(point, m_chHour, TIME_FIELD_HOUR, &dc, &rcTimeField) || PtInField(point, L":", (TIME_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_HOUR != m_selField)
		{
			m_selField = TIME_FIELD_HOUR;
			Invalidate(FALSE);
		}
	}
	
	;
	if (!bFind 
		&& (PtInField(point, m_chMinute, TIME_FIELD_MINUTE, &dc, &rcTimeField) || PtInField(point, L":", (TIME_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_MINUTE != m_selField)
		{
			m_selField = TIME_FIELD_MINUTE;
			Invalidate(FALSE);
			
		}
		bFind = TRUE;
	}

	if (!bFind 
		&& (PtInField(point, m_chSecond, TIME_FIELD_SECOND, &dc, &rcTimeField) || PtInField(point, L" ", (TIME_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_SECOND != m_selField)
		{
			m_selField = TIME_FIELD_SECOND;
			Invalidate(FALSE);
			
		}
		bFind = TRUE;
	}


	QYRect rc;
	GetClientRect(rc);
	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;
	rectArrow.bottom = rectArrow.top + (rc.Height() - (2 * m_nBorderWidth)) / 2;


	if (rectArrow.PtInRect(point))
	{
		if (!m_bPressButtonUp)
		{
			m_bPressButtonUp = TRUE;
			InvalidateRect(&rectArrow);
		}

		switch (m_selField)
		{
		case TIME_FIELD_HOUR:
		{
			int hour = SafeA2I(m_chHour);
			hour++;
			if (24 <= hour)
			{
				hour = 0;
			}
			SafeSprintf(m_chHour, 3, L"%02d", hour);
			Invalidate(FALSE);
		}
		break;
		case TIME_FIELD_MINUTE:
		{
			int minute = SafeA2I(m_chMinute);
			minute++;
			if ( 60 <= minute)
			{
				minute = 0;
			}
			SafeSprintf(m_chMinute, 3, L"%02d", minute);

			Invalidate(FALSE);
		}
		break;
		case TIME_FIELD_SECOND:
		{
			int sec = SafeA2I(m_chSecond);
			sec++;
			if ( 60 <= sec)
			{
				sec = 0;
			}
			SafeSprintf(m_chSecond, 3, L"%02d", sec);

			Invalidate(FALSE);
		}
		break;
		default:break;
		}
	}

	rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top = rectArrow.bottom - (rc.Height() - (2 * m_nBorderWidth)) / 2;
	if (rectArrow.PtInRect(point))
	{
		if (!m_bPressButtonDown)
		{
			m_bPressButtonDown = TRUE;
			InvalidateRect(&rectArrow);
		}

		switch (m_selField)
		{
		case TIME_FIELD_HOUR:
		{
			int hour = SafeA2I(m_chHour);
			hour--;
			if (0 > hour )
			{
				hour = 23;
			}

			SafeSprintf(m_chHour, 3, L"%02d", hour);

			Invalidate(FALSE);
		}
		break;
		case TIME_FIELD_MINUTE:
		{
			int minute = SafeA2I(m_chMinute);
			minute--;
			if (0 > minute)
			{
				minute = 59;
			}
			SafeSprintf(m_chMinute, 3, L"%02d", minute);

			Invalidate(FALSE);
		}
		break;
		case TIME_FIELD_SECOND:
		{
			int sec = SafeA2I(m_chSecond);
			sec--;
			if (0 > sec)
			{
				sec = 59;
			}
			SafeSprintf(m_chSecond, 3, L"%02d", sec);
			Invalidate(FALSE);
		}
		break;
		default:break;
		}
	}

	m_bBeginEdit = TRUE;
	dc.SelectObject(OldFont);

	return TRUE;
}

LRESULT QYTimeCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	QYRect rc;
	GetClientRect(rc);
	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;
	rectArrow.bottom = rectArrow.top + (rc.Height() - (2 * m_nBorderWidth)) / 2;

	if (m_bPressButtonUp)
	{
		m_bPressButtonUp = FALSE;
		InvalidateRect(&rectArrow);
	}

	rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top = rectArrow.bottom - (rc.Height() - (2 * m_nBorderWidth)) / 2;
	if (m_bPressButtonDown)
	{
		m_bPressButtonDown = FALSE;
		InvalidateRect(&rectArrow);
	}
	return TRUE;
}


LRESULT QYTimeCtrl::OnChar(WPARAM wparam, LPARAM lparam)
{
	char ch = wparam;
	if ('0' <= ch && '9' >= ch)
	{
		switch (m_selField)
		{
		case TIME_FIELD_HOUR:
		{
			TCHAR  chSecond[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chSecond, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chSecond, 128, L"%s%c", m_chHour, ch);
			}

			int sec = SafeA2I(chSecond);
			if (0 > sec || 24 <= sec)
			{
				SafeSprintf(m_chHour, 3, L"0%c", ch);
			}
			else
			{
				SafeSprintf(m_chHour, 3, L"%02d", sec);
			}

			Invalidate(FALSE);
		}
			break;
		case TIME_FIELD_MINUTE:
		{
			TCHAR  chSecond[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chSecond, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chSecond, 128, L"%s%c", m_chMinute, ch);
			}

			int sec = SafeA2I(chSecond);
			if (0 > sec || 60 <= sec)
			{
				SafeSprintf(m_chMinute, 3, L"0%c", ch);
			}
			else
			{
				SafeSprintf(m_chMinute, 3, L"%02d", sec);
			}

			Invalidate(FALSE);
		}
			break;
		case TIME_FIELD_SECOND:
		{
			TCHAR  chSecond[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chSecond, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chSecond, 128, L"%s%c", m_chSecond, ch);
			}

			int sec = SafeA2I(chSecond);
			if (0 > sec || 60 <= sec)
			{
				SafeSprintf(m_chSecond, 3, L"0%c", ch);
			}
			else
			{
				SafeSprintf(m_chSecond, 3, L"%02d", sec);
			}

			Invalidate(FALSE);
		}
			break;
		default:break;
		}
	}
	
	return FALSE;
}

LRESULT QYTimeCtrl::LoseFocus()
{
	m_bFocus = FALSE;
	//m_selField = (DATETIME_FIELD)-1;
	Invalidate();

	return TRUE;
}


LRESULT QYTimeCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHot)
	{
		if (m_bHotButtonUp)
		{
			m_bHotButtonUp = FALSE;
		}

		if (m_bHotButtonDown)
		{
			m_bHotButtonDown = FALSE;
		}

		m_bHot = FALSE;
		//m_selField = (DATETIME_FIELD)-1;
		Invalidate();
	}

	return TRUE;
}

LRESULT QYTimeCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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

		Invalidate();
	}

	QYRect rc;
	GetClientRect(rc);
	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;
	rectArrow.bottom = rectArrow.top + (rc.Height() - (2 * m_nBorderWidth)) / 2;
	

	if (rectArrow.PtInRect(point))
	{
		if (!m_bHotButtonUp)
		{
			m_bHotButtonUp = TRUE;
			InvalidateRect(&rectArrow);
		}
	}
	else
	{
		if (m_bHotButtonUp)
		{
			m_bHotButtonUp = FALSE;
			InvalidateRect(&rectArrow);
		}
	}


	rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top = rectArrow.bottom - (rc.Height() - (2 * m_nBorderWidth)) / 2;
	if (rectArrow.PtInRect(point))
	{
		if (!m_bHotButtonDown)
		{
			m_bHotButtonDown = TRUE;
			InvalidateRect(&rectArrow);
		}
	}
	else
	{
		if (m_bHotButtonDown)
		{
			m_bHotButtonDown = FALSE;
			InvalidateRect(&rectArrow);
		}
	}

	return TRUE;
}

LRESULT QYTimeCtrl::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	
	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}


void QYTimeCtrl::DrawTimeArrow(HDC hDC, QYRect *rect, bool bUp )
{
	POINT pt;
	RECT rectReal;

	COLORREF color = QY_THEME.COMBOBOX_ARROW_PEN_COLOR_NORMAL;
	COLORREF btnCor = QY_THEME.COMBOBOX_ARROW_BK_COLOR_HOVER;
	if (m_bFocus)
	{
		color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
	}
	else
	{
		if (m_bHot)
		{
			color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
		}
	}

	HPEN hSysBtnPen = CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, color);
	HPEN oldPen = (HPEN)SelectObject(hDC, hSysBtnPen);

	int sideLen = min(rect->Height(), rect->Width()) / 2;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen;

	if (bUp)
	{
		if (m_bHotButtonUp)
		{
			if (m_bPressButtonUp)
			{
				btnCor = QY_THEME.COMBOBOX_ARROW_BK_COLOR_DOWN;
			}

			HBRUSH  arrowBrush = CreateSolidBrush(btnCor);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, arrowBrush);
			//FillRect(hDC, &rc, arrowBrush);
			Rectangle(hDC, rect->left, rect->top, rect->right, rect->bottom);
			SelectObject(hDC, oldBrush);
			DeleteObject(arrowBrush);
		}

		int bottom = rectReal.bottom  - sideLen / 4;
		MoveToEx(hDC, rectReal.left, bottom - 1, &pt);
		LineTo(hDC, rectReal.left + sideLen / 2, bottom - 1 - sideLen / 2);
		LineTo(hDC, rectReal.left + sideLen + 1, bottom);
	}
	else
	{
		if (m_bHotButtonDown)
		{
			if (m_bPressButtonDown)
			{
				btnCor = QY_THEME.COMBOBOX_ARROW_BK_COLOR_DOWN;
			}

			HBRUSH  arrowBrush = CreateSolidBrush(btnCor);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, arrowBrush);
			//FillRect(hDC, &rc, arrowBrush);
			Rectangle(hDC, rect->left, rect->top, rect->right, rect->bottom);
			SelectObject(hDC, oldBrush);
			DeleteObject(arrowBrush);

		}

		int top = rectReal.top + sideLen / 4;
		MoveToEx(hDC, rectReal.left, top + 1, &pt);
		LineTo(hDC, rectReal.left + sideLen / 2, top + 1 + sideLen / 2);
		LineTo(hDC, rectReal.left + sideLen + 1, top);
	}

	(HPEN)SelectObject(hDC, oldPen);
	DeleteObject(hSysBtnPen);
}


LRESULT QYTimeCtrl::OnPaint()
{
	QYPaintDC dc(this);
	QYPen PenLine;
	HPEN OldPen;
	HFONT OldFont;

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

	dc.Rectangle(rc);
 	Brush.DeleteObject();
 	PenLine.DeleteObject();


	OldFont = (HFONT)dc.SelectObject(m_pFont);

	int nOldMode = dc.SetBkMode(TRANSPARENT);
	COLORREF clrOld = dc.SetTextColor(RGB(0, 0, 0));
	QYRect rcTimeField = rc;
	rcTimeField.OffsetRect(m_nClientPadWidth, 0);

		DrawField(m_chHour, TIME_FIELD_HOUR, &dc, &rcTimeField);
		DrawField(L":", (TIME_FIELD)-1, &dc, &rcTimeField);
		DrawField(m_chMinute, TIME_FIELD_MINUTE, &dc, &rcTimeField);
		DrawField(L":", (TIME_FIELD)-1, &dc, &rcTimeField);
		DrawField(m_chSecond, TIME_FIELD_SECOND, &dc, &rcTimeField);
		DrawField(L" ", (TIME_FIELD)-1, &dc, &rcTimeField);



	dc.SetTextColor(clrOld);
	dc.SetBkMode(nOldMode);
	dc.SelectObject(OldFont);

	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;
	rectArrow.bottom = rectArrow.top + (rc.Height() - (2 * m_nBorderWidth)) / 2;

	DrawTimeArrow(dc.GetDC(), &rectArrow,true);

	rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top = rectArrow.bottom - (rc.Height() - (2 * m_nBorderWidth)) / 2;


	DrawTimeArrow(dc.GetDC(), &rectArrow,false);

	return TRUE;
}

BOOL QYTimeCtrl::PtInField(QYPoint &pt, const TCHAR *timeText, TIME_FIELD fd, QYDC *pDc, QYRect* rect)
{
	QYSize testSize = pDc->GetTextExtent(timeText);
	rect->right = rect->left + testSize.cx;
	BOOL ret = rect->PtInRect(pt);
	rect->OffsetRect(testSize.cx, 0);

	return ret;
}

void QYTimeCtrl::DrawField(const TCHAR *timeText, TIME_FIELD fd, QYDC *pDc, QYRect* rect)
{
	QYSize testSize = pDc->GetTextExtent(timeText);
	rect->right = rect->left + testSize.cx;
	
	if (m_bFocus && fd == m_selField)
	{
		int top = rect->top + (rect->Height() - testSize.cy) / 2;
		pDc->FillSolidRect(rect->left, top, rect->right, top + testSize.cy, QY_THEME.LISTBOX_BK_COLOR_SELECT);
		pDc->SetTextColor(QY_THEME.TEXT_COLOR_SELECT);
	}
	else
	{
		pDc->SetTextColor(QY_THEME.TAB_TEXT_COLOR_NORMAL);
	}
	pDc->DrawText(timeText, SafeStrlen(timeText), rect, DT_VCENTER | DT_SINGLELINE | DT_LEFT);

	rect->OffsetRect(testSize.cx, 0);
}

QYTime& QYTimeCtrl::GetTime()const
{
	QYTime dt;

	dt.SetTime(SafeA2I(m_chHour), SafeA2I(m_chMinute), SafeA2I(m_chSecond));

	return dt;
}



void QYTimeCtrl::SetTime(int hour, int mintue, int second)
{
	if (0 > hour || hour >= 60)
	{
		hour = 0;
	}
	//FormatTimeStr(m_chHour, sysTime.wHour);
	SafeSprintf(m_chHour, 3, L"%02d", hour);
	if (0 > mintue || mintue >= 60)
	{
		mintue = 0;
	}
	//FormatTimeStr(m_chMinute, sysTime.wMinute);
	SafeSprintf(m_chMinute, 3, L"%02d", mintue);
	if (0 > second || second >= 60)
	{
		second = 0;
	}
	//FormatTimeStr(m_chSecond, sysTime.wSecond);
	SafeSprintf(m_chSecond, 3, L"%02d", second);
}
