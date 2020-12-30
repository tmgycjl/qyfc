#include "QYFolderEdit.h"
#include "QYDC.h"
#include "shlobj.h"

QYFolderEdit::QYFolderEdit() :QYWindow()
{
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bLBtDown = false;
	m_bFocus = FALSE;
	m_nClientPadWidth = 5;
	m_pButtonBackImage = nullptr;
	m_nBorderWidth = 1;
}

QYFolderEdit::~QYFolderEdit()
{
}


BOOL QYFolderEdit::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{

	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYFolderEdit",
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

	m_pButtonBackImage = new QYPicture;
	if (!m_pButtonBackImage->Load(IDP_FOLDER, QY_IMAGE_PNG))
	{
		SAFE_DELETE(m_pButtonBackImage);
	}

	OnSize(0,0);

	return ret;
}

LRESULT QYFolderEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
		switch (message)
		{
		case WM_NCDESTROY:
		{
			SAFE_DELETE(m_pButtonBackImage);
		}
		break;
		case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
		case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_ERASEBKGND:case WM_PAINT:{return OnPaint();break;}
		case WM_MOUSEMOVE:{return OnMouseMove(0, QYPoint(LOSHORT(lParam), HISHORT(lParam)));}break;
		case WM_MOUSELEAVE:{return OnMouseLeave(wParam, lParam);}
		case WM_SETFOCUS:{return  OnSetFocus();}break;
		case WM_SIZE:{OnSize(LOWORD(lParam), HIWORD(lParam));}break;
		case WM_KILLFOCUS:{LoseFocus();}break;
		default:break;
		}

	return 0L;
}


LRESULT QYFolderEdit::OnSetFocus()
{
	m_bFocus = TRUE;
	Invalidate();

	return  TRUE;
}

LRESULT QYFolderEdit::OnSize(int cx, int cy)
{
	return TRUE;
}

LRESULT QYFolderEdit::OnLButtonDown(UINT nFlags, QYPoint point)
{
	QYRect rc;
	GetClientRect(rc);

	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;

	if (rectArrow.PtInRect(point))
	{
		if (!m_bLBtDown)
		{
			m_bLBtDown = TRUE;
			Invalidate(FALSE);
		}
		
	}

	return TRUE;
}

LRESULT QYFolderEdit::OnLButtonUp(UINT nFlags, QYPoint point)
{
	QYRect rc;
	GetClientRect(rc);

	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;

	if (rectArrow.PtInRect(point))
	{
		if (m_bLBtDown)
		{
			m_bLBtDown = FALSE;
			

			BROWSEINFO bi = { 0 };
			TCHAR path[MAX_PATH];
			bi.hwndOwner = m_hWnd;
			QYString str;
			//str.LoadString( IDS_CHOOSE_FOLDER );
			str = L"";
			bi.lpszTitle = str;
			bi.pszDisplayName = path;
			bi.pidlRoot = NULL;
			bi.ulFlags = BIF_RETURNONLYFSDIRS;
			bi.lpfn = 0;
			LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

			TCHAR pPath[MAX_PATH];

			if (pidl)
			{
				SHGetPathFromIDList(pidl, pPath);
				m_strDir = pPath;

				IMalloc* imalloc = NULL;
				if (SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(pidl);
					imalloc->Release();
				}
			}

			::SendMessage(::GetParent(m_hWnd),QY_WM_FOLDEREDIT_CHANGED, 0, 0);

			Invalidate(FALSE);
		}

	}

	return TRUE;
}

LRESULT QYFolderEdit::LoseFocus()
{
	m_bFocus = FALSE;
	Invalidate();

	return TRUE;
}


LRESULT QYFolderEdit::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHot)
	{
		if (m_bHotButton)
		{
			m_bHotButton = FALSE;
		}
		m_bHot = FALSE;
		//m_selField = (DATETIME_FIELD)-1;
		Invalidate();
	}

	return TRUE;
}

LRESULT QYFolderEdit::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHot )
	{
		m_bHot = TRUE;

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = GetHwnd();
		TrackMouseEvent(&tme);

		Invalidate();
	}

	QYRect rc;
	GetClientRect(rc);
	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;

	if (rectArrow.PtInRect(point))
	{
		if (!m_bHotButton)
		{
			m_bHotButton = TRUE;
			InvalidateRect(&rectArrow);
		}
	}
	else
	{
		if (m_bHotButton)
		{
			m_bHotButton = FALSE;
			InvalidateRect(&rectArrow);
		}
	}

	return TRUE;
}

LRESULT QYFolderEdit::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	
	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}


LRESULT QYFolderEdit::OnPaint()
{
	QYPaintDC dc(this);
	QYPen PenLine;
	HPEN OldPen;
	HFONT OldFont;

	COLORREF corPen = RGB(135, 155, 200);

		if (m_bFocus)
		{
			corPen = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
		}
		else
		{
			if (m_bHot)
			{
				corPen = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
			}
			else
			{
				corPen = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
			}
		}
	
	PenLine.CreatePen(PS_SOLID, 1, corPen);
	OldPen = (HPEN)dc.SelectObject(&PenLine);

	QYBrush Brush;
	HBRUSH OldBrush;

	Brush.CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);
	
	OldBrush = (HBRUSH)dc.SelectObject(&Brush);

	QYRect rc;
	GetClientRect(rc);

	dc.Rectangle(rc);
 	Brush.DeleteObject();
 	PenLine.DeleteObject();

	OldFont = (HFONT)dc.SelectObject(m_pFont);

	int nOldMode = dc.SetBkMode(TRANSPARENT);
	COLORREF clrOld = dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	QYRect rectArrow = rc;
	rectArrow.right = rc.right - m_nBorderWidth;
	rectArrow.left = rc.right - rc.Height();
	rectArrow.bottom -= m_nBorderWidth;
	rectArrow.top += m_nBorderWidth;

	QYRect rcText = rc;
	rcText.left += m_nClientPadWidth;
	rcText.right = rectArrow.left - m_nClientPadWidth ;

	dc.DrawText(m_strDir, m_strDir.Length(), &rcText, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_PATH_ELLIPSIS);

		if (m_bHotButton)
		{
			COLORREF btnCor = QY_THEME.COMBOBOX_ARROW_BK_COLOR_HOVER;
			if (m_bLBtDown)
			{
				btnCor = QY_THEME.COMBOBOX_ARROW_BK_COLOR_DOWN;
			}

			HBRUSH  arrowBrush = CreateSolidBrush(btnCor);
			//HBRUSH oldBrush = (HBRUSH)dc.SelectObject( arrowBrush);
			dc.FillRect(rectArrow, arrowBrush);
			
			QYPoint pt;
			dc.MoveToEx(rectArrow.left, rectArrow.top, &pt);
			dc.LineTo(rectArrow.left, rectArrow.bottom);

			//dc.SelectObject(oldBrush);
			DeleteObject(arrowBrush);
		}

		if (nullptr != m_pButtonBackImage)
		{
			rectArrow.DeflateRect(2,2);
			m_pButtonBackImage->Draw(dc.GetDC(), rectArrow);
		}

	dc.SetTextColor(clrOld);
	dc.SetBkMode(nOldMode);
	dc.SelectObject(OldFont);

	return TRUE;
}

const TCHAR* QYFolderEdit::GetDir( )
{
	return  m_strDir;
}

void  QYFolderEdit::SetDir(LPCTSTR strDir)
{
	m_strDir = strDir;
	Invalidate();
}