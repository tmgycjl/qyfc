#include "QYRectBackground.h"
#include "QYDC.h"


QYRectBackground::QYRectBackground(QYSize size)
{
	m_Size = size;
	m_pImage = nullptr;
}


QYRectBackground::~QYRectBackground()
{

}


BOOL QYRectBackground::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYRectBackground",
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

LRESULT QYRectBackground::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_PAINT:{return OnPaint(); }break;
	case WM_NCPAINT:{return TRUE; break; }
	case QY_WM_LISTCTRL_ITEM_RCLICK:
	case QY_WM_DATETABLE_SELECTED:
	case QY_WM_FOLDEREDIT_CHANGED:
	case WM_COMMAND:
	case QY_WM_LISTCTRL_ITEM_CLICK:
	case QY_WM_BUTTON_DOWN:
	case   QY_WM_EDIT_CHANGED:
	{
		::SendMessage(::GetParent(m_hWnd), message, wParam, lParam);
	}
	return TRUE;
	default:break;
	}

	return FALSE;
}

LRESULT QYRectBackground::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, m_colorBk);

	if (nullptr != m_pImage)
	{
		m_pImage->Draw(MemDc.GetDC(), rect.left, rect.top, rect.Width(), rect.Height(),
			0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}


BOOL QYRectBackground::SetImage(const char* strImageFileName)
{
	if (nullptr != m_pImage)
	{
		SAFE_DELETE(m_pImage);
	}

	if (nullptr != strImageFileName)
	{
		m_pImage = new QYPicture(IMAGE_STATUS_NORMAL);
		assert(nullptr != m_pImage);
		return m_pImage->load(strImageFileName);
	}

	return TRUE;
}
