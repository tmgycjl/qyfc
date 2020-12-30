#include "QYDateCtrl.h"
#include "QYDC.h"


QYDateCtrl::QYDateCtrl() :QYWindow()
{
	nMargin = 2;
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bLBtDown = false;
	m_bFocus = FALSE;
	m_selField = DATE_FIELD_YEAR;
	m_nClientPadWidth = 5;
	m_bBeginEdit = FALSE;
	m_bHotDateButton = FALSE;

	m_nBorderWidth = 1;

	m_pDropDateTable = nullptr;
}

QYDateCtrl::~QYDateCtrl()
{
}


BOOL QYDateCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYDateCtrl",
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

LRESULT QYDateCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
		switch (message)
		{
		case WM_NCDESTROY:
		{
			SAFE_DESTROY_WINDOW_PTR(m_pDropDateTable);
		}
			break;
		case WM_CHAR:{OnChar(wParam, lParam);}break;
		case WM_RBUTTONUP:return TRUE;break;
		case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_ERASEBKGND:case WM_PAINT:{return OnPaint();break;}
		case WM_MOUSEMOVE:{return OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam)));}break;
		case WM_MOUSELEAVE:{return OnMouseLeave(wParam, lParam);}
		case WM_SETFOCUS:{return  OnSetFocus();}break;
		case WM_SIZE:{OnSize(LOWORD(lParam), HIWORD(lParam));}break;
		case WM_KILLFOCUS:{LoseFocus();}break;
		case WM_KEYDOWN:{OnKeyDown(wParam, lParam);}break;
		case CB_SELECTSTRING:
		{
			QYDate *pDate = (QYDate*)wParam;
			if (nullptr != pDate)
			{
				SetDate(pDate->GetYear(), pDate->GetMonth(), pDate->GetDay());
				::SendMessage(::GetParent(m_hWnd), CB_SELECTSTRING, wParam, 0);
			}

			return TRUE;
		}
		break;
		default:break;
		}

	return 0L;
}


LRESULT QYDateCtrl::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_UP:
	case VK_LEFT:
	{
		if (DATE_FIELD_YEAR < m_selField)
		{
			m_selField = (DATE_FIELD)((int)m_selField - 1);
		}
		else
		{
			m_selField = DATE_FIELD_DAY;
		}
		Invalidate(FALSE);
		
		return TRUE;
	}
	case VK_DOWN:
	case VK_RIGHT:
	{
		if (DATE_FIELD_DAY > m_selField)
		{
			m_selField = (DATE_FIELD)((int)m_selField + 1);
		}
		else
		{
			m_selField = DATE_FIELD_YEAR;
		}

		Invalidate(FALSE);
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}

LRESULT QYDateCtrl::OnSetFocus()
{
	m_bFocus = TRUE;
	Invalidate();

	return  TRUE;
}

LRESULT QYDateCtrl::OnSize(int cx, int cy)
{
	return TRUE;
}

