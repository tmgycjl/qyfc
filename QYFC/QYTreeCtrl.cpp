#include "QYTreeCtrl.h"
#include "QYDC.h"
#include "QYSize.h"
#include "QYApp.h"

QYTreeCtrl::QYTreeCtrl() :QYWindow()
{
	m_nItemHeight = 20;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_bVShowScroll = FALSE;
	m_bHShowScroll = FALSE;
	m_nScrollPos = 0;
	m_nVScrollWidth = 0;
	m_nHScrollHeight = 0;
	m_nSelItem = 0;
	m_nHoverItem = 0;
	m_bDown = FALSE;
	m_bFocus = FALSE;
	m_bReDraw = TRUE;
	m_bHasButton = TRUE;
	m_rectItemList = 0;
	m_nHScrollPos = 0;
	m_bCheckFlag = FALSE;
	m_bDraging = FALSE;
	m_bOpenTrackPosTime = FALSE;
	m_sizeButton = 12;
	m_sizeImage = 0;
	m_sizeCheckBox = 0;
	m_nClearanceWidth = 2;
	m_bDragFlag = FALSE;
	m_picNormalBox = nullptr;
	m_picSelectBox = nullptr;
	m_picExpand = nullptr;
	m_picCollapse = nullptr;
	m_bImageFlag = TRUE;
	m_bAlwaysHilight = FALSE;
	m_pCurrentListItem = nullptr;
	m_bCheckOnlyFlag = FALSE;
	m_CheckOnlyItem = 0;
}

QYTreeCtrl::~QYTreeCtrl()
{
	SAFE_DELETE(m_picNormalBox);
	SAFE_DELETE(m_picSelectBox);
	SAFE_DELETE(m_picExpand);
	SAFE_DELETE(m_picCollapse);
	Clear();
}

LRESULT QYTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//	g_ConsoleLog.Print("message :%x\n", message);
	switch (message)
	{
	case WM_NCDESTROY:
	{
		Clear();

		m_VScrollBar.DestroyWindow();
		m_HScrollBar.DestroyWindow();
		SAFE_DESTROY_WINDOW_PTR(m_pToolTip);
	}
	break;
	case WM_SETFOCUS:
	{
		m_bFocus = TRUE; 
		InvalidateRect(&m_rectItemList); 

	}
	break;
	case WM_KILLFOCUS:
	{
		m_bFocus = FALSE; 
		InvalidateRect(&m_rectItemList);
	}
	break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_RBUTTONDOWN:{OnRButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_LBUTTONDBLCLK:{OnLButtonDBClick(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_RBUTTONUP:{OnRButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_SETCURSOR:{ return OnSetCursor(wParam, lParam); }break;
	case WM_VSCROLL:
	{
		if (onVScroll(this, LOWORD(wParam)))
		{
			InvalidateRect(&m_rectItemList);
		}

		return TRUE;
	}
	break;
	case WM_MOUSEWHEEL:{onMouseWheel(this, LOWORD(wParam), HIWORD(wParam)); }break;
	case WM_TIMER:
	{
		if (onTimer(this, wParam))
		{
			InvalidateRect(&m_rectItemList);
		}
	}
	break;
	case WM_HSCROLL:
	{
		if (onHScroll(this, LOWORD(wParam)))
		{
			InvalidateRect(&m_rectItemList);
		}
		return TRUE; 
	}
	break;

	case QY_WM_TREECTRL_CHECK_ALL:{SetCheckAll(wParam); }break;
	default:break;
	}

	return 0L;
}

BOOL QYTreeCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYTreeCtrl",
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

	m_picExpand = new QYPicture;
	assert(nullptr != m_picExpand);
	m_picExpand->Load(IDP_EXPAND, QY_IMAGE_PNG);
	m_picCollapse = new QYPicture;;
	assert(nullptr != m_picCollapse);
	m_picCollapse->Load(IDP_COLLAPSE, QY_IMAGE_PNG);

	m_sizeButton = QYSize(m_picExpand->GetWidth(), m_picExpand->GetHeight());

	m_pToolTip = new QYToolTip;
	m_pToolTip->Create();

	//SetFont((HFONT)(QYApp::m_pMsDefultFont->m_hObject));
	return ret;
}

LRESULT QYTreeCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	if (m_bDown)
	{
		m_bDown = FALSE;
		InvalidateRect(&m_rectItemList);
		
		struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
		if (nullptr != actionCB)
		{
			QYPropertyList properties;
			properties.addProperty("id", getID());
			//properties.addProperty("item", m_nSelItem);
			properties.addProperty("action", "click");
			actionCB->callback(&properties);
		}
		else
		{
			::SendMessage(::GetParent(m_hWnd), QY_WM_TREECTRL_ITEM_CLICK, (WPARAM)m_hWnd, m_nSelItem);
		}
	}

	if (m_bDragFlag && m_bDraging)
	{
		EndDrag();
	}

	return TRUE;
}

LRESULT QYTreeCtrl::OnRButtonUp(UINT nFlags, QYPoint point)
{
	if (m_bDown)
	{
		m_bDown = FALSE;
		InvalidateRect(&m_rectItemList);
		::SendMessage(::GetParent(m_hWnd), QY_WM_TREECTRL_ITEM_RCLICK, (WPARAM)m_hWnd, m_nSelItem);
	}
	return TRUE;
}

QYTreeCtrlItem* QYTreeCtrl::GetParentItem(QYTreeCtrlItem* pItem)
{
	return  (nullptr != pItem) ? pItem->pParentItem : nullptr;
}

DWORD QYTreeCtrl::GetParentItem(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		return (nullptr != pItem->pParentItem) ? pItem->pParentItem->id : 0;
	}

	return 0;
}

