#include "QYListCtrl.h"
#include "QYDC.h"
#include "QYIPAddress.h"

unsigned int QYListCtrl::m_nMaxItemCount = 100000;
LISTCTRL_SORT_PARAM QYListCtrl::listctrl_sort_param;

QYListCtrl::QYListCtrl() :QYWindow()
{
	m_nItemHeight = 24;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nHeaderHeight = 30;
	m_nPagePaneHeight = 0;
	m_nSelItem = -1;
	m_bDown = FALSE;
	m_bFocus = FALSE;
	m_nLastHoverItem = -1;
	m_bReDraw = TRUE;
	m_rectItemList = 0;
	m_bCheckFlag = FALSE;
	m_bShowPagePane = FALSE;
	m_nVisibleItemCount = m_nMaxItemCount;
	m_nFirstVisibleItem = 0;
	m_bAlternateColorFlag = TRUE;
	m_nSeparatedWidth = 1;
    m_pHeaderCtrl = nullptr;
	m_pPagePane = nullptr;
	m_picNormalBox = nullptr;
	m_picSelectBox = nullptr;
    m_alwaysSelected = FALSE;
}

QYListCtrl::~QYListCtrl()
{
	SAFE_DELETE(m_picNormalBox);
	SAFE_DELETE(m_picSelectBox);
	Clear();
}

LRESULT QYListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		(nullptr == m_pHeaderCtrl) ? m_pHeaderCtrl = new QYHeaderCtrl : 0;

		if (nullptr == m_pHeaderCtrl->GetHwnd())
		{
			if (!m_pHeaderCtrl->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, 0, m_hWnd))
			{
				return FALSE;
			}
		}
	}
	break;
	case WM_NCDESTROY:
	{
		Clear();
		m_VScrollBar.DestroyWindow();
		SAFE_DESTROY_WINDOW_PTR(m_pHeaderCtrl);
		m_HScrollBar.DestroyWindow();
		SAFE_DESTROY_WINDOW_PTR(m_pPagePane);
		break;
	}
	case WM_SETFOCUS:{m_bFocus = TRUE; Invalidate(); }break;
	case WM_KILLFOCUS:{m_bFocus = FALSE; Invalidate(); }break;
	case WM_ERASEBKGND:{return TRUE;break;}
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam));}break;
	case WM_PAINT:{return OnPaint();}break;
	case WM_NCPAINT:{return TRUE;break;}
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;}
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam);}break;
	case WM_SETCURSOR:{ OnSetCursor(wParam, lParam); }break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_RBUTTONDOWN:{OnRButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_LBUTTONDBLCLK:{OnLButtonDBClick(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_RBUTTONUP:{OnRButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
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

		if (3 == wParam)
		{
			m_pHeaderCtrl->m_nScrollPos = m_nHScrollPos;
			m_pHeaderCtrl->Invalidate();
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
	case QY_WM_LISTCTRL_CHECK_ALL:{SetCheckAll(wParam); } return TRUE;
	case QY_WM_COLMUN_WIDTH_CHANGED:OnColmunWidthChanged(wParam); return TRUE;
	case QY_WM_LISTCTRL_CLICK_COLUMN:return OnClickColmun(wParam,lParam);
	case QY_WM_LAST_PAGE:
	{
		if (1 < wParam)
		{
			m_pPagePane->SetPage(wParam - 1);
			UpdateLayout();
		}
	}
	return TRUE;
	case QY_WM_NEXT_PAGE:
	{
		if (m_pPagePane->GetPageRange() > wParam)
		{
			m_pPagePane->SetPage(wParam + 1);
			UpdateLayout();
		}
	}
	return TRUE;
	case QY_WM_FIRST_PAGE:
	{
		if (1 != wParam)
		{
			m_pPagePane->SetPage(1);
			UpdateLayout();
		}
	}
	return TRUE;
	case QY_WM_END_PAGE:
	{
		if (wParam !=  m_pPagePane->GetPageRange())
		{
			m_pPagePane->SetPage(m_pPagePane->GetPageRange());
			UpdateLayout();
		}
	}
	return TRUE;
	default:break;
	}

	return 0L;
}

BOOL QYListCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYListCtrl",
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

LRESULT QYListCtrl::OnRButtonDown(UINT nFlags, QYPoint point)
{
	m_bDown = TRUE;
	SetFocus();

	if (point.y > m_rectItemList.top)
	{
		m_nSelItem = m_nFirstVisibleItem + ((point.y - m_rectItemList.top + m_nScrollPos) / m_nItemHeight);
		InvalidateRect(&m_rectItemList);
	}

	return TRUE;
}

LRESULT QYListCtrl::OnRButtonUp(UINT nFlags, QYPoint point)
{
	if (m_bDown)
	{
		m_bDown = FALSE;
		InvalidateRect(&m_rectItemList);
		NotifyMessage(QY_WM_LISTCTRL_ITEM_RCLICK, (WPARAM)m_hWnd, m_nSelItem);
		//::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_ITEM_RCLICK, (WPARAM)m_hWnd, m_nSelItem);
	}
	return TRUE;
}

LRESULT QYListCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	m_bDown = TRUE;
	SetFocus();
	if (point.y > m_rectItemList.top)
	{
		m_nSelItem = m_nFirstVisibleItem + ((point.y - m_rectItemList.top + m_nScrollPos) / m_nItemHeight);
		InvalidateRect(&m_rectItemList);
	}

	if (m_nSelItem >= m_listItem.size())
	{
        m_nSelItem = -1;
		return TRUE;
	}

	if (m_bCheckFlag && nullptr != m_picNormalBox)
	{
		int i = 0;
		int colmuns = m_pHeaderCtrl->GetItemCount();
		auto it = m_listItem.begin();
		std::advance(it, m_nSelItem);
		//for (; it != m_listItem.end(); it++, i++)
		{
			if (it != m_listItem.end())
			{
				int tempcolum = 0;
				auto headerIt = m_pHeaderCtrl->m_listItem.begin();
				for (auto subIt = (*it)->m_listSubItem.begin();
					subIt != (*it)->m_listSubItem.end() && headerIt != m_pHeaderCtrl->m_listItem.end();
					tempcolum++, subIt++, headerIt++)
				{
					if ((*subIt)->left  <= point.x + m_nHScrollPos
						&& (*subIt)->right  >= point.x + m_nHScrollPos)
					{
						if (0 != tempcolum)
						{
							return TRUE;
						}
						point.x -= (*subIt)->left;
						point.y = (point.y + m_nScrollPos - m_rectItemList.top)%m_nItemHeight;
						QYRect rectTemp2;
						rectTemp2.left = (*subIt)->left+ 5 + 2;
						rectTemp2.top = (m_nItemHeight - m_picNormalBox->GetHeight()) / 2;
						rectTemp2.right = rectTemp2.left + m_picNormalBox->GetWidth();
						rectTemp2.bottom = rectTemp2.top + m_picNormalBox->GetHeight();
						if (rectTemp2.PtInRect(point))
						{
							(*it)->check = (0 == (*it)->check) ? 1 : 0;
							m_pHeaderCtrl->SetCheckAll(IsCheckAll());
						}

						return TRUE;
					}
					
				}
			}
		}
	}
	
	return (LRESULT)TRUE;
}

LRESULT QYListCtrl::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	NotifyMessage(QY_WM_LISTCTRL_ITEM_DBCLICK, m_nSelItem, 0);
	//::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_ITEM_DBCLICK, m_nSelItem, 0);

	return OnLButtonDown(nFlags, point);
}

LRESULT QYListCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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
// 		m_nHoverItem = m_nScrollPos + ((point.y - m_rectItemList.top) / m_nItemHeight);
// 		if ( m_nLastHoverItem != m_nHoverItem)
// 		{
// 			InvalidateRect(&m_rectItemList);
// 			m_nLastHoverItem = m_nHoverItem;
// 		}
	}

	return TRUE;
}

