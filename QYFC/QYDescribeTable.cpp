#include "QYDescribeTable.h"


QYDescribeTable::QYDescribeTable()
{
	m_nBorderWidth = 0;
	m_nBorderWidth = 1;
	m_bVShowScroll = FALSE;
	m_nScrollPos = 0;
	m_nVScrollWidth = 0;
	m_nStep = 1;
	m_bFocus = FALSE;
	m_bReDraw = TRUE;
	m_rectItemList = 0;
	m_bCheckFlag = FALSE;
	m_bAlternateColorFlag = TRUE;
	m_nSeparatedWidth = 1;
	m_bOpenTrackPosTime = FALSE;
	m_pVScrollBar = nullptr;
	m_nHTrackPos = 0;
	m_nFieldWidth = 30;
	m_nTextCap = 10;
	m_nItemHeight = 20;
	m_nRowSpace = 2;
}


QYDescribeTable::~QYDescribeTable()
{
	Clear();
}


void QYDescribeTable::Clear()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_listItem.clear();
}

LRESULT QYDescribeTable::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETFOCUS:{m_bFocus = TRUE; Invalidate(); }break;
	case WM_KILLFOCUS:{m_bFocus = FALSE; Invalidate(); }break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_NCDESTROY:{Clear(); SAFE_DESTROY_WINDOW_PTR(m_pVScrollBar); break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_MOUSEWHEEL:{OnMouseWheel(LOWORD(wParam), HIWORD(wParam), QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_VSCROLL:{OnVScroll(LOWORD(wParam), HIWORD(wParam), (QYScrollCtrl*)lParam); return TRUE; }break;
	case WM_TIMER:OnTimer(wParam); break;
	default:break;
	}
	return 0L;
}


BOOL QYDescribeTable::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYDescribeTable",
		lpWndName,
		dwStyle | WS_CLIPCHILDREN,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);

	if (!ret)
	{
		return FALSE;
	}
	return ret;
}

#include "QYDC.h"
int QYDescribeTable::InsertItem(int item, const TCHAR* strField /* = nullptr */, const TCHAR* strDesc /* = nullptr */, DWORD_PTR data /* = 0 */)
{
	auto *pitem = new QYDescribeTableItem(strField, strDesc, data);
	assert(nullptr != pitem);

	QYPaintDC dc(this);

	auto it = m_listItem.begin();
	if (-1 != item)
	{
		int i = 0;
		auto last_it = it;
		for (; it != m_listItem.end(); it++, i++)
		{
			if (i == item)
			{
				break;
			}
			last_it = it;
		}

		 QYSize size = dc.GetTextExtent(strField);

		 m_nFieldWidth = (m_nFieldWidth < size.cx + m_nTextCap) ? (size.cx + m_nTextCap) : m_nFieldWidth;

		 pitem->textSize = dc.GetTextExtent(strDesc);

		m_listItem.insert(it, pitem);
	}

	return m_listItem.size();
}

LRESULT QYDescribeTable::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush(QY_THEME.HEADERCTRL_BK_COLOR_NORMAL);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	HPEN OldPen;
	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	if (m_rectItemList == 0)
	{
		QYRect rectItem = QYRect(rect.left,
			rect.top + m_nBorderWidth ,
			rect.right - m_nBorderWidth - m_nVScrollWidth,
			rect.bottom  - m_nBorderWidth);
		MemDc.FillRect(rectItem, Brush);
	}
	else
	{
		MemDc.FillRect(m_rectItemList, Brush);
	}

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	auto it = m_listItem.begin();
	
	QYRect rectField(rect.left + m_nBorderWidth + m_nTextCap ,
		rect.top + m_nBorderWidth - m_nItemHeight * m_nScrollPos,
		rect.left + m_nBorderWidth + m_nFieldWidth + m_nTextCap * 2,
		rect.top + m_nBorderWidth - m_nItemHeight * m_nScrollPos);
	QYRect rectdesc(rectField.right + m_nTextCap,
		rect.top + m_nBorderWidth - m_nItemHeight * m_nScrollPos,
		rect.right - m_nBorderWidth - m_nTextCap - (m_bVShowScroll ? m_nVScrollWidth : 0),
		rect.top + m_nBorderWidth - m_nItemHeight * m_nScrollPos);
	QYPoint pt;
	int top = rectdesc.bottom;
	for (; it != m_listItem.end(); it++)
	{
		rectdesc.top = top;
		rectField.top = rectdesc.top;
		rectdesc.bottom = rectdesc.top + (*it)->height;
		rectField.bottom = rectdesc.bottom;
		top = rectdesc.bottom;

		if (rectdesc.bottom < m_nBorderWidth || rectdesc.top > rect.bottom - m_nBorderWidth)
		{
			continue;
		}

		MemDc.MoveToEx(rect.left, rectdesc.bottom, &pt);
		MemDc.LineTo(rect.right, rectdesc.bottom);

		MemDc.MoveToEx(rectField.right, rectdesc.top, &pt);
		MemDc.LineTo(rectField.right, rectdesc.bottom);

		rectField.top += m_nTextCap;
		rectField.bottom -= m_nTextCap;
		MemDc.DrawText(&(*it)->strField, (*it)->strField.Length(), &rectField, DT_LEFT  | DT_WORDBREAK);
		rectdesc.top += m_nTextCap;
		rectdesc.bottom -= m_nTextCap;

		MemDc.DrawMultiText(&(*it)->strDesc, rectdesc, m_nRowSpace);
	}

	if (0 < m_nBorderWidth)
	{
		MemDc.Rectangle(rect, (HBRUSH)GetStockObject(NULL_BRUSH));
	}

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();
	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();

	return TRUE;
}

