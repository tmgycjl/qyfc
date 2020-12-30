#include "QYPagePane.h"
#include "QYDC.h"

QYPagePane::QYPagePane()
{
	m_nButtonHeight = 20;
	m_nButtonWidth = 60;
	m_hotButton = -1;
	m_selButton = -1;
	m_nEditWidth = 30;
	m_nStaticWidth = 30;
	m_rectStatic = 0;
	m_nTotalPages = 1;
	m_nPage = 1;
	m_nBorderWidth = 1;
	//memset(m_picPage, 0, sizeof(QYPicture) * PAGE_BUTTON_MAX);
}

QYPagePane::~QYPagePane()
{
}

LRESULT QYPagePane::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETFOCUS:{return TRUE; }break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_NCDESTROY:{SAFE_DELETE_ARRAY(m_picPage); break; }
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); break; }
	//case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:{OnLButtonDown(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	default:break;
	}
	return 0L;
}

BOOL QYPagePane::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYPagePane",
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

	m_strButtonText[PAGE_BUTTON_FIRST] = L"First";
	m_strButtonText[PAGE_BUTTON_LAST] = L"Last";
	m_strButtonText[PAGE_BUTTON_NEXT] = L"Next";
	m_strButtonText[PAGE_BUTTON_END] = L"End";

	m_buttonCmd[PAGE_BUTTON_FIRST] = QY_WM_FIRST_PAGE;
	m_buttonCmd[PAGE_BUTTON_LAST] = QY_WM_LAST_PAGE;
	m_buttonCmd[PAGE_BUTTON_NEXT] = QY_WM_NEXT_PAGE;
	m_buttonCmd[PAGE_BUTTON_END] = QY_WM_END_PAGE;
	
	m_picPage = new QYPicture[PAGE_BUTTON_MAX];
	assert(nullptr != m_picPage);

	if (!m_picPage[PAGE_BUTTON_FIRST].Load(IDP_SKIP_FIRST_PAGE, QY_IMAGE_PNG))
	{
		return FALSE;
	}

	if (!m_picPage[PAGE_BUTTON_END].Load(IDP_SKIP_END_PAGE, QY_IMAGE_PNG))
	{
		return FALSE;
	}

	if (!m_picPage[PAGE_BUTTON_NEXT].Load(IDP_SKIP_NEXT_PAGE, QY_IMAGE_PNG))
	{
		return FALSE;
	}

	if (!m_picPage[PAGE_BUTTON_LAST].Load(IDP_SKIP_LAST_PAGE, QY_IMAGE_PNG))
	{
		return FALSE;
	}

	m_Edit.SetFlag(1);
	return m_Edit.Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, m_nEditWidth, m_nButtonHeight), m_hWnd);
}

