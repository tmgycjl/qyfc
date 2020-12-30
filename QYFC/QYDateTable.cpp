#include "QYDateTable.h"
#include "QYDC.h"


int QYDateTable::m_LangugeIndex = 0;

DayOfWeekStruct QYDateTable::m_arrayDayOfWeek[] = { 
	"en", { L"Mond",L"Thue",L"Wens",L"Thur",L"Frid",L"Satu",L"Sund" },
	"zh", { L"一", L"二", L"三", L"四", L"五", L"六", L"日" } };

const char* QYDateTable::m_textIDDayOfWeek[DAYS_IN_WEEK] = { "MONDAY", "THUESDAY", "WENSDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY" };

QYString QYDateTable::m_textDayOfWeek[DAYS_IN_WEEK] = { L"Mond", L"Thue", L"Wens", L"Thur", L"Frid", L"Satu", L"Sund" };

QYDateTable::QYDateTable(BOOL bPopFlag) :QYWindow()
{
	m_nOperPaneHeight = 24;
	m_nDownButtonID = -1;
	m_nHoverButtonID = -1;
	m_titleButtonWidth = 200;
	m_nWeekPaneHeight = 24;
	m_pDropList = nullptr;
	m_bReDraw = TRUE;
	m_nBorderWidth = 1;
	m_HoverItem = -1;
	m_pShadow = nullptr;
	m_pSourceWnd = nullptr;
	m_bPopFlag = bPopFlag;
	m_CheckItem = -1;
	
	//m_nOperButtonWidth = 100;
}


QYDateTable::~QYDateTable()
{
}

BOOL QYDateTable::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	if (m_bPopFlag)
	{
		dwExStyle |= WS_EX_TOOLWINDOW;
	}

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYDateTable",
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

	m_pTitleFont = new QYFont;
	m_pTitleFont->CreateFont(QY_THEME.MS_YAHEI_FONT_SIZE, 0, FW_NORMAL, QY_THEME.MS_YAHEI_FONT_TYPE);
	
	Reset();

	if (m_bPopFlag&&m_bShowShadow)
	{
		m_pShadow = new QYWindowShadow;
		m_pShadow->Create(0, this);
	}


	return ret;
}

LRESULT QYDateTable::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case CB_SELECTSTRING:
	{
		(nullptr != m_pDropList) ? m_SelectDate.SetDate(m_pDropList->GetItemData(wParam)): 0;

		UpdateDate();
	}
	break;
	case WM_NCDESTROY:
	{
		for (auto it = m_vectorMarkDate.begin(); it != m_vectorMarkDate.end(); it++)
		{
			SAFE_DELETE(*it);
		}

		m_vectorMarkDate.clear();

		SAFE_DELETE(m_pTitleFont);
		SAFE_DESTROY_WINDOW_PTR(m_pDropList);
		SAFE_DESTROY_WINDOW_PTR(m_pShadow);
	}
	break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_PAINT:{return OnPaint(); }break;
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); break; }
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam); }break;
	case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:{OnLButtonDown(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOSHORT(lParam), HISHORT(lParam))); }break;
	default: 
		break;
	}
	return QYWindow::WindowProc(message, wParam, lParam);
}

QYSize QYDateTable::GetAppropriateSize(int width, int height)
{
	
	QYSize size = 0;

	size.cx = width;
	size.cy = height;
	if (0 < m_nBorderWidth)
	{
		while (0 != (size.cx - (2 * m_nBorderWidth)) % DAYS_IN_WEEK)
		{
			size.cx--;
		}

		while (0 != (size.cy - m_nOperPaneHeight - m_nWeekPaneHeight - (2 * m_nBorderWidth)) % WEEK_NUMS_IN_MONTH)
		{
			size.cy--;
		}
	}
	else
	{
		while (0 != size.cx%DAYS_IN_WEEK)
		{
			size.cx--;
		}

		while (0 != (size.cy - m_nOperPaneHeight - m_nWeekPaneHeight)%WEEK_NUMS_IN_MONTH)
		{
			size.cy--;
		}
	}

	return size;
}

QYSize QYDateTable::GetDefaultSize()
{
	QYSize size = 0;

	size.cx = 280;
	size.cy = 180;
	if (0 < m_nBorderWidth)
	{
		size.cx += (m_nBorderWidth * 2);
		size.cy += (m_nBorderWidth * 2) + m_nOperPaneHeight + m_nWeekPaneHeight;
	}
	else
	{
		size.cy += m_nOperPaneHeight + m_nWeekPaneHeight;
	}

	return size;
}



