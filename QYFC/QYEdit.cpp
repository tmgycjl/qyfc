#include "QYEdit.h"

#include "QYDC.h"

LRESULT CALLBACK QYEdit::EditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QYEdit *pWindow = (QYEdit*)GetWindowLong(hWnd, GWL_USERDATA);
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


QYEdit::QYEdit() :QYWindow()
{
	nMargin = 4;
	m_Flag = 1;
	m_bHot = FALSE;
	m_bFocus = FALSE;
	m_bInComboBox = FALSE;
	m_bPassword = FALSE;
	m_ptCursor.y = m_ptTxtBegin.y = 0;
	m_ptTxtBegin.x = m_ptCursor.x = nMargin;
	m_strEdit = L"";
	m_strSelect = L"";
	m_nCharPos = 0;
	m_nShowChar = 0;
	m_nMaxShowChars = 0;
	m_nSelectBegin = 0;
	m_nSelectEnd = 0;
	m_bLBtDown = false;
	m_bReadOnly = FALSE;
	m_ptFirst.x = m_ptFirst.y = 0;

	m_bFocus = FALSE;
	m_hCursor = nullptr;
	m_nShiftBegin = -1;

	m_nMaxChars = MAX_PATH;

	m_bTimerEnd = TRUE;
}

QYEdit::~QYEdit()
{
}

int  QYEdit::SetFlag(int flag)
{
	int last = m_Flag;
	m_Flag = flag;

	return last;
}

BOOL QYEdit::Create(DWORD dwExStyle,
	LPCTSTR lpClassName,
	LPCTSTR lpWndName,
	DWORD dwStyle,
	QYRect rect,
	HWND hParentWnd)
{
#if 1
	TCHAR szClassName[255] = { 0 };

	if (1 == m_Flag)
	{
		SafeSprintf(szClassName, L"QYEdit");
	}
	else
	{
		SafeSprintf(szClassName, L"EDIT");
	}
	
	BOOL ret = QYWindow::Create(dwExStyle,
		szClassName,
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

	OnSize(0,0);

#else
	m_hWnd = ::CreateWindowEx(0, L"EDIT",
		nullptr,
		WS_CHILD | WS_VISIBLE,
		rect.left,
		rect.top,
		rect.right,
		rect.bottom,
		hParentWnd,
		nullptr,
		QYApp::m_hInstance,
		nullptr);

	if (nullptr != m_hWnd)
	{
		SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
		SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_BORDER);
		SetFont((HFONT)QYApp::m_pMsDefultFont->m_hObject);
		m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (INT_PTR)QYEdit::EditProc);
	}
	return (nullptr != m_hWnd);
#endif
	
	
	return ret;
	
}

LRESULT QYEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (1 == m_Flag)
	{
		switch (message)
		{
		case WM_CHAR:{OnChar(wParam, lParam, m_bPassword);return OnPaint();}break;
		case WM_LBUTTONDBLCLK:{return OnLButtonDBClick(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_RBUTTONUP:return TRUE;break;
		case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
		case WM_ERASEBKGND:case WM_PAINT:{return OnPaint(); }break;
		case WM_MOUSEMOVE:{return OnMouseMove(0, QYPoint(LOSHORT(lParam), LOSHORT(lParam)));}break;
		case WM_MOUSELEAVE:{return OnMouseLeave(wParam, lParam); }break;
		case WM_SETFOCUS:{return  OnSetFocus();}break;
		case WM_SIZE:{OnSize(LOWORD(lParam), HIWORD(lParam));}break;
		case WM_KILLFOCUS:{LoseFocus();}break;
		case WM_KEYDOWN:{OnKeyDown(wParam, lParam);}break;
		case WM_SETCURSOR:return OnSetCursor(wParam, lParam); break;
		case WM_TIMER:OnTimer(wParam); break;
		default:break;
		}
	}
	else
	{
		switch (message)
		{
		case WM_GETDLGCODE:break;
		case WM_ERASEBKGND:
		{
			return OnEraseBackRound((HDC)wParam);
		}
		break;
		case WM_MOUSEMOVE:
		{
			OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));
		}
		break;
		case WM_SETFOCUS:
		{
			m_bFocus = TRUE;
			OnNCPaint();
		}
		break;
		case WM_KILLFOCUS:
		{
			m_bFocus = FALSE;
			OnNCPaint();
		}
		break;
		case WM_MOUSELEAVE:
		{
			OnMouseLeave(wParam, lParam);
		}
		break;
		case WM_NCPAINT:
		{
			break;
			OnNCPaint();
			return TRUE;
		}
		default:
		{
			break;
		}
		}
	}
	return 0L;
}


