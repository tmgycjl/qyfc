#include "QYToolBar.h"
#include "QYDC.h"


#define  MORE_BUTTON_INDEX     65535

QYToolBar::QYToolBar() :QYWindow()
{
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nSelItem = -1;
	m_nHoverItem = -1;
	m_bDown = FALSE;
	m_DockStyle = TB_DOCK_HORI;
	m_nSeparatePixel = 2;
	m_bReDraw = TRUE;
	m_bHasString = TRUE;
	m_bShowMoreBtn = FALSE;
	m_nShowItemCount = 0;
	m_morebtnSize = QYSize(24,24);
	m_colorBk = QY_THEME.CLIENT_COLOR;
	m_pToolTip = nullptr;
	m_pPicMoreButton = nullptr;
	m_pMoreDropList = nullptr;
	m_itemSize = QYSize(20, 20);
	m_padWidth = 10;
    m_maxTextX = 20;
	m_spaceWidth = 0;
	m_colorText = QY_THEME.TEXT_COLOR_NORMAL;
	m_colorSelText = QY_THEME.TEXT_COLOR_SELECT;

}


QYToolBar::~QYToolBar()
{
	Clear();
}

LRESULT QYToolBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETFOCUS:{return TRUE; }break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:{OnLButtonDown(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_TIMER: OnTimer(wParam); break;
	case CB_SELECTSTRING: {::SendMessage(::GetParent(m_hWnd), QY_WM_TOOLBAR_ITEM_SELECTED, m_nShowItemCount + wParam, 0); }break;
	case WM_NCDESTROY:
	{
		SAFE_DESTROY_WINDOW_PTR(m_pToolTip); 
		SAFE_DESTROY_WINDOW_PTR(m_pMoreDropList); 
		SAFE_DELETE(m_pPicMoreButton);
		Clear(); 
	}
	break; 
	
	default:break;
	}
	return 0L;
}

BOOL QYToolBar::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYToolBar",
		lpWndName,
		dwStyle ,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);

	if (!ret)
	{
		return FALSE;
	}

	m_pPicMoreButton = new QYPicture(IMAGE_STATUS_NORMAL|IMAGE_STATUS_HOVER);
	m_pPicMoreButton->Load(IDP_MORE, QY_IMAGE_PNG);

	if (!m_bHasString)
	{
		m_pToolTip = new QYToolTip;
		m_pToolTip->Create();
	}

	return ret;
}

int QYToolBar::GetItemCount()
{
	return m_listItem.size();
}

BOOL QYToolBar::DeleteItem(int item)
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

BOOL QYToolBar::DeleteAllItems()
{
	Clear();

	Invalidate();

	return TRUE;
}

int QYToolBar::insertItem(const char *key, const char* text, std::vector<int> *image, int format)
{
	auto *pitem = new QYToolBarItem(key, text, image, format);

	auto right = 0;

	m_listItem.push_back(pitem);

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	QYPicture *pImage = GetImage(pitem->image[0]);
	if (nullptr != pImage)
	{
		m_maxImageSize.cx = std::fmax(m_maxImageSize.cx, pImage->GetWidth());
		m_maxImageSize.cy = std::fmax(m_maxImageSize.cy, pImage->GetHeight());
	}

	m_maxTextX = std::fmax(m_maxTextX, dc.GetTextExtent(pitem->strText).cx);

	dc.SelectObject(OldFont);

	m_bReDraw ? Invalidate() : 0;

	OnSize(0, 0, 0);

	return m_listItem.size();
}

int QYToolBar::insertItem(int item, const char* text, std::vector<int> *image, int format, DWORD_PTR data)
{
	auto *pitem = new QYToolBarItem(text, image, format, data);

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

		m_listItem.insert(it, pitem);;
	}
	else
	{
		m_listItem.push_back(pitem);
	}

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	QYPicture *pImage = GetImage(pitem->image[0]);
	if (nullptr != pImage)
	{
		m_maxImageSize.cx = std::fmax(m_maxImageSize.cx, pImage->GetWidth());
		m_maxImageSize.cy = std::fmax(m_maxImageSize.cy, pImage->GetHeight());
	}

	m_maxTextX = std::fmax(m_maxTextX, dc.GetTextExtent(pitem->strText).cx);

	dc.SelectObject(OldFont);

	m_bReDraw ? Invalidate() : 0;

	OnSize(0, 0, 0);

	return m_listItem.size();
}