LRESULT QYListCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nHoverItem = -1;
		InvalidateRect(&m_rectItemList);
	}

	return TRUE;
}

int QYListCtrl::GetColumnCount()
{
	assert(nullptr != m_pHeaderCtrl);

	return  m_pHeaderCtrl->GetItemCount();
}

BOOL QYListCtrl::SetItemData(int item, DWORD_PTR data)
{	
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			(*it)->data = data;
			return TRUE;
		}
	}

	return FALSE;
}

QYListCtrlSubItem* QYListCtrl::FindSubItem(int item, int subitem)
{
	if (subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	QYListCtrlItem *pItem = FindItem(item);

	if (nullptr != pItem)
	{
		int tempcolum = 0;
		for (auto subIt = pItem->m_listSubItem.begin(); subIt != pItem->m_listSubItem.end(); subIt++, tempcolum++)
		{
			if (tempcolum == subitem)
			{
				return (*subIt);
			}
		}
	}

	return nullptr;
}

QYListCtrlItem* QYListCtrl::FindItem(int item)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			return (*it);
		}
	}

	return nullptr;
}

BOOL QYListCtrl::SetSubItemData(int item, int subitem, DWORD_PTR data)
{
	if (subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (item == i)
		{
			int tempcolum = 0;
			for (auto subIt = (*it)->m_listSubItem.begin(); subIt != (*it)->m_listSubItem.end(); subIt++,tempcolum++)
			{
				if (tempcolum == subitem)
				{
					(*subIt)->data = data;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

int QYListCtrl::InsertColumn(int column, 
	const TCHAR* strText, 
	int width, 
	int format, 
	enumSortType sortType ,
	DWORD_PTR data)
{
	assert(nullptr != m_pHeaderCtrl);

	int item = m_pHeaderCtrl->InsertItem(column, strText, width, format, sortType, data);
	if (0 < item)
	{
		m_bReDraw ? UpdateLayout() : 0;
	}
	return item;
}

int QYListCtrl::insertColumn(int column,
    const char* text,
    int width,
    int format,
    enumSortType sortType,
    DWORD_PTR data)
{
    assert(nullptr != m_pHeaderCtrl);

    int item = m_pHeaderCtrl->insertItem(column, text, width, format, sortType, data);
    if (0 < item)
    {
        m_bReDraw ? UpdateLayout() : 0;
    }
    return item;
}


BOOL QYListCtrl::SetSortColmun(int colmun, int sortType)
{
	QYHeaderCtrlItem *pCol = m_pHeaderCtrl->GetItem(colmun);
	if (nullptr != pCol)
	{
		pCol->sortType = sortType;
	}

	return FALSE;
}

int QYListCtrl::InsertColumn(const TCHAR* strText, int width, int format, enumSortType sortType, DWORD_PTR data)
{
	assert(nullptr != m_pHeaderCtrl);

	return InsertColumn(m_pHeaderCtrl->GetItemCount(), strText, width, format, sortType, data);
}

int QYListCtrl::insertColumn(const char* text, int width, int format, enumSortType sortType, DWORD_PTR data)
{
    assert(nullptr != m_pHeaderCtrl);

    return insertColumn(m_pHeaderCtrl->GetItemCount(), text, width, format, sortType, data);
}

BOOL QYListCtrl::SetColumnText(int column, const TCHAR* strText)
{
	assert(nullptr != m_pHeaderCtrl);
	return m_pHeaderCtrl->SetItemText(column, strText);
}

TCHAR* QYListCtrl::GetColumnText(int column)
{
	return &m_pHeaderCtrl->GetItem(column)->strText;
}

BOOL QYListCtrl::SetColumnWidth(int column, int width)
{
	assert(nullptr != m_pHeaderCtrl);
	return m_pHeaderCtrl->SetItemWidth(column, width);
}

BOOL  QYListCtrl::DeleteColumn(int column)
{
	assert(nullptr != m_pHeaderCtrl);
	BOOL ret =   m_pHeaderCtrl->DeleteItem(column);

	m_bReDraw ? UpdateLayout() : 0;

	return ret;
}

BOOL QYListCtrl::DeleteAllColumns()
{
	assert(nullptr != m_pHeaderCtrl);
	BOOL ret = m_pHeaderCtrl->DeleteAllItems();

	m_bReDraw ? UpdateLayout() : 0;

	return ret;
}

BOOL QYListCtrl::SetItemHeight(int height)
{
	if (0 != height % 2)
	{
		height++;
	}
	int last = m_nItemHeight;
	m_nItemHeight = height;

	return last;
}

int QYListCtrl::GetItemCount()
{
	return m_listItem.size();
}

BOOL QYListCtrl::GetItemRect(int item, int subitem, QYRect &rect)
{
	int colmuns = m_pHeaderCtrl->GetItemCount();
	if (subitem >= colmuns)
	{
		return FALSE;
	}

	rect.left = m_pHeaderCtrl->GetItem(subitem)->left;
	rect.right = m_pHeaderCtrl->GetItem(subitem)->right;
	rect.top = item  * m_nItemHeight - m_nScrollPos;
	rect.bottom = rect.top + m_nItemHeight;
	return TRUE;
}

int QYListCtrl::InsertItem(int item, const TCHAR* strText, int image , DWORD_PTR data)
{
	if (m_nMaxItemCount < m_listItem.size())
	{
		return -1;
	}

	QYListCtrlItem *pitem = new QYListCtrlItem;
	assert(nullptr != pitem);

	pitem->data = data;
	pitem->check = 0;
	int colmuns = m_pHeaderCtrl->GetItemCount();
	for (int i = 0; i < colmuns; i++)
	{
		QYListCtrlSubItem  *pSubItem = new QYListCtrlSubItem((0 == i) ? (TCHAR*)strText:nullptr);
		
		assert(nullptr != pSubItem);
		QYHeaderCtrlItem *pColumn = m_pHeaderCtrl->GetItem(i);
		if (nullptr != pColumn)
		{
			pSubItem->left = pColumn->left;
			pSubItem->right = pColumn->right;
			pitem->m_listSubItem.push_back(pSubItem);
		}
	}
	
	auto it = m_listItem.begin();
	if (-1 != item)
	{
		int endItem = m_listItem.size();
		if (-1 >= item || item > endItem)
		{
			SAFE_DELETE(pitem);
			return -1;
		}

		advance(it, item);
	}
	else
	{
		//pitem->index = m_listItem.size();
		it = m_listItem.end();
	}

	m_listItem.insert(it, pitem);
	m_pLastItem = pitem;

	m_bReDraw ? UpdateLayout() :0;

	return (m_listItem.size() - 1);
}

int QYListCtrl::insertItem(int item, const char* text, int image, DWORD_PTR data)
{
	if (m_nMaxItemCount < m_listItem.size())
	{
		return -1;
	}

	QYListCtrlItem *pitem = new QYListCtrlItem;
	assert(nullptr != pitem);

	pitem->data = data;
	pitem->check = 0;
	int colmuns = m_pHeaderCtrl->GetItemCount();
	for (int i = 0; i < colmuns; i++)
	{
		QYListCtrlSubItem  *pSubItem = new QYListCtrlSubItem((0 == i) ? text : "");

		assert(nullptr != pSubItem);
		QYHeaderCtrlItem *pColumn = m_pHeaderCtrl->GetItem(i);
		if (nullptr != pColumn)
		{
			pSubItem->left = pColumn->left;
			pSubItem->right = pColumn->right;
			pitem->m_listSubItem.push_back(pSubItem);
		}
	}

	auto it = m_listItem.begin();
	if (-1 != item)
	{
		int endItem = m_listItem.size();
		if (-1 >= item || item > endItem)
		{
			SAFE_DELETE(pitem);
			return -1;
		}

		advance(it, item);
	}
	else
	{
		//pitem->index = m_listItem.size();
		it = m_listItem.end();
	}

	m_listItem.insert(it, pitem);
	m_pLastItem = pitem;

	m_bReDraw ? UpdateLayout() : 0;

	return (m_listItem.size() - 1);
}

int QYListCtrl::InsertItem(const TCHAR* strText, int image, DWORD_PTR data)
{
	return InsertItem(-1, strText, image, data);
}


int QYListCtrl::insertItem(const char* text, int image, DWORD_PTR data)
{
	return insertItem(-1, text, image, data);
}

BOOL QYListCtrl::SetItem(int item, int subitem, const TCHAR* strText, DWORD_PTR data )
{
	if ( subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	int endItem = m_listItem.size();
	if (-1 >= item || item >= endItem)
	{
		return 0;
	}

	auto it = m_listItem.begin();
	advance(it, item);

	if (it == m_listItem.end())
	{
		return 0;
	}

	endItem = (*it)->m_listSubItem.size();
	if (-1 >= subitem || subitem >= endItem)
	{
		return 0;
	}

	auto subit = (*it)->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != (*it)->m_listSubItem.end())
	{
		(*subit)->strText = strText;
		(*subit)->data = data;
		return TRUE;
	}

	return FALSE;
}


BOOL QYListCtrl::setItem(int item, int subitem, const char* text, DWORD_PTR data)
{
	if (subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	int endItem = m_listItem.size();
	if (-1 >= item || item >= endItem)
	{
		return 0;
	}

	auto it = m_listItem.begin();
	advance(it, item);

	if (it == m_listItem.end())
	{
		return 0;
	}

	endItem = (*it)->m_listSubItem.size();
	if (-1 >= subitem || subitem >= endItem)
	{
		return 0;
	}

	auto subit = (*it)->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != (*it)->m_listSubItem.end())
	{
		SET_TEXT((*subit)->strText,text)

		(*subit)->data = data;
		return TRUE;
	}

	return FALSE;
}

DWORD_PTR QYListCtrl::GetItemData(int item)
{
	int endItem = m_listItem.size() ;
	if (-1 >= item || item >= endItem)
	{
		return 0;
	}
	auto it = m_listItem.begin();

	advance(it, item);

	if (it != m_listItem.end())
	{
		return (*it)->data;
	}

	return 0;
}

BOOL QYListCtrl::SetSubItemData(int subitem, DWORD_PTR data)
{
	if (nullptr == m_pLastItem)
	{
		return FALSE;
	}

	int endItem = m_pLastItem->m_listSubItem.size();
	if (-1 >= subitem || subitem >= endItem)
	{
		return FALSE;
	}

	auto subit = m_pLastItem->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != m_pLastItem->m_listSubItem.end())
	{
		(*subit)->data = data ;
		return TRUE;
	}
	return FALSE;
}

DWORD_PTR QYListCtrl::GetSubItemData(int item, int subitem)
{
	int endItem = m_listItem.size() - 1;
	if (-1 >= item || item >= endItem)
	{
		return 0;
	}
	auto it = m_listItem.begin();
	advance(it, item);

	if (it == m_listItem.end())
	{
		return 0;
	}

	endItem = (*it)->m_listSubItem.size();
	if (-1 >= subitem || subitem >= endItem)
	{
		return 0;
	}

	auto subit = (*it)->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != (*it)->m_listSubItem.end())
	{
		return (*subit)->data;
	}

	return 0;
}

BOOL QYListCtrl::SetItemData(DWORD_PTR data)
{
	if (nullptr == m_pLastItem)
	{
		return FALSE;
	}

	m_pLastItem->data = data;

	return TRUE;
}

BOOL QYListCtrl::SetItem(int subitem, const TCHAR* strText, DWORD_PTR data )
{
	if (nullptr == m_pLastItem || subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	if (-1 >= subitem)
	{
		return 0;
	}

	auto subit = m_pLastItem->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != m_pLastItem->m_listSubItem.end())
	{
		(*subit)->strText = strText;
		(*subit)->data = data;
        m_bReDraw ? Invalidate() : 0;
		return TRUE;
	}


	return TRUE;
}

BOOL QYListCtrl::setItemText(int item, int subitem, const char* text)
{
	if (nullptr == m_pLastItem || subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	if (-1 >= subitem)
	{
		return 0;
	}

	auto subit = m_pLastItem->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != m_pLastItem->m_listSubItem.end())
	{
		SET_TEXT((*subit)->strText, text)
		m_bReDraw ? Invalidate() : 0;
		return TRUE;
	}


	return TRUE;
}

BOOL QYListCtrl::setItem(int subitem, const char* text, DWORD_PTR data)
{
	if (nullptr == m_pLastItem || subitem >= m_pHeaderCtrl->GetItemCount())
	{
		return FALSE;
	}

	if (-1 >= subitem)
	{
		return 0;
	}

	auto subit = m_pLastItem->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != m_pLastItem->m_listSubItem.end())
	{
		SET_TEXT((*subit)->strText, text)
		//(*subit)->strText = strText;
		(*subit)->data = data;
		m_bReDraw ? Invalidate() : 0;
		return TRUE;
	}


	return TRUE;
}


LPCTSTR QYListCtrl::GetItemText(int item, int subitem)
{
	if (-1 >= item)
	{
		return nullptr;
	}
	auto it = m_listItem.begin();
	advance(it, item);

	if (it == m_listItem.end())
	{
		return nullptr;
	}

	if (-1 >= subitem)
	{
		return nullptr;
	}

	auto subit = (*it)->m_listSubItem.begin();
	advance(subit, subitem);

	if (subit != (*it)->m_listSubItem.end())
	{
		return &(*subit)->strText;
	}

	return nullptr;
}

BOOL QYListCtrl::DeleteItem(int startItem, int count)
{
	if (startItem >= m_listItem.size())
	{
		return FALSE;
	}

	auto itStart = m_listItem.end();
	auto itEnd = m_listItem.end();
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (count > i- startItem && i >= startItem)
		{
			if (i == startItem)
			{
				itStart = it;
			}
			else if (count == i - startItem + 1)
			{
				m_pLastItem == (*it) ? m_pLastItem = nullptr : 0;
				SAFE_DELETE(*it);
				itEnd = ++it;
				
				break;
			}
			
			m_pLastItem == (*it) ? m_pLastItem = nullptr : 0;
			SAFE_DELETE(*it);
		}
	}

	m_listItem.erase(itStart, itEnd);
	m_bReDraw ? UpdateLayout() : 0;

	return TRUE;
}

BOOL QYListCtrl::DeleteItem(int item)
{
	if (-1 >= item)
	{
		return  FALSE;
	}
	auto it = m_listItem.begin();
	advance(it, item);

	if (it != m_listItem.end())
	{
		m_pLastItem == (*it) ? m_pLastItem = nullptr : 0;
        m_nSelItem == item ? m_nSelItem = -1 : 0;
		SAFE_DELETE(*it);
		m_listItem.erase(it);
		m_bReDraw ? UpdateLayout() : 0;
		return TRUE;
	}


	return FALSE;
}

void QYListCtrl::SetHeaderCtrl(QYHeaderCtrl *pHeaderCtrl)
{
	m_pHeaderCtrl = pHeaderCtrl;

	if (nullptr != m_pHeaderCtrl && nullptr != m_pHeaderCtrl->GetHwnd())
	{
		QYRect rect;
		GetClientRect(rect);
		m_pHeaderCtrl->MoveWindow(rect.left + m_nBorderWidth,
			rect.top + m_nBorderWidth,
			rect.Width() - 2 * m_nBorderWidth - m_nVScrollWidth,
			m_nHeaderHeight);
		pHeaderCtrl->Show(SW_SHOW);
	}
}

BOOL QYListCtrl::DeleteAllItems()
{
	Clear();

	UpdateLayout();

	return TRUE;
}

void QYListCtrl::Clear()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		QYListCtrlItem *pItem = *it;
		SAFE_DELETE(pItem);
	}

	 m_pLastItem = nullptr ;

	m_listItem.clear();
}

