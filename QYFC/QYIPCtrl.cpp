#include "QYIPCtrl.h"
#include "QYDC.h"


QYIPCtrl::QYIPCtrl() :QYWindow()
{
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bLBtDown = false;
	m_bFocus = FALSE;
	m_selField = -1;
	m_nClientPadWidth = 5;
	m_bBeginEdit = FALSE;
	m_bHotButtonUp = FALSE;
	m_bHotButtonDown = FALSE;
	m_nBorderWidth = 1;
	m_bPressButtonUp = FALSE;
	m_bPressButtonDown = FALSE;

	m_nIPBlockWidth = 0;
	m_nIPPointWidth = 0;
}

QYIPCtrl::~QYIPCtrl()
{
}



BOOL QYIPCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYIPCtrl",
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

	SetValue((DWORD)0);

	OnSize(0,0);

	return ret;
}

LRESULT QYIPCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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


LRESULT QYIPCtrl::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_UP:
	case VK_LEFT:
	{
		if (0 < m_selField)
		{
			m_selField--;
		}
		else
		{
			m_selField = MAX_IPV4_BLOCK_COUNT - 1;
		}

		Invalidate(FALSE);
		
		return TRUE;
	}
	case VK_DOWN:
	case VK_RIGHT:
	{
		if ((MAX_IPV4_BLOCK_COUNT - 1) > m_selField)
		{
			m_selField++;
		}
		else
		{
			m_selField = 0;
		}

		Invalidate(FALSE);
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}

LRESULT QYIPCtrl::OnSetFocus()
{
	m_bFocus = TRUE;
	Invalidate();

	return  TRUE;
}

LRESULT QYIPCtrl::OnSize(int cx, int cy)
{
	QYRect rect;
	GetClientRect(rect);
	QYPaintDC dc(this);
	HFONT  OldFont = (HFONT)dc.SelectObject(m_pFont);

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_nIPPointWidth = tm.tmMaxCharWidth;
	m_nIPBlockWidth = (rect.Width() - (2 * m_nClientPadWidth) - (3 * m_nIPPointWidth)) / 4;
	dc.SelectObject(OldFont);

	return TRUE;
}


LRESULT QYIPCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	//::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_ITEM_DBCLICK, m_nSelItem, 0);

	return FALSE;
}


LRESULT QYIPCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	::SetFocus(m_hWnd);

	QYRect rc;
	GetClientRect(rc);
	rc.OffsetRect(m_nClientPadWidth, 0);

	for (int i = 0; i < MAX_IPV4_BLOCK_COUNT; i++)
	{
		rc.right = rc.left + m_nIPBlockWidth;
		if (rc.PtInRect(point))
		{
			m_selField = i;
			Invalidate(FALSE);
			m_bBeginEdit = TRUE;
			return TRUE;
		}
		else
		{
			rc.left += m_nIPBlockWidth;
			rc.right = rc.left + m_nIPPointWidth;
			if (rc.PtInRect(point))
			{
				m_selField = i;
				Invalidate(FALSE);
				m_bBeginEdit = TRUE;
				return TRUE;
			}
			rc.left += m_nIPPointWidth;
		}
	}

	return TRUE;
}

LRESULT QYIPCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
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


LRESULT QYIPCtrl::OnChar(WPARAM wparam, LPARAM lparam)
{
	if (0 > m_selField || MAX_IPV4_BLOCK_COUNT <= m_selField)
	{
		return TRUE;
	}

	char ch = wparam;
	if ('0' <= ch && '9' >= ch )
	{
			TCHAR  chTemp[128] = { 0 };
			if (m_bBeginEdit)
			{
				SafeSprintf(chTemp, 128, L"%c", ch);
				m_bBeginEdit = FALSE;
			}
			else
			{
				SafeSprintf(chTemp, 128, L"%s%c", m_chIP[m_selField], ch);
			}

			int nTemp = SafeA2I(chTemp);
			if (0 <= nTemp && 0xff >= nTemp)
			{
				SafeSprintf(m_chIP[m_selField], MAX_IPV4_ADDRESS_BLOCK_SIZE, L"%d", nTemp);
			}
			else
			{
				if ((MAX_IPV4_BLOCK_COUNT - 1) > m_selField)
				{
					m_selField++;
				}

				SafeSprintf(chTemp, 128, L"%c", ch);
			    nTemp = SafeA2I(chTemp);
				if (0 <= nTemp && 0xff >= nTemp)
				{
					SafeSprintf(m_chIP[m_selField], MAX_IPV4_ADDRESS_BLOCK_SIZE, L"%d", nTemp);

				}
				else
				{
					return FALSE;
				}
			}

			if (99 < nTemp && 0xff > nTemp)
			{
				if ((MAX_IPV4_BLOCK_COUNT - 1) > m_selField)
				{
					m_selField++;
				}
				else
				{
					m_selField = 0;
				}
				m_bBeginEdit = TRUE;
			}

			Invalidate(FALSE);		
	}
	else if ('.' == ch || ' ' == ch)
	{
		if ((MAX_IPV4_BLOCK_COUNT - 1) > m_selField)
		{
			m_selField++;
		}
		else
		{
			m_selField = 0;
		}

		Invalidate(FALSE);
	}
	
	return FALSE;
}

