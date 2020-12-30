#include "QYDropList.h"
#include "QYDC.h"
#include "QYShadowBorder.h"
#include "QYLanguage.h"

QYDropList::QYDropList(QYWindow *pSounrceWnd) :QYWindow()
{
	m_nItemHeight = 20;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nSelItem = -1;
	m_pSourceWnd = NULL;
	m_DefaultHeight = m_nItemHeight * 10 + 2 * m_nBorderWidth;
	m_bDownScrollBar = FALSE;
	m_nStep = 1;
	//m_pScrollBar = nullptr;
	m_pShadow = nullptr;
	m_pSourceWnd = pSounrceWnd;
	m_bDeleteRectDown = FALSE;
	m_bDeleteRectHot = FALSE;
	m_deleteBtnState = button_state_normal;
	m_bDeleted = FALSE;
	m_nMinVisibleItems = 4;
}

QYDropList::~QYDropList()
{
	Clear();
}

LRESULT QYDropList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE;break;}
	case WM_PAINT:{return OnPaint();}break;
	case WM_SIZE:{OnSize(0, 0, 0);}break;
	case WM_NCPAINT:{return TRUE;break;}
	case WM_NCDESTROY:{Clear();SAFE_DESTROY_WINDOW_PTR(m_pShadow);/*SAFE_DESTROY_WINDOW_PTR(m_pScrollBar)*/;break;}
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;}
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam);}break;
	//case WM_MOUSEWHEEL:{OnMouseWheel(LOWORD(wParam), HIWORD(wParam), QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_SETFOCUS:{}break;
	case WM_KILLFOCUS:{}break;
	case WM_MOVE:{OnMove(LOWORD(lParam), HIWORD(lParam));break;}
	case WM_VSCROLL:
	{
		if (onVScroll(this, LOWORD(wParam)))
		{
			InvalidateRect(&m_rectItem);
		}

		return TRUE;
	}break;
	case WM_MOUSEWHEEL:{onMouseWheel(this, LOWORD(wParam), HIWORD(wParam)); }break;
	case WM_TIMER:
	{
		if (onTimer(this, wParam))
		{
			InvalidateRect(&m_rectItem);
		}
	}
	break;
	default:{break; }
	}

	return 0L;
}


BOOL QYDropList::Create(DWORD dwExStyle,
	LPCTSTR lpClassName, 
	LPCTSTR lpWndName, 
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd )
{
	BOOL ret =  QYWindow::Create(dwExStyle | WS_EX_TOOLWINDOW,
		L"QYDropList",
		lpWndName,
		dwStyle | WS_CLIPCHILDREN,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);

	if (!ret )
	{
		return FALSE;
	}

 	m_pShadow = new QYWindowShadow;
	m_pShadow->Create(0, this, SHADOW_ON_WHOLE);
 	m_pShadow->Show(SW_SHOW);

	return ret;
}

BOOL QYDropList::UpdateLayout()
{

	QYRect rect;
	GetClientRect(rect);

	if (0 >= rect.Width() || 0 >= rect.Height())
	{
		return FALSE;
	}

	int totalItemHeight = m_vectorItem.size()*m_nItemHeight;
	int showPixs = rect.Height() - 2 * m_nBorderWidth - m_nHScrollHeight;

	updateVScrollLayout(this, rect, totalItemHeight - showPixs);

	m_rectItem = rect;
	m_rectItem += QYRect(m_nBorderWidth,
		m_nBorderWidth ,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth - m_nHScrollHeight );

	InvalidateRect(&m_rectItem);

	return TRUE;

}