BOOL QYListCtrl::SetRedraw(BOOL bRedraw)
{
	BOOL last = m_bReDraw;
	m_bReDraw = bRedraw;
	m_bReDraw ? UpdateLayout() : 0;

	return last;
}

LRESULT QYListCtrl::OnPaint()
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
	QYPen Pen(PS_SOLID, m_nSeparatedWidth, QY_THEME.LISTCTRL_SEPARATED_COLOR);
	HPEN  oldPen = (HPEN)MemDc.SelectObject(&Pen);
	QYBrush Brush(QY_THEME.LISTBOX_BK_COLOR_NORMAL);
	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPoint point;
	int colmuns = m_pHeaderCtrl->GetItemCount();

	if (m_rectItemList == 0)
	{
		QYRect rectItem = QYRect(rect.left, 
			rect.top + m_nBorderWidth + m_nHeaderHeight,
			rect.right - m_nBorderWidth - m_nVScrollWidth,
			rect.bottom - m_nHeaderHeight - m_nBorderWidth - m_nPagePaneHeight);
		MemDc.FillRect(rectItem, Brush);
	}
	else
	{
		MemDc.FillRect(m_rectItemList, Brush);
		if (m_bHShowScroll && m_bVShowScroll)
		{
			QYRect rectRightDown = QYRect(rect.right -m_nBorderWidth- m_nVScrollWidth,
				rect.bottom - m_nBorderWidth - m_nHScrollHeight - m_nPagePaneHeight,
				rect.right - m_nBorderWidth,
				rect.bottom - m_nBorderWidth - m_nPagePaneHeight);
			MemDc.FillRect(rectRightDown, Brush);
		}
	}
	
	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	int secondHalfItemCount = m_listItem.size() - m_nFirstVisibleItem;
	int realVisibleItemCount = (secondHalfItemCount >= m_nVisibleItemCount) ? m_nVisibleItemCount : secondHalfItemCount;

	int top = rect.top + m_nHeaderHeight + m_nBorderWidth;
	int bottom = rect.bottom - m_nBorderWidth - m_nHScrollHeight - m_nPagePaneHeight;
	
	int drawVertLine = 0;
	int i = m_nFirstVisibleItem;
	QYRect rectTemp = rect;