LRESULT QYEdit::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_UP:
	case VK_LEFT:
	{
		int nVirtKey = GetKeyState(VK_SHIFT);
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < 0)
			{
				if (m_strSelect.Length()>0)
				{
					if (m_nCharPos < m_nSelectEnd)
						m_nShiftBegin = m_nSelectEnd;
					else
						m_nShiftBegin = m_nSelectBegin;
				}
				else
				{
					m_nShiftBegin = m_nCharPos;
				}
			}
		}
		else
		{
			m_nShiftBegin = -1;
			m_strSelect = L"";
		}
		::HideCaret(m_hWnd);
		const TCHAR *cText = (const TCHAR *)&m_strEdit;
		if (m_nCharPos > 0)
		{
			if ((TCHAR)cText[m_nCharPos - 1] > 127)
			{
				m_nCharPos -= 2;
			}
			else
				m_nCharPos--;
		}
		while (m_nCharPos - m_nShowChar < 0)
		{
			if ((TCHAR)cText[m_nShowChar - 1]>127)
				m_nShowChar -= 2;
			else
				m_nShowChar--;
		}
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < m_nCharPos)
			{
				m_nSelectBegin = m_nShiftBegin;
				m_nSelectEnd = m_nCharPos;
			}
			else
			{
				m_nSelectBegin = m_nCharPos;
				m_nSelectEnd = m_nShiftBegin;
			}
			m_strSelect = m_strEdit.Mid(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
		}
		m_ptCursor.x = PosFromChar(m_nCharPos);
		Invalidate(FALSE);
		::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		::ShowCaret(m_hWnd);
		return TRUE;
	}
	case VK_DOWN:
	case VK_RIGHT:
	{
		int nVirtKey = GetKeyState(VK_SHIFT);
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < 0)
			{
				if (m_strSelect.Length()>0)
				{
					if (m_nCharPos < m_nSelectEnd)
						m_nShiftBegin = m_nSelectEnd;
					else
						m_nShiftBegin = m_nSelectBegin;
				}
				else
				{
					m_nShiftBegin = m_nCharPos;
				}
			}
		}
		else
		{
			m_nShiftBegin = -1;
			m_strSelect = L"";
		}
		::HideCaret(m_hWnd);
		const TCHAR *cText = (const TCHAR *)&m_strEdit;
		if (m_nCharPos < m_strEdit.Length())
		{
			if ((TCHAR)cText[m_nCharPos]>127)
			{
				m_nCharPos += 2;
			}
			else
				m_nCharPos++;
		}
		cText += m_nShowChar;
		while (m_nCharPos - m_nShowChar > m_nMaxShowChars)
		{
			if (((TCHAR)*cText) > 127 && ((TCHAR)*(cText + 1)) > 127)
			{
				m_nShowChar += 2;
				cText += 2;
			}
			else
			{
				m_nShowChar++;
				cText++;
			}
		}
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < m_nCharPos)
			{
				m_nSelectBegin = m_nShiftBegin;
				m_nSelectEnd = m_nCharPos;
			}
			else
			{
				m_nSelectBegin = m_nCharPos;
				m_nSelectEnd = m_nShiftBegin;
			}
			m_strSelect = m_strEdit.Mid(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
		}
		m_ptCursor.x = PosFromChar(m_nCharPos);
		Invalidate(FALSE);
		::ShowCaret(m_hWnd);
		::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		return TRUE;
	}
	case VK_HOME:
	{
		int nVirtKey = GetKeyState(VK_SHIFT);
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < 0)
			{
				m_nSelectEnd = m_nCharPos;
				m_nShiftBegin = m_nCharPos;
			}
			else
				m_nSelectEnd = m_nShiftBegin;
			m_nSelectBegin = 0;
			m_strSelect = m_strEdit.Mid(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
		}
		else
		{
			m_nShiftBegin = -1;
			m_strSelect = L"";
			m_nSelectBegin = m_nSelectEnd = 0;
		}
		::HideCaret(m_hWnd);
		m_nCharPos = 0;
		m_nShowChar = 0;
		m_ptCursor.x = PosFromChar(m_nCharPos);
		Invalidate(FALSE);
		::ShowCaret(m_hWnd);
		::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		return TRUE;
	}
	case VK_END:
	{
		int nVirtKey = GetKeyState(VK_SHIFT);
		if (nVirtKey&SHIFTED)
		{
			if (m_nShiftBegin < 0)
			{
				m_nSelectBegin = m_nCharPos;
				m_nShiftBegin = m_nCharPos;
			}
			else
				m_nSelectBegin = m_nShiftBegin;
			m_nSelectEnd = m_strEdit.Length();
			m_strSelect = m_strEdit.Mid(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
		}
		else
		{
			m_nShiftBegin = -1;
			m_strSelect = L"";
			m_nSelectBegin = m_nSelectEnd = 0;
		}
		::HideCaret(m_hWnd);
		m_nCharPos = m_strEdit.Length();
		//////////////////////////////////////////////////////////////
		const TCHAR *cText = (const TCHAR *)&m_strEdit;
		int nIndex = m_nCharPos - 1;
		int nShowChars = 0;
		while (nShowChars < m_nMaxShowChars)
		{
			if ((unsigned char)cText[nIndex]>127)
			{
				nIndex -= 2;
				nShowChars += 2;
			}
			else
			{
				nIndex -= 1;
				nShowChars += 1;
			}
		}
		//////////////////////////////////////////////////////////////
		m_nShowChar = m_nCharPos - nShowChars;//m_nMaxShowChars;
		if (m_nShowChar < 0)
			m_nShowChar = 0;
		m_ptCursor.x = PosFromChar(m_nCharPos);
		Invalidate(FALSE);
		::ShowCaret(m_hWnd);
		::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		return TRUE;
	}
	case VK_DELETE:
	{
		if (m_strEdit.Length()>0 && m_nCharPos < m_strEdit.Length())
		{
			const TCHAR *cText = (const TCHAR *)&m_strEdit;
			if ((TCHAR)cText[m_nCharPos] > 127)
			{
				m_strEdit.Delete(m_nCharPos, 2);
			}
			else
				m_strEdit.Delete(m_nCharPos);
			Invalidate(FALSE);
		}
		break;
	}
	default:
		break;
	}

	return TRUE;
}