DWORD QYTreeCtrl::GetFirstItem(DWORD parentItem)
{
	QYTreeCtrlItem *pItem = nullptr;
	
	if (0 == parentItem)
	{
		m_CurrentIt = m_listItem.begin();
		m_pCurrentListItem = &m_listItem;
		if (m_listItem.end() != m_CurrentIt)
		{
			return (*m_CurrentIt)->id;
		}
	}
	else
	{
		pItem = FindItem(nullptr, parentItem);
		if (nullptr != pItem)
		{
			if (0 < pItem->m_listSubItem.size())
			{
				m_CurrentIt = pItem->m_listSubItem.begin();
				m_pCurrentListItem = &pItem->m_listSubItem;
				if (pItem->m_listSubItem.end() != m_CurrentIt)
				{
					return (*m_CurrentIt)->id;
				}
			}
		}
	}
	
	

	return 0;
}

DWORD QYTreeCtrl::GetNextItem()
{
	if (nullptr != m_pCurrentListItem)
	{
		if (m_pCurrentListItem->end() != m_CurrentIt)
		{
			m_CurrentIt++;
			if (m_pCurrentListItem->end() != m_CurrentIt)
			{
				return (*m_CurrentIt)->id;
			}
		}
	}

	return 0;
}

int QYTreeCtrl::GetItemLevel(QYTreeCtrlItem *pItem)
{
	int level = 0;
	QYTreeCtrlItem *parentItem = pItem;
	do
	{
		parentItem = GetParentItem(parentItem);
	} while (++level && 0 != parentItem);

	return level;
}

int QYTreeCtrl::GetItemLevel(DWORD item)
{
	int level = 0;
	DWORD parentItem = item;
	do
	{
		parentItem = GetParentItem(parentItem);
	} while (++level && 0 != parentItem);

	return level;
}

LRESULT QYTreeCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	m_bDown = TRUE;
	SetFocus();

	m_DownPoint = point;

	if (point.y > m_rectItemList.top && point.y < m_rectItemList.bottom)
	{
		QYTreeCtrlItem *pItem = GetItemFromPoint(point);

		m_nSelItem = (nullptr != pItem) ? pItem->id : 0;

		InvalidateRect(&m_rectItemList);

		QYRect rectItem;
		if (GetItemRect(m_nSelItem, rectItem) )
		{
			if (rectItem.PtInRect(point))
			{
				QYTreeCtrlItem *pItem = FindItem(nullptr, m_nSelItem);
				if (nullptr != pItem )
				{
					int level = GetItemLevel(m_nSelItem);

					if (m_bCheckFlag)
					{
						QYRect rectCheckBox;
						rectCheckBox.left = 2 + m_rectItemList.left + m_sizeButton.cx \
							+ 2+ (level - 1) * (m_sizeButton.cx + 5) - m_nHScrollPos;
						rectCheckBox.top = rectItem.top + (rectItem.Height() - m_sizeCheckBox.cy) / 2;
						rectCheckBox.right = m_sizeCheckBox.cx + rectCheckBox.left;
						rectCheckBox.bottom = rectCheckBox.top + m_sizeCheckBox.cy;
						if (rectCheckBox.PtInRect(point))
						{
							if (m_bCheckOnlyFlag )
							{
								pItem->check = 1;

								if ( m_CheckOnlyItem != pItem->id)
								{
									QYTreeCtrlItem *pLastItem = FindItem(nullptr, m_CheckOnlyItem);
									if (nullptr != pLastItem)
									{
										pLastItem->check = 0;
										
									}
									m_CheckOnlyItem = pItem->id;
								}
							}
							else
							{
								(1 == pItem->check) ? pItem->check = 0 : pItem->check = 1;
							}

							struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
							if (nullptr != actionCB)
							{
								QYPropertyList properties;
								properties.addProperty("id", getID());
								properties.addProperty("action", "item_checked");
								actionCB->callback(&properties);
							}
							else
							{
								::SendMessage(::GetParent(m_hWnd), QY_WM_TREECTRL_ITEM_CHECKED, (WPARAM)m_hWnd, m_nSelItem);
							}

							InvalidateRect(&m_rectItemList);

							return TRUE;
						}
					}

					if (0 < pItem->m_listSubItem.size())
					{
						QYRect rectButton;
						rectButton.left = 2 + m_rectItemList.left + (level - 1) * (m_sizeButton.cx + 5) - m_nHScrollPos;
						rectButton.top = rectItem.top + (rectItem.Height() - m_sizeButton.cy) / 2;
						rectButton.right = m_sizeButton.cx + rectButton.left;
						rectButton.bottom = rectButton.top + m_sizeButton.cy;
						if (rectButton.PtInRect(point))
						{
							(1 == pItem->expand) ? pItem->expand = 0 : pItem->expand = 1;
							UpdateLayout();
							return TRUE;
						}
					}
				}
				else
				{

				}
			}
			
		}
		
	}

	return TRUE;
}

