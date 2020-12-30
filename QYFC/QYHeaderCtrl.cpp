#include "QYHeaderCtrl.h"
#include "QYDC.h"

QYHeaderCtrl::QYHeaderCtrl() :QYWindow()
{
	m_nBorderWidth = 0;
	m_bHover = FALSE;
	m_nSelItem = -1;
	m_nHoverItem = -1;
	m_bDown = FALSE;
	m_nSeparatedWidth = 1;
	m_nScrollPos = 0;
	m_bCheckFlag = FALSE;
	m_bCheckAll = FALSE;
	m_picSelectBox = nullptr;
	m_picNormalBox = nullptr;
	m_bChangingWidth = FALSE;
	m_hCursor = nullptr;
}

QYHeaderCtrl::~QYHeaderCtrl()
{
	SAFE_DELETE(m_picSelectBox);
	SAFE_DELETE(m_picNormalBox);

	Clear();
}

void QYHeaderCtrl::SetCheckAll(BOOL bCheckAll)
{
	m_bCheckAll = bCheckAll;
	Invalidate();
}

void QYHeaderCtrl::SetCheckFlag(BOOL bCheckFlag)
{
	m_bCheckFlag = bCheckFlag;
	if (m_bCheckFlag)
	{
		m_picNormalBox = new QYPicture;
		m_picNormalBox->Load(IDP_CHECK_NORMAL, QY_IMAGE_PNG);
		m_picSelectBox = new QYPicture;;
		m_picSelectBox->Load(IDP_CHECK_SELECTED, QY_IMAGE_PNG);
	}
	else
	{
		SAFE_DELETE(m_picSelectBox);
		SAFE_DELETE(m_picNormalBox);
	}
}

LRESULT QYHeaderCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETFOCUS:{return TRUE;}break;
	case WM_ERASEBKGND:{return TRUE;break;}
	case WM_PAINT:{return OnPaint();}break;
	case WM_NCPAINT:{return TRUE;break;}
	case WM_NCDESTROY:{Clear();break;}
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam);}break;
	case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:{OnLButtonDown(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOSHORT(lParam), HISHORT(lParam)));}break;
	case WM_SETCURSOR:return OnSetCursor(wParam, lParam);; break;
	case WM_TIMER: OnTimer(wParam); break;
	default:break;
	}
	return 0L;
}


BOOL QYHeaderCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYHeaderCtrl",
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
	return ret;
}


int QYHeaderCtrl::GetItemCount()
{
	return m_listItem.size();
}


BOOL QYHeaderCtrl::DeleteItem(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (i == item)
		{
			SAFE_DELETE(*it);
			m_listItem.erase(it); 
			Invalidate();
			return TRUE;
		}
	}

	return FALSE;
}

BOOL QYHeaderCtrl::DeleteAllItems()
{
	Clear();

	Invalidate();

	return TRUE; 
}

int QYHeaderCtrl::InsertItem(int item, 
	const TCHAR* strText, 
	int width, 
	int format,
	enumSortType sortType,
	DWORD_PTR data)
{
	auto *pitem = new QYHeaderCtrlItem(strText,format,sortType,data);
	assert(nullptr != pitem);

	QYPaintDC dc(this);

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

		if (it == m_listItem.end())
		{
			pitem->left = m_nBorderWidth + (last_it != m_listItem.end() ? (*last_it)->right + m_nSeparatedWidth: 0);
		}
		else
		{
			pitem->left = (*it)->right + m_nSeparatedWidth;
		}

		QYSize size = dc.GetTextExtent(strText);

		pitem->right = pitem->left + ((size.cx + 10) > width ? (size.cx + 10) : width);

		m_listItem.insert(it, pitem);
		right = pitem->right + m_nSeparatedWidth;
	}

	m_nLastItem = item;
	Invalidate();
	return m_listItem.size();
}

