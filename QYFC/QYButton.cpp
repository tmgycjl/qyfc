#include "QYButton.h"
#include "QYLanguage.h"
LRESULT CALLBACK QYButton::ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QYButton *pWindow = (QYButton*)GetWindowLong(hWnd, GWL_USERDATA);
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


#include "QYDC.h"

QYButton::QYButton() :QYWindow()
{
	m_bHover = FALSE;
	m_bDown = FALSE;
	m_bDisable = FALSE;
	m_Flag = BUTTON_DEFAULT;
	m_bGetText = FALSE;
	m_bFocus = FALSE;
	m_Style = 1;
	m_pImage = nullptr;
}

QYButton::~QYButton()
{
	SAFE_DELETE(m_pImage);
}

BOOL QYButton::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	return QYWindow::Create(dwExStyle,
		L"QYButton",
		lpWndName,
		dwStyle | WS_CHILD,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd);
}

LRESULT QYButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDBLCLK:  //ÆÁ±ÎË«»÷ÊÂ¼þ
	{
		PostMessage(WM_LBUTTONDOWN, wParam, lParam);
		return TRUE;
		break;
	}
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
	{
		return OnPaint();
	}
	case WM_LBUTTONDOWN:
	{
		return OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		 break;
	}
	case WM_LBUTTONUP:
	{
		return  OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		 break;
	}
	case WM_MOUSEMOVE:
	{
		return OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		break;
	}
	case WM_MOUSELEAVE:
	{
		 return OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_SETFOCUS:
	{
		m_bFocus = TRUE;
		InvalidateRect( NULL, TRUE);
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
		Invalidate();
	}
	break;
	case WM_NCPAINT:
	{
		//OnNCPaint();
		return TRUE;
	}
	default:
	{
		break;
	}
	}
	return 0L;
}

#include "QYDC.h"


BOOL QYButton::OnPaint()
{
	//return FALSE;
	QYPaintDC paintDC(this);
	QYRect		RC;
	GetClientRect(RC);

	QYMemDC dc(&paintDC, RC);
	 
	

	if (nullptr != m_pImage)
	{
		dc.FillSolidRect(RC, m_colorBk);
		
		int iconLeft = 0;
		int iconTop = 0;
		int desWidth = RC.Width();;
		int desHeight = RC.Height();

		int imageWidth = m_pImage->GetWidth();
		int imageHeight = m_pImage->GetHeight();

		if (RC.Width() >= imageWidth && RC.Height() >= imageHeight)
		{
			iconLeft = RC.left + (RC.Width() - imageWidth) / 2;
			iconTop = RC.top + (RC.Height() - imageHeight) / 2;
			 desWidth = (RC.Width() >= imageWidth) ? imageWidth : RC.Width() - 2;
			 desHeight = (RC.Height() >= imageHeight) ? imageHeight : RC.Height() - 2;
		}
		else if (RC.Width() <= imageWidth)
		{
			if ((float)RC.Width() / (float)RC.Height() > (float)imageWidth / (float)imageHeight)
				{
					iconTop = RC.top;
					desHeight = RC.Height();
					desWidth = RC.Height()*((float)imageWidth / (float)imageHeight);
					iconLeft = RC.left + (RC.Width() - desWidth) / 2;
				}
				else
				{
					iconLeft = RC.left;
					desWidth = RC.Width();
					desHeight = RC.Width()*((float)imageHeight / (float)imageWidth);
					iconTop = RC.top + (RC.Height() - desWidth) / 2;
				}
		}
		else if (RC.Width() > imageWidth && RC.Height() < imageHeight)
		{
			iconTop = RC.top;
			desHeight = RC.Height();
			desWidth = RC.Width()*((float)RC.Height() / (float)imageHeight);
			iconLeft = RC.left + (RC.Width() - desWidth) / 2;
			
		}

			if (m_bDown)
			{
				m_pImage->SetPressed();
			}
			else
			{
				if (m_bHover)
				{
					m_pImage->SetHovered();
				}
			}
	
			m_pImage->Draw(dc.GetDC(), iconLeft, iconTop, desWidth, desHeight);
	}	
	else
	{
		QYBrush Brush;
		HBRUSH hOldBrush;
		QYPen Pen;
		HPEN  hOldPen;
		QYPoint		PT(2, 2);
		HFONT  hOldFont;


		

		dc.SetBkMode(TRANSPARENT);

		hOldFont = (HFONT)dc.SelectObject(m_pFont);

		dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

		if (0)
		{
			if (m_bDown)
			{
				Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_FOCUS);
				Brush.CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_DOWN);
			}
			else
			{
				Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_FOCUS);
				Brush = CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_DOWN);
			}

		}
		else if (m_bDisable)
		{
			dc.SetTextColor(QY_THEME.TEXT_COLOR_DISABLE);
			Brush.CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_DISABLE);
			Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_DISABLE);
		}
		else
		{
			
				if (m_bDown)
				{
					Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_FOCUS);
					Brush.CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_DOWN);
				}
				else
				{
					if (m_bHover)
					{
						Brush.CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_HOVER);

						Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_HOVER);
					}
					else
					{
						if (m_Flag&BUTTON_TRANSPRENT_BACKGROUND)
						{
							Brush = CreateSolidBrush(m_colorBk);
							//dc.FillSolidRect(RC, m_colorBk);
						}
						else
						{
							Pen.CreatePen(PS_SOLID, m_nBorderWidth, QY_THEME.BUTTON_BORDER_COLOR_NORMAL);
							Brush = CreateSolidBrush(QY_THEME.BUTTON_BK_COLOR_NORMAL);
						}

					
					}
				}
		}

			hOldBrush = (HBRUSH)dc.SelectObject(&Brush);
			hOldPen = (HPEN)dc.SelectObject(&Pen);
			if (0 < m_nBorderWidth)
			{
				dc.Rectangle(RC);
				
			}
			else
			{
				dc.FillRect(RC, Brush);
			}

			
			dc.SelectObject(hOldBrush);
			dc.SelectObject(hOldPen);

		
		
		int off = m_bFocus ? 1 : 0;
		//dc.Rectangle( RC.left + off, RC.top + off, RC.right - off, RC.bottom - off,(HBRUSH)::GetStockObject(NULL_BRUSH));


		dc.DrawText(m_strWindowText, m_strWindowText.Length(), &RC, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		
		dc.SelectObject(hOldFont);

		Brush.DeleteObject();
		Pen.DeleteObject();
	}
	

	paintDC.BitBlt(0, 0, RC.Width(), RC.Height(), &dc, 0, 0, SRCCOPY);

	return TRUE;
}