LRESULT QYTreeCtrl::OnRButtonDown(UINT nFlags, QYPoint point)
{
	m_bDown = TRUE;
	SetFocus();

	if (point.y > m_rectItemList.top && point.y < m_rectItemList.bottom)
	{
		QYTreeCtrlItem *pItem = GetItemFromPoint(point);
		m_nSelItem = (nullptr != pItem) ? pItem->id : 0;
		InvalidateRect(&m_rectItemList);
	}
	return TRUE;
}

LRESULT QYTreeCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	//OnLButtonDown(nFlags, point);

	if (0 != m_nSelItem)
	{
		QYTreeCtrlItem *pItem = FindItem(nullptr, m_nSelItem);
		if (nullptr != pItem && 0 < pItem->m_listSubItem.size())
		{
			(1 == pItem->expand) ? pItem->expand = 0 : pItem->expand = 1;
			UpdateLayout();
		}


		struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
		if (nullptr != actionCB)
		{
			QYPropertyList properties;
			properties.addProperty("id", getID());
			//properties.addProperty("item", m_nSelItem);
			properties.addProperty("action", "double_click");
			actionCB->callback(&properties);
		}
		else
		{
			::SendMessage(::GetParent(m_hWnd), QY_WM_TREECTRL_ITEM_DBCLICK, (WPARAM)m_hWnd, m_nSelItem);
		}
		
	}
	
	return TRUE;
}

LRESULT QYTreeCtrl::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);

		m_bHover = TRUE;
		InvalidateRect(&m_rectItemList);
	}
	else
	{
		if (m_bDragFlag 
			&& m_bDown
			&& !m_bDraging 
			&& 0 != m_nSelItem
			&& (10 < std::abs(point.x - m_DownPoint.x)
			|| 10 < std::abs(point.y - m_DownPoint.y)))
		{
			BeginDrag();
		}
	}

	if (point.y > m_rectItemList.top && point.y < m_rectItemList.bottom)
	{
		int lastHoverItem = m_nHoverItem;

		QYTreeCtrlItem *pItem = GetItemFromPoint(point);
		m_nHoverItem = (nullptr != pItem) ? pItem->id : 0;

		 if (0 < m_nHoverItem && lastHoverItem != m_nHoverItem)
		{
			const TCHAR *pText = pItem->strText;
			int itemWidth = GetItemSpan(pItem->textWidth, GetItemLevel(pItem));
			//g_ConsoleLog.Print("itemWidth.cx:%d\n", itemWidth);

			if (itemWidth - m_nHScrollPos > m_rectItemList.right)
			{
				m_pToolTip->Show(SW_HIDE);

				m_pToolTip->SetText(pText);

				QYPoint pt;
				
				pt.y = m_nItemHeight * ((point.y - m_rectItemList.top) / m_nItemHeight);
				pt.x = itemWidth - pItem->textWidth;
				ClientToScreen(&pt);
				m_pToolTip->TrackPopup(nullptr, pt);
			}
			else
			{
				m_pToolTip->Show(SW_HIDE);
			}
		}
	}


	return TRUE;
}

LRESULT QYTreeCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nHoverItem = -1;
		InvalidateRect(&m_rectItemList);
		(nullptr != m_pToolTip) ? m_pToolTip->Show(SW_HIDE) : 0;
	}

	return TRUE;
}

BOOL QYTreeCtrl::SetItemData(DWORD item, DWORD_PTR data)
{
	int i = 0;
	
	return FALSE;
}

BOOL QYTreeCtrl::SetItemText(DWORD item, const TCHAR* strText)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		pItem->strText = strText;
		Invalidate();
		return TRUE;
	}

	return FALSE;
}


BOOL QYTreeCtrl::setItemText(DWORD item, const char* text)
{
    QYTreeCtrlItem *pItem = FindItem(nullptr, item);
    if (nullptr != pItem)
    {
        SET_TEXT(pItem->strText, text)
       // pItem->strText = strText;
        Invalidate();
        return TRUE;
    }

    return FALSE;
}

