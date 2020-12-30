#include "QYListBox.h"
#include "QYDC.h"

QYListBox::QYListBox() :QYWindow()
{
	m_nItemHeight = 16;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_bShowScroll = FALSE;
	m_nScrollPos = 0;
	m_nScrollWidth = 0;
	m_nStep = 1;
	m_nSelItem = -1;
	m_bOpenTrackPosTime = FALSE;
	m_pScrollBar = nullptr;
	m_bDown = FALSE;
}

QYListBox::~QYListBox()
{
	Clear();
}

LRESULT QYListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCCALCSIZE:
	{
		UINT style = GetWindowLong(m_hWnd, GWL_STYLE);

		style &= ~LBS_SORT;
		style &= ~WS_BORDER;
		SetWindowLong(m_hWnd, GWL_STYLE, style);
	}
	break;
	case WM_SETFOCUS:
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
		return OnNCPaint();
		break;
	}
	case WM_NCDESTROY:
	{
		Clear();

		SAFE_DESTROY_WINDOW_PTR(m_pScrollBar);
		break;
	}
	case WM_MOUSEMOVE:
	{
		 OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		break;
	}
	case WM_MOUSELEAVE:
	{
		OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		OnMouseWheel(LOWORD(wParam), HIWORD(wParam), QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	case WM_LBUTTONDOWN:
	{
		OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	case WM_LBUTTONUP:
	{
		OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	case WM_VSCROLL:
	{
		OnVScroll(LOWORD(wParam), HIWORD(wParam), (QYScrollCtrl*)lParam);
	}
	break;
	case WM_TIMER:
		OnTimer(wParam);
		break;
	default:
	{
		break;
	}
	}
	return 0L;
}


BOOL QYListBox::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle ,
		L"QYListBox",
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
	SetFont((HFONT)(QYApp::m_pMsDefultFont->m_hObject));


	return ret;
}

LRESULT QYListBox::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bDown)
	{
		m_bDown = TRUE;
		m_nSelItem = m_nScrollPos + (point.y - m_rectItemList.top) / m_nItemHeight;
		InvalidateRect(&m_rectItemList);
	}

	return TRUE;
}


LRESULT QYListBox::OnMouseMove(UINT nFlags, QYPoint point)
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
		InvalidateRect(&m_rectItemList);
	}
	else
	{
		if (m_bDown)
		{
			m_nSelItem = m_nScrollPos + (point.y - m_rectItemList.top) / m_nItemHeight;

			InvalidateRect(&m_rectItemList);
		}
	}

	return TRUE;
}

LRESULT QYListBox::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		Invalidate();
	}
	return TRUE;
}

int QYListBox::GetItemCount()
{
	return m_vectorItem.size();
}

// CDropList 消息处理程序
int QYListBox::AddItem(LPCTSTR strText, DWORD_PTR data)
{
	QYListBoxItem *item = new QYListBoxItem;
	item->strText = strText;
	item->data = data;
	item->rect.top = m_nBorderWidth + m_vectorItem.size() * m_nItemHeight;
	item->rect.bottom = item->rect.top + m_nItemHeight;
	m_vectorItem.push_back(item);

	QYRect rect;
	GetClientRect(rect);

	int nDefaultHeight = rect.Height() - 2 * m_nBorderWidth;
	int showItems = nDefaultHeight / m_nItemHeight;

	int realHeight = 0;
	int nScrollRange = 0;
	if (showItems >= m_vectorItem.size())
	{
		m_bShowScroll = FALSE;
		m_nScrollWidth = 0;
		if (nullptr != m_pScrollBar && nullptr != m_pScrollBar->GetHwnd())
		{
			m_pScrollBar->Show(SW_HIDE);
		}
	}
	else
	{
		 nScrollRange = m_vectorItem.size() -showItems;
		m_bShowScroll = TRUE;
		m_nScrollWidth = QY_THEME.SCROLLBAR_WIDTH;
		if (nullptr == m_pScrollBar)
		{
			m_pScrollBar = new QYScrollCtrl;
		}

		int lastRange = 0;
		if (nullptr == m_pScrollBar->GetHwnd())
		{
			if (!m_pScrollBar->Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), m_hWnd))
			{
				return 0;
			}

			QYRect rectScroll = rect;
			rectScroll.right = rect.Width() - m_nBorderWidth;
			rectScroll.left = rectScroll.right - m_nScrollWidth;
			rectScroll.top = m_nBorderWidth;
			rectScroll.bottom = rect.Height() - m_nBorderWidth;
			m_pScrollBar->MoveWindow(rectScroll);

			 lastRange = nScrollRange;
			m_pScrollBar->SetRange(nScrollRange);
			m_nStep = lastRange / nScrollRange;
			m_pScrollBar->SetPos(0);
			m_pScrollBar->Show(SW_SHOW);
		}

		 lastRange = nScrollRange;
		m_pScrollBar->SetRange(nScrollRange);
		m_nStep = lastRange / nScrollRange;
		m_pScrollBar->SetPos(m_pScrollBar->GetPos());
		//m_pScrollBar->SetPos(m_ps);
	}

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth, m_nBorderWidth, -m_nScrollWidth - m_nBorderWidth, -m_nBorderWidth);
	InvalidateRect(&m_rectItemList);

	return m_vectorItem.size();
}

BOOL QYListBox::DeleteAllItems()
{
	Clear();
	return TRUE;
}