LRESULT QYDateTable::OnSize(UINT nType, int cx, int cy)
{
	// QYListCtrl::OnSize(nType, cx, cy);

	int width = (cx - 2 * m_nBorderWidth) / DAYS_IN_WEEK;
	for (int i = 0; i < DAYS_IN_WEEK; i++)
	{
		//SetColumnWidth(i,width );
	}

	QYRect rect;
	GetClientRect(rect);

	m_sizeItem.cy = (rect.Height() - (2 * m_nBorderWidth) - m_nOperPaneHeight - m_nWeekPaneHeight) / WEEK_NUMS_IN_MONTH;
	m_sizeItem.cx = (rect.Width() - (2 * m_nBorderWidth)) / DAYS_IN_WEEK;

	m_rectLastMonth = QYRect(rect.left + m_nBorderWidth,
		rect.top + m_nBorderWidth,
		rect.left + m_nBorderWidth + m_nOperPaneHeight,
		rect.top + m_nBorderWidth + m_nOperPaneHeight);

	m_rectNextMonth = QYRect(rect.right - m_nBorderWidth - m_nOperPaneHeight,
		rect.top + m_nBorderWidth,
		rect.right - m_nBorderWidth ,
		rect.top + m_nBorderWidth + m_nOperPaneHeight);

	int titleButtonWidth = m_rectNextMonth.left - m_rectLastMonth.right;
	int left = (titleButtonWidth > m_titleButtonWidth) ? (titleButtonWidth - m_titleButtonWidth) / 2 : 0;
	m_rectTitleButton = QYRect(m_rectLastMonth.right + left,
		rect.top + m_nBorderWidth,
		m_rectNextMonth.left - left,
		rect.top + m_nBorderWidth + m_nOperPaneHeight);

	m_rectItem = QYRect(rect.left + m_nBorderWidth,
		rect.bottom - m_nBorderWidth - (m_sizeItem.cy * WEEK_NUMS_IN_MONTH),
		rect.right - m_nBorderWidth,
		rect.bottom - m_nBorderWidth);


	Invalidate();

	if (m_bShowShadow && nullptr != m_pShadow)
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	return TRUE;
}

BOOL QYDateTable::SetDateData(int year, int month, int day, DWORD_PTR data)
{
	

	return TRUE;
}

void QYDateTable::UpdateDate()
{
#if 1

	WORD days = QYDate::GetDaysByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());
	WORD dayofweek = QYDate::GetStartDayOfWeekByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());

	QYDate lastDate = m_SelectDate;
	lastDate.LastMonth();
	int lastDays = QYDate::GetDaysByYearAndMonth(lastDate.GetYear(), lastDate.GetMonth());
	QYDate nextDate = m_SelectDate;
	nextDate.NextMonth();
	//int nextDays = QYDate::GetDaysByYearAndMonth(nextDate.GetYear(), nextDate.GetMonth());
	int curDay = 1;

	for (int week = 0; week < WEEK_NUMS_IN_MONTH; week++)
	{
		for (int day = 0; day < DAYS_IN_WEEK; day++)
		{
			if ((0 == week && day < dayofweek))
			{
				m_DayArray[week][day].nValue = lastDays - (dayofweek - 1 - day);
				m_DayArray[week][day].date.SetDate(lastDate.GetYear(), lastDate.GetMonth(), lastDays - (dayofweek - 1 - day));
				m_DayArray[week][day].textColor = QY_THEME.TEXT_COLOR_DISABLE;
				m_DayArray[week][day].color = QY_THEME.CLIENT_COLOR;
			}
			else if (curDay > days)
			{
				m_DayArray[week][day].nValue = curDay - days;
				m_DayArray[week][day].date.SetDate(nextDate.GetYear(), nextDate.GetMonth(), curDay - days);
				m_DayArray[week][day].textColor = QY_THEME.TEXT_COLOR_DISABLE;
				m_DayArray[week][day].color = QY_THEME.CLIENT_COLOR;
				curDay++;
			}
			else
			{
				m_DayArray[week][day].nValue = curDay;
				m_DayArray[week][day].date.SetDate(m_SelectDate.GetYear(), m_SelectDate.GetMonth(), curDay);

				QYDate dt(m_SelectDate.GetYear(), m_SelectDate.GetMonth(), m_DayArray[week][day].nValue);
				 
				if (dt == m_NowDate)
				{
					m_DayArray[week][day].color = QY_THEME.DATETABLE_CURRENT_DAY_BK_COLOR;
					m_DayArray[week][day].textColor = QY_THEME.TEXT_COLOR_NORMAL;
				}
				else
				{
					m_DayArray[week][day].color = QY_THEME.CLIENT_COLOR;
					m_DayArray[week][day].textColor = QY_THEME.TEXT_COLOR_NORMAL;
				}
				curDay++;
			}

			
			m_DayArray[week][day].strValue.Format(L"%d", m_DayArray[week][day].nValue);
		}

		
	}

	m_CheckItem = -1;
	::SendMessage(::GetParent(m_hWnd), QY_WM_DATETABLE_CHANGED, (WPARAM)m_hWnd, 0);
	Invalidate();