LRESULT QYDropList::OnMouseMove(UINT nFlags, QYPoint point)
{
	SetFocus();
	if (!m_bHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 10;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bHover = TRUE;
		InvalidateRect(&m_rectItem);
	}
	else
	{
		
			int i = 0;
			QYRect rectTemp;
			for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++, i++)
			{
				rectTemp = (*it)->rect;
				rectTemp.OffsetRect(0, -m_nScrollPos );
				if (rectTemp.PtInRect(point))
				{
					int lastSelItem = m_nSelItem;

					m_nSelItem = i;
					if (lastSelItem != m_nSelItem)
					{
						InvalidateRect(&m_rectItem);
					}

					if (m_bDeleted)
					{
						if (point.x > rectTemp.right - m_nVScrollWidth - m_nItemHeight && point.x < rectTemp.right - m_nVScrollWidth)
						{
							if (!m_bDeleteRectHot)
							{
								m_bDeleteRectHot = TRUE;
								m_deleteBtnState = button_state_hot;
								Invalidate();

							}
							return TRUE;
						}
						if (m_bDeleteRectHot)
						{
							m_bDeleteRectHot = FALSE;
							m_deleteBtnState = button_state_normal;
							Invalidate();
						}
					}

					return TRUE;
				}

			
			}

			if (i == m_vectorItem.size())
			{
				m_nSelItem = -1;
				InvalidateRect(&m_rectItem);
			}
	}

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYDropList::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nSelItem = -1;
		InvalidateRect(&m_rectItem);
	}
	return TRUE;
}

int QYDropList::addItem(const char *key, const char* text, DWORD_PTR data)
{
    QYDropListItem *item = new QYDropListItem;
    SET_TEXT_ID(item->textID, text, item->strText);
    item->key = key;
	item->data = data;
    item->rect.top = m_nItemHeight * m_vectorItem.size();
    item->rect.bottom = item->rect.top + m_nItemHeight;
    m_vectorItem.push_back(item);

    return m_vectorItem.size();
}
// CDropList 消息处理程序
int QYDropList::AddItem(LPCTSTR strText, DWORD_PTR data)
{
	//std::vector<QYDropListItem> m_vectorIem;
	QYDropListItem *item = new QYDropListItem;
	item->strText = strText;
	item->data = data;
	item->rect.top = m_nItemHeight * m_vectorItem.size();
	item->rect.bottom = item->rect.top + m_nItemHeight;
	m_vectorItem.push_back(item);

#if 0

	QYRect rect;
	GetWindowRect(rect);
	QYRect rectClient;
	GetClientRect(rectClient);

	auto cY = GetSystemMetrics(SM_CYSCREEN);

	m_DefaultHeight = cY - rect.top;
	m_DefaultHeight = m_nItemHeight * (m_DefaultHeight / m_nItemHeight) + 2 * m_nBorderWidth;

	int realHeight = 0;
	int itemTotalHeight = m_nItemHeight * m_vectorItem.size() + 2 * m_nBorderWidth;
	if (m_DefaultHeight >= itemTotalHeight)
	{
		realHeight = itemTotalHeight;
		m_nScrollRange = 0;
		m_bShowScroll = FALSE;
		m_nScrollWidth = 0;
	}
	else
	{
		realHeight = m_DefaultHeight;
		m_nScrollRange = (itemTotalHeight - m_DefaultHeight) / m_nItemHeight;
		m_bShowScroll = TRUE;
		m_nScrollWidth = SCROLLBAR_WIDTH;
	}

	//rect.bottom = rect.top + realHeight;
	//MoveWindow(rect, TRUE);

	if (nullptr == m_pShadow)
	{
		m_pShadow = new QYWindowShadow;
		m_pShadow->Create(0, this, QYWindowShadow::SHADOW_ON_RIGHT_BOTTOM, TRUE);
	}

	m_pShadow->OnMove(rect);

	if (m_bShowScroll)
	{
		m_rectScroll = rectClient;
		m_rectScroll.right = rectClient.Width() - m_nBorderWidth;
		m_rectScroll.left = m_rectScroll.right - m_nScrollWidth;
		m_rectScroll.top = m_nBorderWidth;
		m_rectScroll.bottom = rectClient.Height() - m_nBorderWidth;

		if (nullptr == m_pScrollBar)
		{
			m_pScrollBar = new QYScrollCtrl;
		}

		if (nullptr == m_pScrollBar->GetHwnd())
		{
			if (!m_pScrollBar->Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), m_hWnd))
			{
				return 0;
			}
		}

		m_pScrollBar->MoveWindow(m_rectScroll);
		int lastRange = m_nScrollRange;
		m_pScrollBar->SetRange(m_nScrollRange);
		m_nStep = lastRange / m_nScrollRange;
		m_pScrollBar->SetPos(0);
		m_pScrollBar->Show(SW_SHOW);
	}
	else
	{
		if (nullptr != m_pScrollBar && nullptr != m_pScrollBar->GetHwnd())
		{
			m_pScrollBar->Show(SW_HIDE);
		}
	}
