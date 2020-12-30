#include "QYTabCtrl.h"
#include "QYDC.h"

QYTabCtrl::QYTabCtrl() :QYWindow()
{
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nSelItem = 0;
	m_bDown = FALSE;
	m_TabDockPos = TAB_DOCK_TOP;
	m_nItemHeight = QY_THEME.DEFAULT_TAB_HEIGHT;
	m_nSeparatePixel = 2;
	m_bReDraw = TRUE;
	m_colorText = QY_THEME.TAB_TEXT_COLOR_NORMAL;
	m_colorSelText = QY_THEME.TAB_TEXT_COLOR_SELECT;
	m_colorBk = QY_THEME.TAB_BK_COLOR_NORMAL;
	//m_pSelFont = nullptr;
}

QYTabCtrl::~QYTabCtrl()
{
	Clear();

// 	if (nullptr != m_pSelFont)
// 	{
// 		m_pSelFont->DeleteObject();
// 		SAFE_DELETE(m_pSelFont);
// 	}
}

LRESULT QYTabCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETFOCUS:{return TRUE; }break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_NCDESTROY:{Clear(); break; }
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:{OnLButtonDown(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_TIMER: OnTimer(wParam); break;
	default:break;
	}
	return 0L;
}

BOOL QYTabCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYTabCtrl",
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

	//m_pSelFont = new QYFont;
	//m_pSelFont->CreateFont(MS_YAHEI_FONT_SIZE, 0, FW_NORMAL, MS_YAHEI_FONT_TYPE, TRUE);

	return ret;
}

int QYTabCtrl::GetItemCount()
{
	return m_listItem.size();
}

BOOL QYTabCtrl::DeleteItem(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (i == item)
		{
			SAFE_DELETE(*it);
			m_listItem.erase(it);
			m_bReDraw ? Invalidate() : 0;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL QYTabCtrl::DeleteAllItems()
{
	Clear();

	Invalidate();

	return TRUE;
}

int QYTabCtrl::InsertItem(int item,QYWindow *pWnd, const TCHAR* strText, int width, int format, DWORD_PTR data)
{
	auto *pitem = new QYTabCtrlItem(pWnd,strText, data);
	pitem->format = (255 > format) ? format : DT_CENTER;
	if (nullptr != pWnd)
	{
		::SetParent(pWnd->GetHwnd(), m_hWnd);
		pWnd->Show((item == m_nSelItem) ? SW_SHOW : SW_HIDE);
	}

	auto right = 0;
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

// 		if (it == m_listItem.end())
// 		{
// 			pitem->rect.left = m_nBorderWidth + (last_it != m_listItem.end() ? (*last_it)->rect.right : 0);
// 		}
// 		else
// 		{
// 			pitem->rect.left = (*it)->rect.right ;
// 		}
// 		pitem->rect.right = pitem->rect.left + width;

		m_listItem.insert(it, pitem);
		//right = pitem->rect.right ;
	}

	m_bReDraw ? Invalidate() : 0;
	return m_listItem.size();
}

BOOL QYTabCtrl::SetItemText(int item, const TCHAR* strText)
{
	auto *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		pItem->strText = strText;
		m_bReDraw ? Invalidate() : 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL QYTabCtrl::SetItemText(QYWindow *pWnd, const TCHAR* strText)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if ((*it)->pWnd == pWnd)
		{
			(*it)->strText = strText;
			OnSize(0, 0, 0);
			m_bReDraw ? Invalidate() : 0;
			return TRUE;
		}
	}

	return FALSE;
}

int QYTabCtrl::GetSelectItem()
{
	return m_nSelItem;
}

DWORD_PTR QYTabCtrl::GetItemData(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			return (*it)->data;
		}
	}

	return 0;
}

int QYTabCtrl::SetItemHeight(const int height)
{
	int lastHeight = m_nItemHeight;
	m_nItemHeight = height;
	m_bReDraw ? Invalidate() : 0;

	return lastHeight;
}


QYTabCtrlItem* QYTabCtrl::GetItem(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			return *it;
		}
	}

	return nullptr;
}