LRESULT QYPagePane::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);
	QYPen separetedPen(PS_SOLID, m_nSeparatedWidth, QY_THEME.HEADERCTRL_SEPARATED_COLOR);
	HPEN  oldPen = (HPEN)MemDc.SelectObject(&separetedPen);

	QYBrush Brush(QY_THEME.PAGEPANE_BK_COLOR_NORMAL);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	if (0 == m_nBorderWidth)
	{
		MemDc.FillRect(rect, Brush);
	}
	else
	{
		QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
		HPEN OldPen;

		OldPen = (HPEN)MemDc.SelectObject(&PenLine);
		MemDc.Rectangle(rect);

		MemDc.SelectObject(OldPen);
		PenLine.DeleteObject();
	}

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	TCHAR szPages[100] = { 0 };
	SafeSprintf(szPages, L"/ %d", m_nTotalPages);

	MemDc.DrawText(szPages, SafeStrlen(szPages), &m_rectStatic, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	QYPen penHover;
	penHover.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_DOWN);

	for (int i = 0; i < PAGE_BUTTON_MAX; i++)
	{
		QYRect rectPic = m_rectButton[i];
		
			if (m_hotButton == i)
			{
				oldPen = (HPEN)MemDc.SelectObject(&penHover);
				MemDc.Rectangle(m_rectButton[i], (HBRUSH)GetStockObject(NULL_BRUSH));
				MemDc.SelectObject(oldPen);
			}

			if (m_selButton == i)
			{
				rectPic.OffsetRect(1, 1);
			}

			QYRect rectTemp2;

			m_picPage[i].Draw(MemDc.GetDC(),
				rectPic.left,
				rectPic.top,
				m_picPage[i].GetWidth(),
				m_picPage[i].GetHeight());

			//MemDc.Rectangle(m_rectButton[i],(HBRUSH)GetStockObject(GRAY_BRUSH));
			//MemDc.DrawText(&m_strButtonText[i], m_strButtonText[i].Length(), &rectText, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		
	}
	

	MemDc.SelectObject(OldFont);
	MemDc.SelectObject(oldPen);

	separetedPen.DeleteObject();

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYPagePane::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	m_nButtonHeight = (m_picPage[PAGE_BUTTON_FIRST].GetHeight() > rect.Height()) ? rect.Height() : m_picPage[PAGE_BUTTON_FIRST].GetHeight();
	m_nButtonWidth = (m_picPage[PAGE_BUTTON_FIRST].GetWidth() > rect.Width()) ? rect.Width() : m_picPage[PAGE_BUTTON_FIRST].GetWidth();

	int left = rect.left + (rect.Width() - 4 * m_nButtonWidth - 4 * 10 - m_nEditWidth-  5 - m_nStaticWidth) / 2;
	int top = rect.top + (rect.Height() - m_nButtonHeight) / 2;
	m_rectButton[PAGE_BUTTON_FIRST].left = left;
	m_rectButton[PAGE_BUTTON_FIRST].right = m_rectButton[PAGE_BUTTON_FIRST].left + m_nButtonWidth;
	m_rectButton[PAGE_BUTTON_FIRST].top = top;
	m_rectButton[PAGE_BUTTON_FIRST].bottom = m_rectButton[PAGE_BUTTON_FIRST].top + m_nButtonHeight;

	top = rect.top + (rect.Height() - m_nButtonHeight) / 2;
	m_nButtonHeight = (m_picPage[PAGE_BUTTON_LAST].GetHeight() > rect.Height()) ? rect.Height() : m_picPage[PAGE_BUTTON_LAST].GetHeight();
	m_nButtonWidth = (m_picPage[PAGE_BUTTON_LAST].GetWidth() > rect.Width()) ? rect.Width() : m_picPage[PAGE_BUTTON_LAST].GetWidth();

	left = m_rectButton[PAGE_BUTTON_FIRST].right + 10;
	m_rectButton[PAGE_BUTTON_LAST].left = left;
	m_rectButton[PAGE_BUTTON_LAST].right = m_rectButton[PAGE_BUTTON_LAST].left + m_nButtonWidth;
	m_rectButton[PAGE_BUTTON_LAST].top = top;
	m_rectButton[PAGE_BUTTON_LAST].bottom = m_rectButton[PAGE_BUTTON_LAST].top + m_nButtonHeight;

	if (nullptr != m_Edit.GetHwnd())
	{
		left = m_rectButton[PAGE_BUTTON_LAST].right + 10;
		m_Edit.MoveWindow(left, top, m_nEditWidth, m_nButtonHeight);
		left += m_nEditWidth + 5 ;

		m_rectStatic.left = left;
		m_rectStatic.right = m_rectStatic.left + m_nStaticWidth;
		m_rectStatic.top = top;
		m_rectStatic.bottom = m_rectStatic.top + m_nButtonHeight;
		left += m_nStaticWidth + 10;
	}

	top = rect.top + (rect.Height() - m_nButtonHeight) / 2;
	m_nButtonHeight = (m_picPage[PAGE_BUTTON_NEXT].GetHeight() > rect.Height()) ? rect.Height() : m_picPage[PAGE_BUTTON_NEXT].GetHeight();
	m_nButtonWidth = (m_picPage[PAGE_BUTTON_NEXT].GetWidth() > rect.Width()) ? rect.Width() : m_picPage[PAGE_BUTTON_NEXT].GetWidth();

	m_rectButton[PAGE_BUTTON_NEXT].left = left;
	m_rectButton[PAGE_BUTTON_NEXT].right = m_rectButton[PAGE_BUTTON_NEXT].left + m_nButtonWidth;
	m_rectButton[PAGE_BUTTON_NEXT].top = top;
	m_rectButton[PAGE_BUTTON_NEXT].bottom = m_rectButton[PAGE_BUTTON_NEXT].top + m_nButtonHeight;

	top = rect.top + (rect.Height() - m_nButtonHeight) / 2;
	m_nButtonHeight = (m_picPage[PAGE_BUTTON_END].GetHeight() > rect.Height()) ? rect.Height() : m_picPage[PAGE_BUTTON_END].GetHeight();
	m_nButtonWidth = (m_picPage[PAGE_BUTTON_END].GetWidth() > rect.Width()) ? rect.Width() : m_picPage[PAGE_BUTTON_END].GetWidth();

	left = m_rectButton[PAGE_BUTTON_NEXT].right + 10;
	m_rectButton[PAGE_BUTTON_END].left = left;
	m_rectButton[PAGE_BUTTON_END].right = m_rectButton[PAGE_BUTTON_END].left + m_nButtonWidth;
	m_rectButton[PAGE_BUTTON_END].top = top;
	m_rectButton[PAGE_BUTTON_END].bottom = m_rectButton[PAGE_BUTTON_END].top + m_nButtonHeight;

	return TRUE;
}