#endif
}

void QYDateTable::SetTiltle(LPCTSTR strTitle)
{
	m_TitleText = strTitle;
	if (nullptr != m_hWnd)
	{
		QYPaintDC dc(this);
		HFONT oldfont = (HFONT)dc.SelectObject(&m_pTitleFont);
		m_titleButtonWidth = dc.GetTextExtent(&m_TitleText).cx + 2;
		dc.SelectObject(oldfont);

		QYRect rect;
		GetClientRect(rect);
		int titleButtonWidth = m_rectNextMonth.left - m_rectLastMonth.right;
		int left = (titleButtonWidth > m_titleButtonWidth) ? (titleButtonWidth - m_titleButtonWidth) / 2 : 0;
		m_rectTitleButton = QYRect(m_rectLastMonth.right + left,
			rect.top + m_nBorderWidth,
			m_rectNextMonth.left - left,
			rect.top + m_nBorderWidth + m_nOperPaneHeight);

		InvalidateRect(&m_rectTitleButton);
	}

	
}


void QYDateTable::SetDate(WORD year, WORD month, WORD day)
{
	if (0 == year && month == 0 && day == 0)
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);
		m_NowDate.SetDate(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
		m_SelectDate = m_NowDate;
	}
	else
	{
		m_NowDate.SetDate(year, month, day);
		m_SelectDate = m_NowDate;
	}

	UpdateDate();
}