int QYToolBar::InsertItem(int item, const TCHAR* strText, std::vector<int> *image, int format, DWORD_PTR data)
{
	auto *pitem = new QYToolBarItem(strText, image, format, data);
	
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

		m_listItem.insert(it, pitem);;
	}
	else
	{
		m_listItem.push_back(pitem);
	}

	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	QYPicture *pImage = GetImage(pitem->image[0]);
	if (nullptr != pImage)
	{
		m_maxImageSize.cx = std::fmax(m_maxImageSize.cx, pImage->GetWidth());
		m_maxImageSize.cy = std::fmax(m_maxImageSize.cy, pImage->GetHeight());
	}

	m_maxTextX = std::fmax(m_maxTextX, dc.GetTextExtent(pitem->strText).cx);

	dc.SelectObject(OldFont);

	m_bReDraw ? Invalidate() : 0;

	OnSize(0, 0, 0);

	return m_listItem.size();
}

BOOL QYToolBar::SetItemText(int item, const TCHAR* strText)
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

BOOL QYToolBar::setItemText(int item, const char* text)
{
	auto *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		SET_TEXT_ID(pItem->textID,text,pItem->strText)
		m_bReDraw ? Invalidate() : 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


DWORD_PTR QYToolBar::GetItemData(int item)
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


void QYToolBar::getItemRect(int item, QYRect &rect)
{
	QYRect rectClient;
	GetClientRect(rectClient);

	rect = rectClient;


}

std::string QYToolBar::getItemKey(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			return (*it)->key;
		}
	}

	return "";
}

QYToolBarItem* QYToolBar::GetItem(int item)
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

QYToolBarItem* QYToolBar::getItem(const char *itemKey)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (itemKey == (*it)->key)
		{
			return *it;
		}
	}

	return nullptr;
}

void QYToolBar::Clear()
{
	for (QYToolBarItem *it : m_listItem)
	{
		SAFE_DELETE(it);
	}
	m_listItem.clear();
}

