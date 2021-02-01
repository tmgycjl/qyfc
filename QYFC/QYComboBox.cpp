#include "QYComboBox.h"

#include "QYDC.h"
LRESULT CALLBACK QYComboBox::ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QYComboBox *pWindow = (QYComboBox*)GetWindowLong(hWnd, GWLP_USERDATA);
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


QYComboBox::QYComboBox() :QYWindow()
{
	m_Flag = 0;
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bReadOnly = TRUE;
	m_pDropList = nullptr;
}

QYComboBox::~QYComboBox()
{
}

TCHAR* GetTrueTypeFont(HDC hDC, DWORD &nFontSize)
{
	//query font size
	nFontSize = GetFontData(hDC, 0, 0, NULL, 0);
	TCHAR * pFont = (TCHAR *)new TCHAR(nFontSize);
	if (pFont == NULL)
		return NULL;
	GetFontData(hDC, 0, 0, pFont, nFontSize);
	return pFont;
}


short readBigEndianWord(BYTE * p)
{
	return (p[0] << 8) + p[1];
}

struct NameRecord
{
	short platformID;
	short platformSpecificID;
	short languageID;
	short nameID;
	short length;
	short offset;
};

void readNameRecord(NameRecord * nr, BYTE * p)
{
	nr->platformID = readBigEndianWord(p);
	nr->platformSpecificID = readBigEndianWord(p + 2);
	nr->languageID = readBigEndianWord(p + 4);
	nr->nameID = readBigEndianWord(p + 6);
	nr->length = readBigEndianWord(p + 8);
	nr->offset = readBigEndianWord(p + 10);
}