void QYListBox::Clear()
{
	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++)
	{
		QYListBoxItem *pItem = *it;
		SAFE_DELETE(pItem);
	}

	m_vectorItem.clear();
}

LRESULT QYListBox::OnNCPaint()
{
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);
	QYRect rectClient;
	GetClientRect(rectClient);
	int offsetX = (rect.Width() - rectClient.Width()) / 2;
	int offsetY = (rect.Height() - rectClient.Height()) / 2;

	rectClient.OffsetRect(offsetX, offsetY);

	ExcludeClipRect(pDC->GetDC(),rectClient.left,rectClient.top,rectClient.right,rectClient.bottom);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)pDC->SelectObject(&PenLine);

	QYBrush brush;
	brush.CreateSolidBrush(QY_THEME.LISTBOX_BK_COLOR_NORMAL);
	HBRUSH oldbrush = (HBRUSH)pDC->SelectObject(&brush);

	pDC->Rectangle(0, 0, rect.right - rect.left, rect.bottom - rect.top);
	
	pDC->SelectObject(OldPen);
	pDC->SelectObject(oldbrush);
	brush.DeleteObject();
	PenLine.DeleteObject();

	ReleaseDC(pDC);

	return TRUE;
}


LRESULT QYListBox::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush(QY_THEME.LISTBOX_BK_COLOR_NORMAL);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	MemDc.FillRect(rect, Brush);

	MemDc.SelectObject(OldBrush);
	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	int ScrollPos = m_nScrollPos;
	if (m_bShowScroll)
	{
		int showItems = rect.Height() / m_nItemHeight;
		int itemTotalItems =  m_vectorItem.size();
		if (m_nScrollPos == m_pScrollBar->GetRange()* m_nStep
			&& m_nScrollPos < (itemTotalItems - showItems))
		{
			ScrollPos = itemTotalItems - showItems;
		}
	}

	auto it = m_vectorItem.begin();
	int i = 0;
	QYRect rectTemp;
	for (; it != m_vectorItem.end(); it++, i++)
	{
		rectTemp = rect;
		rectTemp.left += m_nBorderWidth ;
		rectTemp.right -= m_nBorderWidth + m_nScrollWidth;
		rectTemp.top = (*it)->rect.top;
		rectTemp.bottom = (*it)->rect.bottom;

		rectTemp.OffsetRect(0, -ScrollPos *m_nItemHeight);

		if (rectTemp.bottom >= rect.top + m_nBorderWidth
			&& rectTemp.top < rect.top + m_nBorderWidth)
		{
			continue;
		}
		else if (rectTemp.top <= rect.bottom - m_nBorderWidth
			&& rectTemp.bottom > rect.bottom - m_nBorderWidth)
		{
			break;
		}

		if (m_nSelItem != i)
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.LISTBOX_BK_COLOR_NORMAL);
			MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
		}
		else
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.LISTBOX_BK_COLOR_SELECT);
			MemDc.SetTextColor(QY_THEME.TEXT_COLOR_SELECT);
		}

		rectTemp.left += 5;
		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER);
	}

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}



BOOL QYListBox::OnMove(int cx, int cy)
{

	return FALSE;
}


LRESULT QYListBox::OnSize(UINT nType, int cx, int cy)
{
	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);
	QYRect  m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth, m_nBorderWidth, -m_nScrollWidth - m_nBorderWidth, -m_nBorderWidth);

	return TRUE;
}

#if 1

void QYListBox::OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
			//g_ConsoleLog.Print("SetTimer\n");
			SetTimer( 2, 10, nullptr);
			m_bOpenTrackPosTime = TRUE;
		}
	}
		break;
	case SB_THUMBPOSITION:
	{
		//g_ConsoleLog.Print("KillTimer\n");
		KillTimer( 2);
		m_bOpenTrackPosTime = FALSE;
	}
		break;
	default:
		break;
	}
	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
#endif

LRESULT QYListBox::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDown)
	{
		m_bDown = FALSE;
		InvalidateRect(&m_rectItemList);
	}
	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}

LRESULT QYListBox::OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bShowScroll)
	{
		if (nFlags == 0)
		{
			KillTimer( 1);
			KillTimer(2);
			m_nZDelta = zDelta;
			m_nWheelCount = 0;
			SetTimer(1, 20, nullptr);
		}
	}


	return TRUE;
}


void QYListBox::OnTimer(UINT_PTR nIDEvent)
{
	if (10 <= m_nWheelCount++ && 2 != nIDEvent)
	{
		KillTimer( 1);
		return;
	}

	if (1 == nIDEvent)
	{
		if (m_nZDelta < 0)
		{
			if (m_nScrollPos >= m_pScrollBar->GetRange() * m_nStep)
			{
				return;
			}
			m_nScrollPos += m_nStep;
			m_pScrollBar->SetPos(m_pScrollBar->GetPos() + 1);
			InvalidateRect(&m_rectItemList);
		}
		else
		{
			if (m_nScrollPos <= 0)
			{
				return;
			}
			m_nScrollPos -= m_nStep;
			m_pScrollBar->SetPos(m_pScrollBar->GetPos() - 1);
			InvalidateRect(&m_rectItemList);
		}
	}
	else if (2 == nIDEvent)
	{
		int nlastPos = m_nScrollPos;
		m_nScrollPos = m_nTrackPos;
		if (nlastPos != m_nScrollPos)
		{
			//g_ConsoleLog.Print("track pos\n");
			InvalidateRect(&m_rectItemList);
		}
	}
}