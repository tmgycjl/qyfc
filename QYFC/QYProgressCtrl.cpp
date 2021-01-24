#include "QYProgressCtrl.h"
#include "QYDC.h"

QYProgressCtrl::QYProgressCtrl()
{
	m_GrooveBkCor = QY_THEME.PROGRESS_BK_COLOR;
	m_BkCor = RGB(122, 122, 122);
	m_ProcessBkCor = QY_THEME.PROGRESS_PROCESS_COLOR;
	m_nPos = 0;
	m_nRange = 0;
	m_bHover = FALSE;
}


QYProgressCtrl::~QYProgressCtrl()
{
}

BOOL QYProgressCtrl::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	return QYWindow::Create(dwExStyle,
		L"QYProgress",
		lpWndName,
		dwStyle,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
}

void   QYProgressCtrl::SetBkColor(COLORREF grooveBkColor, COLORREF progressBkColor)
{
	m_GrooveBkCor = grooveBkColor;
	m_ProcessBkCor = progressBkColor;
	Invalidate();
}

LRESULT QYProgressCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	case WM_PAINT:
	{
		return OnPaint();
	}
	case WM_NCPAINT:
		//return OnNCPaint();
		break;
	case WM_LBUTTONDOWN:
	{
		struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
		if (nullptr != actionCB)
		{
			QYRect rc;
			GetClientRect(rc);
			int pos = m_nRange * LOSHORT(lParam) / rc.Width();
			QYPropertyList properties;
			properties.addProperty("id", getID());
			properties.addProperty("action", "lbuttondown");
			properties.addProperty("pos", pos);
			actionCB->callback(&properties);
		}
	}
	break;
	default:break;
	}
	return 0L;
}

BOOL QYProgressCtrl::OnPaint()
{
	QYPaintDC paintDC(this);

	QYRect		rt;
	GetClientRect(rt);

	QYMemDC dc(&paintDC, rt);

	QYRect rtGroove = rt;
	rtGroove.top = rt.top + 3;
	rtGroove.bottom = rt.bottom - 3;
	QYRect rtProcess = rt;
	rtProcess.right = (float)(rt.Width() * m_nPos) / (float)m_nRange;

	//dc.Rectangle(rt, PROGRESS_BK_COLOR, BUTTON_BORDER_COLOR_NORMAL);
	dc.FillSolidRect(rt, m_GrooveBkCor);
	dc.FillSolidRect(rtProcess, m_ProcessBkCor);

	paintDC.BitBlt(0, 0, rt.Width(), rt.Height(), &dc, 0, 0, SRCCOPY);

	return TRUE;
}


LRESULT QYProgressCtrl::OnNCPaint()
{
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);

	QYPen pen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_NORMAL);
	HPEN oldpen = (HPEN)pDC->SelectObject(&pen);

	pDC->Rectangle(0, 0, rect.right - rect.left, rect.bottom - rect.top,(HBRUSH)::GetStockObject(NULL_BRUSH));

	pDC->SelectObject(oldpen);

	pen.DeleteObject();

	ReleaseDC(pDC);

	return TRUE;
}

void QYProgressCtrl::SetPos(int pos)
{
	m_nPos = pos;
	Invalidate();
}


void QYProgressCtrl::SetRange(int range)
{
	m_nRange = range;
}

int QYProgressCtrl::GetRange()
{
	return m_nRange;
}


int QYProgressCtrl::GetPos()
{
	return m_nPos;
}
