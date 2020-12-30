#include "QYButtonListCtrl.h"
#include "QYDC.h"

void (WINAPI *g_ButtonCmdcallback)(WPARAM wparam, LPARAM lparam) = nullptr;


QYButtonListCtrl::QYButtonListCtrl()
{
	m_pButtonFont = new QYFont;
	//m_pButtonSelFont = new QYFont;
	m_selButtonItem = -1;

	m_pButtonFont->CreateFont(QY_THEME.MS_YAHEI_FONT_SIZE, 0, FW_NORMAL, QY_THEME.MS_YAHEI_FONT_TYPE, 0);
}

QYButtonListCtrl::~QYButtonListCtrl()
{
	if (m_pButtonFont)
	{
		m_pButtonFont->DeleteObject();
		SAFE_DELETE(m_pButtonFont);
	}

	for (auto it = m_vectorButton.begin(); it != m_vectorButton.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vectorButton.clear();
}

LRESULT QYButtonListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return QYListCtrl::WindowProc(message, wParam, lParam);
}

BOOL QYButtonListCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYListCtrl::Create(dwExStyle,
		L"QYButtonListCtrl",
		lpWndName,
		dwStyle | WS_CLIPCHILDREN,
		rect,
		hParentWnd);

	if (!ret)
	{
		return FALSE;
	}

	if (nullptr == m_pButtonFont)
	{
		m_pButtonFont = new QYFont;

		m_pButtonFont->CreateFont(QY_THEME.BUTTON_FONT_SIZE, 0, FW_NORMAL, QY_THEME.DLG_FONT_TYPE, 0);
	}
	
	if (nullptr == m_pButtonSelFont)
	{
		m_pButtonSelFont = new QYFont;

		m_pButtonSelFont->CreateFont(QY_THEME.BUTTON_FONT_SIZE, 0, FW_NORMAL, QY_THEME.DLG_FONT_TYPE, 1);
	}

	return ret;
}

LRESULT QYButtonListCtrl::OnLButtonUp(UINT nFlags, QYPoint point)
{
	if (-1 != m_downButtonItem)
	{
		for (auto btnIt = m_vectorButton.begin(); btnIt != m_vectorButton.end(); btnIt++)
		{
			if ((*btnIt)->m_nColmun == m_downButtonItem)
			{
				m_downButtonItem = -1;
				InvalidateRect(&m_rectItemList);
				NotifyMessage(WM_COMMAND, (WPARAM)(*btnIt)->m_ID, (LPARAM)m_nSelItem);
				m_nSelItem = -1;
				return TRUE;
			}
		}
	}
	return TRUE;
}

LRESULT QYButtonListCtrl::OnMouseMove(UINT nFlags, QYPoint point)
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
		m_nHoverItem = m_nFirstVisibleItem + m_nScrollPos + (point.y - m_rectItemList.top) / m_nItemHeight;
		if (m_nLastHoverItem != m_nHoverItem)
		{
			InvalidateRect(&m_rectItemList);
			m_nLastHoverItem = m_nHoverItem;
		}
		else if (-1 != m_nHoverItem)
		{
			if (m_nHoverItem >= m_listItem.size())
			{
				return TRUE;
			}

			int i = 0;
			int colmuns = m_pHeaderCtrl->GetItemCount();
			auto it = m_listItem.begin();
			advance(it, m_nHoverItem);
			if (it != m_listItem.end())
			{
					int tempcolum = 0;
					auto headerIt = m_pHeaderCtrl->m_listItem.begin();
					for (auto subIt = (*it)->m_listSubItem.begin(); 
						subIt != (*it)->m_listSubItem.end() && headerIt != m_pHeaderCtrl->m_listItem.end();
						tempcolum++, subIt++, headerIt++)
					{
						QYListCtrlButton *pButton = (QYListCtrlButton*)(*headerIt)->data;
						if (nullptr != pButton
							&& pButton->m_nColmun == tempcolum
							&& (*subIt)->left <= point.x && (*subIt)->right >= point.x)
						{
							if (m_selButtonItem != pButton->m_nColmun)
							{
								m_selButtonItem = pButton->m_nColmun;
								InvalidateRect(&m_rectItemList);
							}
							return TRUE;
						}
						
					}
				
			}
		}

		if (-1 != m_selButtonItem)
		{
			m_selButtonItem = -1;
			InvalidateRect(&m_rectItemList);
		}
	}

	return TRUE;
}