void QYTreeCtrl::selectItem(DWORD item)
{
	m_nSelItem = item;
	Invalidate();
}

BOOL QYTreeCtrl::SetItemImage(DWORD item, int image)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		pItem->image = image;
		Invalidate();
		return TRUE;
	}

	return FALSE;
}

BOOL QYTreeCtrl::SetCheckFlag(BOOL bCheckFlag)
{
	BOOL lastCheckFlag = m_bCheckFlag;

	m_bCheckFlag = bCheckFlag;

	if (m_bCheckFlag)
	{
		m_picNormalBox = new QYPicture;
		assert(nullptr != m_picNormalBox);
		m_picNormalBox->Load(IDP_CHECK_NORMAL, QY_IMAGE_PNG);
		m_picSelectBox = new QYPicture;;
		assert(nullptr != m_picSelectBox);
		m_picSelectBox->Load(IDP_CHECK_SELECTED, QY_IMAGE_PNG);

		m_sizeCheckBox = QYSize(m_picNormalBox->GetWidth(), m_picSelectBox->GetHeight());
	}
	else
	{
		SAFE_DELETE(m_picSelectBox);
		SAFE_DELETE(m_picNormalBox);
	}

	UpdateLayout();

	return lastCheckFlag;
}

BOOL QYTreeCtrl::SetCheckOnlyFlag(BOOL bCheckOnlyFlag)
{
	BOOL lastCheckOnlyFlag = bCheckOnlyFlag;

	m_bCheckOnlyFlag = bCheckOnlyFlag;

	return m_bCheckFlag;
}

BOOL QYTreeCtrl::SetHasButton(BOOL bHasButton)
{
	BOOL  last = m_bHasButton;
	m_bHasButton = bHasButton;
	Invalidate();

	return last;
}

BOOL QYTreeCtrl::SetCheckAll(BOOL bCheckAll)
{
	int i = 0;

	InvalidateRect(&m_rectItemList);

	return TRUE;
}

int  QYTreeCtrl::SetItemHeight(int height)
{
	if (0 != height % 2)
	{
		height++;
	}
	if (0 >= height)
	{
		height = 20;
	}

	int last = m_nItemHeight;
	m_nItemHeight = height;

	return last;
}

int QYTreeCtrl::GetItemSpan(int textWidth, int level)
{
	int totalWidth = m_nBorderWidth + m_nClearanceWidth + level * (m_sizeButton.cx + m_nClearanceWidth) + textWidth;

	if (m_bCheckFlag)
	{
		totalWidth += m_sizeCheckBox.cx + m_nClearanceWidth;
	}

	if (m_bImageFlag)
	{
		totalWidth += m_sizeImage.cx + m_nClearanceWidth;
	}

	return  totalWidth;
}

BOOL QYTreeCtrl::GetMaxVisibleItemWidth(int &width, QYTreeCtrlItem *pParentItem)
{
	int itemWidth = 0;
	if (nullptr == pParentItem)
	{
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
		{
			itemWidth = GetItemSpan((*it)->textWidth, GetItemLevel((*it)));
			
			width = (width < itemWidth) ? itemWidth : width;
			
			if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
			{
				GetMaxVisibleItemWidth(width, (*it));
			}
		}
	}
	else
	{
		int count = 0;
		for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
		{
			 itemWidth = GetItemSpan((*it)->textWidth, GetItemLevel((*it)));

			width = (width < itemWidth) ? itemWidth : width;

			if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
			{
				GetMaxVisibleItemWidth(width, (*it));
			}
		}
	}

	return TRUE;
}

int QYTreeCtrl::GetVisibleItemCount(QYTreeCtrlItem *pParentItem)
{
	int count = 0;
	if (nullptr == pParentItem)
	{
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
		{
			if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
			{
				count += GetVisibleItemCount((*it));
			}
			count++;
		}
	}
	else
	{
		for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
		{
			if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
			{
				count += GetVisibleItemCount((*it));
			}
			count++;
		}
	}

	return count;
}

int QYTreeCtrl::GetChildItemCount(DWORD item)
{
	if (0 == item)
	{
		return m_listItem.size();
	}
	else
	{
		QYTreeCtrlItem *pItem = FindItem(nullptr, item);
		if (nullptr != pItem)
		{
			return pItem->m_listSubItem.size();
		}
	}
	
	return 0;
}

BOOL  QYTreeCtrl::GetItemRect(DWORD item, QYRect &rect)
{
	int index = GetVisibleIndex(item);
	if (-1 == index)
	{
		return FALSE;
	}

	rect = m_rectItemList;
	rect.top += m_nItemHeight * index - m_nScrollPos;
	rect.bottom = rect.top + m_nItemHeight;

	return TRUE;
}


int QYTreeCtrl::GetVisibleIndex(DWORD item)
{
	int index = -1;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, index++)
	{
		if ((*it)->id == item)
		{
			return ++index;
		}
		if (1 == (*it)->expand && 0 <  (*it)->m_listSubItem.size())
		{
			if (GetVisibleIndex((*it), item, index))
			{
				return ++index;
			}
		}
	}

	return index;
}

