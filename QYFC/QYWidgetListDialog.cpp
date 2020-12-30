#include "QYWidgetListDialog.h"
#include "QYDC.h"
#include "QYLinearBitmap.h"

QYWidgetListDialog::QYWidgetListDialog() :QYDialog()
{
	m_ButtonPaneHeight = 40;
}

QYWidgetListDialog::~QYWidgetListDialog()
{
	for (QYWidgetList *p : m_listWidgetList){SAFE_DESTROY_WINDOW_PTR(p);}
}

QYButton* QYWidgetListDialog::AddButton(UINT nID, LPCTSTR szText)
{
    if (!m_enableBottomPane)
    {
        return nullptr;
    }

	QYButton *pButton = new QYButton;
	assert(nullptr != pButton);

	if (!pButton->Create(0, nullptr, szText, WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), this->GetHwnd()))
	{
		return nullptr;
	}

	pButton->SetCtrlID(nID);

	if (m_mapButton.end() != m_mapButton.find(nID))
	{
		return nullptr;
	}

	m_mapButton.insert(std::map<UINT, QYButton*>::value_type(nID, pButton));

	OnSize(0, 0, 0);

	return pButton;
}

QYButton* QYWidgetListDialog::addButton(UINT nID,const char *text)
{
    if (!m_enableBottomPane)
    {
        return nullptr;
    }

    QYButton *pButton = new QYButton;
    assert(nullptr != pButton);

    if (!pButton->Create(0, nullptr, L"", WS_CHILD | WS_VISIBLE, QYRect(0, 0, 0, 0), this->GetHwnd()))
    {
        return nullptr;
    }

    pButton->SetCtrlID(nID);

    if (m_mapButton.end() != m_mapButton.find(nID))
    {
        return nullptr;
    }

    m_mapButton.insert(std::map<UINT, QYButton*>::value_type(nID, pButton));

    pButton->setWindowText(text);

    OnSize(0, 0, 0);

    return pButton;
}

void QYWidgetListDialog::SetButtonText(UINT nID, LPCTSTR szText)
{
	auto it = m_mapButton.find(nID);
	if (m_mapButton.end() != it)
	{
		it->second->SetWindowText((TCHAR*)szText);
	}
}


void QYWidgetListDialog::setButtonText(UINT nID, const char *text)
{

    auto it = m_mapButton.find(nID);
    if (m_mapButton.end() != it)
    {
        it->second->setWindowText(text);
    }
}


QYButton* QYWidgetListDialog::GetButton(UINT nID)
{
	auto it = m_mapButton.find(nID);
	if (m_mapButton.end() != it)
	{
		it->second;
	}

	return nullptr;
}

void QYWidgetListDialog::DoDataExchange(QYDataExchange* pDX)
{
	QYDialog::DoDataExchange(pDX);
}


BOOL QYWidgetListDialog::OnInitDialog()
{
	QYDialog::OnInitDialog();

	CenterWindow();

	m_WidgetListTab.SetBorderWidth(0);
	if (!m_WidgetListTab.Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, 0, this->GetHwnd()))
	{
		return FALSE;
	}

	m_WidgetListTab.SetBorderWidth(0);
	m_WidgetListTab.SetItemHeight(0);

	OnSize(0, 0, 0);

	return TRUE;
}

LRESULT QYWidgetListDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (IDCANCEL == wParam)
		{
			EndDialog(0);
		}
	}
	break;
	case WM_NCDESTROY:
	{
		for (auto it = m_mapButton.begin(); it != m_mapButton.end(); it++)
		{
			SAFE_DESTROY_WINDOW_PTR(it->second);
		}
		m_mapButton.clear();
	}
	break;
	case WM_SIZE:{OnSize(wParam, LOWORD(lParam), HIWORD(lParam)); }break;
	case WM_PAINT:{return OnPaint(); }break;
	default:
		break;
	}
	return QYDialog::WindowProc(message, wParam, lParam);
}

