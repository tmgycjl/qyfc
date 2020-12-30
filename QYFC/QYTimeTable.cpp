#include "QYTimeTable.h"
#include "QYDC.h"

QYTimeTable::QYTimeTable() :QYWindow()
{
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nSelItem = 0;
	m_bDown = FALSE;
//	m_TabDockPos = TAB_DOCK_TOP;
	m_nItemHeight = 28;
	//m_nSeparatePixel = 2;
	m_bReDraw = TRUE;
	m_nHeaderHeight = 30;
	m_OriginalHeaderHeight = 30;
	//m_pSelFont = nullptr;
	m_textWdith = 0;
	m_OriginalTextWdith = 0;
	m_subItemWidth = 0;
	m_leaveWidth = 5;
	m_pEditPic = nullptr;
	m_bDownEditButton = false;
	m_validTimeColor = RGB(118, 189,123);
}

QYTimeTable::~QYTimeTable()
{
	Clear();

	SAFE_DELETE(m_pEditPic);
//  	if (nullptr != m_pTimeFont)
//  	{
// 		m_pTimeFont->DeleteObject();
// 		SAFE_DELETE(m_pTimeFont);
//  	}
}

LRESULT QYTimeTable::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
//	case WM_TIMER: OnTimer(wParam); break;
	default:break;
	}
	return 0L;
}

BOOL QYTimeTable::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYTimeTable",
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

	//m_pTimeFont = new QYFont;
	//m_pTimeFont->CreateFont(MS_YAHEI_FONT_SIZE, 0, FW_NORMAL, L"Arial", TRUE);

	return ret;
}

int QYTimeTable::GetItemCount()
{
	return m_listItem.size();
}


int QYTimeTable::InsertItem(int item,const TCHAR* strText, DWORD_PTR data)
{
	QYPaintDC dc(this);
	
	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	auto *pitem = new QYTimeTableItem(strText, data);

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

		m_listItem.insert(it, pitem);
		QYSize size = dc.GetTextExtent(strText);

		m_OriginalTextWdith  = ((m_textWdith - 2 * 5) > size.cx) ? m_textWdith : size.cx + 2 * 5;
		m_textHeight = size.cy;
	}

	dc.SelectObject(OldFont);

	m_bReDraw ? OnSize(0,0,0) : 0;
	return m_listItem.size();
}


DWORD_PTR QYTimeTable::GetItemData(int item)
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

int QYTimeTable::SetItemHeight(const int height)
{
	int lastHeight = m_nItemHeight;
	m_nItemHeight = height;
	m_bReDraw ? Invalidate() : 0;

	return lastHeight;
}

QYTimeTableItem* QYTimeTable::GetItem(int item)
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

void QYTimeTable::Clear()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_listItem.clear();
}

void QYTimeTable::ClearTime()
{
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++)
	{
		memset((*it)->minArray, 0, MINIUTE_IN_DAY);

		(*it)->vectorValidPos.clear();

	}
}