BOOL QYTabCtrl::GetLastItemRect(QYRect &rect)
{
	if (0 < m_listItem.size())
	{
		auto it = m_listItem.back();
		rect = (*it).rect;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void QYTabCtrl::Clear()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_listItem.clear();
}

LRESULT QYTabCtrl::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);
	QYPen spiderPen(PS_SOLID, 1, QY_THEME.TAB_BOTTOM_SPLIT_COLOR);
	QYBrush Brush(m_colorBk);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	if (0 == m_nBorderWidth)
	{
		MemDc.FillRect(rect.left,rect.top,rect.right,rect.bottom, Brush);

		HPEN oldPen = (HPEN)MemDc.SelectObject(&spiderPen);
		QYPoint pt;
		MemDc.MoveToEx(rect.left, rect.bottom , &pt);
		MemDc.LineTo(rect.right, rect.bottom );
		MemDc.SelectObject(oldPen);
		spiderPen.DeleteObject();
	}
	else
	{
		QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
		HPEN OldPen = (HPEN)MemDc.SelectObject(&PenLine);
		MemDc.Rectangle(rect);

		MemDc.SelectObject(OldPen);
		PenLine.DeleteObject();
	}

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	auto it = m_listItem.begin();
	int i = 0;
	QYRect rectTemp;
	COLORREF color = 0;
	for (; it != m_listItem.end(); it++, i++)
	{
		QYRect rectText = (*it)->rect;
		if (m_nSelItem == i)
		{
			//MemDc.FillSolidRect((*it)->rect, TAB_BK_COLOR_SEL);

			color = MemDc.SetTextColor(m_colorSelText);
			QYRect rectSeparate = 0;

			if (TAB_DOCK_LEFT == m_TabDockPos)
			{
				rectSeparate.left = (*it)->rect.right - m_nSeparatePixel;
				rectSeparate.right = (*it)->rect.right;
				rectSeparate.top = (*it)->rect.top;
				rectSeparate.bottom = (*it)->rect.bottom;
			}
			else if (TAB_DOCK_TOP == m_TabDockPos)
			{
				rectSeparate.left = (*it)->rect.left;
				rectSeparate.right = (*it)->rect.right;
				rectSeparate.top = (*it)->rect.bottom - m_nSeparatePixel;
				rectSeparate.bottom = (*it)->rect.bottom;
			}
			

			MemDc.FillSolidRect(rectSeparate, QY_THEME.TAB_BK_COLOR_HOT);
			//MemDc.FillSolidRect(rectSeparate, TAB_BK_COLOR_SEL);
			//MemDc.FillSolidRect(rectSeparate.left, rectSeparate.bottom - m_nSeparatePixel, rectSeparate.right, rectSeparate.bottom, TAB_BK_COLOR_HOT);
		}
		else
		{
			if (m_nHoverItem == i)
			{
				//MemDc.SelectObject(m_pSelFont);
				color = MemDc.SetTextColor(m_colorSelText);
				//MemDc.FillSolidRect((*it)->rect.left, (*it)->rect.bottom - m_nSeparatePixel, (*it)->rect.right, (*it)->rect.bottom, TAB_BK_COLOR_HOT);
			}
			else
			{
				color = MemDc.SetTextColor(m_colorText);
				//MemDc.FillSolidRect((*it)->rect, TAB_BK_COLOR_NORMAL);
				//MemDc.FillSolidRect((*it)->rect.left, (*it)->rect.bottom - m_nSeparatePixel, (*it)->rect.right, (*it)->rect.bottom, TAB_BK_COLOR_HOT);
			}
		}

		if (DT_LEFT == (*it)->format)
		{
			rectText.left += 5;
		}
		else if (DT_RIGHT == (*it)->format)
		{
			rectText.right -= 5;
		}

		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectText, (*it)->format | DT_SINGLELINE | DT_VCENTER);
		//MemDc.SelectObject(m_pFont);
		MemDc.SetTextColor(color);
	}

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	int i = 0;
	QYPaintDC dc(this);
	HFONT oldfont = (HFONT)dc.SelectObject(m_pFont);

	int left = rect.left + m_nBorderWidth;
	int top = rect.top + m_nBorderWidth;
	int right = rect.right - m_nBorderWidth;
	int bottom = rect.bottom - m_nBorderWidth;

	rect.DeflateRect(m_nBorderWidth, m_nBorderWidth);

	int averageItemHeight = 0;

	if (TAB_DOCK_RIGHT == m_TabDockPos || TAB_DOCK_LEFT == m_TabDockPos)
	{
		m_nItemHeight = std::fmin(rect.Width() - 2 * m_nBorderWidth, m_nItemHeight);
		averageItemHeight = (bottom - top) / m_listItem.size();
		top = ((bottom - top) - averageItemHeight*m_listItem.size()) / 2;
	}
	else
	{
		m_nItemHeight = std::fmin(rect.Height() - 2 * m_nBorderWidth, m_nItemHeight);
	}

	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++,i++)
	{
		QYSize size = dc.GetTextExtent(&(*it)->strText);

		switch (m_TabDockPos)
		{
		case TAB_DOCK_TOP:
		{
			(*it)->rect.left = left;
			left = (*it)->rect.right = (*it)->rect.left + size.cx + 10;
			(*it)->rect.top = top;
			(*it)->rect.bottom = (*it)->rect.top + m_nItemHeight;
			(nullptr != (*it)->pWnd) ? (*it)->pWnd->MoveWindow(rect.left + m_nBorderWidth,
				(*it)->rect.bottom + m_nSeparatePixel,
				rect.Width() - 2 * m_nBorderWidth,
				bottom - (*it)->rect.bottom - m_nSeparatePixel) : 0;
		}
		break;
		case TAB_DOCK_RIGHT:
		{
			
		}
		break;
		case TAB_DOCK_LEFT:
		{
			(*it)->rect.left = left;
			(*it)->rect.right = (*it)->rect.left  + m_nItemHeight;
			(*it)->rect.top = top;
			top = (*it)->rect.bottom = (*it)->rect.top + averageItemHeight;
			(nullptr != (*it)->pWnd) ? (*it)->pWnd->MoveWindow((*it)->rect.right + m_nSeparatePixel,
				(*it)->rect.top ,
				right - (*it)->rect.right - m_nSeparatePixel,
				rect.Height() - 2 * m_nBorderWidth) : 0;
		}
		break;
		case TAB_DOCK_BOTTOM:
		{

		}
		break;
		default:
			return FALSE;
		}
	}

	dc.SelectObject(oldfont);

	Invalidate();

	return TRUE;
}