void QYDateTable::DrawTopPane(HDC hDC, QYRect *rectTopPane)
{
	HPEN hBtnPen = CreatePen(PS_SOLID, QY_THEME.LARGE_ARROW_PEN_WIDTH, QY_THEME.SCROLLBAR_ARROW_COLOR_NORMAL);
	HPEN hBtnHoverPen = CreatePen(PS_SOLID, QY_THEME.LARGE_ARROW_PEN_WIDTH, QY_THEME.SCROLLBAR_ARROW_COLOR_HOVER);

	RECT rectTemp;
	rectTemp.top = rectTopPane->top;
	rectTemp.left = rectTopPane->left;
	rectTemp.bottom = rectTopPane->bottom;
	rectTemp.right = rectTopPane->right;

	RECT rectReal;
	QYRect *rect = &m_rectLastMonth;
	int sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen;
	POINT pt;
	
		if (LAST_MONTH_BUTTON == m_nDownButtonID)
		{
			OffsetRect(&rectReal, -1, 0);
		}

		HPEN oldpen = nullptr;

		if (LAST_MONTH_BUTTON == m_nHoverButtonID)
		{
			 oldpen = (HPEN)::SelectObject(hDC, hBtnHoverPen);
		}
		else
		{
			oldpen = (HPEN)::SelectObject(hDC, hBtnPen);
		}
		

		MoveToEx(hDC, rectReal.right, rectReal.top, &pt);
		LineTo(hDC, rectReal.right - sideLen / 2, rectReal.top + sideLen / 2);

		LineTo(hDC, rectReal.right , rectReal.bottom );

		rect = &m_rectNextMonth;
		sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
		sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
		rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
		rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
		rectReal.bottom = rectReal.top + sideLen;
		rectReal.right = rectReal.left + sideLen;
		if (NEXT_MONTH_BUTTON == m_nDownButtonID)
		{
			OffsetRect(&rectReal, 1, 0);
		}

		if (NEXT_MONTH_BUTTON == m_nHoverButtonID)
		{
			 ::SelectObject(hDC, hBtnHoverPen);
		}
		else
		{
			 ::SelectObject(hDC, hBtnPen);
		}

		MoveToEx(hDC, rectReal.left, rectReal.top, &pt);
		LineTo(hDC, rectReal.left + sideLen / 2, rectReal.top + sideLen / 2);
		LineTo(hDC, rectReal.left , rectReal.bottom );

		//MoveToEx(hDC, rectTemp.left, rectTemp.bottom, &pt);
		//LineTo(hDC, rectReal.right, rectTemp.bottom );

	//::DeleteObject(hBtnPen);

	//hBtnPen = CreatePen(PS_SOLID, SMALL_ARROW_PEN_WIDTH, LISTCTRL_SEPARATED_COLOR);
	//::SelectObject(hDC, hBtnPen);

	RECT rectTitleButton;
	rectTitleButton.left = m_rectLastMonth.right;
	rectTitleButton.right = m_rectNextMonth.left;
	rectTitleButton.top = rectTopPane->top;
	rectTitleButton.bottom = rectTopPane->bottom;

	COLORREF oldTextColor = 0;
	if (TITLE_BUTTON == m_nHoverButtonID)
	{
		oldTextColor = ::SetTextColor(hDC, QY_THEME.TEXT_COLOR_HOVER);
	}
	else
	{
		oldTextColor = ::SetTextColor(hDC, QY_THEME.TEXT_COLOR_NORMAL);
	}

	rectReal = rectTitleButton;
	if (TITLE_BUTTON == m_nDownButtonID)
	{
		OffsetRect(&rectReal, 1, 1);
	}

	HFONT oldfont = (HFONT)::SelectObject(hDC, m_pTitleFont->m_hObject);

	TCHAR chTitleText[128] = { 0 };
	SafeSprintf(chTitleText, L" %d - %02d ", m_SelectDate.GetYear(), m_SelectDate.GetMonth());

	::DrawText(hDC, chTitleText, SafeStrlen(chTitleText), &rectReal, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	//MoveToEx(hDC, rectTemp.left, rectTemp.bottom, &pt);
	//LineTo(hDC, rectTemp.right, rectTemp.bottom);

	::SelectObject(hDC, oldpen);
	::SetTextColor(hDC, oldTextColor);
	::SelectObject(hDC, oldfont);
	::DeleteObject(hBtnPen);
	::DeleteObject(hBtnHoverPen);
}

void QYDateTable::DrawWeekPane(QYDC *pDC, QYRect *rect)
{
	QYRect rcWeek = *rect;
	rcWeek.right = rcWeek.left + m_sizeItem.cx;
	for (int i = 0; i < DAYS_IN_WEEK; i++)
	{
		pDC->DrawText(m_textDayOfWeek[i],
			SafeStrlen(m_textDayOfWeek[i]),
			&rcWeek, 
			DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_WORD_ELLIPSIS);
		rcWeek.OffsetRect(m_sizeItem.cx, 0);
	}
}


void QYDateTable::DrawDayPane(QYDC *pDC, QYRect *rect)
{
	QYRect rcDay(rect->left, rect->top, rect->left + m_sizeItem.cx, rect->top + m_sizeItem.cy);
	
	QYBrush defBrush(QY_THEME.CLIENT_COLOR);
	//QYBrush DesignatedBrush(DATETABLE_MARK_DAY_BK_COLOR);
	QYBrush nowTimeBrush(QY_THEME.DATETABLE_CURRENT_DAY_BK_COLOR);

	COLORREF lastTextCor = 0;
	COLORREF lastCor = QY_THEME.CLIENT_COLOR;
	for (int i = 0; i < WEEK_NUMS_IN_MONTH; i++)
	{
		for (int j = 0; j < DAYS_IN_WEEK; j++)
		{
			if (0 < m_DayArray[i][j].nValue)
			{
				if (lastCor != m_DayArray[i][j].color)
				{
					if (QY_THEME.DATETABLE_CURRENT_DAY_BK_COLOR == m_DayArray[i][j].color)
					{
						pDC->FillRect(rcDay, nowTimeBrush);
					}

					lastCor = m_DayArray[i][j].color;
				}
				
				for (auto it = m_vectorMarkDate.begin(); it != m_vectorMarkDate.end(); it++)
				{
					if (*(*it) == m_DayArray[i][j].date)
					{
						QYPen selPen(PS_SOLID, 1, QY_THEME.DATETABLE_MARK_DAY_BK_COLOR);
						HPEN oldPen = (HPEN)pDC->SelectObject(&selPen);
						HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
						HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(nullBrush);
						pDC->Rectangle(rcDay);
						pDC->SelectObject(&selPen);
						pDC->SelectObject(oldBrush);
						break;
					}
				}

					if (i == LOBYTE(m_HoverItem) && j == HIBYTE(m_HoverItem))
					{
						int borderColor = 0;
						if (m_HoverItem != m_CheckItem)
						{
							borderColor = QY_THEME.DATETABLE_HOVER_DAY_BORDER_COLOR;
						}
						else
						{
							borderColor = QY_THEME.DATETABLE_CHECK_DAY_BORDER_COLOR;
						}

						QYPen selPen(PS_SOLID, 1, borderColor);
						HPEN oldPen = (HPEN)pDC->SelectObject(&selPen);
						HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
						HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(nullBrush);
						pDC->Rectangle(rcDay);
						pDC->SelectObject(&selPen);
						pDC->SelectObject(oldBrush);
					}
					else if (i == LOBYTE(m_CheckItem) && j == HIBYTE(m_CheckItem))
					{
						QYPen selPen(PS_SOLID, 1, QY_THEME.DATETABLE_CHECK_DAY_BORDER_COLOR);
						HPEN oldPen = (HPEN)pDC->SelectObject(&selPen);
						HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
						HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(nullBrush);
						pDC->Rectangle(rcDay);
						pDC->SelectObject(&selPen);
						pDC->SelectObject(oldBrush);
					}

					


				if (lastTextCor != m_DayArray[i][j].textColor)
				{
					pDC->SetTextColor(m_DayArray[i][j].textColor);
					lastTextCor = m_DayArray[i][j].textColor;
				}
				
				pDC->DrawText(m_DayArray[i][j].strValue, m_DayArray[i][j].strValue.Length(), &rcDay, DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_WORD_ELLIPSIS);
			}
			
			rcDay.OffsetRect(m_sizeItem.cx, 0);
		}
		rcDay.OffsetRect(0,m_sizeItem.cy);
		rcDay.left = rect->left ;
		rcDay.right = rect->left + m_sizeItem.cx;
	}
}

void QYDateTable::DrawNowDatePane(QYDC *pDC, QYRect *rect)
{

}


LRESULT QYDateTable::OnPaint()
{
	if (!m_bReDraw)
	{
		return TRUE;
	}

	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);
	QYBrush Brush(QY_THEME.CLIENT_COLOR);
	HBRUSH OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	MemDc.FillRect(rect, Brush);

	MemDc.SetBkMode(TRANSPARENT);

	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	QYRect rectTopPane = QYRect(rect.left + m_nBorderWidth,
		rect.top + m_nBorderWidth ,
		rect.right - m_nBorderWidth,
		rect.top + m_nBorderWidth + m_nOperPaneHeight);

	DrawTopPane(MemDc.GetDC(), &rectTopPane);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYRect rectWeek = rectTopPane;
	rectWeek.top = rectTopPane.bottom;
	rectWeek.bottom = rect.bottom - m_nBorderWidth - (m_sizeItem.cy * WEEK_NUMS_IN_MONTH);
	DrawWeekPane(&MemDc, &rectWeek);
	//MemDc.FillRect(rect, Brush);
	QYRect rectDay = rectWeek;
	rectDay.top = rectWeek.bottom;
	rectDay.bottom = rectDay.top + (m_sizeItem.cy * WEEK_NUMS_IN_MONTH);
	DrawDayPane(&MemDc, &rectDay);

	MemDc.SelectObject(OldBrush);
	Brush.DeleteObject();

	int top = rect.top  + m_nOperPaneHeight + m_nBorderWidth;
	int bottom = rect.bottom - m_nBorderWidth ;
	int colmuns =DAYS_IN_WEEK;
	int i = 0;
	QYRect rectTemp = rect;
#if 0
	for (auto it = m_listItem.begin(); it != m_listItem.end(); it++, i++)
	{
		rectTemp.left = rect.left + m_nBorderWidth;
		rectTemp.right = rect.right - m_nBorderWidth ;
		rectTemp.top = top + i  *m_nItemHeight ;
		rectTemp.bottom = top + (i + 1) *m_nItemHeight ;

		if (rectTemp.bottom <= top)
		{
			continue;
		}
		else if (rectTemp.top >= bottom - (m_nItemHeight / 2))
		{
			break;
		}

		int tempcolum = 0;
		for (auto subIt = (*it)->m_listItem.begin(); 
			subIt != (*it)->m_listItem.end() && colmuns > tempcolum++;
			subIt++)
		{
			rectTemp.left = (*subIt)->left   ;
			rectTemp.right = (*subIt)->right ;

				if (0 == ((QYDateTableItem*)(*subIt))->date)
				{
					MemDc.FillSolidRect(rectTemp,LISTCTRL_BK_COLOR_NORMAL);
					continue;
				}
				else
				{
					MemDc.FillSolidRect(rectTemp, ((QYDateTableItem*)(*subIt))->color);
				}

				if (m_nSelItem == i && m_nSelSubItem == tempcolum)
				{
					MemDc.Rectangle(rectTemp, (HBRUSH)::GetStockObject(NULL_BRUSH));
				}

			rectTemp.bottom -= m_nItemHeight / 2;
			MemDc.DrawText(&(*subIt)->strText, (*subIt)->strText.Length(),&rectTemp, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			rectTemp.bottom += m_nItemHeight / 2;
		}
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

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYDateTable::OnLButtonUp(UINT nFlags, QYPoint point)
{
	int last = m_nDownButtonID;

	if (!m_bPopFlag && -1 != m_CheckItem)
	{
		::SendMessage(::GetParent(m_hWnd), CB_SELECTSTRING, (WPARAM)&m_SelectDate, 0);
	}
	

	m_nDownButtonID = -1;
	if (LAST_MONTH_BUTTON == last)
	{
		InvalidateRect(&m_rectLastMonth);
		GotoLastMonth();
	}
	else if (NEXT_MONTH_BUTTON == last)
	{
		InvalidateRect(&m_rectNextMonth);
		GotoNextMonth();
	}
	else if (TITLE_BUTTON == last)
	{
		InvalidateRect(&m_rectTitleButton);
		//GotoNextMonth();
	}
	
	return TRUE;
}

BOOL QYDateTable::Show(int nCmdShow)
{
	if (m_bPopFlag)
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
	}
	

	return QYWindow::Show(nCmdShow);
}

LRESULT QYDateTable::OnLButtonDown(UINT nFlags, QYPoint point)
{
	QYRect rect;
	GetClientRect(rect);

	if (rect.PtInRect(point))
	{
		if (point.y > (m_nBorderWidth + m_nOperPaneHeight + m_nWeekPaneHeight))
		{
			if (-1 != m_HoverItem)
			{
				if (m_bPopFlag)
				{
					ReleaseCapture();

					Show(SW_HIDE);

					int col = HIBYTE(m_HoverItem);
					int row = LOBYTE(m_HoverItem);

					if (WEEK_NUMS_IN_MONTH > row && 0 <= row && DAYS_IN_WEEK > col && 0 <= col)
					{
						WORD days = QYDate::GetDaysByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());
						WORD dayofweek = QYDate::GetStartDayOfWeekByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());
						if (0 == dayofweek)
						{
							dayofweek = 6;
						}
						QYDate lastDate = m_SelectDate;
						lastDate.LastMonth();
						int lastDays = QYDate::GetDaysByYearAndMonth(lastDate.GetYear(), lastDate.GetMonth());
						QYDate nextDate = m_SelectDate;
						nextDate.NextMonth();
						//int nextDays = QYDate::GetDaysByYearAndMonth(nextDate.GetYear(), nextDate.GetMonth());

						if ((0 == row && col < (dayofweek)))
						{
							m_SelectDate.SetDate(lastDate.GetYear(), lastDate.GetMonth(), lastDays - (dayofweek  - 1 - col));
						}
						else if ((row * DAYS_IN_WEEK + col + 1 - (dayofweek ))> days)
						{
							m_SelectDate.SetDate(nextDate.GetYear(), nextDate.GetMonth(), row * DAYS_IN_WEEK + col + 1 - (dayofweek ) - days);
						}
						else
						{
							m_SelectDate.SetDate(0, 0, m_DayArray[row][col].nValue);
						}
						
						
						m_pSourceWnd->SendMessage(CB_SELECTSTRING, (WPARAM)&m_SelectDate, 0);
					}
					m_HoverItem = -1;
				}
				else
				{
					int col = HIBYTE(m_HoverItem);
					int row = LOBYTE(m_HoverItem);

					if (WEEK_NUMS_IN_MONTH > row && 0 <= row && DAYS_IN_WEEK > col && 0 <= col)
					{
						WORD days = QYDate::GetDaysByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());
						WORD dayofweek = QYDate::GetStartDayOfWeekByYearAndMonth(m_SelectDate.GetYear(), m_SelectDate.GetMonth());
						QYDate lastDate = m_SelectDate;
						lastDate.LastMonth();
						int lastDays = QYDate::GetDaysByYearAndMonth(lastDate.GetYear(), lastDate.GetMonth());
						QYDate nextDate = m_SelectDate;
						nextDate.NextMonth();
						//int nextDays = QYDate::GetDaysByYearAndMonth(nextDate.GetYear(), nextDate.GetMonth());

						if ((0 == row && col < (dayofweek)))
						{
							m_checkDate.SetDate(lastDate.GetYear(), lastDate.GetMonth(), lastDays - (dayofweek - col -1));
						}
						else if ((row * DAYS_IN_WEEK + col + 1 - (dayofweek))> days)
						{
							m_checkDate.SetDate(nextDate.GetYear(), nextDate.GetMonth(), row * DAYS_IN_WEEK + col + 1 - (dayofweek) - days);
						}
						else
						{
							m_checkDate.SetDate(m_SelectDate.GetYear(), m_SelectDate.GetMonth(), m_DayArray[row][col].nValue);
						}

						struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
						if (nullptr != actionCB)
						{
							QYPropertyList properties;
							properties.addProperty("id", getID());
							properties.addProperty("action", "selected");
							actionCB->callback(&properties);
						}
						else
						{
							::SendMessage(::GetParent(m_hWnd), QY_WM_DATETABLE_SELECTED, (WPARAM)m_hWnd, 0);
						}

						m_CheckItem = m_HoverItem;
						Invalidate();
						
					}
				}
			}
			
		}
		else
		{
			if (m_rectLastMonth.PtInRect(point))
			{
				m_nDownButtonID = LAST_MONTH_BUTTON;
				InvalidateRect(&m_rectLastMonth);
			}
			else if (m_rectNextMonth.PtInRect(point))
			{
				m_nDownButtonID = NEXT_MONTH_BUTTON;
				InvalidateRect(&m_rectNextMonth);
			}
			else if (m_rectTitleButton.PtInRect(point))
			{
				return TRUE;
				m_nDownButtonID = TITLE_BUTTON;
				InvalidateRect(&m_rectTitleButton);

				if (nullptr == m_pDropList)
				{
					m_pDropList = new QYDropList;
				}

				m_pDropList->DeleteAllItems();

				for (auto i = m_SelectDate.GetYear() - 10; i < m_SelectDate.GetYear() + 10; i++)
				{
					TCHAR szYear[255] = { 0 };
					SafeSprintf(szYear, L"%d", i);
					m_pDropList->AddItem(szYear, i);
				}

				if (0 == m_pDropList->GetItemCount())
				{
					return TRUE;
				}

				if (nullptr == m_pDropList->GetHwnd())
				{
					if (!m_pDropList->Create(0, NULL, NULL, WS_CHILD, QYRect(0, 0, 0, 0),
						::GetDesktopWindow()))
					{
						return FALSE;
					}
				}

				if (m_pDropList->IsWindowVisible())
				{
					m_pDropList->Show(SW_HIDE);
					m_pDropList->DeleteAllItems();
					SAFE_DESTROY_WINDOW_PTR(m_pDropList);
				}
				else
				{
					QYPoint point(m_rectTitleButton.left, m_rectTitleButton.bottom);
					ClientToScreen(&point);
					m_pDropList->TrackPopup(point, m_rectTitleButton.Width(), TPDL_RIGHT_BOTTOM, this);
					m_pDropList->Show(SW_SHOW);
				}
			}

			
		}
	}
	else
	{
		ReleaseCapture();
		Show(SW_HIDE);
	}

	return TRUE;
}