#endif
	return m_vectorItem.size();
}


void QYDropList::SetItemHeight(int itemHeight)
{
	m_nItemHeight = itemHeight;
}

int QYDropList::GetItemCount()
{
	return  m_vectorItem.size();
}

BOOL QYDropList::DeleteAllItems()
{
	Clear();

	return TRUE;
}

void QYDropList::Clear()
{
	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++)
	{
		QYDropListItem *pItem = *it;
		SAFE_DELETE(pItem);
	}

	m_vectorItem.clear();
}

int QYDropList::GetCurSel()
{
	return m_nCursel;
}

BOOL QYDropList::SetCurSel(int cusor)
{
	if (cusor > m_vectorItem.size() - 1)
	{
		return FALSE;
	}
	else
	{
		m_nCursel = cusor;
		return TRUE;
	}
}

int QYDropList::GetCurSel(LPCTSTR strText)
{
	//std::vector<QYDropListItem>::iterator it;
	int i = 0;
	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++, i++)
	{
		if ((*it)->strText == strText)
		{
			return i;
		}
	}

	return -1;
}

QYString QYDropList::GetCuselString(int nCursel)
{
	QYString srtEmpty = L"";
	auto it = m_vectorItem.begin();
	int i = 0;
	for (; it != m_vectorItem.end(); it++, i++)
	{
		if (i == nCursel)
		{
			return (*it)->strText;
		}
	}

	return srtEmpty;
}



DWORD_PTR QYDropList::GetItemData(int item)
{
	auto it = m_vectorItem.begin();
	int i = 0;
	for (; it != m_vectorItem.end(); it++, i++)
	{
		if (i == item)
		{
			return (*it)->data;
		}
	}

	return 0;
}

int QYDropList::FindData(DWORD_PTR data)
{
	auto it = m_vectorItem.begin();
	int i = 0;
	for (; it != m_vectorItem.end(); it++, i++)
	{
		if ((*it)->data == data)
		{
			return i;
		}
	}

	return -1;
}

void QYDropList::TrackPopup(QYPoint point, int width,int pos, QYWindow *pWnd)
{
	if (NULL == pWnd)
	{
		return;
	}

	m_pSourceWnd = pWnd;

	m_nScrollPos = 0;
	if (m_bVShowScroll)
	{
		m_VScrollBar.SetRange(0);
		m_VScrollBar.SetPos(0);
	}
	auto cY = GetSystemMetrics(SM_CYSCREEN);
	BOOL toUp = FALSE;
	if (cY < point.y + (m_nItemHeight * m_nMinVisibleItems))
	{
		QYRect recvSourceWnd;
		pWnd->GetWindowRect(recvSourceWnd);
		m_DefaultHeight = recvSourceWnd.top;
		toUp = TRUE;
	}
	else
	{
		m_DefaultHeight = cY - point.y;
	}
	
	m_DefaultHeight = m_nItemHeight * (m_DefaultHeight / m_nItemHeight) + 2 * m_nBorderWidth;

	int realHeight = 0;
	int itemTotalHeight = m_nItemHeight * m_vectorItem.size() + 2 * m_nBorderWidth;
	if (m_DefaultHeight >= itemTotalHeight)
	{
		realHeight = itemTotalHeight;
	}
	else
	{
		realHeight = m_DefaultHeight;
	}

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	int realWidth = 0;

	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++)
	{
		realWidth = std::fmax(realWidth, dc.GetTextExtent((*it)->strText).cx + 10);
	}

	dc.SelectObject(OldFont);

	realWidth = std::fmax(realWidth, width);

	int i = 0;
	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++)
	{
		(*it)->rect.left = m_nBorderWidth;
		(*it)->rect.right = realWidth - m_nBorderWidth - m_nVScrollWidth;
		(*it)->rect.top = m_nBorderWidth + m_nItemHeight * (i++);												
		(*it)->rect.bottom = m_nBorderWidth + m_nItemHeight * i;
	}

	

	QYRect rectWnd(point.x, 
		!toUp ? point.y : (m_DefaultHeight - realHeight),
		point.x + realWidth, 
		!toUp ? (point.y + realHeight) : m_DefaultHeight);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, point.x, rectWnd.top, realWidth, realHeight, SWP_DRAWFRAME);

	//MoveWindow(rectWnd, TRUE);

	(nullptr != m_pShadow) ? m_pShadow->OnMove(rectWnd) : 0;
	
}