LRESULT QYIPCtrl::LoseFocus()
{
	m_bFocus = FALSE;
	//m_selField = (DATETIME_FIELD)-1;
	Invalidate();

	return TRUE;
}


LRESULT QYIPCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
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

LRESULT QYIPCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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

	return TRUE;
}

LRESULT QYIPCtrl::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	
	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}

LRESULT QYIPCtrl::OnPaint()
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
	QYRect rcField = rc;
	rcField.OffsetRect(m_nClientPadWidth, 0);

	for (int i = 0; i < MAX_IPV4_BLOCK_COUNT; i++)
	{
		rcField.right = rcField.left + m_nIPBlockWidth;

		QYSize testSize = dc.GetTextExtent(m_chIP[i]);
		if (m_bFocus && i == m_selField)
		{
			int top = rcField.top + (rcField.Height() - testSize.cy) / 2;
			dc.FillSolidRect(rcField.left, top, rcField.right, top + testSize.cy, QY_THEME.LISTBOX_BK_COLOR_SELECT);
		}

		dc.DrawText(m_chIP[i], SafeStrlen(m_chIP[i]), &rcField, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
		
		if (i < (MAX_IPV4_BLOCK_COUNT - 1))
		{
			rcField.left += m_nIPBlockWidth;
			rcField.right = rcField.left + m_nIPPointWidth;
			dc.DrawText(L".", SafeStrlen(L"."), &rcField, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

			rcField.left += m_nIPPointWidth;
		}
	}


	dc.SetTextColor(clrOld);
	dc.SetBkMode(nOldMode);
	dc.SelectObject(OldFont);

	return TRUE;
}


void QYIPCtrl::SetValue(LPCTSTR ip)
{
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;
	int p4 = 0;
	SafeSscanf(ip, L"%d.%d.%d.%d", &p1, &p2, &p3, &p4);
	if (0 > p1 || 255 < p1)
	{
		p1 = 0;
	}

	SafeSprintf(m_chIP[0], L"%d", p1);

	if (0 > p2 || 255 < p2)
	{
		p2 = 0;
	}
	SafeSprintf(m_chIP[1], L"%d", p2);
	if (0 > p3 || 255 < p3)
	{
		p3 = 0;
	}
	SafeSprintf(m_chIP[2], L"%d", p3);
	if (0 > p4 || 255 < p4)
	{
		p4 = 0;
	}
	SafeSprintf(m_chIP[3], L"%d", p4);

	Invalidate();
}

void QYIPCtrl::setValue(const char *ip)
{
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;
	int p4 = 0;
	sscanf_s(ip, "%d.%d.%d.%d", &p1, &p2, &p3, &p4);
	if (0 > p1 || 255 < p1)
	{
		p1 = 0;
	}

	SafeSprintf(m_chIP[0], L"%d", p1);

	if (0 > p2 || 255 < p2)
	{
		p2 = 0;
	}
	SafeSprintf(m_chIP[1], L"%d", p2);
	if (0 > p3 || 255 < p3)
	{
		p3 = 0;
	}
	SafeSprintf(m_chIP[2], L"%d", p3);
	if (0 > p4 || 255 < p4)
	{
		p4 = 0;
	}
	SafeSprintf(m_chIP[3], L"%d", p4);

	Invalidate();
}


void QYIPCtrl::SetValue(DWORD ip)
{
	SafeSprintf(m_chIP[0], L"%d", ip&0x000000ff);
	SafeSprintf(m_chIP[1], L"%d", (ip>>8)&0x000000ff);
	SafeSprintf(m_chIP[2], L"%d", (ip >>16) & 0x000000ff);
	SafeSprintf(m_chIP[3], L"%d", (ip >>24) & 0x000000ff);
}

DWORD  QYIPCtrl::GetValue() const
{
	DWORD ip = 0;
	ip |= SafeA2I(m_chIP[3]);
	ip <<= 8;
	ip |= SafeA2I(m_chIP[2]);
	ip <<= 8;
	ip |= SafeA2I(m_chIP[1]);
	ip <<= 8;
	ip |= SafeA2I(m_chIP[0]);

	return ip;
}

void  QYIPCtrl::GetValue(QYString& ip) 
{
	ip.Format(L"%s.%s.%s.%s", m_chIP[0], m_chIP[1], m_chIP[2], m_chIP[3]);
}


void  QYIPCtrl::getValue(std::string &ip)
{
	QYString sIP;

	sIP.Format(L"%s.%s.%s.%s", m_chIP[0], m_chIP[1], m_chIP[2], m_chIP[3]);
	
	if (MAX_WINDOW_TEXT_LENGTH - 1 < sIP.Length()*sizeof(TCHAR))
	{
		return ;
	}

	char dstText[MAX_WINDOW_TEXT_LENGTH] = { 0 };

	QYString::SafeW2AUTF8(dstText, MAX_WINDOW_TEXT_LENGTH, sIP);

	ip = dstText;
}