LRESULT QYTimeTable::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	

	QYBrush Brush(QY_THEME.TIMETABLE_ITEM_COLOR);
	QYBrush headerBrush(QY_THEME.TIMETABLE_HEADER_COLOR);
	QYBrush fillBrush(m_validTimeColor);

	HBRUSH OldBrush;

	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYRect rectHeader = rect;
	rectHeader.top += m_nBorderWidth;
	rectHeader.bottom = rectHeader.top  + m_nHeaderHeight;
	rectHeader.left += m_nBorderWidth;
	rectHeader.right -= m_nBorderWidth;
	

	QYRect rectItem = rect;
	rectItem.top = rectHeader.bottom;
	rectItem.bottom -= m_nBorderWidth;
	rectItem.left += m_nBorderWidth;
	rectItem.right -= m_nBorderWidth;

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	if (0 == m_nBorderWidth)
	{
		MemDc.FillRect(rectItem, Brush);
	}
	else
	{
		MemDc.Rectangle(rect);
	}

	MemDc.FillRect(rectHeader, headerBrush);

	MemDc.SelectObject(OldBrush);

	Brush.DeleteObject();
	headerBrush.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	auto it = m_listItem.begin();
	int i = 0;
	QYRect rectTemp;
	QYRect rectText;

	QYPoint point;

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	//HFONT OldFont = (HFONT)MemDc.SelectObject(m_pTimeFont);
	int sLeft = m_textWdith + m_nBorderWidth ;
	int validWidth = rect.right - sLeft - m_leaveWidth;

	if (nullptr != m_pEditPic)
	{
		int left = rectHeader.left + (m_textWdith - m_pEditPic->GetWidth()) / 2;
		m_pEditPic->Draw(MemDc.GetDC(), left, rectHeader.top);
		//g_ConsoleLog.Print("left:%d,top:%d,width:%d,height:%d\n", left, rectHeader.top, m_textWdith, rectHeader.Height());
	}

	for (int hour = 0; hour < HOURS_IN_DAY + 1; hour++)
	{
		MemDc.MoveToEx(sLeft + hour * m_subItemWidth, rectHeader.bottom, &point);
		MemDc.LineTo(sLeft + hour * m_subItemWidth, rect.bottom - m_nBorderWidth);

		TCHAR szHour[100] = { 0 };
		SafeSprintf(szHour, L"%d", (hour >=HOURS_IN_DAY  )?0 :hour );
		int offset = (1 < SafeStrlen(szHour)) ? 5 : 2;
		MemDc.TextOut(sLeft + hour * m_subItemWidth - offset, rectHeader.bottom - m_textHeight, szHour);
	}


	COLORREF color = 0;
	
	for (; it != m_listItem.end(); it++, i++)
	{
		rectText.left = rect.left + m_nBorderWidth;
		rectText.top = rect.top + m_nBorderWidth + m_nHeaderHeight + i * m_nItemHeight;
		rectText.bottom = rectText.top + m_nItemHeight;
		rectText.right = rectText.left + m_textWdith;

		rectTemp.top = rectText.top + (m_nItemHeight / 6);
		rectTemp.bottom = rectText.bottom - (m_nItemHeight / 6);

		for (auto itMin = (*it)->vectorValidPos.begin(); itMin != (*it)->vectorValidPos.end(); itMin++)
		{
			int left = LOWORD((*itMin));
			int right = HIWORD((*itMin));
			rectTemp.left = sLeft + (float)(m_subItemWidth* HOURS_IN_DAY) *((float)left / (float)MINIUTE_IN_DAY);
			rectTemp.right = sLeft + (float)(m_subItemWidth* HOURS_IN_DAY) *((float)right / (float)MINIUTE_IN_DAY);

			MemDc.FillRect(rectTemp, fillBrush);
		}
// 		if (m_nSelItem == i)
// 		{
// 			
// 		}
// 		else
// 		{
// 			
// 		}

		MemDc.MoveToEx(rect.left + m_nBorderWidth , rectHeader.bottom + i * m_nItemHeight, &point);
		MemDc.LineTo(rect.right - m_nBorderWidth -m_leaveWidth, rectHeader.bottom + i * m_nItemHeight);
		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectText, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.SelectObject(OldFont);
	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYTimeTable::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	if (0 < m_listItem.size())
	{
		int validHeight = (rect.Height() - 2 * m_nBorderWidth - m_OriginalHeaderHeight);
		m_nItemHeight = validHeight / m_listItem.size();
		m_nHeaderHeight = m_OriginalHeaderHeight + validHeight - m_nItemHeight * m_listItem.size();
	}

	int validWidth = rect.Width() - 2 * m_nBorderWidth - m_OriginalTextWdith - m_leaveWidth;
	 m_subItemWidth = (validWidth / HOURS_IN_DAY);
	 m_textWdith = m_OriginalTextWdith + validWidth - m_subItemWidth * HOURS_IN_DAY ;


	Invalidate();

	return TRUE;
}



LRESULT QYTimeTable::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bDown)
	{
		m_bDown = TRUE;
		
		if (!m_bDownEditButton)
		{
			QYRect rect;
			GetClientRect(rect);

			QYRect rectEditButton = rect;
			rectEditButton.top += m_nBorderWidth;
			rectEditButton.bottom = rectEditButton.top + m_nHeaderHeight;
			rectEditButton.left += m_nBorderWidth;
			rectEditButton.right = rectEditButton.left + m_textWdith;
			rectEditButton.right -= m_nBorderWidth;
			if (rectEditButton.PtInRect(point))
			{
				m_bDownEditButton = true;
			}

			
		}
	}

	return TRUE;
	//CWnd::OnLButtonDown(nFlags, point);
}

LRESULT QYTimeTable::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_bDown)
	{
		m_bDown = FALSE;
		Invalidate();
	}
	
	if (m_bDownEditButton)
	{
		::SendMessage(::GetParent(m_hWnd), QY_WM_TIMETABLE_EDIT, (WPARAM)m_hWnd, 0);
		m_bDownEditButton = false;
	}
	
	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}


LRESULT QYTimeTable::OnMouseMove(UINT nFlags, QYPoint point)
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


	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYTimeTable::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		//m_nHoverItem = -1;
		Invalidate();
	}
	return TRUE;
}

void QYTimeTable::PutInTimeframe(int item,unsigned int sTime, unsigned int eTime)
{
	QYTimeTableItem *pItem = GetItem(item);
	if (nullptr != pItem)
	{
		unsigned int sMin = (sTime / 100) * MINIUTE_IN_HOUR + sTime % 100;
		unsigned int eMin = (eTime / 100) * MINIUTE_IN_HOUR + eTime % 100;

		for (int i = sMin; i < eMin; i++)
		{
			pItem->minArray[i] = 1;
		}

		int begin = 0;
		long Pos = 0;
		//int end = 0;

		pItem->vectorValidPos.clear();

		for (int i = 0; i < MINIUTE_IN_DAY; i++)
		{
			if (0 == begin && 1 == pItem->minArray[i])
			{
				begin = i;
			}

			if (0 != begin && 0 == pItem->minArray[i] )
			{
				Pos = MAKELONG(begin, i);

				pItem->vectorValidPos.push_back(Pos);

				begin = 0;
			}
		}

		Invalidate();
	}
}

void QYTimeTable::SetEditImage(UINT imageID)
{
	SAFE_DELETE(m_pEditPic);

	if (!m_pEditPic->Load(imageID, QY_IMAGE_PNG))
	{
		return;
	}
}

void QYTimeTable::SetEditImage(LPCTSTR imageName)
{
	SAFE_DELETE(m_pEditPic);

	m_pEditPic = new QYPicture;

	if (!m_pEditPic->Load(imageName))
	{
		return;
	}
}