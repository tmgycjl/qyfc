#include "QYStatic.h"
#include "QYDC.h"
#include "QYLanguage.h"

LRESULT CALLBACK QYStatic::StaticProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QYStatic *pWindow = (QYStatic*)GetWindowLong(hWnd, GWL_USERDATA);
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

QYStatic::QYStatic() :QYWindow()
{
	m_textFormat = DT_LEFT | DT_EDITCONTROL | DT_WORD_ELLIPSIS | DT_VCENTER | DT_SINGLELINE;
}


QYStatic::~QYStatic()
{
}

BOOL QYStatic::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYStatic",
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


	return ret;
}



LRESULT QYStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:OnPaint(); break;
	default:break;
	}
	return FALSE;
}

LRESULT QYStatic::OnPaint()
{
	QYPaintDC dc(this);
	
	QYRect rect;
	GetClientRect(rect);			
	QYBrush brush(CreateSolidBrush(m_colorBk));
	HBRUSH oldbrush = (HBRUSH)dc.SelectObject(&brush);

	//dc.Rectangle(0, 0, rect.right - rect.left, rect.bottom - rect.top);
	dc.FillRect(rect, brush);

	HFONT  hOldFont;
	dc.SetBkMode(TRANSPARENT);

	dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	hOldFont = (HFONT)dc.SelectObject(m_pFont);
	//rect.left += 2;

	dc.DrawText(m_strWindowText, m_strWindowText.Length(), &rect, m_textFormat);
	//::DrawText(dc.GetDC(), text, SafeStrlen(text) * sizeof(TCHAR), &rt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	return TRUE;
}

void QYStatic::SetTextFormat(UINT removeFormat, UINT addFormat)
{
	m_textFormat &= ~removeFormat;
	m_textFormat |= addFormat;
}