LRESULT QYWidgetListDialog::OnPaint()
{
	QYPaintDC dc(this);


	QYRect WinRC;
	GetClientRect(WinRC);

	QYMemDC memDC(&dc, WinRC);

	QYDC *pDC = &memDC;

	QYBrush hClientBrush(QY_THEME.DLG_CLIENT_COLOR);
	QYBrush NullBrush(GetStockObject(NULL_BRUSH));
	auto oldbrush = (HBRUSH)pDC->SelectObject(&NullBrush);

	QYPen BorderPen(PS_SOLID, m_NCBorderWidth, m_bActive ? QY_THEME.DLG_BORDER_PEN_COLOR_ACTIVE : QY_THEME.DLG_BORDER_PEN_COLOR_UNACTIVE);

	auto oldpen = (HPEN)pDC->SelectObject(&BorderPen);

	auto borderWidth = 0;

	if (!m_bShowShadow)
	{
		pDC->Rectangle(0, 0, WinRC.Width(), WinRC.Height());
		borderWidth = m_NCBorderWidth;
	}


	QYRect rectCaption;
	rectCaption.left = borderWidth;
	rectCaption.top = borderWidth;
	rectCaption.right = WinRC.right - borderWidth;
	rectCaption.bottom = m_CaptionHeight + borderWidth;

	QYBrush CaptionBrush(QY_THEME.DLG_CAPTION_COLOR);
	//pDC->FillRect(rectCaption, CaptionBrush);

	QYLinearBitmap linearBitmap(LINEAR_RGB_ID_CAPTION, rectCaption.Width(), 0, TRUE);

	linearBitmap.Draw(pDC, &rectCaption);

	Button_state state2 = button_state_normal;
	if (m_ButtonState == m_ButtonDownState && m_ButtonState == bsClose)
	{
		state2 = button_state_down;
	}
	else
	{
		state2 = (bsClose == m_ButtonState) ? button_state_hot : button_state_normal;
	}

	DrawCloseRect(pDC, m_rectClose, state2);

	if (!m_disableMaxBox || !m_disableMinBox)
	{
		if (m_disableMaxBox)
		{
			state2 = button_state_disable;
		}
		else
		{
			if (m_ButtonState == m_ButtonDownState &&  m_ButtonState == bsMax)
			{
				state2 = button_state_down;
			}
			else
			{
				if (bsMax == m_ButtonState || bsRes == m_ButtonState)
				{
					state2 = button_state_hot;
				}
				else
				{
					state2 = button_state_normal;
				}
			}

		}

		DrawMaxRect(pDC, m_rectMax, !m_bZoom, state2);

		if (m_disableMinBox)
		{
			state2 = button_state_disable;
		}
		else
		{
			if (m_ButtonState == m_ButtonDownState&& m_ButtonState == bsMin)
			{
				state2 = button_state_down;
			}
			else
			{
				state2 = (bsMin == m_ButtonState) ? button_state_hot : button_state_normal;
			}
		}
		DrawMinRect(pDC, m_rectMin, state2);
	}

	if (nullptr != m_hIcon)
	{
		ICONINFO icoInfo;
		GetIconInfo(m_hIcon, &icoInfo);
		pDC->DrawIconEx(rectCaption.left + (rectCaption.Height() - icoInfo.xHotspot) / 2,
			rectCaption.left + (rectCaption.Height() - icoInfo.yHotspot) / 2,
			m_hIcon,
			icoInfo.xHotspot,
			icoInfo.yHotspot);

		::DeleteObject(icoInfo.hbmColor);
		::DeleteObject(icoInfo.hbmMask);

		rectCaption.left += icoInfo.xHotspot + 2;
	}

	HFONT oldFont = (HFONT)pDC->SelectObject(m_pFont);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_bActive ? QY_THEME.CAPTION_TEXT_COLOR_ACTIVE : QY_THEME.CAPTION_TEXT_COLOR_ACTIVE);


	rectCaption.left += m_FrameWidth;
	pDC->DrawText(m_strWindowText, m_strWindowText.Length(), &rectCaption, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);

	CaptionBrush.DeleteObject();
	BorderPen.DeleteObject();
	pDC->SelectObject(oldFont);

	QYRect rcClient = WinRC;
	rcClient.top = rectCaption.bottom;
	rcClient.left += borderWidth;
	rcClient.right -= borderWidth;
    rcClient.bottom -= borderWidth + (m_enableBottomPane?m_ButtonPaneHeight:0);

	pDC->FillRect(rcClient, hClientBrush);

    if (m_enableBottomPane)
    {
        rcClient.top = rcClient.bottom;
        rcClient.bottom += m_ButtonPaneHeight;
        pDC->FillSolidRect(rcClient, QY_THEME.DLG_BUTTON_PANE_BK_COLOR);
    }
	
	hClientBrush.DeleteObject();

	pDC->SelectObject(&oldpen);



	dc.BitBlt(0, 0, WinRC.Width(), WinRC.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();

	return TRUE;
}

BOOL QYWidgetListDialog::OnSize(UINT nType, int cx, int cy)
{
    QYRect rect;
    GetClientRect(rect);
    
	if (nullptr != m_WidgetListTab.GetHwnd())
	{
		m_WidgetListTab.MoveWindow(rect.left,
			rect.top,
			rect.Width(),
            rect.Height() - (m_enableBottomPane?m_ButtonPaneHeight:0));
	}

    if (m_enableBottomPane)
    {
        int buttonWidth = 70;
        int buttonCap = 30;
        int rightPad = 20;
        int buttonHeight = 26;
        int topPad = (m_ButtonPaneHeight - buttonHeight) / 2;
        QYRect rectButton;

        int i = 0;
        for (auto it = m_mapButton.begin(); it != m_mapButton.end(); it++, i++)
        {
            rectButton.left = i * (buttonWidth + buttonCap) + rect.right - rightPad - ((m_mapButton.size() - 1)*buttonCap) - (m_mapButton.size() * buttonWidth);
            rectButton.right = rectButton.left + buttonWidth;
            rectButton.bottom = rect.bottom - m_NCBorderHeight - topPad;
            rectButton.top = rectButton.bottom - buttonHeight;

            it->second->MoveWindow(rectButton);
        }
    }
	

	return TRUE;
}

QYWidgetList* QYWidgetListDialog::AddWidgetList(LPCTSTR strTitle)
{
	if (nullptr == m_WidgetListTab.GetHwnd())
	{
		return nullptr;
	}

	QYWidgetList *plist = new QYWidgetList(TRUE);
	plist->SetBorderWidth(0);
	if (!plist->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, 0, m_WidgetListTab.GetHwnd()))
	{
		SAFE_DELETE(plist);
		return nullptr;
	}


	m_WidgetListTab.InsertItem(m_WidgetListTab.GetItemCount(), plist, strTitle);
	m_listWidgetList.push_back(plist);

	if (1 < m_listWidgetList.size())
	{
		m_WidgetListTab.SetItemHeight(QY_THEME.DEFAULT_TAB_HEIGHT);
	}

	return plist;
}

void QYWidgetListDialog::UpdateChildWindowPos()
{
	if (nullptr != m_WidgetListTab.GetHwnd())
	{
		m_WidgetListTab.OnSize(0, 0, 0);
	}
}


QYWidgetList*  QYWidgetListDialog::ShowWidgetList(QYWidgetList *pWidgetList)
{
	return (QYWidgetList*)m_WidgetListTab.SelectItem(pWidgetList);
}