BOOL QYTreeCtrl::GetVisibleIndex(QYTreeCtrlItem *pParentItem, DWORD item, int &index)
{
	for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
	{
		if ((*it)->id == item)
		{
			index++;
			return TRUE;
		}
		if (1 == (*it)->expand && 0 < (*it)->m_listSubItem.size())
		{
			if (GetVisibleIndex((*it), item, index))
			{
				index++;
				return TRUE;
			}
		}

		index++;
	}
	return FALSE;
}

QYTreeCtrlItem* QYTreeCtrl::GetItemFromPoint(QYPoint point)
{
	int findItemIndex =( m_nScrollPos + point.y - m_rectItemList.top) / m_nItemHeight;

	int startIndex = -1;
	return GetItemFromIndex(nullptr, findItemIndex, startIndex);
}

QYTreeCtrlItem* QYTreeCtrl::GetItemFromIndex(QYTreeCtrlItem *pParentItem, int index, int &findIndex)
{
	if (nullptr == pParentItem)
	{
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
		{
			findIndex++;

			if (index == findIndex)
			{
				return (*it);
			}
			else if ((*it)->expand && 0 < (*it)->m_listSubItem.size())
			{
				auto item = GetItemFromIndex((*it), index, findIndex);
				if (nullptr != item)
				{
					return item;
				}
				else if(findIndex > index)
				{
					return nullptr;
				}
			}

		}
	}
	else
	{
		for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
		{
			findIndex++;
			if (index == findIndex)
			{
				return (*it);
			}
			else if ((*it)->expand && 0 < (*it)->m_listSubItem.size())
			{
				auto item = GetItemFromIndex((*it), index, findIndex);
				if (nullptr != item)
				{
					return item;
				}
				else if (findIndex > index)
				{
					return nullptr;
				}
			}
		}
	}

	return nullptr;
}

QYTreeCtrlItem* QYTreeCtrl::FindItem(QYTreeCtrlItem *pParentItem, DWORD item)
{
	if (nullptr == pParentItem)
	{
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
		{
			if ((*it)->id == item)
			{
				return (*it);
			}
			else
			{
				QYTreeCtrlItem *pItem = FindItem((*it), item);
				if (nullptr != pItem)
				{
					return pItem;
				}
			}
		}
	}
	else
	{
		for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
		{
			if ((*it)->id == item)
			{
				return (*it);
			}
			else
			{
				QYTreeCtrlItem *pItem = FindItem((*it), item);
				if (nullptr != pItem)
				{
					return pItem;
				}
			}
		}
	}
	
	return nullptr;
}

DWORD QYTreeCtrl::InsertItem(DWORD parentItem, const TCHAR* strText, int image, DWORD_PTR data)
{
	QYPaintDC dc(this);
	dc.SelectObject(m_pFont);
	DWORD id = 0;
	
	int realImage = image;
	QYPicture *pic = GetImage(image);
	if (nullptr == pic)
	{
		realImage = -1;
	}
	else
	{
		QYSize sizeImage = QYSize(pic->GetWidth(), pic->GetHeight());
		m_sizeImage.cx = std::fmax(m_sizeImage.cx, sizeImage.cx);
		m_sizeImage.cy = std::fmax(m_sizeImage.cy, sizeImage.cy);
	}

	if (0 == parentItem)
	{
		QYTreeCtrlItem *pItem = new QYTreeCtrlItem(nullptr, (TCHAR*)strText, realImage, data);
		assert(nullptr != pItem);
		pItem->id = reinterpret_cast<DWORD>(pItem);
		
		QYSize size = dc.GetTextExtent(&pItem->strText);
		pItem->textWidth = size.cx;
		m_listItem.push_back(pItem);

		id = pItem->id;
	}
	else
	{
		QYTreeCtrlItem *pParentItem = FindItem(nullptr,parentItem);
		if (nullptr == pParentItem)
		{
			return 0;
		}

		QYTreeCtrlItem *pItem = new QYTreeCtrlItem(pParentItem, (TCHAR*)strText, realImage, data);
		assert(nullptr != pItem);
		pItem->id = reinterpret_cast<DWORD>(pItem);
		QYSize size = dc.GetTextExtent(&pItem->strText);
		pItem->textWidth = size.cx;
		pParentItem->m_listSubItem.push_back(pItem);

		id = pItem->id;
	}
	
	m_bReDraw ? UpdateLayout() : 0;

	return id;
}