LRESULT QYButtonListCtrl::OnLButtonDown(UINT nFlags, QYPoint point)
{
	m_bDown = TRUE;
	SetFocus();
	if (point.y > m_rectItemList.top)
	{
		m_nSelItem = m_nFirstVisibleItem + m_nScrollPos + ((point.y - m_rectItemList.top) / m_nItemHeight);
		InvalidateRect(&m_rectItemList);
	}

	if (0 > m_nSelItem || m_nSelItem >= m_listItem.size())
	{
		return TRUE;
	}

	int i = 0;
	int colmuns = m_pHeaderCtrl->GetItemCount();
	auto it = m_listItem.begin();
	advance(it, m_nSelItem);
	if (it != m_listItem.end())
	{
		int tempcolum = 0;
		auto headerIt = m_pHeaderCtrl->m_listItem.begin();
		for (auto subIt = (*it)->m_listSubItem.begin(); 
			subIt != (*it)->m_listSubItem.end() && headerIt != m_pHeaderCtrl->m_listItem.end();
			tempcolum++, subIt++,headerIt++)
		{
			for (auto btnIt = m_vectorButton.begin(); btnIt != m_vectorButton.end(); btnIt++)
			{
				QYListCtrlButton *pButton = (QYListCtrlButton*)(*headerIt)->data;
				if (nullptr != pButton
					&& pButton->m_nColmun == tempcolum
					&& (*subIt)->left <= point.x && (*subIt)->right >= point.x)
				{
					m_downButtonItem = pButton->m_nColmun;
					InvalidateRect(&m_rectItemList);
					return TRUE;
				}
			}
		}
	}

	m_downButtonItem = -1;

	return TRUE;
}