BOOL QYDescribeTable::SetRedraw(BOOL bRedraw)
{
	BOOL last = m_bReDraw;
	m_bReDraw = bRedraw;
	m_bReDraw ? UpdateLayout() : 0;

	return last;
}


BOOL QYDescribeTable::UpdateLayout()
{
	QYRect rect;
	GetClientRect(rect);

	if (0 >= rect.Width() || 0 >= rect.Height())
	{
		return FALSE;
	}

	QYPaintDC dc(this);

	int descWidth = rect.Width() - 2 * m_nBorderWidth - m_nFieldWidth;
	int showHeight = rect.Height() - 2 * m_nBorderWidth;
	int totalItemHeight = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		totalItemHeight += (*it)->height = m_nTextCap * 2 + (((*it)->textSize.cx / (descWidth - 2 * m_nTextCap - QY_THEME.SCROLLBAR_WIDTH)) + m_nRowSpace) * (*it)->textSize.cy;
	}

	if (totalItemHeight <= showHeight)
	{
		if (m_bVShowScroll)
		{
			if (nullptr != m_pVScrollBar && nullptr != m_pVScrollBar->GetHwnd())
			{
				m_pVScrollBar->SetPos(0);
				m_nScrollPos = 0;
				m_nStep = 0;
				m_pVScrollBar->Show(SW_HIDE);
			}

			m_bVShowScroll = FALSE;
			m_nVScrollWidth = 0;
		}

	}
	else
	{
		m_bVShowScroll = TRUE;
		m_nVScrollWidth = QY_THEME.SCROLLBAR_WIDTH;
	}

	if (m_bVShowScroll)
	{
		int nScrollRange = ((totalItemHeight - showHeight) / m_nItemHeight) + 1;
		if (nullptr == m_pVScrollBar)
		{
			m_pVScrollBar = new QYScrollCtrl;
		}

		if (nullptr == m_pVScrollBar->GetHwnd())
		{
			if (!m_pVScrollBar->Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), m_hWnd))
			{
				return FALSE;
			}
		}

		QYRect rectScroll = rect;
		rectScroll.right = rect.right - m_nBorderWidth;
		rectScroll.left = rectScroll.right - m_nVScrollWidth;
		rectScroll.top = rect.top + m_nBorderWidth;
		rectScroll.bottom = rect.bottom - m_nBorderWidth;
		m_pVScrollBar->MoveWindow(rectScroll);

		int lastRange = nScrollRange;
		int count = 0;
		while (0 < m_pVScrollBar->SetRange(nScrollRange) && count++ < showHeight)
		{
			lastRange = ++nScrollRange;
		}

		m_nStep = (0 < nScrollRange) ? (lastRange / nScrollRange) : 0;
		m_nScrollPos = m_nStep * m_pVScrollBar->GetPos();
		m_pVScrollBar->SetPos(m_pVScrollBar->GetPos());
		m_pVScrollBar->Show(SW_SHOW);
	}

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,m_nBorderWidth,-m_nVScrollWidth - m_nBorderWidth,-m_nBorderWidth);

	InvalidateRect(&m_rectItemList);


	return TRUE;

	QYRect rectItem;

	int realHeight = 0;
	int secondHalfItemCount = m_listItem.size() ;
	int showItems = (rect.Height() - 2 * m_nBorderWidth ) / 1;
	if (0 >= showItems)
	{
		return FALSE;
	}

	if (1)
	{
		if (m_bVShowScroll)
		{
			if (nullptr != m_pVScrollBar && nullptr != m_pVScrollBar->GetHwnd())
			{
				m_pVScrollBar->SetPos(0);
				m_nScrollPos = 0;
				m_nStep = 0;
				m_pVScrollBar->Show(SW_HIDE);
			}

			m_bVShowScroll = FALSE;
			m_nVScrollWidth = 0;
		}

		
	}
	else
	{
		m_bVShowScroll = TRUE;
		m_nVScrollWidth = QY_THEME.SCROLLBAR_WIDTH;
	}

	if (m_bVShowScroll)
	{
		int 	nScrollRange = showItems - showItems;
		if (nullptr == m_pVScrollBar)
		{
			m_pVScrollBar = new QYScrollCtrl;
		}

		if (nullptr == m_pVScrollBar->GetHwnd())
		{
			if (!m_pVScrollBar->Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), m_hWnd))
			{
				return FALSE;
			}
		}

		QYRect rectScroll = rect;
		rectScroll.right = rect.right - m_nBorderWidth;
		rectScroll.left = rectScroll.right - m_nVScrollWidth;
		rectScroll.top = rect.top + m_nBorderWidth;
		rectScroll.bottom = rect.bottom - m_nBorderWidth ;
		m_pVScrollBar->MoveWindow(rectScroll);

		int lastRange = nScrollRange;
		int count = 0;
		while (0 < m_pVScrollBar->SetRange(nScrollRange) && count++ < showItems)
		{
			lastRange = ++nScrollRange;
		}

		m_nStep = (0 < nScrollRange) ? (lastRange / nScrollRange) : 0;
		m_nScrollPos = m_nStep * m_pVScrollBar->GetPos();
		m_pVScrollBar->SetPos(m_pVScrollBar->GetPos());
		m_pVScrollBar->Show(SW_SHOW);

	}

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,
		m_nBorderWidth ,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth );

	InvalidateRect(&m_rectItemList);

	return TRUE;
}


