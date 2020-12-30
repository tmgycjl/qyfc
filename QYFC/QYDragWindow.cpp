#include "QYDragWindow.h"
#include "QYDC.h"

QYDragWindow::QYDragWindow()
{
	m_sizeWnd = QYSize(24, 24);
}


QYDragWindow::~QYDragWindow()
{
}

LRESULT QYDragWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCDESTROY:SAFE_DELETE(m_pPic); break;
	//case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	//case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	default:{break; }
	}

	return FALSE;
}


BOOL QYDragWindow::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle | WS_EX_TOOLWINDOW,
		L"QYDragWindow",
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

	m_pPic = new QYPicture;
	m_pPic->Load(IDP_DRAG,QY_IMAGE_PNG);

	m_sizeWnd = QYSize(m_pPic->GetWidth(),m_pPic->GetHeight());

	return ret;
}

LRESULT QYDragWindow::OnSize(UINT nType, int cx, int cy)
{
	return TRUE;
}

void QYDragWindow::TrackPopup(QYPoint point)
{
	//g_ConsoleLog.Print("x:%d,y:%d,cx:%d,cy:%d\n", point.x - m_sizeWnd.cx / 2, point.y - m_sizeWnd.cy / 2, m_sizeWnd.cx, m_sizeWnd.cy);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, point.x - m_sizeWnd.cx / 2, point.y - m_sizeWnd.cy / 2, m_sizeWnd.cx, m_sizeWnd.cy, SWP_DRAWFRAME);

	Show(SW_SHOW);
}

LRESULT QYDragWindow::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rectClient;
	GetClientRect(rectClient);

	dc.FillSolidRect(rectClient, QY_THEME.CLIENT_COLOR);
	m_pPic->Draw(dc.GetDC(), rectClient);

	return TRUE;
}