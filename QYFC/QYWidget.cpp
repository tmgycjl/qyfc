#include "QYWidget.h"
#include "QYDC.h"


QYWidget::QYWidget(QYSize size)
{
	m_Size = size;
	m_pImage = nullptr;
}


QYWidget::~QYWidget()
{
	
}


BOOL QYWidget::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYWidget",
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

LRESULT QYWidget::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE; break; }
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
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
	case WM_RBUTTONDOWN:
	{
		struct w_funcCB_t *actionCB = getCallback(QY_CALLBACK_EVENT);
		if (nullptr != actionCB)
		{
			QYPropertyList properties;
			properties.addProperty("id", getID());
			properties.addProperty("action", "rbuttondown");
			actionCB->callback(&properties);
		}
	}
	break;
	default:break;
	}

	return FALSE;
}

LRESULT QYWidget::OnPaint()
{
	QYPaintDC dc(this);

	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	MemDc.FillSolidRect(rect, m_colorBk);

	if (nullptr != m_pImage)
	{
		int iconLeft = rect.left + (rect.Width() - m_pImage->GetWidth()) / 2;
		int iconTop = rect.top + (rect.Height() - m_pImage->GetHeight()) / 2;
		int desWidth = (rect.Width() >= m_pImage->GetWidth()) ? m_pImage->GetWidth() : rect.Width() - 2;
		int desHeight = (rect.Height() >= m_pImage->GetHeight()) ? m_pImage->GetHeight() : rect.Height() - 2;

		//m_pImage->Draw(MemDc.GetDC(), iconLeft, iconTop, desWidth, desHeight);
		m_pImage->Draw(MemDc.GetDC(), rect.left, rect.top, rect.Width(), rect.Height(),
			0, 0, m_pImage->GetWidth(),m_pImage->GetHeight());
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

LRESULT QYWidget::OnSize(UINT nType, int cx, int cy)
{
	
	return QYWindow::onSize(nType, cx, cy);
}



BOOL QYWidget::SetImage(const char* strImageFileName)
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