LRESULT QYToolBar::OnPaint()
{
	int i = 0;
	QYRect rectTemp;
	COLORREF color = 0;
	QYRect itemRect = 0;

	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush(m_colorBk);
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

	rect.DeflateRect(m_nBorderWidth, m_nBorderWidth);

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	color = MemDc.SetTextColor(m_colorText);

	int top = rect.top + (rect.Height() - m_itemSize.cy) / 2;
	int left = rect.left + m_padWidth ;

	auto it = m_listItem.begin();
	auto dstImage = 0;
	for (; it != m_listItem.end(); it++, i++)
	{
		if (TB_DOCK_HORI == m_DockStyle)
		{
			itemRect.left = left + i * (m_itemSize.cx + m_spaceWidth);
			itemRect.top = top;
			itemRect.bottom = itemRect.top + m_itemSize.cy;
			itemRect.right = itemRect.left + m_itemSize.cx ;
		}
		else if (TB_DOCK_VERT == m_DockStyle)
		{
			itemRect.left = rect.left;
			itemRect.right = rect.right ;
			itemRect.top = i * m_itemSize.cy ;
			itemRect.bottom = itemRect.top + m_itemSize.cy;
		}

		if (itemRect.right > rect.right  - m_padWidth - m_morebtnSize.cx)
		{
			break;
		}
		
		if (0 == (*it)->readOnly )
		{
			if (-1 == (*it)->image[IMAGE_STATUS_HOVER])
			{
				if (m_nSelItem == i || 1 == (*it)->check)
				{
					MemDc.Rectangle(itemRect, QY_THEME.ITEM_BK_COLOR_SEL, QY_THEME.ITEM_BORBER_COLOR_SEL);
					MemDc.SetTextColor(m_colorSelText);
				}
				else
				{
					if (m_nHoverItem == i)
					{
						MemDc.Rectangle(itemRect, m_colorBk, QY_THEME.ITEM_BORBER_COLOR_SEL);
						MemDc.SetTextColor(m_colorSelText);
					}
					else
					{
						MemDc.SetTextColor(m_colorText);
					}
				}

				dstImage = (*it)->image[IMAGE_STATUS_NORMAL];
			}
			else
			{
				if (m_nSelItem == i || 1 == (*it)->check)
				{
					dstImage = (*it)->image[IMAGE_STATUS_PRESS];
				}
				else
				{
					if (m_nHoverItem == i)
					{
						dstImage = (*it)->image[IMAGE_STATUS_HOVER];
					}
					else
					{
						dstImage = (*it)->image[IMAGE_STATUS_NORMAL];
					}
				}

				
			}
		}
		
		QYRect rectText = itemRect;
		int dtFormat = DT_LEFT;
		if (TB_TEXT_ALIGN_RIGHT== (*it)->format)
		{
			if (-1 != dstImage)
			{
				QYPicture *pic = GetImage(dstImage);
				if (nullptr != pic)
				{
					int iconLeft = itemRect.left + (itemRect.Width() - pic->GetWidth()) / 2;
					if (m_bHasString)
					{
						QYSize size = MemDc.GetTextExtent((*it)->strText);
						iconLeft = itemRect.left + (itemRect.Width() - itemRect.Height() - size.cx) / 2;
					}

					int desWidth = (itemRect.Width() >= pic->GetWidth()) ? pic->GetWidth() : itemRect.Width() - 2;
					int desHeight = (itemRect.Height() >= pic->GetHeight()) ? pic->GetHeight() : itemRect.Height() - 2;
#if 0

					if (0 == (*it)->readOnly && m_nHoverItem == i)
					{
						pic->SetHovered();
					}
#endif

					pic->Draw(MemDc.GetDC(), iconLeft, top + (itemRect.Height() - pic->GetHeight()) / 2, desWidth, desHeight);

					rectText.left = iconLeft + desWidth + 5;
				}
				else
				{
					dtFormat = DT_CENTER;
				}
				
			}

			
		}
		else if (TB_TEXT_ALIGN_BOTTOM == (*it)->format)
		{
			if (-1 != dstImage)
			{
				QYPicture *pic = GetImage(dstImage);
				if (nullptr != pic)
				{
					int iconLeft = itemRect.left + (itemRect.Width() - pic->GetWidth()) / 2;
					if (m_bHasString)
					{
						QYSize size = MemDc.GetTextExtent((*it)->strText);
						iconLeft = itemRect.left + (itemRect.Width() - itemRect.Height() - size.cx) / 2;
					}

					int desWidth = (itemRect.Width() >= pic->GetWidth()) ? pic->GetWidth() : itemRect.Width() - 2;
					int desHeight = (itemRect.Height() >= pic->GetHeight()) ? pic->GetHeight() : itemRect.Height() - 2;
#if 0


					if (0 == (*it)->readOnly && m_nHoverItem == i)
					{
						pic->SetHovered();
					}
#endif
					pic->Draw(MemDc.GetDC(), iconLeft, (itemRect.Height() - pic->GetHeight()) / 2, desWidth, desHeight);

					rectText.left = iconLeft + desWidth + 5;
				}
				else
				{
					dtFormat = DT_CENTER;
				}

			}
		}
		else if (TB_TEXT_ALIGN_CENTER == (*it)->format)
		{
			dtFormat = DT_CENTER;
			QYSize textSize = MemDc.GetTextExtent((*it)->strText);
			rectText.right = rectText.left + textSize.cx + m_padWidth;
			rectText.right = (rectText.right > rect.right) ? rect.right : rectText.right;
		}
		
		if (m_bHasString )
		{
			if (-1 != (*it)->image[IMAGE_STATUS_NORMAL])
			{
				MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectText, dtFormat | DT_SINGLELINE | DT_VCENTER);
			}
			else
			{
				MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &itemRect, dtFormat | DT_SINGLELINE | DT_VCENTER);
			}
			
		}
	}

	MemDc.SetTextColor(color);

	if (m_bShowMoreBtn)
	{
		QYRect morebtnRect;

		morebtnRect.right = rect.right - m_padWidth;
		morebtnRect.left = morebtnRect.right - m_morebtnSize.cx;
		morebtnRect.top = rect.top + (rect.Height() - m_morebtnSize.cy) / 2;
		morebtnRect.bottom = morebtnRect.top + m_morebtnSize.cy;

		if (m_nSelItem == MORE_BUTTON_INDEX)
		{
			MemDc.Rectangle(morebtnRect, QY_THEME.ITEM_BK_COLOR_SEL, QY_THEME.ITEM_BORBER_COLOR_SEL);
			MemDc.SetTextColor(m_colorSelText);
		}
		else
		{
			if (m_nHoverItem == MORE_BUTTON_INDEX)
			{
				MemDc.Rectangle(morebtnRect, m_colorBk, QY_THEME.ITEM_BORBER_COLOR_SEL);
				MemDc.SetTextColor(m_colorSelText);
			}
			else
			{
				color = MemDc.SetTextColor(m_colorText);
			}
		}

		if (nullptr != m_pPicMoreButton)
		{
			int iconLeft = morebtnRect.left + (morebtnRect.Width() - m_pPicMoreButton->GetWidth()) / 2;
			int desWidth = (morebtnRect.Width() >= m_pPicMoreButton->GetWidth()) ? m_pPicMoreButton->GetWidth() : morebtnRect.Width() - 2;
			int desHeight = (morebtnRect.Height() >= m_pPicMoreButton->GetHeight()) ? m_pPicMoreButton->GetHeight() : morebtnRect.Height() - 2;

			m_pPicMoreButton->Draw(MemDc.GetDC(), iconLeft, morebtnRect.top + (morebtnRect.Height() - m_pPicMoreButton->GetHeight()) / 2, desWidth, desHeight);
		}
	}
	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYToolBar::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	rect.DeflateRect(m_nBorderWidth, m_nBorderWidth);

	int nLastShowItemCount = m_nShowItemCount;

	if (rect.Width() <= 0 || rect.Height() <= 0)
	{
		return TRUE;
	}
	
	if (m_maxImageSize.cy > rect.Height())
	{
		m_itemSize.cx  = m_itemSize.cy = rect.Height();
	}
	else
	{
		m_itemSize.cx  = m_itemSize.cy = m_maxImageSize.cy + (rect.Height() - m_maxImageSize.cy) / 2;
	}

	if (m_bHasString)
	{
		m_itemSize.cx += m_maxTextX + 2 * m_padWidth;

		if (0 >= m_maxImageSize.cy)
		{
			m_itemSize.cy = rect.Height();
		}
	}

   // g_ConsoleLog.Print("size.cx:%d, m_maxImageSize.cy:%d,m_maxTextX:%d,m_padWidth:%d\n", m_itemSize.cx, m_maxImageSize.cy, m_maxTextX, m_padWidth);

	m_nShowItemCount = ((rect.Width() - 2 * m_nBorderWidth - 2 * m_padWidth) - m_morebtnSize.cx) / (m_itemSize.cx + m_spaceWidth);
	if (m_listItem.size() > m_nShowItemCount)
	{
		m_bShowMoreBtn = TRUE;

		if ( nLastShowItemCount != m_nShowItemCount)
		{
			auto it = m_listItem.begin();
			advance(it, m_nShowItemCount);

			m_vectorHideItem.clear();

			for (; it != m_listItem.end(); it++)
			{
				if (0 == (*it)->readOnly)
				{
					m_vectorHideItem.push_back(*it);
				}
				
			}
		}
		
	}
	else
	{
		m_bShowMoreBtn = FALSE;
		m_vectorHideItem.clear();
	}

	return TRUE;
}

