#include "QYColorListCtrl.h"
#include "QYDC.h"



QYColorListCtrl::QYColorListCtrl()
{
}


QYColorListCtrl::~QYColorListCtrl()
{
}

BOOL QYColorListCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYListCtrl::Create(dwExStyle,
		L"QYColorListCtrl",
		lpWndName,
		dwStyle,
		rect,
		hParentWnd);

	if (!ret)
	{
		return FALSE;
	}

	return ret;
}

LRESULT QYColorListCtrl::OnPaint()
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
			rect.top + m_nBorderWidth + m_nHeaderHeight - m_nPagePaneHeight,
			rect.right - m_nBorderWidth - m_nVScrollWidth,
			rect.bottom - m_nHeaderHeight - m_nBorderWidth - m_nPagePaneHeight);
		MemDc.FillRect(rectItem, Brush);
	}
	else
	{
		MemDc.FillRect(m_rectItemList, Brush);
		if (m_bHShowScroll && m_nVScrollWidth)
		{
			QYRect rectRightDown = QYRect(rect.right - m_nBorderWidth - m_nVScrollWidth,
				rect.bottom - m_nBorderWidth - m_nHScrollHeight - m_nPagePaneHeight,
				rect.right - m_nBorderWidth,
				rect.bottom - m_nBorderWidth - m_nPagePaneHeight);
			MemDc.FillRect(rectRightDown, Brush);
		}
	}

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	int top = rect.top + m_nHeaderHeight + m_nBorderWidth;
	int bottom = rect.bottom - m_nBorderWidth - m_nHScrollHeight - m_nPagePaneHeight;

	int drawVertLine = 0;
	int i = m_nFirstVisibleItem;
	QYRect rectTemp = rect;
#if 1
	int secondHalfItemCount = m_listItem.size() - m_nFirstVisibleItem;
	int realVisibleItemCount = (secondHalfItemCount >= m_nVisibleItemCount) ? m_nVisibleItemCount : secondHalfItemCount;

	auto it = m_listItem.begin();
	advance(it, m_nFirstVisibleItem);
	for (; it != m_listItem.end(); it++, i++)
	{
		if (i >= m_nFirstVisibleItem + realVisibleItemCount)
		{
			break;
		}

		rectTemp.left = rect.left + m_nBorderWidth;
		rectTemp.right = rect.right - m_nBorderWidth + m_nVScrollWidth;
		rectTemp.top = top + (i - m_nFirstVisibleItem - m_nScrollPos)*m_nItemHeight;
		rectTemp.bottom = top + (i - m_nFirstVisibleItem + 1 - m_nScrollPos) *m_nItemHeight ;

		if (rectTemp.bottom <= top)
		{
			continue;
		}
		else if (rectTemp.top >= bottom - (m_nItemHeight / 2))
		{
			break;
		}

		rectTemp.bottom -= m_nSeparatedWidth;

// 		if (m_bAlternateColorFlag && i % 2)
// 		{
// 			MemDc.FillSolidRect(rectTemp, LISTCTRL_BK_COLOR_NORMAL2);
// 		}
// 		else
		{
			MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL);
		}

		MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

		if (m_nSelItem == i )
		{
			QYPen pensel;
			pensel.CreatePen(PS_SOLID, 1, RGB(0, 122, 204));
			HPEN oldpen = (HPEN)MemDc.SelectObject(&pensel);
			MemDc.Rectangle(rect.left + m_nBorderWidth,
				rectTemp.top,rect.right - m_nBorderWidth - m_nVScrollWidth,rectTemp.bottom, (HBRUSH)::GetStockObject(NULL_BRUSH));
			//MemDc.SetTextColor(TEXT_COLOR_SELECT);
			MemDc.SelectObject(oldpen);
			pensel.DeleteObject();
		}

		QYPoint point;
		MemDc.MoveToEx(rectTemp.left, rectTemp.bottom, &point);
		MemDc.LineTo(rectTemp.right, rectTemp.bottom);

		int tempcolum = 0;
		for (auto subIt = (*it)->m_listSubItem.begin(); subIt != (*it)->m_listSubItem.end()
			&& colmuns > tempcolum;
			subIt++, tempcolum++)
		{
			//MemDc.MoveToEx((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.top, &point);
			//MemDc.LineTo((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.bottom);

			rectTemp.left = (*subIt)->left + 5 - m_nHScrollPos;
			rectTemp.right = (*subIt)->right -5 - m_nHScrollPos;

			if (-1 != ((QYColorListCtrlSubItem*)(*subIt))->color)
			{
				MemDc.FillSolidRect(rectTemp.left - 4,rectTemp.top + 1,rectTemp.right,rectTemp.bottom - 1, ((QYColorListCtrlSubItem*)(*subIt))->color);
			}

			if (m_bCheckFlag && 0 == tempcolum)
			{
				if (0 == (*it)->check)
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
			MemDc.DrawText(&(*subIt)->strText, (*subIt)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_WORD_ELLIPSIS);
		}

		drawVertLine = 1;
	}
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

BOOL QYColorListCtrl::SetItemBkColor(int item, int subitem, COLORREF color)
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
			for (auto subIt = (*it)->m_listSubItem.begin(); subIt != (*it)->m_listSubItem.end(); subIt++, tempcolum++)
			{
				if (tempcolum == subitem)
				{
					((QYColorListCtrlSubItem*)(*subIt))->color = color;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}


int QYColorListCtrl::InsertItem(int item, const TCHAR* strText, int image, DWORD_PTR data)
{
	QYListCtrlItem *pitem = new QYListCtrlItem;
	assert(nullptr != pitem);

	pitem->data = data;
	pitem->check = 0;
	int colmuns = m_pHeaderCtrl->GetItemCount();
	for (int i = 0; i < colmuns; i++)
	{
		QYListCtrlSubItem  *pSubItem = new QYColorListCtrlSubItem((0 == i) ? (TCHAR*)strText : nullptr);

		assert(nullptr != pSubItem);

		pSubItem->left = m_pHeaderCtrl->GetItem(i)->left;
		pSubItem->right = m_pHeaderCtrl->GetItem(i)->right;
		pitem->m_listSubItem.push_back(pSubItem);
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

	return m_listItem.size();
}

int QYColorListCtrl::InsertItem(const TCHAR* strText, int image, DWORD_PTR data)
{
	return InsertItem(-1, strText, image, data);
}

BOOL QYColorListCtrl::SetItem(int item, int subitem, const TCHAR* strText, COLORREF color, DWORD_PTR data)
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
			int j = 0;
			for (auto subit = (*it)->m_listSubItem.begin(); subit != (*it)->m_listSubItem.end(); subit++, j++)
			{
				if (subitem == j)
				{
					(*subit)->strText = strText;
					(*subit)->data = data;
					((QYColorListCtrlSubItem*)(*subit))->color = color;
					return TRUE;
				}
			}
		}
	}

	m_bReDraw ? InvalidateRect(&m_rectItemList) : 0;

	return FALSE;
}

BOOL QYColorListCtrl::SetItem(int subitem, const TCHAR* strText, COLORREF color, DWORD_PTR data)
{
	if (nullptr == m_pLastItem || subitem >= m_pHeaderCtrl->GetItemCount())
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
		(*subit)->strText = strText;
		(*subit)->data = data;
		((QYColorListCtrlSubItem*)(*subit))->color = color;
		return TRUE;
	}

	return FALSE;
}