LRESULT QYEdit::OnSetFocus()
{
	GetClientRect(m_rcWnd);

	::CreateCaret(m_hWnd, (HBITMAP)NULL, 1, m_rcWnd.Height() - 2);
	::ShowCaret(m_hWnd);
	
	m_nCharPos = m_strEdit.Length();
	m_ptCursor.x = PosFromChar(m_nCharPos);	
	::SetCaretPos(m_ptCursor.x, m_ptCursor.y);

	m_bFocus = TRUE;
	Invalidate(TRUE);
	return  TRUE;
}

LRESULT QYEdit::OnSize(int cx, int cy)
{
	GetClientRect(m_rcWnd);
	QYPaintDC dc(this);
	HFONT  OldFont = (HFONT)dc.SelectObject(m_pFont);
	dc.GetTextMetrics(&m_tm);
	QYSize TestSize = dc.GetTextExtent(L"1");
	m_ptTxtBegin.y = m_ptCursor.y = (m_rcWnd.Height() - TestSize.cy) / 2;
	dc.SelectObject(OldFont);
	m_nMaxShowChars = (m_rcWnd.Width() - 2 * nMargin) / TestSize.cx;

	return TRUE;
}

int  QYEdit::CharFromPos(int nPosX)
{
	QYString strTemp = m_strEdit.Mid(m_nShowChar);
	int nCharIndex = m_nShowChar;
	int nStringLen = strTemp.Length();
	TCHAR	TempChar[4];
	int nBeginPos = nMargin;
	QYSize TxtSize = 0;
	const TCHAR	*cText = (const TCHAR *)&strTemp;
	QYPaintDC dc(this);
	HFONT  OldFont = (HFONT)dc.SelectObject(m_pFont);
	while (nStringLen > 0)
	{
			memcpy(TempChar, cText, 1 * sizeof(TCHAR));
			TempChar[1] = 0;
			cText += 1;
			nStringLen -= 1;
			nCharIndex += 1;
			TxtSize = dc.GetTextExtent(TempChar, 1);

		if (nBeginPos + TxtSize.cx > nPosX)
		{
			if (nPosX - nBeginPos < nBeginPos + TxtSize.cx - nPosX)
				nCharIndex -= SafeStrlen(TempChar);
			break;
		}
		nBeginPos += TxtSize.cx;
	}
	dc.SelectObject(OldFont);
	return nCharIndex;
}

int QYEdit::PosFromChar(UINT nChar)
{
	if (nChar < m_nShowChar)
	{
		m_nShowChar = nChar;
		return nMargin;
	}
	else
	{
		QYString strTemp = m_strEdit.Mid(m_nShowChar, nChar - m_nShowChar);
		QYPaintDC dc(this);
		HFONT  OldFont = (HFONT)dc.SelectObject(m_pFont);
		QYSize TestSize = dc.GetTextExtent(&strTemp);
		dc.SelectObject(OldFont);
		return (nMargin + TestSize.cx);
	}
}