int QYHeaderCtrl::insertItem(int item,
    const char* text,
    int width,
    int format,
    enumSortType sortType,
    DWORD_PTR data)
{
    auto *pitem = new QYHeaderCtrlItem(text, format, sortType, data);
    assert(nullptr != pitem);

    QYPaintDC dc(this);

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

        if (it == m_listItem.end())
        {
            pitem->left = m_nBorderWidth + (last_it != m_listItem.end() ? (*last_it)->right + m_nSeparatedWidth : 0);
        }
        else
        {
            pitem->left = (*it)->right + m_nSeparatedWidth;
        }

        QYSize size = dc.GetTextExtent(pitem->strText);

        pitem->right = pitem->left + ((size.cx + 10) > width ? (size.cx + 10) : width);

        m_listItem.insert(it, pitem);
        right = pitem->right + m_nSeparatedWidth;
    }

    m_nLastItem = item;
    Invalidate();
    return m_listItem.size();
}


BOOL QYHeaderCtrl::SetItemText(int item, const TCHAR* strText)
{
	auto *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		pItem->strText = strText;
		Invalidate();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL QYHeaderCtrl::SetItemWidth(int item, const int width)
{
	int i = 0;
	int off = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (i == item)
		{
			off = width - (*it)->right + (*it)->left;
			(*it)->right += off;
		}
		else if (i > item)
		{
			(*it)->left += off;
			(*it)->right += off;
		}
	}

	Invalidate();

	::SendMessage(::GetParent(m_hWnd), QY_WM_COLMUN_WIDTH_CHANGED, item, 0);

	return TRUE;
}

QYHeaderCtrlItem* QYHeaderCtrl::GetItem(int item)
{
#if 0
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			return *it;
		}
	}
	return nullptr;
#else
	if (-1 == item)
	{
		return  nullptr;
	}
	auto it = m_listItem.begin();
	advance(it, item);
	return (it != m_listItem.end()) ? (*it) : nullptr;
#endif

}

BOOL QYHeaderCtrl::GetLastItemRect(QYRect &rect)
{
	if (0 < m_listItem.size())
	{
		auto it = m_listItem.back();
		rect.left = it->left;
		rect.right = it->right;
		return TRUE;
	}
	else
	{
		rect = 0;
		return FALSE;
	}
	
}

void QYHeaderCtrl::Clear()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_listItem.clear();
}

void QYHeaderCtrl::DrawArrow(HDC hDC, QYRect *rect, BOOL ascend)
{
	COLORREF color = QY_THEME.COMBOBOX_ARROW_PEN_COLOR_NORMAL;
	
	HPEN hSysBtnPen = CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, color);

	SelectObject(hDC, hSysBtnPen);

	RECT rectReal ;
	int sideLen = min(rect->Height(), rect->Width()) / 2;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top =  rect->top;
	rectReal.left = (rect->Width() - sideLen *2) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen * 2;
	POINT pt;
	int top = rectReal.top;
	int bottom = rectReal.bottom;
	if (1 == ascend)
	{
		MoveToEx(hDC, rectReal.left, bottom, &pt);
		LineTo(hDC, rectReal.left + sideLen , top );
		LineTo(hDC, rectReal.left + sideLen * 2 + 1, bottom + 1);
	}
	else
	{
		MoveToEx(hDC, rectReal.left, top, &pt);
		LineTo(hDC, rectReal.left + sideLen , bottom);
		LineTo(hDC, rectReal.left + sideLen * 2 + 1, top - 1);
	}
	

	DeleteObject(hSysBtnPen);
}

