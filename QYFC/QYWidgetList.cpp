#include "QYWidgetList.h"
#include "QYDC.h"
#include "QYEdit.h"
#include "QYComboBox.h"
#include "QYSwitchButton.h"
#include "QYDateTimeCtrl.h"
#include "QYDateTable.h"
#include "QYIPCtrl.h"
#include "QYFolderEdit.h"


QYWidgetList::QYWidgetList(BOOL bInTab) :QYWindow()
{
	m_nMaxID         = 0;
	m_ctrlWidth      = 100;
	m_ctrlSeparateHeigth = 16;
	m_padWidth = m_ctrlSeparateHeigth;
	m_padHeight = 16;
	m_bReDraw = TRUE;
	m_lastCtrlBottom = m_ctrlSeparateHeigth;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_bFocus = FALSE;
	m_bReDraw = TRUE;
	m_bAlternateColorFlag = TRUE;
	m_nMaxTitleTextWidth = 0;
	m_nTitleCtrlRance = 10;
	m_workRect = 0;
	m_bInTab = bInTab;
	m_Orientation = ORIENTATION_VERTICAL;
}


QYWidgetList::~QYWidgetList()
{
	m_VScrollBar.DestroyWindow();
	for (QYWidget *p : m_listItem){ SAFE_DESTROY_WINDOW_PTR(p); }
	m_listItem.clear();
}

BOOL QYWidgetList::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYWidgetList",
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

BOOL QYWidgetList::Create(QYWindow *pParent, QYRect rect)
{
	m_pParent = pParent;

	m_workRect = rect;

	return TRUE;
}

LRESULT QYWidgetList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:break;
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_VSCROLL:
	{
		if (onVScroll(this, LOWORD(wParam)))
		{
			ScrollItem(m_nScrollPos);
		}

		return TRUE; 
	}
	break;
	case WM_MOUSEWHEEL:{onMouseWheel(this, LOWORD(wParam), HIWORD(wParam)); }break;
	case WM_TIMER:
	{
		if (onTimer(this, wParam))
		{
			ScrollItem(m_nScrollPos);
		}
	}
	break;
	case QY_WM_LISTCTRL_ITEM_RCLICK:
	case QY_WM_DATETABLE_SELECTED:
	case QY_WM_FOLDEREDIT_CHANGED:
	case QY_WM_BUTTON_DOWN:
    case QY_WM_LISTCTRL_ITEM_CLICK:
    case QY_WM_EDIT_CHANGED:
	case WM_COMMAND:
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

LRESULT QYWidgetList::OnPaint()
{
	if (!m_bReDraw){return TRUE;}

	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, QY_THEME.CLIENT_COLOR);

	dc.BitBlt(0,0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	
	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYWidgetList::OnSize(UINT nType, int cx, int cy)
{
	return UpdateLayout();
}

QYWidget* QYWidgetList::Append(QYSize sizeWidget)
{
	auto right = 0;

	QYWidget *pWidget = new QYWidget(sizeWidget);
	assert(nullptr != pWidget);

	if (!pWidget->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), m_hWnd))
	{
		SAFE_DELETE(pWidget);
		return nullptr;
	}

	m_listItem.push_back(pWidget);

	m_lastCtrlBottom += sizeWidget.cy + m_ctrlSeparateHeigth;

	return pWidget;
}


void QYWidgetList::ScrollItem(int off)
{
	QYRect rect;
	GetClientRect(rect);

	QYRect rectCtrl;

	QYRect rectItem(rect.right - m_ctrlWidth - m_padWidth, m_padHeight - m_nBorderWidth - off , rect.right - m_nVScrollWidth - m_padWidth, m_padHeight);

	int realWidth = rect.Width() - 2 * m_nBorderWidth - m_nVScrollWidth;
	for (QYWidget *pItem:m_listItem)
	{
		QYSize sizeItem = pItem->GetSize();
		if (sizeItem.cx > 0)
		{
			rectItem.left = rect.left + m_nBorderWidth + (realWidth - sizeItem.cx) / 2;
			rectItem.right = rectItem.left + sizeItem.cx;
		}
		else
		{
			rectItem.left = rect.left + m_nBorderWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth;
		}

		rectItem.bottom = rectItem.top + sizeItem.cy;

		pItem->MoveWindow(rectItem);
		rectItem.top = rectItem.bottom + m_ctrlSeparateHeigth ;
	}

	InvalidateRect(&m_rectItemList);
}

BOOL QYWidgetList::UpdateLayout()
{
	QYRect rect;
	GetClientRect(rect);

	if (0 >= rect.Width() || 0 >= rect.Height())
	{
		return FALSE;
	}

	if (m_enableScroll)
	{
		int showPixs = rect.Height();
		int totalHeight = m_lastCtrlBottom;
		updateVScrollLayout(this, rect, totalHeight - showPixs);
	}

	m_padWidth = (rect.Width() - m_nVScrollWidth - m_ctrlWidth - m_nTitleCtrlRance - m_nMaxTitleTextWidth) / 2;
	if (0 > m_padWidth)
	{
		m_padWidth = 20;
	}

	QYRect rectItem(0, m_nBorderWidth + m_padHeight, 0, 0);
	int realWidth = rect.Width() - 2 * m_nBorderWidth - m_nVScrollWidth;
	for (QYWidget *pItem:m_listItem)
	{
		QYSize sizeItem = pItem->GetSize();
		if (sizeItem.cx > 0)
		{
			rectItem.left = rect.left + m_nBorderWidth + (realWidth - sizeItem.cx) / 2;
			rectItem.right = rectItem.left + sizeItem.cx ;
		}
		else
		{
			rectItem.left = rect.left + m_nBorderWidth;
			rectItem.right = rect.right - m_nBorderWidth - m_nVScrollWidth;
		}

		if (sizeItem.cy > 0)
		{
			rectItem.bottom = rectItem.top + sizeItem.cy;
		}
		else if (sizeItem.cy >= BOTTOM_TO_PARENT)
		{
			rectItem.bottom = rect.bottom - m_nBorderWidth;
		}

		pItem->MoveWindow(rectItem);
		rectItem.top = rectItem.bottom + m_ctrlSeparateHeigth;
	}

	m_rectItemList = rect;
	m_rectItemList += QYRect(m_nBorderWidth,
		m_nBorderWidth ,
		-m_nVScrollWidth - m_nBorderWidth,
		-m_nBorderWidth );

	InvalidateRect(&m_rectItemList);

	if (m_enableScroll && m_bVShowScroll)
	{
		m_VScrollBar.Invalidate();
	}
	

	return TRUE;
}


void QYWidgetList::enableScroll(BOOL enable)
{
	m_enableScroll = enable;
	UpdateLayout();
}