LRESULT QYDateCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	::SetFocus(m_hWnd);

	QYRect rcTimeField;
	GetClientRect(rcTimeField);

	QYRect rectArrow = rcTimeField;
	rectArrow.right = rcTimeField.right - m_nBorderWidth;
	rectArrow.left = rcTimeField.right - rcTimeField.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;

	if (rectArrow.PtInRect(point))
	{
		if (nullptr == m_pDropDateTable)
		{
			m_pDropDateTable = new QYDateTable(TRUE);
		}

		if (nullptr == m_pDropDateTable->GetHwnd())
		{
			m_pDropDateTable->ShowShadow(TRUE);
			if (!m_pDropDateTable->Create(0, NULL, NULL, WS_CHILD, QYRect(0, 0, 0, 0),
				::GetDesktopWindow()))
			{
				return FALSE;
			}
		}

		if (m_pDropDateTable->IsWindowVisible())
		{
			m_pDropDateTable->Show(SW_HIDE);
		}
		else
		{
			m_pDropDateTable->Reset();
			QYRect rectWnd;
			GetWindowRect(rectWnd);
			m_pDropDateTable->TrackPopup(QYPoint(rectWnd.left, rectWnd.bottom), rectWnd.Width(), TPDL_RIGHT_BOTTOM, this,GetDate());

			m_pDropDateTable->Show(SW_SHOW);
		}

	}

	rcTimeField.OffsetRect(m_nClientPadWidth, 0);

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);
	BOOL bFind = FALSE;
	if (!bFind 
		&& (PtInField(point, m_chYear, DATE_FIELD_YEAR, &dc, &rcTimeField) || PtInField(point, L"-", (DATE_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_YEAR != m_selField)
		{
			m_selField = DATE_FIELD_YEAR;
			Invalidate(FALSE);
		}
		bFind = TRUE;
	}
	;

	if (!bFind 
		&& (PtInField(point, m_chMonth, DATE_FIELD_MONTH, &dc, &rcTimeField) || PtInField(point, L"-", (DATE_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_MONTH != m_selField)
		{
			m_selField = DATE_FIELD_MONTH;
			Invalidate(FALSE);
		}
		bFind = TRUE;
	}
	
	if (!bFind 
		&& (PtInField(point, m_chDay, DATE_FIELD_DAY, &dc, &rcTimeField) || PtInField(point, L" ", (DATE_FIELD)-1, &dc, &rcTimeField)))
	{
		//if (DATETIME_FIELD_DAY != m_selField)
		{
			m_selField = DATE_FIELD_DAY;
			Invalidate(FALSE);
		}
		bFind = TRUE;
	}

	
	m_bBeginEdit = TRUE;
	return TRUE;
}


LRESULT QYDateCtrl::OnChar(WPARAM wparam, LPARAM lparam)
{
	char ch = wparam;
	if ('0' <= ch && '9' >= ch)
	{
		switch (m_selField)
		{
		case DATE_FIELD_YEAR:
		{
			TCHAR  chYear[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chYear, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chYear, 128, L"%s%c", m_chYear, ch);
			}

			int year = SafeA2I(chYear);
			if (0 > year || 10000 <= year)
			{
				SafeSprintf(m_chYear, 5, L"000%c", ch);
			}
			else
			{
				SafeSprintf(m_chYear, 5, L"%04d", year);
			}
			
			Invalidate(FALSE);
		}
		break;
		case DATE_FIELD_MONTH:
		{
			TCHAR  chSecond[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chSecond, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chSecond, 128, L"%s%c", m_chMonth, ch);
			}

			int sec = SafeA2I(chSecond);
			if (1 > sec || 12 < sec)
			{
				SafeSprintf(m_chMonth, 3, L"0%c", ch);
			}
			else
			{
				SafeSprintf(m_chMonth, 3, L"%02d", sec);
			}

			Invalidate(FALSE);
		}
			break;
		case DATE_FIELD_DAY:
		{
			TCHAR  chSecond[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chSecond, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chSecond, 128, L"%s%c", m_chDay, ch);
			}

			int sec = SafeA2I(chSecond);
			int year = SafeA2I(m_chYear);
			int month = SafeA2I(m_chMonth);
			
			if (1 > sec || QYDateTime::GetDaysByYearAndMonth(year, month) < sec)
			{
				SafeSprintf(m_chDay, 3, L"0%c", ch);
			}
			else
			{
				SafeSprintf(m_chDay, 3, L"%02d", sec);
			}

			Invalidate(FALSE);
		}
			break;
		default:break;
		}
	}
	

	return FALSE;
}

LRESULT QYDateCtrl::LoseFocus()
{
	m_bFocus = FALSE;
	//m_selField = (DATETIME_FIELD)-1;
	Invalidate();

	return TRUE;
}


LRESULT QYDateCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHot)
	{
		if (m_bHotDateButton)
		{
			m_bHotDateButton = FALSE;
		}
		m_bHot = FALSE;
		//m_selField = (DATETIME_FIELD)-1;
		Invalidate();
	}

	return TRUE;
}

LRESULT QYDateCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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

	if (rectArrow.PtInRect(point))
	{
		if (!m_bHotDateButton)
		{
			m_bHotDateButton = TRUE;
			InvalidateRect(&rectArrow);
		}
	}
	else
	{
		if (m_bHotDateButton)
		{
			m_bHotDateButton = FALSE;
			InvalidateRect(&rectArrow);
		}
	}

	return TRUE;
}

LRESULT QYDateCtrl::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	
	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}