LRESULT QYHeaderCtrl::OnPaint()
{
	QYPaintDC dc(this); 

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);
	QYPen separetedPen(PS_SOLID, m_nSeparatedWidth, QY_THEME.HEADERCTRL_SEPARATED_COLOR);
	HPEN  oldPen = (HPEN)MemDc.SelectObject(&separetedPen);

	QYBrush Brush(QY_THEME.HEADERCTRL_BK_COLOR_NORMAL);
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
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	auto it = m_listItem.begin();
	int i = 0;
	QYRect rectTemp;
	QYPoint pt;
	for (; it != m_listItem.end(); it++, i++)
	{
		rectTemp = rect;
		rectTemp.top += m_nBorderWidth;
		rectTemp.bottom -= m_nBorderWidth ;
		rectTemp.left = (*it)->left - m_nScrollPos;
		rectTemp.right = (*it)->right - m_nScrollPos;

		
		MemDc.MoveToEx(rectTemp.right, rectTemp.top + 1, &pt);
		MemDc.LineTo(rectTemp.right, rectTemp.bottom - 1);

		QYRect rectText = rectTemp;
			if (m_nSelItem == i)
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.HEADERCTRL_BK_COLOR_DOWN);
				rectText.OffsetRect(1, 1);
			//	MemDc.SetTextColor(TEXT_COLOR_NORMAL);
			}
			else
			{
				if (m_nHoverItem == i)
				{
					MemDc.FillSolidRect(rectTemp, QY_THEME.HEADERCTRL_BK_COLOR_HOVER);
					//	MemDc.SetTextColor(TEXT_COLOR_NORMAL);
				}
				else
				{
					MemDc.FillSolidRect(rectTemp, QY_THEME.HEADERCTRL_BK_COLOR_NORMAL);
				}
				//MemDc.SetTextColor(TEXT_COLOR_SELECT);
			}

		if (0xff != (*it)->ascend)
		{
			QYRect rectArrow = rectTemp;
			rectArrow.bottom = 10;
			rectArrow.top = 2;
			DrawArrow(MemDc.GetDC(), &rectArrow, (BOOL)(*it)->ascend);
		}

		if (0 == i && m_bCheckFlag)
		{
			if (!m_bCheckAll)
			{
				if (nullptr != m_picNormalBox)
				{
					QYRect rectTemp2;
					rectTemp2.left = rectTemp.left + 2;
					rectTemp2.top = rectTemp.top + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2;
					rectTemp2.right = rectTemp2.left + m_picNormalBox->GetWidth();
					rectTemp2.bottom = rectTemp2.top + m_picNormalBox->GetHeight();

					m_picNormalBox->Draw(MemDc.GetDC(),
						rectTemp.left + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2,
						rectTemp.top + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2,
						m_picNormalBox->GetWidth(), m_picNormalBox->GetHeight());

					rectText.left = rectTemp2.right + 5;
				}
			}
			else
			{
				if (nullptr != m_picSelectBox)
				{
					QYRect rectTemp2;
					rectTemp2.left = rectTemp.left + 2;
					rectTemp2.top = rectTemp.top + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2;
					rectTemp2.right = rectTemp2.left + m_picNormalBox->GetWidth();
					rectTemp2.bottom = rectTemp2.top + m_picNormalBox->GetHeight();
					m_picSelectBox->Draw(MemDc.GetDC(),
						rectTemp.left + (rectTemp.Height() - m_picSelectBox->GetHeight()) / 2,
						rectTemp.top + (rectTemp.Height() - m_picSelectBox->GetHeight()) / 2,
						m_picSelectBox->GetWidth(),
						m_picSelectBox->GetHeight());
					rectText.left = rectTemp2.right + 5;
				}
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
		
		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectText, (*it)->format | DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_WORD_ELLIPSIS);
	}

	MemDc.SelectObject(OldFont);
	MemDc.SelectObject(oldPen);

	separetedPen.DeleteObject();

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYHeaderCtrl::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	return TRUE;
}

LRESULT QYHeaderCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	return TRUE;
}