#if 1
	auto it = m_listItem.begin();
	advance(it, m_nFirstVisibleItem);
	for (;it != m_listItem.end() ;it++, i++)
	{
		if (i >= m_nFirstVisibleItem + realVisibleItemCount)
		{
			break;
		}
		rectTemp.left = rect.left + m_nBorderWidth ;
		rectTemp.right = rect.right - m_nBorderWidth + m_nVScrollWidth ;
		rectTemp.top = top + i*m_nItemHeight - m_nFirstVisibleItem*m_nItemHeight - m_nScrollPos;
		rectTemp.bottom = top + (i + 1)*m_nItemHeight - m_nFirstVisibleItem*m_nItemHeight - m_nScrollPos;

		if (rectTemp.bottom <= top)
		{
			continue;
		}
		else if ( rectTemp.top >= bottom )
		{
			break;
		}

		rectTemp.bottom -= m_nSeparatedWidth;
        if (m_bFocus || (m_alwaysSelected && !m_bFocus))
		{
				if (m_nSelItem != i )
				{
					if (m_bAlternateColorFlag && i % 2)
					{
						MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL2);
					}
					else
					{
						MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL);
					}
					//MemDc.FillSolidRect(rectTemp, LISTCTRL_BK_COLOR_NORMAL);
					MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
				}
				else
				{
					MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_SELECT);
					MemDc.SetTextColor(QY_THEME.TEXT_COLOR_SELECT);
				}
		}
		else
		{
			if (m_nSelItem == i )
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_LOSE_FOCUS);
				MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
			}
			else
			{
				if (m_bAlternateColorFlag && i % 2)
				{
					MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL2);
				}
				else
				{
					MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL);
				}
				MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
			}
			
		}

		QYPoint point;
		MemDc.MoveToEx(rectTemp.left, rectTemp.bottom, &point);
		MemDc.LineTo(rectTemp.right, rectTemp.bottom);
		//rectTemp.bottom += m_nSeparatedWidth;
		
		int tempcolum = 0;
		for (auto subIt = (*it)->m_listSubItem.begin(); subIt != (*it)->m_listSubItem.end() 
			&& colmuns > tempcolum; 
			subIt++, tempcolum++)
		{
			//MemDc.MoveToEx((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.top, &point);
		//	MemDc.LineTo((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.bottom);
	
			rectTemp.left = (*subIt)->left + 5 - m_nHScrollPos;
			rectTemp.right = (*subIt)->right - 5 - m_nHScrollPos;
			
			if (m_bCheckFlag && 0 == tempcolum)
			{
				if (0 == (*it)->check)
				{
					if (nullptr != m_picNormalBox)
					{
						QYRect rectTemp2;
						rectTemp2.left = rectTemp.left +2;
						rectTemp2.top = rectTemp.top + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2;
						rectTemp2.right = rectTemp2.left + m_picNormalBox->GetWidth();
						rectTemp2.bottom = rectTemp2.top + m_picNormalBox->GetHeight();

						m_picNormalBox->Draw(MemDc.GetDC(),
							rectTemp.left + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2,
							rectTemp.top + (rectTemp.Height() - m_picNormalBox->GetHeight()) / 2,
							m_picNormalBox->GetWidth(), m_picNormalBox->GetHeight());

						rectTemp.left = rectTemp2.right + 5;
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

						rectTemp.left = rectTemp2.right + 5;
					}
				}
			}
			MemDc.DrawText(&(*subIt)->strText, (*subIt)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER | DT_CENTER| DT_WORD_ELLIPSIS);
		}

		drawVertLine = 1;
	}

	//g_ConsoleLog.Print("pool %d\n", count);