BOOL QYButton::OnLButtonDown( UINT nFlags, QYPoint point)
{
	if (m_bDisable)
	{
		return TRUE;
	}

	m_bDown = TRUE;
	SetCapture();
	SetFocus();
	//::SetCursor(LoadCursor(NULL, IDC_HAND));
	Invalidate();

	int ctrlID = GetDlgCtrlID();
	if (0 == ctrlID)
	{
		ctrlID = GetCtrlID();
	}
	::SendMessage(::GetParent(m_hWnd), QY_WM_BUTTON_DOWN, ctrlID, (LPARAM)(m_hWnd));

	return TRUE;
}

BOOL QYButton::OnLButtonUp(UINT nFlags, QYPoint point)
{
	if (m_bDisable)
	{
		return TRUE;
	}

	m_bDown = FALSE;
	m_bHover = FALSE;
	ReleaseCapture();

	//::SetCursor(LoadCursor(NULL, IDC_ARROW));

	Invalidate();
	int ctrlID = GetDlgCtrlID();
	if (0 == ctrlID)
	{
		ctrlID = GetCtrlID();
	}

    QYPropertyList properties;
    properties.addProperty("id", getID());
    struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
    if (nullptr != actionCB)
    {
        actionCB->callback(&properties);
    }
	else
	{
		::SendMessage(::GetParent(m_hWnd), WM_COMMAND, ctrlID, (LPARAM)(m_hWnd));
	}

   
	
	return TRUE;
}

BOOL QYButton::LoseFocus()
{
	m_bDown = FALSE;
	m_bFocus = FALSE;
	
	InvalidateRect( NULL, TRUE);
	//SendMessage(WM_KILLFOCUS, (long)GetHwnd(), 0);
	return TRUE;
}

BOOL QYButton::OnMouseMove(UINT nFlags, QYPoint point)
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
}

LRESULT QYButton::OnMouseLeave(WPARAM, LPARAM)
{
	m_bHover = FALSE;

	Invalidate();
	
	return TRUE;
}

BOOL QYButton::SetImage(UINT uResID, int states)
{
	if (nullptr != m_pImage)
	{
		SAFE_DELETE(m_pImage);
	}

	m_pImage = new QYPicture(states);
	assert(nullptr != m_pImage);
	return m_pImage->Load(uResID);

	return TRUE;
}

BOOL QYButton::setImage(const char *imagePath,int state)
{
	if (nullptr == imagePath || 0 >= strlen(imagePath))
	{
		return FALSE;
	}

	if (nullptr != m_pImage)
	{
		SAFE_DELETE(m_pImage);
	}

	m_pImage = new QYPicture(state);
	assert(nullptr != m_pImage);
	return m_pImage->load(imagePath);

	return TRUE;
}

BOOL QYButton::SetImage(QYPicture *pImage)
{
	if (nullptr != m_pImage)
	{
		SAFE_DELETE(m_pImage);
	}
	m_pImage = pImage;
	Invalidate();

	return TRUE;
}

void QYButton::Enable(BOOL bEnable)
{
    m_bDisable = !bEnable;
    Invalidate();
}
