#include "QYSwitchButton.h"

#include "QYDC.h"
QYSwitchButton::QYSwitchButton() :QYButton()
{
	m_bCheck = FALSE;
	
}

QYSwitchButton::~QYSwitchButton()
{
	SAFE_DELETE(m_picNormalBox);
	SAFE_DELETE(m_picSelectBox);
}

BOOL QYSwitchButton::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL  ret =  QYWindow::Create(dwExStyle,
		L"QYSwitchButton",
		nullptr,
		dwStyle | WS_CHILD,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
	if (!ret)
	{
		return FALSE;
	}
	m_picNormalBox = new QYPicture;
	m_picNormalBox->Load(IDP_SWITCH_OFF, QY_IMAGE_PNG);
	m_picSelectBox = new QYPicture;;
	m_picSelectBox->Load(IDP_SWITCH_ON, QY_IMAGE_PNG);

	return TRUE;
}

LRESULT QYSwitchButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	case WM_PAINT:
	{
		return OnPaint();
	}
	case WM_LBUTTONDOWN:
	{
		m_bCheck = !m_bCheck;
		InvalidateRect(NULL, TRUE);
		return TRUE;
	}
	case WM_LBUTTONDBLCLK:
	{
		PostMessage(WM_LBUTTONDOWN, wParam, lParam);
		return TRUE;
	}
		break;
	case WM_LBUTTONUP:
	{
		return OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	case WM_MOUSELEAVE:
	{
		OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_MOUSEMOVE:
	{
		return TRUE;
		return OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	case WM_SETFOCUS:
	{
		m_bFocus = TRUE;
		InvalidateRect(NULL, TRUE);
	}
	break;
	case WM_KILLFOCUS:
	{
		LoseFocus();
	}
	break;
	case WM_ENABLE:
	{
		m_bDisable = (1 == wParam) ? FALSE : TRUE;
		InvalidateRect(NULL, TRUE);
	}
	break;
	case WM_NCPAINT:
	{
		OnNCPaint();
		return TRUE;
	}
	default:
	{
		break;
	}
	}
	return 0L;
}

LRESULT QYSwitchButton::OnNCPaint()
{
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);
	QYRect rectClient;
	GetClientRect(rectClient);
	int offsetX = (rect.Width() - rectClient.Width()) / 2;
	int offsetY = (rect.Height() - rectClient.Height()) / 2;

	rectClient.OffsetRect(offsetX, offsetY);

	QYBrush brush;
	brush.CreateSolidBrush(QY_THEME.CLIENT_COLOR);
	HBRUSH oldbrush = (HBRUSH)pDC->SelectObject(&brush);

	pDC->SelectObject(oldbrush);
	brush.DeleteObject();

	ReleaseDC(pDC);

	return TRUE;
}