LRESULT QYToolBar::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	return TRUE;
}

LRESULT QYToolBar::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bDown)
	{
		m_bDown = TRUE;

		if (m_bShowMoreBtn)
		{
			QYRect rect;
			GetClientRect(rect);
			QYRect morebtnRect = 0;
			morebtnRect.right = rect.right - m_nBorderWidth - m_padWidth;
			morebtnRect.left = morebtnRect.right - m_morebtnSize.cx;
			morebtnRect.top = rect.top + (rect.Height() - m_morebtnSize.cy) / 2;
			morebtnRect.bottom = morebtnRect.top + m_morebtnSize.cy;
			if (morebtnRect.PtInRect(point))
			{
				m_nSelItem = MORE_BUTTON_INDEX;
			}
		}
		else
		{
			
		}

		if (MORE_BUTTON_INDEX != m_nSelItem)
		{
			//m_nSelItem = (point.x - m_nBorderWidth - m_padWidth) / (m_itemSize.cx + m_spaceWidth);

			int xOffset = point.x - m_nBorderWidth - m_padWidth;

			int itemsOfLeft = xOffset / (m_itemSize.cx + m_spaceWidth);

			itemsOfLeft += 1;

			if ((itemsOfLeft*m_itemSize.cx + (itemsOfLeft - 1)*m_spaceWidth) > xOffset)
			{
				m_nSelItem = itemsOfLeft - 1;
			}
			else
			{
				m_nSelItem = -1;
			}
		}

		if ((0 <= m_nSelItem && m_nSelItem < m_listItem.size())
			|| MORE_BUTTON_INDEX == m_nSelItem)
		{
			Invalidate();

			return TRUE;
		}

	}

	return TRUE;
	//CWnd::OnLButtonDown(nFlags, point);
}

