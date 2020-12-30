#include "QYCtrlList.h"
#include "QYDC.h"
#include "QYEdit.h"
#include "QYComboBox.h"
#include "QYSwitchButton.h"
#include "QYDateTimeCtrl.h"
#include "QYDateTable.h"
#include "QYIPCtrl.h"
#include "QYFolderEdit.h"


QYCtrlList::QYCtrlList(BOOL bInTab) :QYWindow()
{
	m_nMaxID         = 0;
	m_ctrlWidth      = 100;
	m_ctrlSeparateHeigth = 20;
	m_padWidth = 20;
	m_padHeight = 20;
	m_bReDraw = TRUE;
	m_lastCtrlBottom = m_ctrlSeparateHeigth;

	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_bVShowScroll = FALSE;

	m_nScrollPos = 0;
	m_nVScrollWidth = 0;

	m_nStep = 1;

	m_bFocus = FALSE;
	m_bReDraw = TRUE;

	m_bAlternateColorFlag = TRUE;

	m_bOpenTrackPosTime = FALSE;

	m_nMaxTitleTextWidth = 0;

	m_nTitleCtrlRance = 10;

	m_workRect = 0;

	m_bInTab = bInTab;
}


QYCtrlList::~QYCtrlList()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_listItem.clear();
}

BOOL QYCtrlList::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYCtrlList",
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


	return ret;
}

BOOL QYCtrlList::Create(QYWindow *pParent, QYRect rect)
{
	m_pParent = pParent;

	m_workRect = rect;

	return TRUE;
}

LRESULT QYCtrlList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		
	}
	break;
	case WM_NCDESTROY:
	{
		m_VScrollBar.DestroyWindow();
		
		break;
	}
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_VSCROLL:{OnVScroll(LOWORD(wParam), HIWORD(wParam), (QYScrollCtrl*)lParam); return TRUE; }break;
	case WM_MOUSEWHEEL:{OnMouseWheel(LOWORD(wParam), HIWORD(wParam), QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_TIMER:OnTimer(wParam); break;
	case QY_WM_LISTCTRL_ITEM_RCLICK:
	{
		int n = 0;
	}
	case QY_WM_DATETABLE_SELECTED:
	case QY_WM_FOLDEREDIT_CHANGED:
	{
		if (m_bInTab)
		{
			::SendMessage(::GetParent(::GetParent(m_hWnd)), message, wParam, lParam);
		}
		else
		{
			::SendMessage(::GetParent(m_hWnd), message, wParam, lParam);
		}

	}
		break;
	default:break;
	}

	return 0L;
}