LRESULT QYEdit::OnLButtonDBClick(UINT nFlags, QYPoint point)
{
	m_strSelect = m_strEdit;
	m_nSelectBegin = 0;
	m_nSelectEnd = m_strEdit.Length() ;

	Invalidate(FALSE);

	return TRUE;
}

LRESULT QYEdit::OnLButtonDown(UINT nFlags, QYPoint point)
{
	
	::SetCapture(m_hWnd);
	m_nCharPos = CharFromPos(point.x);			// 从当前鼠标的位置获得对应字符的序号
	if (m_nCharPos > m_strEdit.Length())	// 修正 m_nCharPos ，确保其值不会超过字符串的长度
		m_nCharPos = m_strEdit.Length();

	m_ptCursor.x = PosFromChar(m_nCharPos);		// 设置光标的位置
	::SetCaretPos(m_ptCursor.x, m_ptCursor.y);

	m_bLBtDown = true;							// 设置右键按下标志位

	m_ptFirst = point;
	m_strSelect = L"";
	m_nSelectBegin = m_nSelectEnd = 0;
	m_nShiftBegin = m_nCharPos;//-1;

	if (m_hWnd != GetFocus())
	{
		::SetFocus(m_hWnd);
	}
	

	Invalidate(FALSE);

	return TRUE;
}

LRESULT QYEdit::OnLButtonUp(UINT nFlags, QYPoint point)
{
	m_bLBtDown = false;
	
	KillTimer(1);
	KillTimer(2);
	m_bTimerEnd = TRUE;
	ReleaseCapture();

	return TRUE;
}

void QYEdit::CopyString()
{
	if (m_bPassword)
	{
		return;
	}

	if (m_strSelect.Length() <= 0)
		return;
	if (OpenClipboard(nullptr))
	{
		EmptyClipboard();
		HGLOBAL hClipboardData;
		int size = (m_strSelect.Length() + 1)* sizeof(TCHAR);
		hClipboardData = GlobalAlloc(GMEM_MOVEABLE, size);

		TCHAR * pchData = (TCHAR*)GlobalLock(hClipboardData);
		ZeroMemory(pchData,  size);
		memcpy(pchData, m_strSelect.Tchar(), size);
		GlobalUnlock(hClipboardData);

		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		CloseClipboard();
	}
}

void QYEdit::PasteString()
{
	if (m_bPassword)
	{
		return;
	}

	if (OpenClipboard(m_hWnd))
	{
		if (::IsClipboardFormatAvailable(CF_TEXT) ||
			::IsClipboardFormatAvailable(CF_OEMTEXT))
		{
			::HideCaret(m_hWnd);
			HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
		
			TCHAR *pchData = (TCHAR*)GlobalLock(hClipboardData);
			if (m_strSelect.Length() > 0)
			{
				m_strEdit.Delete(m_nSelectBegin, m_strSelect.Length());
				m_nCharPos = m_nSelectBegin;
				m_strSelect = L"";
			}
			m_strSelect =  pchData;
			GlobalUnlock(hClipboardData);

			m_strEdit.Insert(m_nCharPos, &m_strSelect);
			m_strBackEdit.Insert(m_nCharPos, &m_strSelect);

			m_nCharPos += m_strSelect.Length();
			const TCHAR	*cText = (const TCHAR *)m_strEdit;
			cText += m_nShowChar;
			while (IsTextLargeThanWndRect())
			{
				if (((TCHAR)*cText) > 127 &&
					((TCHAR)*(cText + 1)) > 127)
				{
					m_nShowChar += 2;
					cText += 2;
				}
				else
				{
					m_nShowChar++;
					cText++;
				}
			}

			m_ptCursor.x = PosFromChar(m_nCharPos);
			Invalidate(FALSE);
			::ShowCaret(m_hWnd);
			::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
			m_strSelect = L"";
		}
		CloseClipboard();
	}
}

void QYEdit::CutString()
{
	if (m_bPassword)
	{
		return;
	}

	if (m_strSelect.Length() <= 0)
		return;
	m_strEdit.Delete(m_nSelectBegin, m_strSelect.Length());
	m_strBackEdit.Delete(m_nSelectBegin, m_strSelect.Length());

	CopyString();
	::HideCaret(m_hWnd);
	m_nCharPos = m_nSelectBegin;
	if (m_nShowChar > 0)
	{
		m_nShowChar -= m_strSelect.Length();
		if (m_nShowChar < 0)
			m_nShowChar = 0;
	}
	m_strSelect = L"";
	m_ptCursor.x = PosFromChar(m_nCharPos);
	Invalidate(FALSE);
	::ShowCaret(m_hWnd);
	::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
}