DWORD QYTreeCtrl::insertItem(DWORD parentItem, const char* text, int image, DWORD_PTR data)
{
	QYPaintDC dc(this);
	dc.SelectObject(m_pFont);
	DWORD id = 0;

	int realImage = image;
	QYPicture *pic = GetImage(image);
	if (nullptr == pic)
	{
		realImage = -1;
	}
	else
	{
		QYSize sizeImage = QYSize(pic->GetWidth(), pic->GetHeight());
		m_sizeImage.cx = std::fmax(m_sizeImage.cx, sizeImage.cx);
		m_sizeImage.cy = std::fmax(m_sizeImage.cy, sizeImage.cy);
	}

	if (0 == parentItem)
	{
		QYTreeCtrlItem *pItem = new QYTreeCtrlItem(nullptr, text, realImage, data);
		assert(nullptr != pItem);
		pItem->id = reinterpret_cast<DWORD>(pItem);

		QYSize size = dc.GetTextExtent(&pItem->strText);
		pItem->textWidth = size.cx;
		m_listItem.push_back(pItem);

		id = pItem->id;
	}
	else
	{
		QYTreeCtrlItem *pParentItem = FindItem(nullptr, parentItem);
		if (nullptr == pParentItem)
		{
			return 0;
		}

		QYTreeCtrlItem *pItem = new QYTreeCtrlItem(pParentItem, text, realImage, data);
		assert(nullptr != pItem);
		pItem->id = reinterpret_cast<DWORD>(pItem);
		QYSize size = dc.GetTextExtent(&pItem->strText);
		pItem->textWidth = size.cx;
		pParentItem->m_listSubItem.push_back(pItem);

		id = pItem->id;
	}

	m_bReDraw ? UpdateLayout() : 0;

	return id;
}


DWORD_PTR QYTreeCtrl::GetItemData(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		return pItem->data;
	}

	return 0;
}

BOOL QYTreeCtrl::SetItem(DWORD item, const TCHAR* strText, DWORD_PTR data)
{
	int j = 0;
	
	return TRUE;
}


std::string  QYTreeCtrl::getItemText(DWORD item)
{
	const TCHAR *text = GetItemText(item);
	if (nullptr != text)
	{
		std::string sText;
		char szText[1024] = { 0 };
		QYString::SafeW2AUTF8(szText, 1024, text);
		sText = szText;

		return sText;
	}

	return "";
}

LPCTSTR QYTreeCtrl::GetItemText(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		return &pItem->strText;
	}

	return L"";
}

BOOL QYTreeCtrl::DeleteItem(QYTreeCtrlItem *pParentItem, DWORD item)
{
	for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
	{
		if (item == (*it)->id)
		{
			if (0 < (*it)->m_listSubItem.size())
			{
				Clear(*it);
			}
			SAFE_DELETE(*it); 
			pParentItem->m_listSubItem.erase(it);
			(m_nSelItem == item) ? m_nSelItem = 0 : 0;
			return TRUE;
		}
		BOOL ret = FALSE;
		if ((ret = DeleteItem((*it), item)))
		{
			return ret;
		}
	}

	return FALSE;
}


BOOL QYTreeCtrl::DeleteItem(DWORD item)
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		if (item == (*it)->id)
		{
			if (0 < (*it)->m_listSubItem.size())
			{
				Clear(*it);
			}
			SAFE_DELETE(*it);
			m_listItem.erase(it);
			(m_nSelItem == item) ? m_nSelItem = 0 : 0;
			m_bReDraw ? UpdateLayout() : 0;
			return TRUE;
		}
		BOOL ret = FALSE;
		if ((ret = DeleteItem((*it), item)))
		{
			m_bReDraw ? UpdateLayout() : 0;
			return ret;
		}
	}

	return FALSE;
}

BOOL QYTreeCtrl::DeleteAllItems()
{
	Clear();

	UpdateLayout();

	return TRUE;
}

void QYTreeCtrl::Clear(QYTreeCtrlItem *pParentItem )
{
	if (nullptr == pParentItem)
	{
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
		{
			if (0 < (*it)->m_listSubItem.size())
			{
				Clear(*it);
			}

			SAFE_DELETE(*it);
		}

		m_listItem.clear();
	}
	else
	{
		for (auto it = pParentItem->m_listSubItem.begin(); it != pParentItem->m_listSubItem.end(); it++)
		{
			if (0 < (*it)->m_listSubItem.size())
			{
				Clear(*it);
			}
			SAFE_DELETE(*it);
		}

		pParentItem->m_listSubItem.clear();
	}
	
	return;
}

BOOL QYTreeCtrl::SetRedraw(BOOL bRedraw)
{
	BOOL last = m_bReDraw;
	m_bReDraw = bRedraw;

	UpdateLayout();

	return last;
}

BOOL QYTreeCtrl::Expand(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem && 0 < pItem->m_listSubItem.size())
	{
		 pItem->expand = 1;
		Invalidate();

		return TRUE;
	}

	return FALSE;
}

BOOL QYTreeCtrl::Collapse(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem && 0 < pItem->m_listSubItem.size())
	{
		 pItem->expand = 0;
		Invalidate();
		return TRUE;
	}

	return FALSE;
}