LRESULT QYPagePane::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	return TRUE;
}

LRESULT QYPagePane::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	for (int i = 0; i < PAGE_BUTTON_MAX; i++)
	{
		if (m_rectButton[i].PtInRect(point))
		{
			if (m_selButton != i)
			{
				m_selButton = i;
				Invalidate();
			}

			return TRUE;
		}
	}
	return TRUE;
	//CWnd::OnLButtonDown(nFlags, point);
}

LRESULT QYPagePane::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_selButton != -1)
	{
		::SendMessage(::GetParent(m_hWnd), m_buttonCmd[m_selButton], m_nPage, 0);
		m_selButton = -1;
		Invalidate();
	}
	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}

LRESULT QYPagePane::OnMouseMove(UINT nFlags, QYPoint point)
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
		Invalidate();
	}
	else
	{
		for (int i = 0; i < PAGE_BUTTON_MAX; i++)
		{
			if (m_rectButton[i].PtInRect(point))
			{
				if (m_hotButton != i)
				{
					m_hotButton = i;
					Invalidate();
				}
				
				return TRUE;
			}
		}

		if (m_hotButton != -1)
		{
			m_hotButton = -1;
			Invalidate();
		}

	}

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

BOOL QYPagePane::SetButtonText(int button, LPCTSTR strText)
{
	if (PAGE_BUTTON_FIRST > button || PAGE_BUTTON_END < button)
	{
		return FALSE;
	}

	m_strButtonText[button] = strText;

	QYPaintDC dc(this);
	HFONT oldfont = (HFONT)dc.SelectObject(&m_pFont);
	int width  = dc.GetTextExtent(&m_strButtonText[button]).cx + 10;
	dc.SelectObject(oldfont);

	m_nButtonWidth = (m_nButtonWidth > width) ? m_nButtonWidth : width;
	OnSize(0, 0, 0);

	return TRUE;
}

int QYPagePane::SetPage(int page)
{ 
	int last = m_nPage;
	//if (page != m_nPage)
	{ 
		if (nullptr != m_Edit.GetHwnd())
		{
			QYString str = page;
			m_Edit.SetText(&str);
		}
		m_nPage = page; 
		Invalidate();
	} 

	return last;
}

int QYPagePane::SetPageRange(int pageRange)
{ 
	int last = m_nTotalPages;
	if (pageRange != m_nTotalPages)
	{
		m_nTotalPages = pageRange; 
		Invalidate(); 
	} 

	return last;
}