LRESULT QYEdit::OnChar(WPARAM wparam, LPARAM lparam, BOOL bPasswordFlag)
{
	int backChar = (int)wparam;
	
	if (!m_bReadOnly)
	{
		::CreateCaret(m_hWnd, (HBITMAP)NULL, 1, m_rcWnd.Height() - 2);
		switch (wparam)
		{
		case 8:	
		{
			if (m_strSelect.Length() > 0)
			{
				m_strEdit.Delete(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
				m_strBackEdit.Delete(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
				m_strSelect = L"";

				int maxCount = GetMaxShowCharCount();

				m_nCharPos = m_nSelectBegin;
				if (m_nCharPos > maxCount)
				{
					m_nShowChar = m_nCharPos - maxCount;
				}
				else
				{
					m_nShowChar = 0;
				}
				m_ptCursor.x = PosFromChar(m_nCharPos);

				m_nSelectBegin = m_nSelectEnd = 0;
				m_nShiftBegin = -1;
				Invalidate(FALSE);
			}
			else if (m_nCharPos > 0 && m_strEdit.Length() > 0)
			{
				const TCHAR *cText = (const TCHAR *)&m_strEdit;
				if ((TCHAR)cText[m_nCharPos - 1] > 127)
				{
					m_strEdit.Delete(m_nCharPos - 1, 1);
					m_strBackEdit.Delete(m_nCharPos - 1, 1);
					m_nCharPos -= 1;
				}
				else
				{
					m_strEdit.Delete(m_nCharPos - 1);
					m_strBackEdit.Delete(m_nCharPos - 1);
					m_nCharPos--;
				}
				if (m_nShowChar > 0 && (TCHAR)cText[m_nShowChar - 1] > 127)
					m_nShowChar -= 1;
				else
					m_nShowChar--;

				if (m_nShowChar < 0)
					m_nShowChar = 0;
				m_ptCursor.x = PosFromChar(m_nCharPos);
				Invalidate(FALSE);
			}
		}
		break;
		default:
		{
			if (backChar < 32)
			{
				switch (backChar)
				{
				case 3:
				{
					CopyString();
				}
				break;
				case 22:
				{
					PasteString();
				}
				break;
				case 24:
				{
					CutString();
				}
				}
			}
			else
			{
				

				TCHAR nChar = (TCHAR)wparam;
				if (bPasswordFlag)
				{
					nChar = 9679;
				}
				
				if (m_strSelect.Length() > 0)
				{
					m_strEdit.Delete(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);
					m_strBackEdit.Delete(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);

					m_strSelect = L"";
					m_nCharPos = m_nSelectBegin;
				}

				m_nSelectBegin = m_nSelectEnd = 0;
				m_nShiftBegin = -1;

				if (m_nMaxChars <= m_strEdit.Length())
				{
					::ShowCaret(m_hWnd);
					::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
					return FALSE;
				}

				m_strEdit.Insert(m_nCharPos, nChar);
				m_strBackEdit.Insert(m_nCharPos, backChar);

				m_nCharPos++;
				if (nChar > 127)
				{
					
						while (IsTextLargeThanWndRect())
						{
							m_nShowChar++;
						}
					
						m_ptCursor.x = PosFromChar(m_nCharPos);
						Invalidate(FALSE);
					
				}
				else
				{
					const TCHAR	*cText = (const TCHAR *)&m_strEdit;
					cText += m_nShowChar;

					while (IsTextLargeThanWndRect())
					{
						if (((TCHAR)*cText) > 127 &&
							((TCHAR)*(cText + 1)) > 127)
						{
							m_nShowChar += 1;
							cText += 1;
						}
						else
						{
							m_nShowChar++;
							cText++;
						}
					}
					m_ptCursor.x = PosFromChar(m_nCharPos);
					Invalidate(FALSE);
				}
			}

		}

		break;
		}
		::ShowCaret(m_hWnd);
		::SetCaretPos(m_ptCursor.x, m_ptCursor.y);

        NotifyMessage(QY_WM_EDIT_CHANGED, 0, 0);
	}

	return FALSE;
}

BOOL  QYEdit::IsTextLargeThanWndRect()
{
	auto  strTemp = m_strEdit.Right(m_strEdit.Length() - m_nShowChar);
	QYPaintDC dc(this);
	auto  OldFont = (HFONT)dc.SelectObject(m_pFont);
	auto TestSize = dc.GetTextExtent(&strTemp);
	m_ptTxtBegin.y = m_ptCursor.y = (m_rcWnd.Height() - TestSize.cy) / 2;
	dc.SelectObject(OldFont);

	if (TestSize.cx >( m_rcWnd.Width() - 2 * nMargin))
	{
		return TRUE;
	}

	return FALSE;
}

LRESULT QYEdit::LoseFocus()
{
	::HideCaret(m_hWnd);
	::DestroyCaret();
	m_bFocus = FALSE;
	Invalidate(TRUE);
	//(1 == m_Flag) ? OnPaint() : OnNCPaint();
	return TRUE;
}

BOOL  QYEdit::SetReadOnly(BOOL bReadOnly )
{ 
	if (0 == m_Flag)
	{
		UINT style = ::GetWindowLong(m_hWnd, GWL_STYLE);
		bReadOnly ? style |= ES_READONLY : style &= ~ES_READONLY;

		::SetWindowLong(m_hWnd, GWL_STYLE, style);
		::PostMessageW(m_hWnd, EM_SETREADONLY, bReadOnly ? 1 : 0, 0);
	}
	BOOL temp = m_bReadOnly; 
	m_bReadOnly = bReadOnly; 
	Invalidate(); 
	return temp; 
}

BOOL  QYEdit::SetInComboBox(BOOL bInComboBox)
{
	BOOL temp = m_bInComboBox;
	m_bInComboBox = bInComboBox;
	return temp;
}

LRESULT QYEdit::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHot)
	{
		m_bHot = FALSE;
		!m_bReadOnly ? Invalidate() : 0;
		if (m_bInComboBox)
		{
			//::SendMessage(::GetParent(m_hWnd), WM_MOUSELEAVE, 1, lparam);
			//::SendMessage(::GetParent(m_hWnd), WM_MOUSEMOVE, MAKELONG(po), (LPARAM)this);
		}
		m_hCursor = LoadCursor(NULL, IDC_ARROW);
		PostMessage(WM_SETCURSOR, 0, 0);
	}

	return TRUE;
}