LRESULT QYDescribeTable::OnSize(UINT nType, int cx, int cy)
{
	return UpdateLayout();
}

void QYDescribeTable::OnTimer(UINT_PTR nIDEvent)
{
	if (10 <= m_nWheelCount++ && 2 != nIDEvent && 3 != nIDEvent)
	{
		KillTimer(1);
		return;
	}

	if (1 == nIDEvent)
	{
		int offset = 1;
		if (m_nZDelta < 0)
		{
			if (m_nScrollPos >= m_pVScrollBar->GetRange() * m_nStep)
			{
				return;
			}

			//while (m_nStep > 0 && offset * m_nStep < m_nItemHeight){ offset++; };

			m_nScrollPos += m_nStep;
			m_pVScrollBar->SetPos(m_pVScrollBar->GetPos() + offset);
			InvalidateRect(&m_rectItemList);
		}
		else
		{
			if (m_nScrollPos <= 0)
			{
				return;
			}
			//while (m_nStep > 0 && offset * m_nStep < m_nItemHeight){ offset++; };
			m_nScrollPos -= m_nStep;
			m_pVScrollBar->SetPos(m_pVScrollBar->GetPos() - offset);
			InvalidateRect(&m_rectItemList);
		}
	}
	else if (2 == nIDEvent)
	{
		int nlastPos = m_nScrollPos;
		m_nScrollPos = m_nTrackPos;
		if (nlastPos != m_nScrollPos)
		{
			InvalidateRect(&m_rectItemList);
		}
	}
}

LRESULT QYDescribeTable::OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt)
{
	if (m_bVShowScroll)
	{
		if (nFlags == 0)
		{
			KillTimer(1);
			KillTimer(2);
			KillTimer(3);
			m_nZDelta = zDelta;
			m_nWheelCount = 0;
			SetTimer(1, 20, nullptr);
		}
	}

	return TRUE;
}

void QYDescribeTable::OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_LINEDOWN:
	case SB_LINEUP:
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	{
		m_nScrollPos = m_nStep * nPos;
		InvalidateRect(&m_rectItemList);
	}
	break;
	case SB_THUMBTRACK:
	{
		m_nTrackPos = m_nStep * nPos;

		if (!m_bOpenTrackPosTime)
		{
			SetTimer(2, 10, nullptr);
			m_bOpenTrackPosTime = TRUE;
		}
	}
	break;
	case SB_THUMBPOSITION:
	{
		KillTimer(2);
		m_bOpenTrackPosTime = FALSE;
	}
	break;
	default:
		break;
	}
}