LRESULT QYDateTable::OnMouseMove(UINT nFlags, QYPoint point)
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

		PostMessage(WM_SETCURSOR, 0, 0);
	}
	else 
	{
		int lastSelItem = m_HoverItem;
		m_HoverItem = -1;

		if (m_rectLastMonth.PtInRect(point))
		{
			m_nHoverButtonID = LAST_MONTH_BUTTON;
			InvalidateRect(&m_rectLastMonth);
		}
		else if (m_rectNextMonth.PtInRect(point))
		{
			m_nHoverButtonID = NEXT_MONTH_BUTTON;
			InvalidateRect(&m_rectNextMonth);
		}
		else if (m_rectTitleButton.PtInRect(point))
		{
			m_nHoverButtonID = TITLE_BUTTON;
			InvalidateRect(&m_rectTitleButton);
		}
		else
		{
			int last = m_nHoverButtonID;
			m_nHoverButtonID = -1;
			if (LAST_MONTH_BUTTON == last)
			{
				InvalidateRect(&m_rectLastMonth);
			}
			else if (NEXT_MONTH_BUTTON == last)
			{
				InvalidateRect(&m_rectNextMonth);
			}
			else if (TITLE_BUTTON == last)
			{
				InvalidateRect(&m_rectTitleButton);
				//GotoNextMonth();
			}

			
			
			QYRect rcDay(m_rectItem.left, m_rectItem.top, m_rectItem.left + m_sizeItem.cx, m_rectItem.top + m_sizeItem.cy);
			for (int i = 0; i < WEEK_NUMS_IN_MONTH; i++)
			{
				for (int j = 0; j < DAYS_IN_WEEK; j++)
				{
					if (rcDay.PtInRect(point))
					{
						m_HoverItem = MAKEWORD(i, j);
 						//InvalidateRect(&rcDay);
						(m_HoverItem != lastSelItem) ? Invalidate() : 0;
						return TRUE;
					}
					rcDay.OffsetRect(m_sizeItem.cx, 0);
				}
				rcDay.OffsetRect(0, m_sizeItem.cy);
				rcDay.left = m_rectItem.left;
				rcDay.right = m_rectItem.left + m_sizeItem.cx;
			}
#if 0


			if (-1 != lastSelItem && lastSelItem != m_SelItem)
			{
				QYRect rcLastDay(m_rectItem.left + (m_sizeItem.cx * HIBYTE(lastSelItem)),
					m_rectItem.top + (m_sizeItem.cy * LOBYTE(lastSelItem)),
					m_rectItem.left + (m_sizeItem.cx * (1 + HIBYTE(lastSelItem))),
					m_rectItem.top + (m_sizeItem.cy * (1 + LOBYTE(lastSelItem))));

				if (!rcLastDay.PtInRect(point))
				{
					InvalidateRect(&rcLastDay);
				}
			}
#endif
		}

		if (-1 == m_HoverItem && -1 != lastSelItem)
		{
			Invalidate();
		}
	}

	return TRUE;
}