LRESULT QYHeaderCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	KillTimer( 1);

	if (!m_bDown)
	{
		m_bDown = TRUE;
		QYRect rect;
		GetClientRect(rect);
		auto it = m_listItem.begin();
		for (int i = 0; it != m_listItem.end(); it++, i++)
		{
			rect.left = (*it)->left - m_nScrollPos;
			rect.right = (*it)->right - m_nScrollPos;
			if (rect.PtInRect(point))
			{
				if (m_bCheckFlag && 0 == i)
				{
					QYRect rectTemp2;
					rectTemp2.left = rect.left + 2;
					rectTemp2.top = rect.top + 2;
					rectTemp2.right = rectTemp2.left + m_picNormalBox->GetWidth();
					rectTemp2.bottom = rectTemp2.top + m_picNormalBox->GetHeight();
					if (rectTemp2.PtInRect(point))
					{
						m_bCheckAll = !m_bCheckAll;
						Invalidate();
						::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_CHECK_ALL, m_bCheckAll, 0);
						return TRUE;
					}
				}

				if (point.x >= rect.right - 4)
				{
					m_LastPoint = point;
					m_nSelItem = i;
					QYHeaderCtrlItem *pItem = GetItem(m_nSelItem);
					if (nullptr != pItem)
					{
						m_nLastWidth = pItem->right - pItem->left;
						m_nRealWidth = m_nLastWidth;
						SetCapture();
						SetTimer(1, 20, nullptr);
					}
				
				}
				else if (point.x <= rect.left + 4 && 0 != i)
				{
					m_LastPoint = point;
					m_nSelItem = i - 1;
					QYHeaderCtrlItem *pItem = GetItem(m_nSelItem);
					if (nullptr != pItem)
					{
						m_nLastWidth = pItem->right - pItem->left;
						m_nRealWidth = m_nLastWidth;
						SetCapture();
						SetTimer(1, 20, nullptr);
					}
				}
				else
				{
					m_nSelItem = i;
				}

				Invalidate();
				return TRUE;
			}
		}
		Invalidate();
	}

	return TRUE;
	//CWnd::OnLButtonDown(nFlags, point);
}

LRESULT QYHeaderCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	KillTimer( 1);
	if (!m_bChangingWidth)
	{
		::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_CLICK_COLUMN, m_nSelItem, 0);
	}
	m_bDown = FALSE;
	m_nSelItem = -1;
	Invalidate();
	

	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}

void QYHeaderCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		SetItemWidth(m_nSelItem,m_nRealWidth);
	}
}

LRESULT QYHeaderCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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
		if (m_bDown && m_bChangingWidth)
		{
			int realWidth = m_nLastWidth + point.x - m_LastPoint.x;
			int minWidth = 2;
			if (m_bCheckFlag && m_nSelItem == 0 && nullptr != m_picNormalBox )
			{
				minWidth = 2 * 5 + m_picNormalBox->GetWidth();
			}

			if (realWidth > minWidth)
			{
				m_nRealWidth = realWidth;
			}
		}
		else
		{
			QYRect rect;
			GetClientRect(rect);
			auto it = m_listItem.begin();
			auto lastit = it;
			for (int i = 0; it != m_listItem.end(); it++, i++)
			{
				rect.left = (*it)->left - m_nScrollPos;
				rect.right = (*it)->right - m_nScrollPos;
				if (rect.PtInRect(point))
				{
					if (point.x >= rect.right - 4)
					{
						m_nHoverItem = i;
						m_bChangingWidth = TRUE;
						m_hCursor = LoadCursor(nullptr, IDC_SIZEWE);
						PostMessage(WM_SETCURSOR, 0, 0);
					}
					else if (point.x <= rect.left + 4 && 0 != i)
					{
						m_nHoverItem = i - 1;
						m_bChangingWidth = TRUE;
						m_hCursor = LoadCursor(nullptr, IDC_SIZEWE);
						PostMessage(WM_SETCURSOR, 0, 0);
					}
					else
					{
						m_hCursor = LoadCursor(nullptr, IDC_ARROW);
						PostMessage(WM_SETCURSOR, 0, 0);
						m_nHoverItem = i;
						m_bChangingWidth = FALSE;
					}

					Invalidate();
					return TRUE;
				}

				lastit = it;
			}

			m_hCursor = LoadCursor(nullptr, IDC_ARROW);
			PostMessage(WM_SETCURSOR, 0, 0);
			m_nHoverItem = -1;
		}
		
	}

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYHeaderCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nHoverItem = -1;
		Invalidate();
	}
	return TRUE;
}

LRESULT QYHeaderCtrl::OnSetCursor(WPARAM wparam, LPARAM lparam)
{

	::SetCursor(nullptr == m_hCursor ? ::LoadCursor(nullptr,IDC_ARROW): m_hCursor);
	return TRUE;
}

void QYHeaderCtrl::updateText()
{
	for (QYHeaderCtrlItem *it : m_listItem)
	{
		SET_TEXT_ID(it->textID, it->textID.c_str(), it->strText)
	}

	Invalidate();
}