void QYDropList::DrawDeleteRect(QYDC *pDC, QYRect &rect)
{
	QYPen  hSysBtnPen;

	QYRect rectReal = rect;
	HBRUSH nullBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);

	HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(nullBrush);

	COLORREF penColor = RGB(255, 0, 0);
	int penWidth = QY_THEME.SYS_BOX_PEN_WIDTH * 2;
	if (button_state_hot == m_deleteBtnState)
	{
		pDC->Rectangle(rectReal);
		penColor = RGB(255, 0, 0);
	}
	else if (button_state_down == m_deleteBtnState)
	{
		pDC->FillSolidRect(rectReal, RGB(0, 0, 255));
		//rectReal.DeflateRect(1, 1);
		penColor = RGB(255, 255, 255);
	}

	hSysBtnPen.CreatePen(PS_SOLID, penWidth, penColor);


	HPEN  oldpen = (HPEN)pDC->SelectObject(&hSysBtnPen);

	//pDC->FillRect(rect, &brush);
	// 

	// 	int sideLen = min(rect.Height() -2 , (rect.Width() ) -2);
	// 	rectReal.top = (rect.Height() - sideLen) + rect.top;
	// 	rectReal.left = (rect.Width() - sideLen)  + rect.left;
	// 	rectReal.bottom = rectReal.top + sideLen;
	// 	rectReal.right = rectReal.left + sideLen;

	QYPoint pt;

	rectReal.DeflateRect(4, 4);

	pDC->MoveToEx(rectReal.left + 1, rectReal.top + 1,&pt);
	pDC->LineTo(rectReal.right - 1, rectReal.bottom - 1);
	pDC->MoveToEx(rectReal.left + 1, rectReal.bottom - 1, &pt);
	pDC->LineTo(rectReal.right - 1, rectReal.top + 1);

	pDC->SelectObject(oldpen);
	hSysBtnPen.DeleteObject();


	pDC->SelectObject(oldBrush);
}

LRESULT QYDropList::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush(GetStockObject(NULL_BRUSH));
	HBRUSH OldBrush;
	
	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.MENU_BK_COLOR_NORMAL);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	MemDc.Rectangle(rect);

	MemDc.SelectObject(OldBrush);
	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	//g_ConsoleLog.Print("awdaw\n");
	auto it = m_vectorItem.begin();
	int i = 0;
	QYRect rectTemp;
	for (; it != m_vectorItem.end(); it++, i++)
	{
		rectTemp = (*it)->rect;
		
		rectTemp.OffsetRect(0, -m_nScrollPos );

		if (rectTemp.bottom < rect.top + m_nBorderWidth)
		{
			continue;
		}
		else if (rectTemp.top > rect.bottom - m_nBorderWidth )
		{
			break;
		}

		if (m_nSelItem != i)
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.MENU_BK_COLOR_NORMAL);
			//MemDc.SetTextColor(TEXT_COLOR_NORMAL);
		}
		else
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.MENU_BK_COLOR_SELECT);
			//MemDc.SetTextColor(TEXT_COLOR_SELECT);
			
			if (m_bDeleted)
			{
				QYRect rectDelete = rectTemp;
				rectDelete.left = rectDelete.right - rectDelete.Height();
				DrawDeleteRect(&MemDc, rectDelete);
			}
			
		}

		rectTemp.left += 5;
		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER | DT_LEFT );
	}

	MemDc.SelectObject(OldFont);
	
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	
	return TRUE;
}