int printEnglishName(const char* fontName,HDC dc)
{
	HFONT font = CreateFontA(
		900, 0,
		0, 0,
		40,
		0,
		FALSE,
		FALSE,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH,
		fontName
		);

	 dc = ::CreateCompatibleDC(NULL);
	SelectObject(dc, font);


	const DWORD cMaxNameTableSize = 1024 * 1024;
	DWORD bufferSize = GetFontData(dc, 0, 0, NULL, 0); // "name" backwards
	if (bufferSize == 0 || bufferSize == GDI_ERROR || bufferSize > cMaxNameTableSize)
		return 1;

	BYTE* buffer = (BYTE*)malloc(sizeof(BYTE)*bufferSize);
	if (GetFontData(dc, 'eman', 0, buffer, bufferSize) == GDI_ERROR)
		return 2;

	assert(bufferSize >= 6);

	short format = readBigEndianWord(buffer);
	short count = readBigEndianWord(buffer + 2);
	short stringOffset = readBigEndianWord(buffer + 4);
	assert(bufferSize >= stringOffset);

	BYTE* strings = buffer + stringOffset;
	NameRecord *nameRecords = (NameRecord*)malloc(sizeof(NameRecord)*count);

	UINT offset = 6;
	for (int i = 0; i < count; i++) {
		assert(bufferSize >= offset + 12);

		NameRecord *nr = &nameRecords[i];
		readNameRecord(nr, buffer + offset);
		offset += 12;

		if (nr->platformID == 1 && nr->platformSpecificID == 0 && nr->languageID == 0 && nr->nameID == 6)
		{
			char *name = (char*)malloc(nr->length + 1);
			memcpy(name, strings + nr->offset, nr->length);
			name[nr->length] = '\0';
		//	printf("font: %s, mac, name: %s\n", fontName, name);
			free(name);
		}
		else if (nr->platformID == 3 && nr->platformSpecificID == 1 && nr->languageID == 0x409 && nr->nameID == 1)
		{
			wchar_t *name = (wchar_t*)malloc(nr->length + 2);
			for (int i = 0; i < nr->length; i += 2)
			{
				name[i / 2] = (strings[nr->offset + i] << 8) + strings[nr->offset + i + 1];
			}
			name[nr->length / 2] = 0;

			char *nameAscii = (char*)malloc(nr->length / 2 + 1);
			WideCharToMultiByte(CP_ACP, 0, name, -1, nameAscii, nr->length / 2 + 1, NULL, NULL);
			nameAscii[nr->length / 2] = '\0';

			//std::cout << "font: " << fontName << ", windows, name: " << nameAscii << std::endl;

			free(nameAscii);
			free(name);
		}
	}

	free(nameRecords);
	free(buffer);


	DeleteDC(dc);
	DeleteObject(font);
	return 0;
}

 BOOL QYComboBox::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd )
{
	BOOL ret = QYWindow::Create(dwExStyle,
		L"QYComboBox",
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

	m_pDropList = new QYDropList;
	assert(nullptr != m_pDropList);

	return ret;
}

LRESULT QYComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CTLCOLORLISTBOX:
	{
		return (LRESULT)(HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	break;
	case WM_ERASEBKGND:
	{
		return TRUE;
		break;
	}
	case WM_PAINT:
	{
		return OnPaint();
	}
	break;
	case WM_NCPAINT:
	{
		return OnNCPaint();
		break;
	}
	case WM_NCDESTROY:
	{
		SAFE_DESTROY_WINDOW_PTR(m_pDropList);
		break;
	}
	case WM_MOUSEMOVE:
	{
		return OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		break;
	}
	case WM_MOUSELEAVE:
	{
		OnMouseLeave(wParam, lParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		return OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		PostMessage(WM_LBUTTONDOWN, wParam, lParam);

		return TRUE;
	}
	break;
	case WM_SETFOCUS:
	{
		//g_ConselLog.Print(0, "message:%04x\n", message);
		return OnSerFocus();
	}
	break;
	case WM_KILLFOCUS:
	{
		//g_ConselLog.Print(0, "message:%04x\n", message);
		return  LoseFocus();
	}
	break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case CB_SELECTSTRING:
	{
		if (!m_bReadOnly)
		{
			COMBOBOXINFO ComboBoxInfo;
			ComboBoxInfo.cbSize = sizeof(ComboBoxInfo);
			GetComboBoxInfo(m_hWnd, &ComboBoxInfo);
			::SetWindowText(ComboBoxInfo.hwndItem, &m_pDropList->GetCuselString(wParam));
		}
		else
		{
			SetCurSel(wParam);
		}

		::SendMessage(::GetParent(m_hWnd), CB_SELECTSTRING, wParam, 0);

		return TRUE;
	}
	break;
	default:
	{
		break;
	}
	}
	return 0L;
}

void QYComboBox::SetDropList(QYDropList *pDropList)
{
	m_pDropList = pDropList;
}

void QYComboBox::SetButtonRect(RECT rcButton)
{
	m_rectButton = rcButton;
}

BOOL QYComboBox::DeleteAll()
{
	return  (nullptr != m_pDropList) ? m_pDropList->DeleteAllItems() : FALSE;
}

int QYComboBox::AddString(LPCTSTR buf, DWORD_PTR data)
{ 
	if (nullptr == m_pDropList)
	{
		m_pDropList = new QYDropList;
	}

	return (nullptr != m_pDropList) ?  m_pDropList->AddItem(buf, data) :0;
}

int  QYComboBox::addString(const char *key, const char *text, DWORD_PTR data )
{
    if (nullptr == m_pDropList)
    {
        m_pDropList = new QYDropList;
    }

	return (nullptr != m_pDropList) ? m_pDropList->addItem(key, text, data) : 0;
}

DWORD_PTR  QYComboBox::GetLBData(int item)
{
	return (nullptr != m_pDropList) ? m_pDropList->GetItemData(item) : 0;
}

int QYComboBox::GetCurSel()
{
	return (nullptr != m_pDropList) ? m_pDropList->GetCurSel() : 0;
}

BOOL QYComboBox::SetCurSel(int nCursel)
{
	SetString(&m_pDropList->GetCuselString(nCursel));
	return (nullptr != m_pDropList) ? m_pDropList->SetCurSel(nCursel) : FALSE;
}



QYString QYComboBox::GetString()
{
	return m_strText;
}

void QYComboBox::SetItemHeight(int itemHeight)
{
	return  (nullptr != m_pDropList) ? m_pDropList->SetItemHeight(itemHeight) : 0;
}

int QYComboBox::GetCount()
{
	return  (nullptr != m_pDropList) ? m_pDropList->GetItemCount() : 0;
}

LRESULT QYComboBox::OnLButtonDown(UINT nFlags, QYPoint point)
{
	//g_ConsoleLog.Print("QYComboBox::OnLButtonDown\n");

	if (nullptr == m_pDropList)
	{
		m_pDropList = new QYDropList;
	}

	if (0 == m_pDropList->GetItemCount())
	{
		return TRUE;
	}

	if (nullptr == m_pDropList->GetHwnd())
	{
		if (!m_pDropList->Create(0, NULL, NULL, WS_CHILD, QYRect(0, 0, 0, 0),
			::GetDesktopWindow()))
		{
			return FALSE;
		}
	}

	

	if (m_pDropList->IsWindowVisible())
	{
		m_pDropList->Show(SW_HIDE);
	}
	else
	{
		//m_pDropList->SetCapture();
		QYRect rectWnd;
		GetWindowRect(rectWnd);
		m_pDropList->TrackPopup(QYPoint(rectWnd.left, rectWnd.bottom), rectWnd.Width(), TPDL_RIGHT_BOTTOM, this);
// 		
// 		COMBOBOXINFO ComboBoxInfo;
// 		ComboBoxInfo.cbSize = sizeof(ComboBoxInfo);
// 		GetComboBoxInfo(m_hWnd, &ComboBoxInfo);
	
		m_pDropList->Show(SW_SHOW);
	}

	return TRUE;
}

LRESULT QYComboBox::OnSerFocus()
{
	m_bFocus = TRUE;
	Invalidate();
	return TRUE;
}

LRESULT QYComboBox::LoseFocus()
{
	m_bFocus = FALSE;
	Invalidate();
	return TRUE;
}

LRESULT QYComboBox::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
// 	POINT pt;
// 	::GetCursorPos(&pt);

// 	QYRect rectWnd;
// 	GetWindowRect(rectWnd);
	//g_ConsoleLog.Print("WM_MOUSELEAVE:%04x\n", wparam);
	//if (!rectWnd.PtInRect(pt))
	{
		if (m_bHot)
		{
			//g_ConsoleLog.Print("WM_MOUSELEAVE:%04x\n", wparam);
			m_bHot = FALSE;
			Invalidate();
		}
	}
// 	QYPoint point(&pt);
// 
// 	ScreenToClient(&point);

	

	return TRUE;
}


LRESULT QYComboBox::OnMouseMove(UINT nFlags, QYPoint &point)
{
	if (!m_bHot)
	{
		m_bHot = TRUE;
		if (0 == nFlags)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 100;
			tme.hwndTrack = GetHwnd();
			TrackMouseEvent(&tme);
		}

		Invalidate();
	}

	return nFlags;
}