BOOL QYSwitchButton::OnPaint()
{
	QYPaintDC paintDC(this);

	QYRect		RC;
	QYRect		RCRight;

	GetClientRect(RC);

	QYMemDC dc(&paintDC, RC);
	RCRight = RC;
	
	QYBrush Brush;
	HBRUSH hOldBrush;
	QYPen Pen;
	HPEN  hOldPen;
	QYPoint		PT(2, 2);
	HFONT  hOldFont;

	dc.SetBkMode(TRANSPARENT);

	hOldFont = (HFONT)dc.SelectObject(m_pFont);
	Brush = CreateSolidBrush(QY_THEME.CLIENT_COLOR);

	dc.SetTextColor(QY_THEME.BUTTON_TEXT_COLOR_NORMAL);
	if (m_bFocus)
	{
		Pen.CreatePen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_FOCUS);
	}
	else if (m_bDisable)
	{
		dc.SetTextColor(QY_THEME.BUTTON_TEXT_COLOR_DISABLE);
		Brush.CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_DISABLE);
		Pen.CreatePen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_DISABLE);
	}
	else
	{
		if (m_bDown)
		{
			Pen.CreatePen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_DOWN);
		}
		else
		{
			if (m_bHover)
			{
				Pen.CreatePen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_HOVER);
			}
			else
			{
				Pen.CreatePen(PS_SOLID, 1, QY_THEME.BUTTON_BORDER_COLOR_NORMAL);
			}
		}
	}

	hOldBrush = (HBRUSH)dc.SelectObject(&Brush);
	hOldPen = (HPEN)dc.SelectObject(&Pen);

	dc.FillRect(RC, Brush);

	if (!m_bCheck)
	{
		if (nullptr != m_picNormalBox)
		{
			QYRect rectTemp;
			//rectTemp.left = RC.left + (RC.Height() - m_picNormalBox->GetHeight()) / 2;
			rectTemp.left = RC.left;
			rectTemp.top = RC.top + (RC.Height() - m_picNormalBox->GetHeight()) / 2;
			rectTemp.right = rectTemp.left + m_picNormalBox->GetWidth();
			rectTemp.bottom = rectTemp.top + m_picNormalBox->GetHeight();

			m_picNormalBox->Draw(dc.GetDC(), 
				RC.left ,
				RC.top + (RC.Height() - m_picNormalBox->GetHeight()) / 2,
				m_picNormalBox->GetWidth(), m_picNormalBox->GetHeight());

			if (m_bHover)
			{
				QYBrush brush(GetStockObject(NULL_BRUSH));
				dc.SelectObject(&brush);
				dc.RoundRect(rectTemp, 2, 2);
				dc.SelectObject(&Brush);
			}
			
			RCRight.left = rectTemp.right + 5;
		}
	}
	else
	{
		if (nullptr != m_picSelectBox)
		{
			QYRect rectTemp;
			//rectTemp.left = RC.left + (RC.Height() - m_picNormalBox->GetHeight()) / 2;
			rectTemp.left = RC.left;
			rectTemp.top = RC.top + (RC.Height() - m_picNormalBox->GetHeight()) / 2;
			rectTemp.right = rectTemp.left + m_picNormalBox->GetWidth();
			rectTemp.bottom = rectTemp.top + m_picNormalBox->GetHeight();
			m_picSelectBox->Draw(dc.GetDC(),
				RC.left ,
				RC.top + (RC.Height() - m_picSelectBox->GetHeight()) / 2,
				m_picSelectBox->GetWidth(),
				m_picSelectBox->GetHeight());
			if (m_bHover)
			{
				QYBrush brush(GetStockObject(NULL_BRUSH));
				dc.SelectObject(&brush);
				dc.RoundRect(rectTemp, 2, 2);
				dc.SelectObject(&Brush);
			}

			RCRight.left = rectTemp.right + 5;
		}
	}

	if (!m_bFocus)
	{
		dc.FillRect(RCRight, Brush);
	}
	else
	{
		dc.RoundRect(RCRight, 2, 2);
	}


	dc.DrawText(m_strWindowText, m_strWindowText.Length(), &RCRight, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	dc.SelectObject(hOldBrush);
	dc.SelectObject(hOldPen);
	dc.SelectObject(hOldFont);

	Brush.DeleteObject();
	Pen.DeleteObject();

	paintDC.BitBlt(0, 0, RC.Width(), RC.Height(), &dc, 0, 0, SRCCOPY);

	return TRUE;
}

BOOL QYSwitchButton::SetCheck(BOOL bCheck)
{
	BOOL last = m_bCheck;
	m_bCheck = bCheck;
	Invalidate();
	return last; 
}

BOOL QYSwitchButton::GetCheck()
{
	return m_bCheck;
}

QYSize& QYSwitchButton::GetBackImageSize() const
{
	QYSize size;
	if (nullptr != m_picNormalBox)
	{
		size.cx = m_picNormalBox->GetWidth();
		size.cy = m_picNormalBox->GetHeight();
	}

	if (nullptr != m_picSelectBox)
	{
		size.cx = std::fmax(m_picNormalBox->GetWidth(), size.cx);
		size.cy = std::fmax(m_picNormalBox->GetHeight(), size.cy);
	}

	return size;
}