void QYDateCtrl::DrawDateArrow(HDC hDC, QYRect *rect)
{
	POINT pt;

	if (m_bHotDateButton)
	{
		RECT rc;
		rc.left = rect->left;
		rc.right = rect->right;
		rc.top = rect->top;
		rc.bottom = rect->bottom;
		HBRUSH  arrowBrush = CreateSolidBrush(QY_THEME.COMBOBOX_ARROW_BK_COLOR_HOVER);
		SelectObject(hDC,arrowBrush);
		FillRect(hDC, &rc, arrowBrush);
		DeleteObject( arrowBrush);

		MoveToEx(hDC, rc.left, rc.top, &pt);
		LineTo(hDC, rc.left, rc.bottom);
	}

	COLORREF color = QY_THEME.COMBOBOX_ARROW_PEN_COLOR_NORMAL;
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

	RECT rectReal;
	int sideLen = min(rect->Height(), rect->Width()) / 2;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen;

	int top = rectReal.top + sideLen / 4;
	MoveToEx(hDC, rectReal.left, top + 1, &pt);
	LineTo(hDC, rectReal.left + sideLen / 2, top + 1 + sideLen / 2);
	LineTo(hDC, rectReal.left + sideLen + 1, top);

	(HPEN)SelectObject(hDC, oldPen);
	DeleteObject(hSysBtnPen);
}


LRESULT QYDateCtrl::OnPaint()
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
	COLORREF clrOld = dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
	TCHAR displayText[128] = { 0 };

	//TEXTMETRIC tm;		// 文本字体的信息。
	//SafeSprintf(displayText, L"%s-%s-%s %s:%s:%s", m_chYear, m_chMonth, m_chDay, m_chHour, m_chMinute, m_chSecond);
	//dc.GetTextMetrics(&tm);

	QYRect rcTimeField = rc;
	rcTimeField.OffsetRect(m_nClientPadWidth, 0);
		DrawField(m_chYear, DATE_FIELD_YEAR,&dc, &rcTimeField);
		DrawField(L"-", (DATE_FIELD)-1, &dc, &rcTimeField);
		DrawField(m_chMonth, DATE_FIELD_MONTH, &dc, &rcTimeField);
		DrawField(L"-", (DATE_FIELD)-1, &dc, &rcTimeField);
		DrawField(m_chDay, DATE_FIELD_DAY, &dc, &rcTimeField);
		DrawField(L" ", (DATE_FIELD)-1, &dc, &rcTimeField);

		QYRect rectArrow = rc;
		rectArrow.right = rc.right - m_nBorderWidth;
		rectArrow.left = rc.right - rc.Height();
		rectArrow.bottom -= m_nBorderWidth;
		rectArrow.top += m_nBorderWidth;
		DrawDateArrow(dc.GetDC(), &rectArrow);

	dc.SetTextColor(clrOld);
	dc.SetBkMode(nOldMode);
	dc.SelectObject(OldFont);

	return TRUE;
}

BOOL QYDateCtrl::PtInField(QYPoint &pt, const TCHAR *timeText, DATE_FIELD fd, QYDC *pDc, QYRect* rect)
{
	QYSize testSize = pDc->GetTextExtent(timeText);
	rect->right = rect->left + testSize.cx;
	BOOL ret = rect->PtInRect(pt);
	rect->OffsetRect(testSize.cx, 0);

	return ret;
}

void QYDateCtrl::DrawField(const TCHAR *timeText, DATE_FIELD fd, QYDC *pDc, QYRect* rect)
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
		pDc->SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
	}
	pDc->DrawText(timeText, SafeStrlen(timeText), rect, DT_VCENTER | DT_SINGLELINE | DT_LEFT);

	rect->OffsetRect(testSize.cx, 0);
}



QYDate QYDateCtrl::GetDate()const
{
	QYDate dt;
	
	dt.SetDate(SafeA2I(m_chYear), SafeA2I(m_chMonth), SafeA2I(m_chDay));

	return dt;
}

void  QYDateCtrl::SetDate(int year, int month, int day)
{
	if (1970 > year || year >= 10000)
	{
		year = 1970;
	}
	SafeSprintf(m_chYear, 5, L"%04d", year);
	//FormatTimeStr(m_chYear, sysTime.wYear);

	if (0 >= month || month > 12)
	{
		month = 1;
	}
	//FormatTimeStr(m_chMonth,sysTime.wMonth);
	SafeSprintf(m_chMonth, 3, L"%02d", month);

	if (0 >= day || day > 31)
	{
		day = 1;
	}
	//FormatTimeStr(m_chDay, sysTime.wDay);
	SafeSprintf(m_chDay, 3, L"%02d", day);

	Invalidate();
}