LRESULT QYTabCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	return TRUE;
}

LRESULT QYTabCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bDown)
	{
		m_bDown = TRUE;
		int lastSelItem = m_nSelItem;
		auto it = m_listItem.begin();
		int i = 0;
		for (; it != m_listItem.end(); it++, i++)
		{
			if ((*it)->rect.PtInRect(point))
			{
				if (m_nSelItem == i)
				{
					return TRUE;
				}
				m_nSelItem = i;
				Invalidate();

				::SendMessage(::GetParent(m_hWnd), QY_WM_TABCTRL_ITEM_SELECTED, (WPARAM)m_hWnd, m_nSelItem);
				(nullptr != (*it)->pWnd) ? (*it)->pWnd->Show(SW_SHOW) : 0;
				if (lastSelItem != -1 && lastSelItem != m_nSelItem)
				{
					auto lastIt = m_listItem.begin();
					advance(lastIt, lastSelItem);
					(nullptr != (*lastIt)->pWnd) ? (*lastIt)->pWnd->Show(SW_HIDE) : 0;
				}
				return TRUE;
			}
		}
		
	}

	return TRUE;
	//CWnd::OnLButtonDown(nFlags, point);
}

LRESULT QYTabCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bDown = FALSE;
	Invalidate();
	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}

void QYTabCtrl::OnTimer(UINT_PTR nIDEvent)
{
	
}

LRESULT QYTabCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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

	int lastSelItem = m_nHoverItem;
	auto it = m_listItem.begin();
	for (int i = 0; it != m_listItem.end(); it++, i++)
	{
		if ((*it)->rect.PtInRect(point))
		{
			if (m_nHoverItem == i)
			{
				return TRUE;
			}
			m_nHoverItem = i;

			//::SetCursor(LoadCursor(NULL, IDC_HAND));
			Invalidate();
			return TRUE;
		}
	}

	if (it == m_listItem.end())
	{
		if (-1 != m_nHoverItem)
		{
			m_nHoverItem = -1;
			//::SetCursor(LoadCursor(NULL, IDC_IBEAM));
			Invalidate();
		}
		
	}

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYTabCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nHoverItem = -1;
		//::SetCursor(LoadCursor(NULL, IDC_IBEAM));
		Invalidate();
	}
	return TRUE;
}

BOOL QYTabCtrl::GetItemRectFromIndex(int index, QYRect rectWnd, QYRect &rect)
{
	
	return TRUE;
}


QYWindow* QYTabCtrl::SelectItem(int selItem)
{
	QYTabCtrlItem *pItem = GetItem(m_nSelItem);

	int item = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, item++)
	{
		if (item == selItem)
		{
			if (nullptr != pItem)
			{
				if (m_nSelItem != item)
				{
					int lastSelItem = m_nSelItem;
					m_nSelItem = item;

					Invalidate();

					(nullptr != (*it)->pWnd) ? (*it)->pWnd->Show(SW_SHOW) : 0;
					if (lastSelItem != -1 && lastSelItem != m_nSelItem)
					{
						auto lastIt = m_listItem.begin();
						advance(lastIt, lastSelItem);
						(nullptr != (*lastIt)->pWnd) ? (*lastIt)->pWnd->Show(SW_HIDE) : 0;
					}
				}

				return pItem->pWnd;
			}
			else
			{
				return nullptr;
			}
		}
	}



	return nullptr;
}

QYWindow* QYTabCtrl::SelectItem(QYWindow *pWnd)
{
	 QYTabCtrlItem *pItem =  GetItem(m_nSelItem);
	
	 int item = 0;
	 for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, item++)
	{
		if ((*it)->pWnd == pWnd)
		{
			if (nullptr != pItem )
			{
				if (m_nSelItem != item)
				{
					int lastSelItem = m_nSelItem;
					m_nSelItem = item;

					Invalidate();

					(nullptr != (*it)->pWnd) ? (*it)->pWnd->Show(SW_SHOW) : 0;
					if (lastSelItem != -1 && lastSelItem != m_nSelItem)
					{
						auto lastIt = m_listItem.begin();
						advance(lastIt, lastSelItem);
						(nullptr != (*lastIt)->pWnd) ? (*lastIt)->pWnd->Show(SW_HIDE) : 0;
					}
				}

				return pItem->pWnd;
			}
			else
			{
				return nullptr;
			}
		}
	}

	

	return nullptr;
}