LRESULT QYButtonListCtrl::OnPaint()
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

	QYPen penHover;
	penHover.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_DOWN);

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
		rectTemp.top = top + (i - m_nFirstVisibleItem - m_nScrollPos) *m_nItemHeight ;
		rectTemp.bottom = top + (i - m_nFirstVisibleItem + 1 - m_nScrollPos) *m_nItemHeight;

		if (rectTemp.bottom <= top)
		{
			continue;
		}
		else if (rectTemp.top >= bottom - (m_nItemHeight / 2))
		{
			break;
		}

		rectTemp.bottom -= m_nSeparatedWidth;

			if (m_bAlternateColorFlag && i % 2)
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL2);
			}
			else
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.LISTCTRL_BK_COLOR_NORMAL);
			}
		
			MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

			if (m_nHoverItem == i)
			{
				oldPen = (HPEN)MemDc.SelectObject(&penHover);
				MemDc.Rectangle(rectTemp, (HBRUSH)GetStockObject(NULL_BRUSH));
				MemDc.SelectObject(oldPen);
			}
	
		MemDc.MoveToEx(rectTemp.left, rectTemp.bottom, &point);
		MemDc.LineTo(rectTemp.right, rectTemp.bottom);

		int tempcolum = 0;
		auto headerIt = m_pHeaderCtrl->m_listItem.begin();

		for (auto subIt = (*it)->m_listSubItem.begin(); 
			subIt != (*it)->m_listSubItem.end() && headerIt != m_pHeaderCtrl->m_listItem.end();
			tempcolum++, subIt++,headerIt++)
		{
			//MemDc.MoveToEx((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.top, &point);
			//MemDc.LineTo((*subIt)->right + m_nSeparatedWidth - m_nHScrollPos, rectTemp.bottom);

			rectTemp.left = (*subIt)->left + 5 - m_nHScrollPos;
			rectTemp.right = (*subIt)->right - 5- m_nHScrollPos;
			QYListCtrlButton *pButton = (QYListCtrlButton*)(*headerIt)->data;
			if (nullptr != pButton)
			{
				rectTemp.left -= 5;
				
				 oldPen = (HPEN)MemDc.SelectObject(&penHover);

				COLORREF oldTextColor = 0;
				if (m_downButtonItem == tempcolum && i == m_nSelItem)
				{
					MemDc.Rectangle(rectTemp, (HBRUSH)GetStockObject(GRAY_BRUSH));
					oldTextColor = MemDc.SetTextColor(QY_THEME.TEXT_COLOR_SELECT);
				}
				else
				{
					oldTextColor = MemDc.SetTextColor(pButton->m_colorText);
					if (m_selButtonItem == tempcolum&& i == m_nHoverItem)
					{
						MemDc.Rectangle(rectTemp, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
					}
					else
					{
						MemDc.FillRect(rectTemp,(HBRUSH)GetStockObject(NULL_BRUSH));
					}
				}

				MemDc.SelectObject(oldPen);
				MemDc.SelectObject(m_pButtonFont);
				if (nullptr != pButton->m_pPic)
				{
					int left = rectTemp.left + (rectTemp.Width() - pButton->m_pPic->GetWidth()) / 2;
					int top = rectTemp.top + (rectTemp.Height() - pButton->m_pPic->GetHeight()) / 2;
					pButton->m_pPic->Draw(MemDc.GetDC(), left, top, pButton->m_pPic->GetWidth(), pButton->m_pPic->GetHeight());
				}
				else
				{
					

					MemDc.DrawText(&pButton->m_strText, pButton->m_strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_WORD_ELLIPSIS);
					
				}
				MemDc.SelectObject(m_pFont);
				MemDc.SetTextColor(oldTextColor);

				rectTemp.left += 5;
			}
			else
			{
				MemDc.DrawText(&(*subIt)->strText, (*subIt)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_WORD_ELLIPSIS);
			}
		}

		drawVertLine = 1;
	}
#endif
	if (0 < m_nBorderWidth)
	{
		QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTCTRL_BORDER_COLOR);

		oldPen = (HPEN)MemDc.SelectObject(&PenLine);
		MemDc.Rectangle(rect, (HBRUSH)GetStockObject(NULL_BRUSH));

		MemDc.SelectObject(oldPen);
		PenLine.DeleteObject();
	}

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);
	penHover.DeleteObject();
	Pen.DeleteObject();
	MemDc.DeleteDC();

	return TRUE;
}

void QYButtonListCtrl::AddItemButton(int colmun, UINT ctrlID, LPCTSTR strText, LPCTSTR strImageName)
{
	QYListCtrlButton *button = new QYListCtrlButton;
	button->m_ID = ctrlID;
	button->m_strText = strText;
	button->m_nColmun = colmun;
	if (nullptr != strImageName)
	{
		button->m_pPic = new QYPicture;
		button->m_pPic->Load(strImageName);
	}
	m_vectorButton.push_back(button);

	QYHeaderCtrlItem *pItem = (QYHeaderCtrlItem*)m_pHeaderCtrl->GetItem(colmun);
	if (nullptr != pItem)
	{
		pItem->data = (DWORD_PTR)button;
	}

	//button.m_nWidth = 
}

void QYButtonListCtrl::SetItemButtonText(int colmun, LPCTSTR strText)
{
	for (auto it = m_vectorButton.begin(); it != m_vectorButton.end(); it++)
	{
		if ((*it)->m_nColmun == colmun)
		{
			(*it)->m_strText = strText;
			return;
		}
	}
}

LRESULT QYButtonListCtrl::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	return TRUE;
}