LRESULT QYDateTable::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		int last = m_nHoverButtonID;
		m_nHoverButtonID = -1;
		if (LAST_MONTH_BUTTON == last)
		{
			InvalidateRect(&m_rectLastMonth);
		}
		else if (NEXT_MONTH_BUTTON == last)
		{
			InvalidateRect(&m_rectNextMonth);
		}
		else if (TITLE_BUTTON == last)
		{
			InvalidateRect(&m_rectTitleButton);
			//GotoNextMonth();
		}
		m_bHover = FALSE;
		
		m_HoverItem = -1;
		Invalidate();
		//ReleaseCapture();
		PostMessage(WM_SETCURSOR, 0, 0);
	}

	return TRUE;
}


LRESULT QYDateTable::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	if (m_bHover)
	{
		::SetCursor(LoadCursor(NULL, IDC_HAND));
	}
	
	return TRUE;
}

void QYDateTable::GotoLastMonth()
{
	if (m_SelectDate.GetMonth() <= 1)
	{
		m_SelectDate.SetDate(m_SelectDate.GetYear() - 1, MONTHS_IN_YEAR);
	}
	else
	{
		m_SelectDate.SetDate(0, m_SelectDate.GetMonth() - 1);
	}

	UpdateDate();
}

void QYDateTable::GotoNextMonth()
{
	if (m_SelectDate.GetMonth() >= MONTHS_IN_YEAR)
	{
		m_SelectDate.SetDate(m_SelectDate.GetYear() + 1, 1);
	}
	else
	{
		m_SelectDate.SetDate(0, m_SelectDate.GetMonth() + 1);
	}

	UpdateDate();
	
}