LRESULT QYEdit::OnMouseMove(UINT nFlags, QYPoint point)
{
	if(1 == m_Flag)
	{

	//if (!m_bReadOnly)
	{
		if (m_bLBtDown)
		{
			::HideCaret(m_hWnd);			
			if (m_ptFirst.x > point.x)
			{
				if (point.x <0 && m_nShowChar > 0)
				{
					if (m_bTimerEnd)
					{
						m_bTimerEnd = FALSE;
						SetTimer(1, 100, nullptr);
					}
					
					return FALSE;
				}
				else
				{
					m_nSelectBegin = CharFromPos(point.x);
					m_nSelectEnd = CharFromPos(m_ptFirst.x);
				}
			}
			else
			{
				if (0xffff >= point.x && point.x > m_rcWnd.Width() )
				{
					::ShowCaret(m_hWnd);
					if (m_nCharPos < m_strEdit.Length() )
					{
						if (m_bTimerEnd)
						{
							m_bTimerEnd = FALSE;
							SetTimer(2, 100, nullptr);
						}

						return FALSE;
					}
				}
				else
				{
					m_nSelectBegin = CharFromPos(m_ptFirst.x);
					m_nSelectEnd = CharFromPos(point.x);
				}
				
			}

			if (m_nSelectEnd > m_strEdit.Length())
				m_nSelectEnd = m_strEdit.Length();
			else if (m_nSelectEnd < 0)
				m_nSelectEnd = 0;

			if (m_nSelectBegin > m_strEdit.Length())
				m_nSelectBegin = m_strEdit.Length();
			else if (m_nSelectBegin < 0)
				m_nSelectBegin = 0;

			if (m_nSelectEnd - m_nSelectBegin>0)
				m_strSelect = m_strEdit.Mid(m_nSelectBegin, m_nSelectEnd - m_nSelectBegin);

			m_nCharPos = CharFromPos(point.x);
			if (m_nCharPos<0)
				m_nCharPos = 0;
			else if (m_nCharPos>m_strEdit.Length())
				m_nCharPos = m_strEdit.Length();
			m_ptCursor.x = PosFromChar(m_nCharPos);
			Invalidate(FALSE);

			::ShowCaret(m_hWnd);
			::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		}
	}
	}
	if (!m_bHot )
	{
		m_bHot = TRUE;

		!m_bReadOnly ? OnNCPaint() : 0;

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = GetHwnd();
		TrackMouseEvent(&tme);
		!m_bReadOnly ? Invalidate() : 0;

		m_hCursor = LoadCursor(NULL, IDC_IBEAM);
		PostMessage(WM_SETCURSOR, 0, 0);
	}

	return TRUE;
}

LRESULT QYEdit::OnEraseBackRound(HDC hDC)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	HBRUSH Brush;
	if (m_bReadOnly)
	{
		Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_READONLY);
	}
	else
	{
		Brush = CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);
	}

	FillRect(hDC,&rect, Brush);

	::DeleteObject(Brush);

	return TRUE;
}