void QYTreeCtrl::DrawItem(QYDC *pDC, QYRect rect, QYRect &rectItem, QYTreeCtrlItem *pItem)
{
	int ScrollPos = m_nScrollPos;
	int posoffset = 0;
	if (m_bVShowScroll)
	{
		int showPixs = (rect.Height() - 2 * m_nBorderWidth - m_nHScrollHeight) ;
		int totalHeight = m_listItem.size()*m_nItemHeight;
		if (m_nScrollPos == m_VScrollBar.GetRange()
			&& m_nScrollPos < (totalHeight - showPixs))
		{
			ScrollPos = totalHeight - showPixs;
			posoffset = ScrollPos - m_nScrollPos;
		}
	}

	int top = rect.top + m_nBorderWidth;
	int bottom = rect.bottom - m_nBorderWidth - m_nHScrollHeight;
	int offset = ScrollPos;

	QYRect rectTemp = 0;
	QYRect rectBk = 0;
	std::list<QYTreeCtrlItem*> *plistItem;

	if (nullptr == pItem)
	{
		plistItem = &m_listItem;
		rectItem.left = rect.left + m_nBorderWidth + m_nClearanceWidth - m_nHScrollPos;
		rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth - m_nHScrollPos ;
		rectItem.top = rect.top + m_nBorderWidth - offset;
		rectItem.bottom = rectItem.top ;
	}
	else
	{
		plistItem = &pItem->m_listSubItem;
	}

	int itemLineLeft = 0;
	int itemLineTop = 0;
	int itemLineRight = 0;
	int itemLineBottom = 0;
	int size = plistItem->size();
	int item = 0;
	for (auto it = plistItem->begin(); it != plistItem->end(); it++, item++)
	{
		rectTemp.left = rectItem.left ;
		rectTemp.right = rectItem.right ;
		rectTemp.top = rectItem.bottom ;
		rectTemp.bottom = rectTemp.top + m_nItemHeight;

		if (rectTemp.bottom <= top)
		{
			if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
			{
				rectTemp.left += m_sizeButton.cx + m_nClearanceWidth;
				DrawItem(pDC, rect, rectTemp, *it);
				rectTemp.left -= m_sizeButton.cx + m_nClearanceWidth;
			}
			rectItem = rectTemp;
			continue;
		}
		else if (rectTemp.top >= bottom )
		{
			break;
		}

		rectBk.left   = rect.left;
		rectBk.right  = rect.right - m_nBorderWidth - m_nVScrollWidth;
		rectBk.top    = rectTemp.top;
		rectBk.bottom = rectTemp.bottom;

		if (m_bFocus || m_bAlwaysHilight)
		{
			if (m_nSelItem != (*it)->id - posoffset )
			{
				pDC->FillSolidRect(rectBk, QY_THEME.LISTCTRL_BK_COLOR_NORMAL);
			}
			else
			{
				pDC->FillSolidRect(rectBk, QY_THEME.ITEM_BK_COLOR_SEL);
			}
		}
		else
		{
			if (m_nSelItem == (*it)->id - posoffset)
			{
				//pDC->FillSolidRect(rectBk, LISTCTRL_BK_COLOR_LOSE_FOCUS);
			}
			else
			{
				//pDC->FillSolidRect(rectBk, LISTCTRL_BK_COLOR_NORMAL);
			}
		}

		QYRect rectText = rectTemp;

		rectText.right += m_nHScrollPos;

		QYRect rectButton = 0;
		rectButton.left = rectTemp.left ;
		rectButton.right = rectButton.left + m_sizeButton.cx;
		rectButton.top = rectTemp.top + (rectTemp.Height() - m_sizeButton.cy) / 2;
		rectButton.bottom = rectButton.top + m_sizeButton.cy;

		if (0 <  (*it)->m_listSubItem.size())
		{
			if (1 == (*it)->expand)
			{
				m_picCollapse->Draw(pDC->GetDC(),rectButton);
			}
			else
			{
				m_picExpand->Draw(pDC->GetDC(), rectButton);
			}
		}
		else
		{
			
		}

		rectText.left = rectButton.right + m_nClearanceWidth;

		if (m_bCheckFlag)
		{
			QYRect rectCheckBox = 0;
			rectCheckBox.left = rectText.left;
			rectCheckBox.right = rectCheckBox.left + m_sizeCheckBox.cx;
			rectCheckBox.top = rectTemp.top + (rectTemp.Height() - m_sizeCheckBox.cy) / 2;
			rectCheckBox.bottom = rectCheckBox.top + m_sizeCheckBox.cy;
			if (1 == (*it)->check)
			{
				m_picSelectBox->Draw(pDC->GetDC(), rectCheckBox);
			}
			else
			{
				m_picNormalBox->Draw(pDC->GetDC(), rectCheckBox);
			}

			rectText.left = rectCheckBox.right + m_nClearanceWidth;
		}

		if (m_bImageFlag  && - 1 != (*it)->image)
		{
			QYRect rectImage = 0;
			rectImage.left = rectText.left;
			rectImage.right = rectImage.left + m_sizeImage.cx;
			rectImage.top = rectTemp.top + (rectTemp.Height() - m_sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + m_sizeImage.cy;
			QYPicture *pic = GetImage((*it)->image);
			(nullptr != pic) ? pic->Draw(pDC->GetDC(), rectImage) : 0;
		
			rectText.left = rectImage.right + m_nClearanceWidth;
		}
		
		pDC->DrawText(&(*it)->strText, (*it)->strText.Length(), &rectText, DT_SINGLELINE | DT_VCENTER);

		if (0 < (*it)->m_listSubItem.size() && 1 == (*it)->expand)
		{
			rectTemp.left += m_sizeButton.cx ;
			DrawItem(pDC, rect, rectTemp, *it);
			rectTemp.left -= m_sizeButton.cx ;
		}
		rectItem = rectTemp;
	}
}

LRESULT QYTreeCtrl::OnPaint()
{
	if (!m_bReDraw)
	{
		return TRUE;
	}

	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);
	QYBrush Brush(m_colorBk);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPoint point;

	if (m_rectItemList == 0)
	{
		QYRect rectItem = QYRect(rect.left,
			rect.top + m_nBorderWidth ,
			rect.right - m_nBorderWidth - m_nVScrollWidth,
			rect.bottom  - m_nBorderWidth - m_nHScrollHeight);
		MemDc.FillRect(rectItem, Brush);
	}
	else
	{
		MemDc.FillRect(m_rectItemList, Brush);
		if (m_bHShowScroll && m_nVScrollWidth)
		{
			QYRect rectRightDown = QYRect(rect.right - m_nBorderWidth - m_nVScrollWidth,
				rect.bottom - m_nBorderWidth - m_nHScrollHeight,
				rect.right - m_nBorderWidth,
				rect.bottom - m_nBorderWidth);
			MemDc.FillRect(rectRightDown, Brush);
		}
	}

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	QYRect rt= 0;

	DrawItem(&MemDc, rect, rt, nullptr);
	
	if (0 < m_nBorderWidth)
	{
		QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTCTRL_BORDER_COLOR);
		HPEN OldPen;

		OldPen = (HPEN)MemDc.SelectObject(&PenLine);
		MemDc.Rectangle(rect, (HBRUSH)GetStockObject(NULL_BRUSH));

		MemDc.SelectObject(OldPen);
		PenLine.DeleteObject();
	}

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	//Pen.DeleteObject();
	MemDc.DeleteDC();
	
	
	return TRUE;
}