void QYComboBox::DrawArrow(HDC hDC, QYRect *rect)
{
	COLORREF color = QY_THEME.COMBOBOX_ARROW_PEN_COLOR_NORMAL;
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

	HPEN hSysBtnPen = CreatePen(PS_SOLID, QY_THEME.SYS_BOX_PEN_WIDTH, color);

	SelectObject(hDC, hSysBtnPen);

	RECT rectReal;
	int sideLen = min(rect->Height(), rect->Width())  / 2;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;
	rectReal.right = rectReal.left + sideLen;

	POINT pt;
	int top = rectReal.top + sideLen / 4;
	MoveToEx(hDC, rectReal.left, top +1, &pt);
	LineTo(hDC, rectReal.left + sideLen / 2, top +1 + sideLen / 2);
	LineTo(hDC, rectReal.left + sideLen + 1, top );

	DeleteObject(hSysBtnPen);
}

LRESULT QYComboBox::OnPaint()
{
	QYPaintDC dc(this);
	//g_ConselLog.Print(0, "OnPaint:\n");
	QYRect rect;
	GetClientRect(rect);		

	COLORREF pen_color = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
	if (m_bFocus)
	{
		pen_color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
	}
	else
	{
		if (m_bHot)
		{
			pen_color = QY_THEME.EDIT_BORDER_COLOR_FOCUS;
		}
		else
		{
			pen_color = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
		}
	}

	COLORREF brush_color = QY_THEME.BUTTON_BK_COLOR_NORMAL;
	if (m_bReadOnly)
	{
		if (!m_bHot)
		{
			brush_color = QY_THEME.EDIT_BK_COLOR_NORMAL;
		}
		else
		{
			brush_color = QY_THEME.EDIT_BK_COLOR_NORMAL;
		}
	}
	else
	{
		brush_color = QY_THEME.EDIT_BK_COLOR_NORMAL;
	}

	QYPen pen(PS_SOLID, 1, pen_color);

	HPEN oldpen = (HPEN)dc.SelectObject(&pen);

	QYBrush brush(brush_color);
	HBRUSH oldbrush = (HBRUSH)dc.SelectObject( &brush);

	dc.Rectangle(rect);	

	HFONT  hOldFont;
	dc.SetBkMode(TRANSPARENT);

	COLORREF oldColor = dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	hOldFont = (HFONT)dc.SelectObject(m_pFont);
	rect.left += 5;

	dc.DrawText(&m_strText, m_strText.Length(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	QYRect rectArrow = rect;
	rectArrow.right = rect.right - 1;
	rectArrow.left = m_rectButton.left;
	rectArrow.bottom -= 1;
	rectArrow.top += 1;

	if (m_bHot)
	{
		QYBrush arrowBrush(QY_THEME.COMBOBOX_ARROW_BK_COLOR_HOVER);
		dc.SelectObject(&arrowBrush);
		rect.left = rectArrow.left;
		dc.FillRect(rectArrow, arrowBrush);

		arrowBrush.DeleteObject();

		QYPoint pt;
		dc.MoveToEx(rectArrow.left, rectArrow.top, &pt);
		dc.LineTo(rectArrow.left, rectArrow.bottom);
	}

	DrawArrow(dc.GetDC(),&rectArrow);

	dc.SelectObject(oldbrush);
	dc.SelectObject(hOldFont);
	dc.SelectObject(oldpen);
	pen.DeleteObject();
	brush.DeleteObject();

	dc.SetTextColor(oldColor);

	return TRUE;
}

LRESULT QYComboBox::OnNCPaint()
{
	//g_ConselLog.Print(0, "OnNCPaint:\n");
	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);

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

	QYPen pen(PS_SOLID, 1, color);	

	HPEN oldpen = (HPEN)pDC->SelectObject(&pen);
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);		
	HBRUSH oldbrush = (HBRUSH)pDC->SelectObject(brush);

	pDC->Rectangle(0, 0, rect.Width(), rect.Height());		

	//::FillRgn(pDC->GetDC(),)
	pDC->SelectObject( oldbrush);
	pDC->SelectObject(oldpen);
	pen.DeleteObject();

	ReleaseDC(pDC);
	Invalidate();
	return TRUE;
}

BOOL QYComboBox::SubClassWindow(HWND hWnd)
{
	COMBOBOXINFO ComboBoxInfo;
	ComboBoxInfo.cbSize = sizeof(ComboBoxInfo);
	if (!GetComboBoxInfo(hWnd, &ComboBoxInfo))
	{
		return FALSE;
	}

	SetButtonRect(ComboBoxInfo.rcButton);

	return QYWindow::SubClassWindow(hWnd);
}



void QYComboBox::SetReadOnly(BOOL bReadOnly )
{
	m_bReadOnly = bReadOnly;
	Invalidate();
}


LRESULT QYComboBox::OnSize(UINT nType, int cx, int cy)
{
	COMBOBOXINFO ComboBoxInfo;
	ComboBoxInfo.cbSize = sizeof(ComboBoxInfo);
	if (!GetComboBoxInfo(m_hWnd, &ComboBoxInfo))
	{
		QYRect rect;
		GetClientRect(rect);
		m_rectButton.left = rect.right - rect.Height();
		m_rectButton.top = rect.top;
		m_rectButton.right = rect.right;
		m_rectButton.bottom = rect.bottom;
		return FALSE;
	}

	SetButtonRect(ComboBoxInfo.rcButton);

	return TRUE;
}