LRESULT QYCtrlList::OnPaint()
{
	if (!m_bReDraw)
	{
		return TRUE;
	}

	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, QY_THEME.CLIENT_COLOR);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	MemDc.SetBkMode(TRANSPARENT);
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	int top = m_padHeight - m_nScrollPos * QY_THEME.DEFAULT_CTRL_HEIGHT;
	auto it = m_listItem.begin();
	for (; it != m_listItem.end(); it++)
	{
		QYCtrlListItem *pItem = *it;
		if (nullptr != pItem)
		{
			QYRect rectItem(rect.left + m_padWidth, 
				top,
				rect.right - pItem->m_Size.cx - m_padWidth - m_nTitleCtrlRance, 
				top + pItem->m_Size.cy);
			if (rectItem.bottom < 0 || rectItem.top > rect.bottom - m_nBorderWidth)
			{
				top += pItem->m_Size.cy + m_ctrlSeparateHeigth;
				continue;
			}
			
			MemDc.DrawText(pItem->m_strTitle, pItem->m_strTitle.Length(), &rectItem,  DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_WORD_ELLIPSIS);

			top += pItem->m_Size.cy + m_ctrlSeparateHeigth;
		}
	}

	dc.BitBlt(0,0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.SelectObject(OldFont);
	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYCtrlList::OnSize(UINT nType, int cx, int cy)
{
	return UpdateLayout();
}

int QYCtrlList::SetCtrlWidth(int ctrlWidth)
{
	int width = m_ctrlWidth;
	m_ctrlWidth = ctrlWidth;
	return width;
}

QYWindow* QYCtrlList::AddCtrl(QY_CONTROL_TYPE ctrlType, LPCTSTR strTitle, int ctrlHeight)
{
#if 0


	if (nullptr == m_pParent)
	{
		return nullptr;
	}

	m_hWnd = m_pParent->GetHwnd();
#endif
	auto *pitem = new QYCtrlListItem(m_nMaxID++, ctrlType, strTitle,ctrlHeight);
	assert(nullptr != pitem);

	
	switch (ctrlType)
	{
	case QY_CONTROL_TYPE_FOLDER_LISTCTRL:
	{
		pitem->m_pWnd = new QYListCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYListCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_EDIT:
	{
		pitem->m_pWnd = new QYEdit;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYEdit*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0,0,0,0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
		break;
	case QY_CONTROL_TYPE_FOLDER_EDIT:
	{
		pitem->m_pWnd = new QYFolderEdit;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYFolderEdit*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_IPADDRESS:
	{
		pitem->m_pWnd = new QYIPCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYIPCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_DATETABLE:
	{
		pitem->m_pWnd = new QYDateTable;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		
		if (!((QYDateTable*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}

		pitem->m_Size = ((QYDateTable*)pitem->m_pWnd)->GetAppropriateSize(m_ctrlWidth, ctrlHeight);
		
	}
		break;
	case QY_CONTROL_TYPE_LISTCTRL:
	{
		pitem->m_pWnd = new QYListCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}


		if (!((QYListCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}
	}
	break;
	case QY_CONTROL_TYPE_DATETIME:
	{
		pitem->m_pWnd = new QYDateTimeCtrl;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYDateTimeCtrl*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}


	}
	break;
	case QY_CONTROL_TYPE_COMBOBOX:
	{
			pitem->m_pWnd = new QYComboBox;
			if (nullptr == pitem->m_pWnd)
			{
				return nullptr;
			}

			if (!((QYComboBox*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
			{
				SAFE_DELETE(pitem->m_pWnd);
				return nullptr;
			}
		}
	break;
	case QY_CONTROL_TYPE_BUTTON:
		break;
	case QY_CONTROL_TYPE_SWITCH_BUTTON:
	{
		pitem->m_pWnd = new QYSwitchButton;
		if (nullptr == pitem->m_pWnd)
		{
			return nullptr;
		}

		if (!((QYSwitchButton*)pitem->m_pWnd)->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
		{
			SAFE_DELETE(pitem->m_pWnd);
			return nullptr;
		}

		QYSize& size = ((QYSwitchButton*)pitem->m_pWnd)->GetBackImageSize();
		pitem->m_Size = size;
	}
		break;
	case QY_CONTROL_TYPE_RADIO_BUTTON:
		break;
	default:
	{
		SAFE_DELETE(pitem);
		return nullptr;
	}
	}

	(nullptr != pitem->m_pWnd) ? pitem->m_pWnd->SetNotifyMsgWindow(::GetParent(::GetParent(m_hWnd))) : 0;

	QYPaintDC dc(this);

	auto right = 0;

	QYSize size = dc.GetTextExtent(strTitle);
	m_nMaxTitleTextWidth = std::fmax(m_nMaxTitleTextWidth, size.cx);
	m_listItem.push_back(pitem);

	m_lastCtrlBottom += pitem->m_Size.cy + m_ctrlSeparateHeigth;

	return pitem->m_pWnd;
}

void QYCtrlList::OnVScroll(UINT nSBCode, UINT nPos, QYScrollCtrl* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_LINEDOWN:
	case SB_LINEUP:
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	{
		m_nScrollPos = m_nStep * nPos;
		//InvalidateRect(&m_rectItemList);
	}
	break;
	case SB_THUMBTRACK:
	{
		m_nTrackPos = m_nStep * nPos;

		if (!m_bOpenTrackPosTime)
		{
			SetTimer(2, 10, nullptr);
			m_bOpenTrackPosTime = TRUE;
		}
	}
	break;
	case SB_THUMBPOSITION:
	{
		KillTimer(2);
		m_bOpenTrackPosTime = FALSE;
	}
	break;
	default:
		break;
	}
}


LRESULT QYCtrlList::OnMouseWheel(UINT nFlags, short zDelta, QYPoint pt)
{
	if (m_bVShowScroll)
	{
		if (nFlags == 0)
		{
			KillTimer(1);
			KillTimer(2);
			
			m_nZDelta = zDelta;
			m_nWheelCount = 0;
			SetTimer(1, 20, nullptr);
		}
	}

	return TRUE;
}

void QYCtrlList::OnTimer(UINT_PTR nIDEvent)
{
	if (5 <= m_nWheelCount++ && 2 != nIDEvent)
	{
		KillTimer(1);
		m_nWheelCount = 0;
		return;
	}
	
	if (1 == nIDEvent)
	{
		int offset = 1;
		if (m_nZDelta < 0)
		{
			if (m_nScrollPos >= m_VScrollBar.GetRange() * m_nStep)
			{
				return;
			}

			//while (m_nStep > 0 && offset * m_nStep < m_nItemHeight){ offset++; };

			m_nScrollPos += m_nStep;
			m_VScrollBar.SetPos(m_VScrollBar.GetPos() + offset);
			//InvalidateRect(&m_rectItemList);
			//ScrollWindow(0, m_nStep, &rect, nullptr);
			ScrollItem(m_nScrollPos);
		}
		else
		{
			if (m_nScrollPos <= 0)
			{
				return;
			}
			//while (m_nStep > 0 && offset * m_nStep < m_nItemHeight){ offset++; };
			m_nScrollPos -= m_nStep;
			m_VScrollBar.SetPos(m_VScrollBar.GetPos() - offset);
			ScrollItem(m_nScrollPos);
			//InvalidateRect(&m_rectItemList);
		}
	}
	else if (2 == nIDEvent)
	{
		int nlastPos = m_nScrollPos;
		m_nScrollPos = m_nTrackPos;
		if (nlastPos != m_nScrollPos)
		{
			//InvalidateRect(&m_rectItemList);
			ScrollItem(m_nScrollPos);
		}
	}
}


void QYCtrlList::ScrollItem(int off)
{
	QYRect rect;
	GetClientRect(rect);

	QYRect rectCtrl;

	QYRect rectItem(rect.right - m_ctrlWidth - m_padWidth, m_padHeight - m_nBorderWidth - off * QY_THEME.DEFAULT_CTRL_HEIGHT, rect.right - m_nVScrollWidth - m_padWidth, m_padHeight);
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		QYCtrlListItem *pItem = *it;

		rectItem.bottom = rectItem.top + pItem->m_Size.cy ;

		if (0 == pItem->m_Size.cy)
		{
			rectItem.bottom = rect.Height() - m_nBorderWidth ;
		}
		if (0 == pItem->m_strTitle.Length())
		{
			rectItem.left = rect.left + m_nBorderWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth;
		}
		else
		{
			rectItem.left = rect.right - m_ctrlWidth - m_padWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth - m_padWidth;
		}

		if (0 > rectItem.left)
		{
			rectItem.left = m_nBorderWidth;
		}

		if (nullptr != pItem)
		{
			switch (pItem->m_nCtrlType)
			{
			case QY_CONTROL_TYPE_SWITCH_BUTTON:
			{
				rectItem.left = rect.right - pItem->m_Size.cx - m_padWidth;
				rectItem.right = rectItem.left + pItem->m_Size.cx;
			}
			break;
			case QY_CONTROL_TYPE_DATETABLE:
			{
				rectItem.left = rect.left + (rect.right - m_nBorderWidth - m_nVScrollWidth - pItem->m_Size.cx) / 2;
				rectItem.right = rectItem.left + pItem->m_Size.cx;
				rectItem.bottom = rectItem.top + pItem->m_Size.cy ;
			}
			break;
			default:
				break;
			}


			if (nullptr != pItem->m_pWnd)
			{
				pItem->m_pWnd->MoveWindow(rectItem);
			}
			rectItem.top = rectItem.bottom + m_ctrlSeparateHeigth ;
		}
	}


	InvalidateRect(&m_rectItemList);
}

BOOL QYCtrlList::UpdateLayout()
{
	QYRect rect;
	GetClientRect(rect);

	if (0 >= rect.Width() || 0 >= rect.Height())
	{
		return FALSE;
	}

	int showItems = rect.Height() / QY_THEME.DEFAULT_CTRL_HEIGHT;
	int realVisibleItemCount = m_lastCtrlBottom / QY_THEME.DEFAULT_CTRL_HEIGHT;
	if (realVisibleItemCount < showItems)
	{
		if (m_bVShowScroll)
		{
			if ( nullptr != m_VScrollBar.GetHwnd())
			{
				m_VScrollBar.SetPos(0);
				m_nScrollPos = 0;
				m_nStep = 0;
				m_VScrollBar.Show(SW_HIDE);
			}

			m_bVShowScroll = FALSE;
			m_nVScrollWidth = 0;
		}

	}
	else
	{
		m_bVShowScroll = TRUE;
		m_nVScrollWidth = QY_THEME.SCROLLBAR_WIDTH;
	}

	if (m_bVShowScroll)
	{
		int nScrollRange =  (realVisibleItemCount - showItems) ;
		nScrollRange = (0 == nScrollRange) ? 1 : nScrollRange;
		

		if (nullptr == m_VScrollBar.GetHwnd())
		{
			if (!m_VScrollBar.Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), m_hWnd))
			{
				return FALSE;
			}
		}

		QYRect rectScroll = rect;
		rectScroll.right = rect.right - m_nBorderWidth;
		rectScroll.left = rectScroll.right - m_nVScrollWidth;
		rectScroll.top = rect.top + m_nBorderWidth;
		rectScroll.bottom = rect.bottom - m_nBorderWidth;
		m_VScrollBar.MoveWindow(rectScroll);

		int lastRange = nScrollRange;
		int count = 0;
		while (0 < m_VScrollBar.SetRange(nScrollRange) && count++ <showItems)
		{
			lastRange = ++nScrollRange;
		}

		m_nStep = (0 < nScrollRange) ? (lastRange / nScrollRange) : 0;
		m_nScrollPos = m_nStep * m_VScrollBar.GetPos();
		m_VScrollBar.SetPos(m_VScrollBar.GetPos());
		m_VScrollBar.Show(SW_SHOW);
	}

	m_padWidth = (rect.Width() - m_nVScrollWidth - m_ctrlWidth - m_nTitleCtrlRance - m_nMaxTitleTextWidth) / 2;
	if (0 > m_padWidth)
	{
		m_padWidth = 20;
	}

	QYRect rectItem(rect.right - m_ctrlWidth - m_padWidth, m_padHeight, rect.right - m_nVScrollWidth - m_padWidth, m_padHeight );
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		QYCtrlListItem *pItem = *it;

		

		rectItem.bottom = rectItem.top + pItem->m_Size.cy;

		if (0 == pItem->m_Size.cy)
		{
			rectItem.bottom = rect.Height() - m_nBorderWidth;
		}
		if (0 == pItem->m_strTitle.Length())
		{
			rectItem.left = rect.left + m_nBorderWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth;
		}
		else
		{
			rectItem.left = rect.right - m_ctrlWidth - m_padWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth - m_padWidth;
		}

		if (0 > rectItem.left)
		{
			rectItem.left = m_nBorderWidth;
		}

		if (nullptr != pItem)
		{
			switch (pItem->m_nCtrlType)
			{
			case QY_CONTROL_TYPE_SWITCH_BUTTON:
			{
				rectItem.right = rect.right  - m_padWidth;
				rectItem.left = rectItem.right - pItem->m_Size.cx;
			}
				break;
			case QY_CONTROL_TYPE_DATETABLE:
			{
				pItem->m_Size = ((QYDateTable*)pItem->m_pWnd)->GetAppropriateSize(rect.Width() - m_nVScrollWidth - 2 * m_nBorderWidth, 
					pItem->m_Size.cy);
				rectItem.left = rect.left + (rect.right - m_nBorderWidth - m_nVScrollWidth - pItem->m_Size.cx) / 2;
				rectItem.right = rectItem.left + pItem->m_Size.cx;
				rectItem.bottom = rectItem.top + pItem->m_Size.cy;
			}
				break;
			default:
				break;
			}

			
			if (nullptr != pItem->m_pWnd)
			{
				pItem->m_pWnd->MoveWindow(rectItem);
			}
			rectItem.top = rectItem.bottom + m_ctrlSeparateHeigth;
		}
	}

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,
		m_nBorderWidth ,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth );

	InvalidateRect(&m_rectItemList);

	return TRUE;
}