BOOL QYTreeCtrl::OnMove(int cx, int cy)
{
	return FALSE;
}

BOOL QYTreeCtrl::UpdateLayout()
{
	QYRect rect;
	GetClientRect(rect);

	if (0 == rect.Width() || 0 == rect.Height())
	{
		return TRUE;
	}

	int maxWidth = 0;
	GetMaxVisibleItemWidth(maxWidth, nullptr);

	updateVScrollLayout(this, rect, GetVisibleItemCount() * m_nItemHeight - (rect.Height() - 2 * m_nBorderWidth - m_nHScrollHeight));

	updateHScrollLayout(this, rect, maxWidth -( rect.right - m_nBorderWidth - m_nVScrollWidth - rect.left));

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,
		m_nBorderWidth ,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth - m_nHScrollHeight);

	InvalidateRect(&m_rectItemList);

	return TRUE;
}

LRESULT QYTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	return UpdateLayout();
}

LRESULT QYTreeCtrl::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	if (0 == wparam)
	{
		SetCursor(LoadCursor(nullptr, IDC_NO));
	}
	else if (1 == wparam)
	{
		SetCursor(LoadCursor(nullptr,IDC_ARROW));
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


void QYTreeCtrl::BeginDrag()
{
	SetCapture();
	QYApp::StartDragMouseHook(m_hWnd);
	m_bDraging = TRUE;
}

void QYTreeCtrl::EndDrag()
{
	QYApp::StopDragMouseHook();

	ReleaseCapture();

	m_bDraging = FALSE;
}

BOOL QYTreeCtrl::SetAlwaysHilight(BOOL bAlwaysHilight)
{
	m_bAlwaysHilight = bAlwaysHilight;

	return TRUE;
}

DWORD QYTreeCtrl::GetCheckOnlyItem()
{
	return m_CheckOnlyItem;
}

BOOL QYTreeCtrl::GetCheck(DWORD item)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem)
	{
		return (1 == pItem->check) ? TRUE : FALSE;
	}

	return FALSE;
}

BOOL QYTreeCtrl::SetCheck(DWORD item, BOOL bCheck)
{
	QYTreeCtrlItem *pItem = FindItem(nullptr, item);
	if (nullptr != pItem )
	{
		pItem->check = bCheck ? 1 : 0;
		Invalidate();
	}


	return TRUE;
}