#endif
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

	Pen.DeleteObject();
	MemDc.DeleteDC();

	return TRUE;
}

BOOL QYListCtrl::OnMove(int cx, int cy)
{
	return FALSE;
}


BOOL QYListCtrl::UpdateLayout()
{
	QYRect rect;
	GetClientRect(rect);

	if (0 >= rect.Width() || 0 >= rect.Height())
	{
		return FALSE;
	}

	int totalItemHeight = m_listItem.size()*m_nItemHeight;
	int showPixs = rect.Height() - 2 * m_nBorderWidth - m_nHeaderHeight - m_nHScrollHeight - m_nPagePaneHeight;
	updateVScrollLayout(this, rect, totalItemHeight - showPixs);

	if (nullptr != m_pHeaderCtrl && nullptr != m_pHeaderCtrl->GetHwnd())
	{
		m_pHeaderCtrl->MoveWindow(rect.left + m_nBorderWidth,
			rect.top + m_nBorderWidth,
			rect.Width() - 2 * m_nBorderWidth - m_nVScrollWidth,
			m_nHeaderHeight);
	}

	QYRect rectItem;
	m_pHeaderCtrl->GetLastItemRect(rectItem);
	updateHScrollLayout(this, rect, rectItem.right + m_pHeaderCtrl->m_nSeparatedWidth - rect.right);

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,
		m_nBorderWidth + m_nHeaderHeight,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth - m_nHScrollHeight - m_nPagePaneHeight);

	InvalidateRect(&m_rectItemList);

	return TRUE;
}