void QYDateTable::TrackPopup(QYPoint point, int width, int pos, QYWindow *pWnd, QYDate date)
{
	if (!m_bPopFlag)
	{
		return;
	}
	if (NULL == pWnd )
	{
		return;
	}

	m_SelectDate = date;
	UpdateDate();

	m_pSourceWnd = pWnd;

	auto cY = GetSystemMetrics(SM_CYSCREEN);
	auto cX = GetSystemMetrics(SM_CXSCREEN);
	QYSize defSize = GetDefaultSize();
	if (cY - point.y < defSize.cy)
	{
		point.y = cY - defSize.cy;
	}

	if (cX - point.x < defSize.cx)
	{
		point.x = cX - defSize.cx;
	}

	QYRect rectWnd(point.x, point.y, point.x + defSize.cx, point.y + defSize.cy);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, point.x, point.y, defSize.cx, defSize.cy, SWP_DRAWFRAME);

	(nullptr != m_pShadow) ? m_pShadow->OnMove(rectWnd) : 0;
}

void QYDateTable::Reset()
{
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	m_NowDate.SetDate(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	m_SelectDate = m_NowDate;

	UpdateDate();
}


void QYDateTable::AddMarkDate(QYDate *date)
{
	QYDate *pDate = new QYDate(*date);
	assert(nullptr != pDate);

	for (auto it = m_vectorMarkDate.begin(); it != m_vectorMarkDate.end(); it++)
	{
		if (*date == *(*it))
		{
			SAFE_DELETE(pDate);
			return;
		}
	}

	m_vectorMarkDate.push_back(pDate);
	Invalidate();
}

void QYDateTable::ClearMarkDate()
{
	for (auto it = m_vectorMarkDate.begin(); it != m_vectorMarkDate.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vectorMarkDate.clear();

	m_CheckItem = -1;

	Invalidate();
}

void QYDateTable::SetLanguge(std::string &langID)
{
	for (int i = 0; i < sizeof(m_arrayDayOfWeek) / sizeof(DayOfWeekStruct); i++)
	{
		if (langID == m_arrayDayOfWeek[i].lang_id)
		{
			m_LangugeIndex = i;
			for (int i = 0; i < DAYS_IN_WEEK; i++)
			{
				const char *langText = QYLanguage::instance()->getText(m_textIDDayOfWeek[i]);
				if (0 == strcmp(m_textIDDayOfWeek[i], langText))
				{
					m_textDayOfWeek[i] = m_arrayDayOfWeek[m_LangugeIndex].desc[i];
				}
				else
				{
					SET_TEXT(m_textDayOfWeek[i], langText)
				}
				
			}
			
			return;
		}
	}

	for (int i = 0; i < DAYS_IN_WEEK; i++)
	{
		SET_TEXT(m_textDayOfWeek[i], m_textIDDayOfWeek[i])
	}
}

void QYDateTable::updateText()
{
// 	for (int i = 0; i < DAYS_IN_WEEK; i++)
// 	{
// 		SET_TEXT(m_textDayOfWeek[i], m_textIDDayOfWeek[i])
// 	}

	Invalidate();
}