LRESULT QYToolBar::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDown)
	{
		m_bDown = FALSE;
		
		if (MORE_BUTTON_INDEX == m_nSelItem)
		{
			if (nullptr == m_pMoreDropList)
			{
				m_pMoreDropList = new QYDropList;
				if (!m_pMoreDropList->Create(0, NULL, NULL, WS_CHILD, QYRect(0, 0, 0, 0),
					::GetDesktopWindow()))
				{
					return TRUE;
				}

				QYRect rect;
				GetClientRect(rect);

				m_pMoreDropList->SetItemHeight(rect.Height() - m_nBorderWidth * 2);
			}

			if (m_pMoreDropList->IsWindowVisible())
			{
				m_pMoreDropList->Show(SW_HIDE);
			}
			else
			{
				m_pMoreDropList->DeleteAllItems();

				for (auto it = m_vectorHideItem.begin(); it != m_vectorHideItem.end(); it++)
				{
					m_pMoreDropList->AddItem((*it)->strText,(DWORD_PTR)*it);
				}
				
				QYRect rectWnd;
				GetWindowRect(rectWnd);
				m_pMoreDropList->TrackPopup(QYPoint(rectWnd.right - m_itemSize.cx, rectWnd.bottom), m_itemSize.cx, TPDL_RIGHT_BOTTOM, this);

				m_pMoreDropList->Show(SW_SHOW);
			}

		}
		else
		{
			!m_bHasString ? m_pToolTip->Show(SW_HIDE) : 0;
			QYPropertyList properties;
			properties.addProperty("id", getID());
			properties.addProperty("item", m_nSelItem);
			struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
			if (nullptr != actionCB)
			{
				actionCB->callback(&properties);
			}
			else
			{
				::SendMessage(::GetParent(m_hWnd), QY_WM_TOOLBAR_ITEM_SELECTED, m_nSelItem, 0);
			}
		}

		
		m_nSelItem = -1;

		Invalidate();
	}
	
	
	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}

void QYToolBar::OnTimer(UINT_PTR nIDEvent)
{

}

LRESULT QYToolBar::OnMouseMove(UINT nFlags, QYPoint point)
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

	if (m_bShowMoreBtn)
	{
		QYRect rect;
		GetClientRect(rect);
		QYRect morebtnRect  = 0;
		morebtnRect.right = rect.right - m_nBorderWidth - m_padWidth;
		morebtnRect.left = morebtnRect.right - m_morebtnSize.cx;
		morebtnRect.top = rect.top + (rect.Height() - m_morebtnSize.cy) / 2;
		morebtnRect.bottom = morebtnRect.top + m_morebtnSize.cy;
		if (morebtnRect.PtInRect(point))
		{
			m_nHoverItem = MORE_BUTTON_INDEX;
		}
		else
		{
			m_nHoverItem = -1;
		}
	}

	if (MORE_BUTTON_INDEX !=  m_nHoverItem)
	{
		int xOffset = point.x - m_nBorderWidth - m_padWidth;

		int itemsOfLeft = xOffset / (m_itemSize.cx + m_spaceWidth);
		
		itemsOfLeft += 1;
		
		if ((itemsOfLeft*m_itemSize.cx + (itemsOfLeft - 1)*m_spaceWidth) > xOffset  )
		{
			m_nHoverItem = itemsOfLeft - 1;
		}
		else
		{
			m_nHoverItem = -1;
		}
	}

	if (lastSelItem == m_nHoverItem)
	{
		return TRUE;
	}

	if ((0 <= m_nHoverItem&& m_nHoverItem < m_listItem.size())
		|| MORE_BUTTON_INDEX == m_nHoverItem)
	{
		Invalidate();

		if (!m_bHasString
			&& MORE_BUTTON_INDEX != m_nHoverItem
			&& m_nHoverItem < m_nShowItemCount)
		{
			auto it = m_listItem.begin();
			advance(it, m_nHoverItem);

			m_pToolTip->Show(SW_HIDE);

			if (-1 != (*it)->image[IMAGE_STATUS_NORMAL] && 0 < (*it)->strText.Length())
			{
				m_pToolTip->SetText((*it)->strText);
				m_pToolTip->TrackPopup(this);
			}
			
		}
		else
		{
			!m_bHasString ? m_pToolTip->Show(SW_HIDE) : 0;
		}
		return TRUE;
	}

	if (-1 != m_nHoverItem)
	{
		m_nHoverItem = -1;
		
		!m_bHasString ? m_pToolTip->Show(SW_HIDE) : 0;
	}

	Invalidate();

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYToolBar::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nHoverItem = -1;
		m_nSelItem = -1;
		Invalidate();

		if (!m_bHasString)
		{
			m_pToolTip->Show(SW_HIDE);
		}
	}
	return TRUE;
}