LRESULT QYEdit::OnPaint()
{
	QYPaintDC dc(this);
	QYPen PenLine;
	HPEN OldPen;
	HFONT OldFont;

	COLORREF corPen = RGB(135, 155, 200);

	if (m_bReadOnly)
	{
		corPen = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
	}
	else
	{
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
	}
	
	PenLine.CreatePen(PS_SOLID, 1, corPen);
	OldPen = (HPEN)dc.SelectObject(&PenLine);

	QYBrush Brush;
	HBRUSH OldBrush;

	if (m_bReadOnly)
	{
		Brush.CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);
	}
	else
	{
		if (m_bFocus)
		{
			Brush.CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);
		}
		else
		{
			Brush.CreateSolidBrush(QY_THEME.EDIT_BK_COLOR_NORMAL);
		}

	}

	OldBrush = (HBRUSH)dc.SelectObject(&Brush);

	dc.Rectangle(m_rcWnd);

 	Brush.DeleteObject();
 	PenLine.DeleteObject();

	OldFont = (HFONT )dc.SelectObject(m_pFont);
	int nOldMode = dc.SetBkMode(TRANSPARENT);
	COLORREF clrOld = dc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);

	if (m_strEdit.Length() > 0)	
	{
		QYString strTemp;

		strTemp = m_strEdit.Right(m_strEdit.Length() - m_nShowChar);
		
		dc.TextOut(m_ptTxtBegin.x, m_ptTxtBegin.y, &strTemp);
	}

#if 1

	if (m_strSelect.Length() > 0)
	{
		BLENDFUNCTION bf;
		int nBeginPosX, nEndPosX;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = 0;
		bf.SourceConstantAlpha = 100;

		if (m_nSelectBegin < m_nShowChar)
			nBeginPosX = PosFromChar(m_nShowChar);
		else
			nBeginPosX = PosFromChar(m_nSelectBegin);
		nEndPosX = PosFromChar(m_nSelectEnd);
		HDC MemDC;
		HBITMAP BmpMask, pOldBmp;
		MemDC = ::CreateCompatibleDC(dc.GetDC());
		BmpMask = ::CreateCompatibleBitmap(dc.GetDC(), nEndPosX - nBeginPosX, m_tm.tmHeight);
		pOldBmp = (HBITMAP)::SelectObject(MemDC,BmpMask);
		RECT rt;
		rt.left = 0;
		rt.top = 0;
		rt.right = nEndPosX - nBeginPosX;
		rt.bottom = m_tm.tmHeight;
		HBRUSH selBrush = ::CreateSolidBrush(QY_THEME.EDIT_SELECT_COLOR);
		::FillRect(MemDC, &rt, selBrush);
		AlphaBlend(dc.GetDC(), nBeginPosX, m_ptTxtBegin.y, nEndPosX - nBeginPosX, m_tm.tmHeight,
			MemDC, 0, 0, nEndPosX - nBeginPosX, m_tm.tmHeight, bf);
		::SelectObject(MemDC,pOldBmp);
		::DeleteDC(MemDC);
		DeleteObject(selBrush);
		DeleteObject(BmpMask);
	}
#endif

	dc.SetTextColor(clrOld);
	dc.SetBkMode(nOldMode);
	dc.SelectObject(OldFont);

	return TRUE;
}

LRESULT QYEdit::OnNCPaint()
{
	if (m_bInComboBox)
	{
		return TRUE;
	}

	QYDC *pDC = GetWindowDC();

	QYRect rect;
	GetWindowRect(rect);		
	QYRect rectClient;
	GetClientRect(rectClient);
	int offsetX = (rect.Width() - rectClient.Width()) / 2;
	int offsetY	 = (rect.Height() - rectClient.Height()) / 2;

	rectClient.OffsetRect(offsetX, offsetY);

	COLORREF color = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
	if (m_bReadOnly)
	{
		color = QY_THEME.EDIT_BORDER_COLOR_NORMAL;
	}
	else
	{
		int ret = ExcludeClipRect(pDC->GetDC(),
			rectClient.left,
			rectClient.top,
			rectClient.right,
			rectClient.bottom);

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
	}

	QYPen pen(PS_SOLID, 1, color);	

	HPEN oldpen = (HPEN)pDC->SelectObject( &pen);
	QYBrush brush;
	brush.CreateSolidBrush(!m_bReadOnly ? QY_THEME.EDIT_BK_COLOR_NORMAL : QY_THEME.CLIENT_COLOR);
	HBRUSH oldbrush = (HBRUSH)pDC->SelectObject( &brush);

	pDC->Rectangle( 0, 0, rect.right - rect.left , rect.bottom - rect.top);			

	pDC->SelectObject( oldbrush);
	pDC->SelectObject( oldpen);

	pen.DeleteObject();
	brush.DeleteObject();

	ReleaseDC(pDC);

	return TRUE;
}

