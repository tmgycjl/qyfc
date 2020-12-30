#include "QYToolTip.h"
#include "QYDC.h"
#include "QYApp.h"

QYToolTip::QYToolTip() :QYWindow()
{
	m_pRefWind = nullptr;
}

QYToolTip::~QYToolTip()
{
	for (auto it = m_vectorToolTipNode.begin(); it != m_vectorToolTipNode.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vectorToolTipNode.clear();
}

LRESULT QYToolTip::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//g_ConsoleLog.Print("message:%04x\n", message);

	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_SIZE:{OnSize(0, 0, 0); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case WM_NCDESTROY:{ break; }
	case WM_TIMER:{ OnTimer(wParam); return TRUE;	}break;
	case WM_WINDOWPOSCHANGED:{OnWindowPosChanged(); }break;
	default:{break; }
	}

	return 0L;
}

BOOL QYToolTip::Create()
{
	BOOL ret = QYWindow::Create(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED,
		L"QYToolTip",
		L"",
		WS_CHILD ,
		0,0,0,0,
		::GetDesktopWindow());

	if (!ret)
	{
		return FALSE;
	}

// 	LONG   dwNewLong = GetWindowLong(m_hWnd, GWL_EXSTYLE);
// 	dwNewLong |= WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED;
// 	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewLong);


	return ret;
}

BOOL QYToolTip::AddTool(QYWindow* window, LPCTSTR strText)
{
	for (auto it = m_vectorToolTipNode.begin(); it != m_vectorToolTipNode.end(); it++)
	{
		if (window == (*it)->pWind)
		{
			return FALSE;
		}
	}

	ToolTipNode *node = new ToolTipNode;
	node->pWind = window;
	node->strText = strText;
	m_vectorToolTipNode.push_back(node);

	return TRUE;
}

BOOL QYToolTip::DeleteTool(QYWindow* window)
{
	for (auto it = m_vectorToolTipNode.begin(); it != m_vectorToolTipNode.end(); it++)
	{
		if (window == (*it)->pWind)
		{
			SAFE_DELETE(*it);
			return TRUE;
		}
	}

	return TRUE;
}


BOOL  QYToolTip::Show(int nCmdShow)
{
	if (nCmdShow == SW_HIDE)
	{
		KillTimer(1);
	}
	

	return QYWindow::Show(nCmdShow);
}

void QYToolTip::OnTimer(int nEventID)
{
	if (nEventID == 1)
	{
		QYPaintDC dc(this);

		auto cY = GetSystemMetrics(SM_CYSCREEN);
		auto cX = GetSystemMetrics(SM_CXSCREEN);
		QYSize size = dc.GetTextExtent(m_strText);
		int desWidth = size.cx + 2 * 5;
		int desHeight = size.cy + 2 * 5;

		if (nullptr != m_pRefWind)
		{
			QYRect rectWnd;
			m_pRefWind->GetWindowRect(rectWnd);

			POINT pt;
			::GetCursorPos(&pt);
			QYPoint point = pt;

			if (rectWnd.PtInRect(point))
			{
				int desX = point.x;
				int desY = rectWnd.bottom + 5;

				if (desY > cY)
				{
					desY = rectWnd.top - 5 - desHeight;
				}

				if (desX < 0)
				{
					desX = 0;
				}
				else if (desX + desWidth > cX)
				{
					desX = cX - desWidth;
				}
				//g_ConsoleLog.Print("desX:%d,desY:%d\n", desX, desY);

				int ret = ::SetWindowPos(m_hWnd, HWND_TOPMOST, desX, desY, desWidth, desHeight, SWP_NOACTIVATE);

				Show(SW_SHOW);
				//g_ConsoleLog.Print("SetWindowPos ret:%d\n", ret);
			}
		}
		else if (0 < m_pt.x && 0 < m_pt.y)
		{
			//g_ConsoleLog.Print("point.x:%d,point.y:%d\n", m_pt.x, m_pt.y);
			::SetWindowPos(m_hWnd, HWND_TOPMOST, m_pt.x, m_pt.y, desWidth, desHeight, SWP_NOACTIVATE);

			Show(SW_SHOW);
		}

		KillTimer(1);
	}
}
#if 1

void QYToolTip::TrackPopup(QYWindow *pRefWind,  QYPoint point,int delay)
{
	m_pRefWind = pRefWind;
	m_pt = point;

	SetTimer(1, delay);
}

#endif
LRESULT QYToolTip::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush;
	Brush.CreateSolidBrush(QY_THEME.TOOLTIP_BK_COLOR_NORMAL);

//	HBRUSH OldBrush;

	//OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.TOOLTIP_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	MemDc.FillRect(rect, Brush);

	MemDc.SetBkMode(TRANSPARENT);
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	MemDc.DrawText(m_strText, m_strText.Length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	//MemDc.SelectObject(OldBrush);
	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();
	Brush.DeleteObject();

	MemDc.SelectObject(OldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();
	return TRUE;
}

void QYToolTip::OnWindowPosChanged()
{
	OnSize(0, 0, 0);
}

LRESULT QYToolTip::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	QYPaintDC dc(this); // device context for painting
	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush;
	Brush.CreateSolidBrush(QY_THEME.TOOLTIP_BK_COLOR_NORMAL);

//	HBRUSH OldBrush;

	QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.TOOLTIP_BORDER_COLOR);
	HPEN OldPen;

	OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	MemDc.FillRect(rect, Brush);

	MemDc.SetBkMode(TRANSPARENT);
	MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	MemDc.DrawText(m_strText, m_strText.Length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	QYRect rectWnd;
	GetWindowRect(rectWnd);

	SIZE sizeWindow = { rect.Width(), rect.Height() };
	POINT ptWinPos = { rectWnd.left, rectWnd.top };
	POINT ptSrc = { 0, 0 };
	BLENDFUNCTION bf;

	bf.AlphaFormat = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = 0xff;
	::UpdateLayeredWindow(m_hWnd, dc.GetDC(), &ptWinPos, &sizeWindow, MemDc.GetDC(), &ptSrc, 0, &bf, 2);

	MemDc.SelectObject(OldPen);
	PenLine.DeleteObject();
	Brush.DeleteObject();

	MemDc.SelectObject(OldFont);

	MemDc.DeleteDC();

	return FALSE;
}

void  QYToolTip::SetText(LPCTSTR strText)
{
	m_strText = strText;
	Invalidate();
}