int QYToolBar::SetItemSize(QYSize size)
{
	if (TB_DOCK_HORI == m_DockStyle)
	{
		m_itemSize.cx = size.cx;
	}
	else if (TB_DOCK_VERT == m_DockStyle)
	{
		m_itemSize.cy = size.cy;
	}

	return 0;
}

int QYToolBar::setSpaceWidth(int spaceWidth)
{
	int lastSpaceWidth = m_spaceWidth;
	m_spaceWidth = spaceWidth;

	return lastSpaceWidth;
}

BOOL QYToolBar::GetCheck(int item)
{
	QYToolBarItem *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		return (1 == pItem->check) ? TRUE : FALSE;
	}

	return FALSE;
}

BOOL QYToolBar::SetCheck(int item, BOOL bCheck)
{
	BOOL bLastCheck = FALSE;

	QYToolBarItem *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		bLastCheck = (1 == pItem->check) ? TRUE : FALSE;
		 pItem->check = bCheck ? 1 : 0;
	}

	Invalidate();
	return bLastCheck;
}
BOOL QYToolBar::getCheck(const char *itemKey)
{
	QYToolBarItem *pItem = getItem(itemKey);
	if (nullptr != pItem)
	{
		return (1 == pItem->check) ? TRUE : FALSE;
	}

	return FALSE;
}

BOOL QYToolBar::setCheck(const char *itemKey, BOOL bCheck)
{
	BOOL bLastCheck = FALSE;

	QYToolBarItem *pItem = getItem(itemKey);
	if (nullptr != pItem)
	{
		bLastCheck = (1 == pItem->check) ? TRUE : FALSE;
		pItem->check = bCheck ? 1 : 0;
	}

	Invalidate();
	return bLastCheck;
}



BOOL QYToolBar::SetReadOnly(int item, BOOL bReadOnly)
{
	BOOL bLastReadOnly = FALSE;

	QYToolBarItem *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		bLastReadOnly = (1 == pItem->readOnly) ? TRUE : FALSE;
		pItem->readOnly = bReadOnly ? 1 : 0;
	}

	Invalidate();
	return bLastReadOnly;
}

void QYToolBar::ShowText(BOOL bShowText)
{
	m_bHasString = bShowText; 
	
	if (!m_bHasString)
	{
		if (nullptr == m_pToolTip)
		{
			m_pToolTip = new QYToolTip;
			if (!m_pToolTip->Create())
			{
				return;
			}
		}
	}
	else
	{
		SAFE_DESTROY_WINDOW_PTR(m_pToolTip);
	}
}

void QYToolBar::SetItemImage(int item, int image)
{
	QYToolBarItem *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		pItem->image[IMAGE_STATUS_NORMAL] = image;
		return;
	}

}

void QYToolBar::setItemImage(const char *key,  int imageID)
{
	QYToolBarItem *pItem = getItem(key);
	if (nullptr != pItem)
	{
		pItem->image[IMAGE_STATUS_NORMAL] = imageID;
		return;
	}
}

void QYToolBar::updateText()
{
	for (QYToolBarItem *it : m_listItem)
	{
		SET_TEXT_ID(it->textID, it->textID.c_str(), it->strText)
	}

	Invalidate();
}