BOOL QYEdit::SubClassWindow(HWND hWnd)
{
	UINT style = GetWindowLong(hWnd, GWL_STYLE);
	if (style&ES_READONLY)
	{
		SetReadOnly(TRUE);
	}

	//m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (INT_PTR)QYEdit::EditProc);

	return QYWindow::SubClassWindow(hWnd);
}

BOOL  QYEdit::SetPasswordFlag(BOOL bPasswordFlag)
{
	BOOL ret = m_bPassword;
	m_bPassword = bPasswordFlag;
	Invalidate();

	return ret;
}

TCHAR* QYEdit::GetText()
{
	if (m_bPassword)
	{
		return &m_strBackEdit;
	}
	else
	{
		return  &m_strEdit;
	}
}

int QYEdit::GetText(QYString &strText, int size)
{
	if (m_bPassword)
	{
		strText = m_strBackEdit;
	}
	else
	{
		strText = m_strEdit;
	}

	return strText.Length();
}

void QYEdit::SetText(const TCHAR *lpString)
{
	m_strEdit = lpString;
	m_strBackEdit = lpString;
	if (m_bPassword)
	{
		int len = m_strEdit.Length();
		for (int i = 0; i < len; i++)
		{
			m_strEdit[i] = 9679;
		}
	}
	
	Invalidate();
}

void QYEdit::setText(const char *text)
{
	int dstSize = (strlen(text) + 1)*sizeof(TCHAR);
	TCHAR *dstText = new TCHAR[dstSize];
	memset(dstText, 0, dstSize);

	QYString::SafeUTF82W(dstText, dstSize, text);

	m_strEdit = dstText;
	m_strBackEdit = dstText;
	if (m_bPassword)
	{
		int len = m_strEdit.Length();
		for (int i = 0; i < len; i++)
		{
			m_strEdit[i] = 9679;
		}
	}

	delete[] dstText;

	Invalidate();
}

BOOL QYEdit::getText(std::string &text)
{
	TCHAR *pStr = nullptr;
	if (m_bPassword)
	{
		pStr= &m_strBackEdit;
	}
	else
	{
		pStr=  &m_strEdit;
	}

	if (MAX_WINDOW_TEXT_LENGTH - 1 < SafeStrlen(pStr)*sizeof(TCHAR))
	{
		return FALSE;
	}

	char dstText[MAX_WINDOW_TEXT_LENGTH] = { 0 };

	QYString::SafeW2AUTF8(dstText, MAX_WINDOW_TEXT_LENGTH, pStr);

	text = dstText;

	return TRUE;
}

LRESULT QYEdit::OnSetCursor(WPARAM wparam, LPARAM lparam)
{
	if (nullptr != m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return FALSE;
}


int  QYEdit::GetMaxShowCharCount()
{
	QYPaintDC dc(this);
	auto  OldFont = (HFONT)dc.SelectObject(m_pFont);
	int maxCount = 0;
	QYSize TestSize = 0;
	int startIndex = m_strEdit.Length();

	do
	{
		TestSize = dc.GetTextExtent(m_strEdit.Right(m_strEdit.Length() - startIndex));
		maxCount = m_strEdit.Length() - startIndex;
		startIndex--;
	} while (startIndex >= 0 && TestSize.cx < (m_rcWnd.Width() - 2 * nMargin));
	
	if (TestSize.cx > (m_rcWnd.Width() - 2 * nMargin))
	{
		maxCount--;
	}

	dc.SelectObject(OldFont);

	return maxCount;
}

void    QYEdit::SetMaxChars(short maxChars)
{
	m_nMaxChars = maxChars;
}

void QYEdit::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		if (m_nCharPos > 0)
		{
			m_nCharPos--;
			m_nShowChar--;
			m_ptCursor.x = PosFromChar(m_nCharPos);
			m_nSelectBegin = m_nShowChar;

			Invalidate(FALSE);

			::ShowCaret(m_hWnd);
			::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		}
		else
		{
			KillTimer(1);
			m_bTimerEnd = TRUE;
		}
	}
	else if (2 == nIDEvent)
	{
		if (m_nCharPos < m_strEdit.Length())
		{
			m_nCharPos++;
			m_nShowChar++;
			m_ptCursor.x = PosFromChar(m_nCharPos);
			m_nSelectEnd = m_nCharPos;

			Invalidate(FALSE);
			::ShowCaret(m_hWnd);
			::SetCaretPos(m_ptCursor.x, m_ptCursor.y);
		}
		else
		{
			KillTimer(2);
			m_bTimerEnd = TRUE;
		}
	
	}
}