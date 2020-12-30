#include "QYComboLBox.h"
#include "QYDC.h"

LRESULT CALLBACK QYComboLBox::ComboLBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QYComboLBox *pWindow = (QYComboLBox*)GetWindowLong(hWnd, GWL_USERDATA);
	if (NULL != pWindow)
	{
		if ((INT_PTR)TRUE == pWindow->WindowProc(message, wParam, lParam))
		{
			return (INT_PTR)FALSE;
		}
	}

	if (nullptr != pWindow && nullptr != pWindow->m_oldWndProc)
	{
		return pWindow->m_oldWndProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

QYComboLBox::QYComboLBox()
{
	m_Flag = 0;
	m_bHot = FALSE;
	m_bFocus = FALSE;
}

QYComboLBox::~QYComboLBox()
{
}

LRESULT QYComboLBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	{
		return TRUE;
		break;
	}
	case LB_SETITEMHEIGHT:
	{
		//g_ConselLog.Print(0, "msg:%04x\n", Msg);
	}
	break;
	case WM_DRAWITEM:
	{
		//g_ConselLog.Print(0, "msg:%04x\n", Msg);
	}
	break;
	case WM_MEASUREITEM:
	{
		//g_ConselLog.Print(0, "msg:%04x\n", Msg);
	}
	break;
	case WM_NCPAINT:
	{
		//break;
		OnNCPaint();
		InvalidateRect(NULL, TRUE);
		return TRUE;
	}
	case WM_PAINT:
	{
		//break;
		OnPaint();
		InvalidateRect(NULL, TRUE);
		return TRUE;
	}
	case WM_NCDESTROY:
	{
		return 1;
	}
	case WM_MOUSEMOVE:
	{
		UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
		if (!(style&ES_READONLY))
		{
			QYPoint pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			OnMouseMove(0, pt);
		}
		//break;

		break;
	}
	case WM_MOUSELEAVE:
	{
		if (m_bHot)
		{
			UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
			if (!(style&ES_READONLY))
			{
				m_bHot = FALSE;
				OnNCPaint();
			}

		}
	}
	case WM_SETFOCUS:
	{
		if (0 != wParam)
		{
			UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
			if (!(style&ES_READONLY))
			{
				m_bFocus = TRUE;
				OnNCPaint();
			}

		}
		//	g_ConselLog.Print(0,"WM_SETFOCUS,window:%d,wparam:%d\n",hWnd,wParam);

	}
	break;
	case WM_KILLFOCUS:
	{
		UINT style = GetWindowLong(GetHwnd(), GWL_STYLE);
		if (!(style&ES_READONLY))
		{
			LoseFocus();
		}

	}
	break;
	default:
	{
		break;
	}
	}

	return FALSE;
}


LRESULT QYComboLBox::LoseFocus()
{
	//m_State = bsNormal;
	m_bFocus = FALSE;
	OnNCPaint();
	//SendMessage( hWnd, WM_KILLFOCUS, (long)hWnd, 0);	
	return TRUE;
}

LRESULT QYComboLBox::OnMouseMove( UINT nFlags, QYPoint point)
{
	if (!m_bHot)
	{
		m_bHot = TRUE;

		OnNCPaint();
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = GetHwnd();
		TrackMouseEvent(&tme);
	}
	// 	CRect rect;
	// 	GetClientRect(hWnd,rect);
	// 	BOOL ret = rect.PtInRect(point );
	// 	if( ret ) 
	// 	{
	// 		if( !m_bHot  ) 
	// 		{
	// 			m_bHot = TRUE;
	// 			OnNCPaint(hWnd);
	// 			//UpdateWindow(hWnd);
	// 		//	SetCapture(hWnd);
	// 		}
	// 	} 
	// 	else 
	// 	{
	// 		m_bHot = FALSE;
	// 			OnNCPaint(hWnd);
	// 		//ReleaseCapture();
	// 	}	

	return TRUE;
}

LRESULT QYComboLBox::OnPaint()
{
	QYPaintDC dc(this);
	
	QYRect rect;
	GetClientRect(rect);			//获取客户区域
	QYPen pen(PS_SOLID, 1, QY_THEME.EDIT_BORDER_COLOR_NORMAL);	//创建画笔
	HPEN oldpen = (HPEN)dc.SelectObject( &pen);
	QYBrush brush(QY_THEME.EDIT_BK_COLOR_NORMAL);		//创建画刷
	HBRUSH oldbrush = (HBRUSH)dc.SelectObject(&brush);
	dc.Rectangle(rect.left, rect.top, rect.right, rect.bottom);			//绘制边框

	dc.SelectObject( oldbrush);
	dc.SelectObject(oldpen);
	pen.DeleteObject();
	brush.DeleteObject();

	return TRUE;
}

LRESULT QYComboLBox::OnNCPaint()
{
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);			//获取客户区域
	// 		rect.InflateRect(1,1,1,1);			//将客户区域增大一个像素

	COLORREF color = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
	if (m_bFocus)
	{
		color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
	}
	else
	{
		if (m_bHot)
		{
			color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
		}
	}
	//color = EDIT_BORDER_COLOR_HOVER;
	QYPen pen(PS_SOLID, 1, color);	//创建画笔


	HPEN oldpen = (HPEN)pDC->SelectObject( &pen);
	QYBrush brush(GetStockObject(NULL_BRUSH));		//创建画刷
	HBRUSH oldbrush = (HBRUSH)pDC->SelectObject(&brush);

	pDC->Rectangle(0, 0, rect.right - rect.left, rect.bottom - rect.top);			//绘制边框

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);
	pen.DeleteObject();;


	return TRUE;
}