LRESULT QYListCtrl::OnSize(UINT nType, int cx, int cy)
{
	return UpdateLayout();
}

LRESULT QYListCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	if (m_bDown)
	{
		m_bDown = FALSE;
		InvalidateRect(&m_rectItemList);
		NotifyMessage(QY_WM_LISTCTRL_ITEM_CLICK, (WPARAM)m_hWnd, m_nSelItem);
		//::SendMessage(::GetParent(m_hWnd), QY_WM_LISTCTRL_ITEM_CLICK, (WPARAM)m_hWnd, m_nSelItem);
	}
	return TRUE;
}


LRESULT QYListCtrl::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	return FALSE;
}

BOOL QYListCtrl::SetPageFlag(BOOL bPageFlag, int visibleItemCount)
{
	if (m_bShowPagePane == bPageFlag)
	{
		return TRUE;
	}

	m_bShowPagePane = bPageFlag;
	if (m_bShowPagePane)
	{
		if (nullptr == m_pPagePane)
		{
			m_pPagePane = new QYPagePane;
			if (nullptr == m_pPagePane->GetHwnd())
			{
				if (!m_pPagePane->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE,0,m_hWnd))
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		if (nullptr != m_pPagePane && nullptr != m_pPagePane->GetHwnd())
		{
			m_pPagePane->Show(SW_HIDE);
		}
	}
	m_nVisibleItemCount = visibleItemCount;

	UpdateLayout();

	return TRUE;
}

BOOL QYListCtrl::SetCheckFlag(BOOL bCheckFlag)
{
	BOOL lastCheckFlag = m_bCheckFlag;

	m_bCheckFlag = bCheckFlag;
	assert(nullptr != m_pHeaderCtrl);
	m_pHeaderCtrl->SetCheckFlag(bCheckFlag);
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

	return lastCheckFlag;
}

BOOL QYListCtrl::SetAlternateColorFlag(BOOL bAlternateColorFlag)
{
	BOOL lastAlternateColorFlag = m_bAlternateColorFlag;
	m_bAlternateColorFlag = bAlternateColorFlag;

	return lastAlternateColorFlag;
}

BOOL QYListCtrl::IsCheckAll()
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (!(*it)->check)
		{
			return FALSE;
		}
		
	}

	return TRUE;
}

BOOL QYListCtrl::SetCheckAll(BOOL bCheckAll)
{
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		(*it)->check = bCheckAll;
	}

	InvalidateRect(&m_rectItemList);

	m_pHeaderCtrl->SetCheckAll(bCheckAll);

	return TRUE;
}

BOOL QYListCtrl::SetCheck(int item,BOOL bCheck)
{
	if (-1 >= item)
	{
		return  FALSE;
	}
	auto it = m_listItem.begin();
	advance(it, item);

	if (it != m_listItem.end())
	{
		BOOL bLastCheck = (*it)->check;
		(*it)->check = bCheck;
		return bLastCheck;
	}

	return FALSE;
}

BOOL QYListCtrl::GetCheck(int item)
{
	if (-1 >= item)
	{
		return  FALSE;
	}
	auto it = m_listItem.begin();
	advance(it, item);

	if (it != m_listItem.end())
	{
		return (BOOL)((*it)->check);
	}

	return FALSE;
}

BOOL QYListCtrl::SetCheck( BOOL bCheck)
{
	if (nullptr != m_pLastItem)
	{
		BOOL bCheck = m_pLastItem->check;
		m_pLastItem->check = bCheck;
		return bCheck;
	}
	else
	{
		return FALSE;
	}
}

BOOL QYListCtrl::GetCheck()
{
	if (nullptr != m_pLastItem)
	{
		return (BOOL)(m_pLastItem->check);
	}
	else
	{
		return FALSE;
	}
}

void QYListCtrl::OnColmunWidthChanged(int colmun)
{
	if (nullptr != m_pHeaderCtrl)
	{
		int i = 0;
		int colmuns = m_pHeaderCtrl->GetItemCount();
		for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
		{
			int tempcolum = 0;
			for (auto subIt = (*it)->m_listSubItem.begin(); subIt != (*it)->m_listSubItem.end() && colmuns > tempcolum; subIt++)
			{
				if (tempcolum >= colmun)
				{
					(*subIt)->left =  m_pHeaderCtrl->GetItem(tempcolum)->left;
					(*subIt)->right = m_pHeaderCtrl->GetItem(tempcolum)->right;
				}
				
				tempcolum++;
			}
		}

		UpdateLayout();
	}

	//InvalidateRect(&m_rectItemList);
}