BOOL QYDropList::Show(int nCmdShow)
{
	(nullptr != m_pShadow) ? m_pShadow->Show(nCmdShow) : 0;

	if (nCmdShow == SW_HIDE)
	{
		QYApp::StopPopWndMouseHook();
	}
	else
	{
		QYApp::StartPopWndMouseHook(m_hWnd);
	}
	
	return QYWindow::Show(nCmdShow);
}

LRESULT QYDropList::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	QYRect rect;
	GetClientRect(rect);
	//g_ConsoleLog.Print("QYDropList::OnLButtonDown,x:%d,y:%d\n",point.x,point.y);

	if (rect.PtInRect(point))
	{
		if (point.x < rect.right - m_nVScrollWidth)
		{
			if (-1 != m_nSelItem)
			{
				if (m_bDeleted)
				{
					if (point.x > rect.right - m_nVScrollWidth - m_nItemHeight && point.x < rect.right - m_nVScrollWidth)
					{
						if (!m_bDeleteRectDown)
						{
							m_bDeleteRectDown = TRUE;
							m_deleteBtnState = button_state_down;
							Invalidate();
							return TRUE;
						}
					}
				}
				

				ReleaseCapture();

				Show(SW_HIDE);
				m_bDeleteRectDown = FALSE;
				m_deleteBtnState = button_state_normal;
				m_bDeleteRectHot = FALSE;
				m_nCursel = m_nSelItem;
				//m_pSourceWnd->SetWindowText(GetCuselString(m_nSelItem));
				m_pSourceWnd->SendMessage(CB_SELECTSTRING, m_nCursel, 0);
				m_nSelItem = -1;
			}

			
		}
		else
		{
			
			QYApp::m_hPopWindow = m_hWnd;
			m_bDownScrollBar = TRUE;
			m_LastPoint = point;
			if (m_bVShowScroll)
			{
				m_VScrollBar.SendMessage(WM_LBUTTONDOWN, nFlags, MAKELONG(point.x - rect.right + m_nVScrollWidth - m_nBorderWidth, point.y - m_nBorderWidth));
			}
		//	g_ConsoleLog.Print("droplist lbuttondown %d\n", GetTickCount());
		}

	}
	else
	{
		ReleaseCapture();
		Show(SW_HIDE);
	}

	return TRUE;

	//CWnd::OnLButtonDown(nFlags, point);
}


BOOL QYDropList::OnMove(int cx, int cy)
{
	if (nullptr != m_pShadow )
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	return FALSE;
}

LRESULT QYDropList::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	UpdateLayout();

	if (nullptr != m_pShadow)
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	return TRUE;
}

LRESULT QYDropList::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDeleted)
	{
		if (m_bDeleteRectDown)
		{
			m_bDeleteRectDown = FALSE;
			m_deleteBtnState = button_state_hot;
		}

		QYRect rect;
		GetClientRect(rect);

		if (rect.PtInRect(point))
		{
			if (point.x > rect.right - m_nVScrollWidth - m_nItemHeight && point.x < rect.right - m_nVScrollWidth)
			{
				::PostMessage(::GetParent(m_hWnd), QY_WM_DROPLIST_DELETE_STRING, m_nSelItem, 0);
				Invalidate();
			}
		}
	}
	

	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}