int  QYListCtrl::GetItemDatas(int *itemArray, int items, DWORD_PTR *data)
{
	int itemdatas = 0;
	int i = 0;
	int item = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if (i == itemArray[item])
		{
			data[itemdatas++] = (*it)->data;
			item++;
			if (item >= items)
			{
				return itemdatas;
			}
		}

	}
	return itemdatas;
}

int  QYListCtrl::GetCheckItems(int *itemArray)
{
	int items = 0;
	int i = 0;
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		if ((BOOL)(*it)->check)
		{
			if (nullptr != itemArray)
			{
				itemArray[items++] = i;
			}
			else
			{
				items++;
			}
			
		}

	}
	return items;
}

LRESULT QYListCtrl::OnClickColmun(WPARAM wparam, LPARAM lparam)
{
	SortItems(wparam, lparam);
	
	return TRUE;
}

#include <algorithm>
#include <functional>

 bool QYListCtrl::less_num(const QYListCtrlItem  *item1, const QYListCtrlItem *item2)
{
	auto  subIt1 = item1->m_listSubItem.begin();
	if (item1->m_listSubItem.end() != subIt1)
	{
		advance(subIt1, listctrl_sort_param.column);
	}
	
	if (item1->m_listSubItem.end() == subIt1)
	{
		return false;
	}

	auto  subIt2 = item2->m_listSubItem.begin();
	if (item2->m_listSubItem.end() != subIt2)
	{
		advance(subIt2, listctrl_sort_param.column);
	}

	if (item2->m_listSubItem.end() == subIt2)
	{
		return false;
	}

	switch (listctrl_sort_param.type)
	{
	case SORT_TYPE_NUM:
	{
		if (SafeA2I((*subIt1)->strText) < SafeA2I((*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_FLOAT:
	{
		if (SafeA2F((*subIt1)->strText) < SafeA2F((*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_IP:
	{
		return (QYIPAddress::ConverseIP((*subIt1)->strText) < QYIPAddress::ConverseIP((*subIt2)->strText));
	}
	break;
	case SORT_TYPE_TEXT:
	{
		if (0 > SafeStrcmp((*subIt1)->strText, (*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_DATE:
	{
		int year1 = 0;
		int month1 = 0;
		int day1 = 0;
		if (3 != SafeSscanf((*subIt1)->strText, L"%d/%d/%d", &year1, &month1, &day1))
		{
			if (3 != SafeSscanf((*subIt1)->strText, L"%d-%d-%d", &year1, &month1, &day1))
			{
				return true;
			}
		}

		int year2 = 0;
		int month2 = 0;
		int day2 = 0;

		if (3 != SafeSscanf((*subIt2)->strText, L"%d/%d/%d", &year2, &month2, &day2))
		{
			if (3 != SafeSscanf((*subIt2)->strText, L"%d-%d-%d", &year2, &month2, &day2))
			{
				return true;
			}
		}

		if ((year1 * 10000 + month1 * 100 + day1) < (year2 * 10000 + month2 * 100 + day2))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_TIME:
	{
		int hour1 = 0;
		int minute1 = 0;
		int second1 = 0;
		if (3 != SafeSscanf((*subIt1)->strText, L"%d:%d:%d", &hour1, &minute1, &second1))
		{
			return true;
		}

		int hour2 = 0;
		int minute2 = 0;
		int second2 = 0;
		if (3 != SafeSscanf((*subIt2)->strText, L"%d:%d:%d", &hour2, &minute2, &second2))
		{
			return true;
		}

		if ((hour1 * 10000 + minute1 * 100 + second1) < (hour2 * 10000 + minute2 * 100 + second2))
		{
			return true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}

 bool QYListCtrl::large_num(const QYListCtrlItem  *item1, const QYListCtrlItem *item2)
{
	auto  subIt1 = item1->m_listSubItem.begin();
	if (item1->m_listSubItem.end() != subIt1)
	{
		advance(subIt1, listctrl_sort_param.column);
	}

	if (item1->m_listSubItem.end() == subIt1)
	{
		return false;
	}

	auto  subIt2 = item2->m_listSubItem.begin();
	if (item2->m_listSubItem.end() != subIt2)
	{
		advance(subIt2, listctrl_sort_param.column);
	}

	if (item2->m_listSubItem.end() == subIt2)
	{
		return false;
	}

	switch (listctrl_sort_param.type)
	{
	case SORT_TYPE_NUM:
	{
		if (SafeA2I((*subIt1)->strText) > SafeA2I((*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_FLOAT:
	{
		if (SafeA2F((*subIt1)->strText) > SafeA2F((*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_TEXT:
	{
		if (0 < SafeStrcmp((*subIt1)->strText, (*subIt2)->strText))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_IP:
	{
		return (QYIPAddress::ConverseIP((*subIt1)->strText) > QYIPAddress::ConverseIP((*subIt2)->strText));
	}
		break;
	case SORT_TYPE_DATE:
	{
		int year1 = 0;
		int month1 = 0;
		int day1 = 0;
		if (3 != SafeSscanf((*subIt1)->strText, L"%d/%d/%d", &year1, &month1, &day1))
		{
			if (3 != SafeSscanf((*subIt1)->strText, L"%d-%d-%d", &year1, &month1, &day1))
			{
				return true;
			}
		}

		int year2 = 0;
		int month2 = 0;
		int day2 = 0;

		if (3 != SafeSscanf((*subIt2)->strText, L"%d/%d/%d", &year2, &month2, &day2))
		{
			if (3 != SafeSscanf((*subIt2)->strText, L"%d-%d-%d", &year2, &month2, &day2))
			{
				return true;
			}
		}

		if ((year1 * 10000 + month1 * 100 + day1) > (year2 * 10000 + month2 * 100 + day2))
		{
			return true;
		}
	}
	break;
	case SORT_TYPE_TIME:
	{
		int hour1 = 0;
		int minute1 = 0;
		int second1 = 0;
		if (3 != SafeSscanf((*subIt1)->strText, L"%d:%d:%d", &hour1, &minute1, &second1))
		{
			return true;
		}

		int hour2 = 0;
		int minute2 = 0;
		int second2 = 0;
		if (3 != SafeSscanf((*subIt2)->strText, L"%d:%d:%d", &hour2, &minute2, &second2))
		{
			return true;
		}

		if ((hour1 * 10000 + minute1 * 100 + second1) > (hour2 * 10000 + minute2 * 100 + second2))
		{
			return true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}


BOOL QYListCtrl::Less(LPCTSTR strText1, LPCTSTR strText2, enumSortType sortType)
{
	switch (sortType)
	{
	case SORT_TYPE_NUM:
	{
		if (SafeA2I(strText1) < SafeA2I(strText2))
		{
			return TRUE;
		}
	}
	break;
	case SORT_TYPE_FLOAT:
	{
		if (SafeA2F(strText1) < SafeA2F(strText2))
		{
			return TRUE;
		}
	}
	break;
	case SORT_TYPE_TEXT:
	{
		if (0 > SafeStrcmp(strText1, strText2))
		{
			return TRUE;
		}
	}
		break;
	case SORT_TYPE_DATE:
	{
		int year1 = 0;
		int month1 = 0;
		int day1 = 0;
		if (3 != SafeSscanf(strText1, L"%d/%d/%d", &year1, &month1, &day1))
		{
			if (3 != SafeSscanf(strText1, L"%d-%d-%d", &year1, &month1, &day1))
			{
				return TRUE;
			}
		}

		int year2 = 0;
		int month2 = 0;
		int day2 = 0;

		if (3 != SafeSscanf(strText2, L"%d/%d/%d", &year2, &month2, &day2))
		{
			if (3 != SafeSscanf(strText2, L"%d-%d-%d", &year2, &month2, &day2))
			{
				return TRUE;
			}
		}

		if ((year1 * 10000 + month1 * 100 + day1) < (year2 * 10000 + month2 * 100 + day2))
		{
			return TRUE;
		}
	}
	break;
	case SORT_TYPE_TIME:
	{
		int hour1 = 0;
		int minute1 = 0;
		int second1 = 0;
		if (3 != SafeSscanf(strText1, L"%d:%d:%d", &hour1, &minute1, &second1))
		{
			return TRUE;
		}

		int hour2 = 0;
		int minute2 = 0;
		int second2 = 0;
		if (3 != SafeSscanf(strText2, L"%d:%d:%d", &hour2, &minute2, &second2))
		{
			return TRUE;
		}

		if ((hour1 * 10000 + minute1 * 100 + second1 )< (hour2 * 10000 + minute2 * 100 + second2))
		{
			return TRUE;
		}
	}
	break;
	default:
		break;
	}

	return FALSE;
}

void QYListCtrl::SortItems(int column, int  ascend)
{
	QYHeaderCtrlItem *pColumn = m_pHeaderCtrl->GetItem(column);
	if (nullptr != pColumn)
	{
		if (SORT_TYPE_NONE == pColumn->sortType || 1 >= m_listItem.size())
		{
			return;
		}

		(0xff == pColumn->ascend) ? pColumn->ascend = 0 : 0;
		
		long time = GetTickCount();
		int size = m_listItem.size();
		auto it = m_listItem.begin();

#if 1
		QYListCtrl::listctrl_sort_param.type = pColumn->sortType;
		QYListCtrl::listctrl_sort_param.column = column;
		stable_sort(m_listItem.begin(), m_listItem.end(), (0 == pColumn->ascend) ? less_num : large_num);
#if 0


		it++;
		auto itEnd = m_listItem.end();
		for (; it != itEnd; it++)
		{
			QYListCtrlItem *tempItem = *it;
			auto it2 = it;
			while (it2 != m_listItem.begin())
			{
				auto it3 = it2; it3--;
				auto subIt1 = (*it3)->m_listSubItem.begin();
				advance(subIt1, column);
				auto subIt2 = tempItem->m_listSubItem.begin();
				advance(subIt2, column);
				if (0 == pColumn->ascend)
				{
					if (!Less(&(*subIt2)->strText, &(*subIt1)->strText, (enumSortType)pColumn->sortType))
					{
						break;
					}
				}
				else
				{
					if (Less(&(*subIt2)->strText, &(*subIt1)->strText, (enumSortType)pColumn->sortType))
					{
						break;
					}
				}
					
				*it2 = *it3;
				it2--;
			}
			*it2 = tempItem;
		}
#endif
#else
		auto itBegin = m_listItem.begin();
		auto itEnd = m_listItem.end();itEnd--;
		for (; it != itEnd; it++)
		{
			auto it3 = itEnd;
			int dis = distance(itBegin, it);
			advance(it3, -dis);
			auto it2 = itBegin;
			for (; it2 != it3; it2++)
			{
				auto it4 = it2;
				it4++;
				auto subIt1 = (*it2)->m_listSubItem.begin();
				advance(subIt1, column);
				auto subIt2 = (*it4)->m_listSubItem.begin();
				advance(subIt2, column);
				if (!Less(&(*subIt1)->strText, &(*subIt2)->strText, (enumSortType)pColumn->sortType))
				{
					QYListCtrlItem *pTempItem = *it4;
					*it4 = *it2;
					*it2 = pTempItem;
				}
			}
		}
#endif


		//g_ConsoleLog.Print("sort time:%d\n", GetTickCount() - time);
		pColumn->ascend = 1 - pColumn->ascend;
		InvalidateRect(&m_rectItemList);
	}
}

void QYListCtrl::setAlwaysSelected(BOOL alwaysSelected)
{
    m_alwaysSelected = alwaysSelected;
    Invalidate();
}