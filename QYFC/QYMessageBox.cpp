#include "QYMessageBox.h"
#include "QYButton.h"
#include "QYStatic.h"
#include "QYDC.h"
#include "QYLanguage.h"
#if 0
QYMessageBox::QYMessageBox(const char *strCaption, const char *strMessage, UINT nFlag)
{
	m_strCaption = strCaption;
	m_strMessage = strMessage;
	m_nFlag = nFlag;
}

QYMessageBox::~QYMessageBox()
{
}

BOOL QYMessageBox::OnInitDialog()
{
	QYDialog::OnInitDialog();

	setWindowText(m_strCaption.c_str());

	if (MB_OK == m_nFlag)
	{
		addButton(QY_BUTTON_FIRST_ID, "OK");
	}
	else if (MB_OKCANCEL == m_nFlag)
	{
		addButton(QY_BUTTON_FIRST_ID, "OK");
		addButton(QY_BUTTON_FIRST_ID + 1, "CANCEL");
	}
	int iconWidth = 48;
	int padWidth = 20;

	QYPicture *pic = new QYPicture;
	assert(nullptr != pic);
	pic->Load(IDP_WARNING, L"PNG");
	if (0 < pic->GetWidth())
	{
		iconWidth = pic->GetWidth();
	}

	QYWidgetList *pWidgetList = AddWidgetList(L"");

	QYWidget *pWidget = pWidgetList->Append(QYSize(0,0));

	QYButton *pButton = (QYButton*)pWidget->Append(QY_CONTROL_TYPE_BUTTON, 
		QYRelative(0.0, 0.0), QYRelative(0.0, 1.0),
		QYSize(padWidth, 0), QYSize(padWidth + iconWidth, 0));

	if (nullptr != pButton)
	{
		pButton->SetImage(pic);
	}

	QYStatic *pStatic = (QYStatic*)pWidget->Append(QY_CONTROL_TYPE_STATIC,
		QYRelative(0.0, 0.0), QYRelative(1.0, 1.0),
		QYSize(iconWidth + 2 * padWidth, 0), QYSize(-padWidth, 0));
	if (nullptr != pStatic)
	{
		pStatic->setWindowText(m_strMessage.c_str());
		QYRect rectWindow;
		GetWindowRect(rectWindow);

		QYRect rectClient;
		GetDialogClientRect(rectClient);

		QYPaintDC dc(pStatic);

		HFONT oldFont = (HFONT)dc.SelectObject(pStatic->GetFont());


		QYSize sizeMsg = dc.GetTextExtent(m_strWindowText);
		int diff = sizeMsg.cx + 20 - rectClient.Width() + 88;

		dc.SelectObject(oldFont);

		auto cY = GetSystemMetrics(SM_CYSCREEN);
		auto cX = GetSystemMetrics(SM_CXSCREEN);

		int offsetX = diff / 2;
		if (240 > rectWindow.Width() + diff)
		{
			diff = 240 - rectWindow.Width();
		}
		else if (cX < rectWindow.Width() + diff)
		{
			diff = cX - rectWindow.Width();
		}

		SetWindowPos(nullptr, rectWindow.left - offsetX, rectWindow.top, rectWindow.Width() + diff, rectWindow.Height());
	}

	ShowWidgetList(pWidgetList);

	UpdateChildWindowPos();

	return FALSE;
}


LRESULT QYMessageBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (QY_BUTTON_FIRST_ID + 1 == wParam)
		{
			EndDialog(IDCANCEL);
		}
		else if (QY_BUTTON_FIRST_ID == wParam)
		{
			EndDialog(IDOK);
		}
	}
	break;
	case WM_SIZE:
	{

	}
	break;
	default:
		break;
	}
	return QYDialog::WindowProc(message, wParam, lParam);
}


#else
QYMessageBox::QYMessageBox(const char *strCaption, const char *strMessage, UINT nFlag)
{
	m_strCaption = strCaption;
	m_strMessage = strMessage;
	m_nFlag = nFlag;
}


QYMessageBox::~QYMessageBox()
{
}

BOOL QYMessageBox::OnInitDialog()
{
	QYDialog::OnInitDialog();

	setWindowText(m_strCaption.c_str());
	
	int iconWidth = 48;
	int padWidth = 20;

	QYPicture *pic = new QYPicture;
	assert(nullptr != pic);
	pic->Load(IDP_WARNING, L"PNG");
	if (0 < pic->GetWidth())
	{
		iconWidth = pic->GetWidth();
	}

    QYWidget *pWidget = (QYWidget*)Append("", QY_CONTROL_TYPE_WIDGET, 
        QYRelative(0.0, 0.0), QYRelative(1.0, 1.0), QYSize(0, 0), QYSize(0, -48));

	QYButton *pButton = (QYButton*)pWidget->Append("image",QY_CONTROL_TYPE_BUTTON,
		QYRelative(0.0, 0.0), QYRelative(0.0, 1.0),
		QYSize(m_nBorderWidth + padWidth, 0), QYSize(m_nBorderWidth + padWidth + iconWidth, 0));

	if (nullptr != pButton)
	{
		pButton->SetImage(pic);
		iconWidth = pic->GetWidth();
	}

    int textLeft = m_nBorderWidth + padWidth + iconWidth + padWidth;

	QYStatic *pStatic = (QYStatic*)pWidget->Append("msg", QY_CONTROL_TYPE_STATIC,
		QYRelative(0.0, 0.0), QYRelative(1.0, 1.0),
		QYSize(textLeft, 0), QYSize(-padWidth, 0));

    pWidget = (QYWidget*)Append("oper_widget", QY_CONTROL_TYPE_WIDGET,
        QYRelative(0.0, 1.0), QYRelative(1.0, 1.0), QYSize(0, -48), QYSize(0, 0));

    if (MB_OK == m_nFlag)
    {
        m_buttonOK = (QYButton *)pWidget->Append("search", QY_CONTROL_TYPE_BUTTON,
            QYRelative(0.5, 1.0), QYRelative(0.5, 1.0), QYSize(-40, -36), QYSize(40, -12));
        m_buttonOK->setWindowText("OK");
    }
    else if (MB_OKCANCEL == m_nFlag)
    {
        m_buttonOK = (QYButton *)pWidget->Append("search", QY_CONTROL_TYPE_BUTTON,
            QYRelative(0.5, 1.0), QYRelative(0.5, 1.0), QYSize(-120, -36), QYSize(-40, -12));
        m_buttonOK->setWindowText("OK");

        m_buttonCancel = (QYButton *)pWidget->Append("add", QY_CONTROL_TYPE_BUTTON,
            QYRelative(0.5, 1.0), QYRelative(0.5, 1.0), QYSize(40, -36), QYSize(120, -12));
        m_buttonCancel->setWindowText("CANCEL");
    }

	if (nullptr != pStatic)
	{
		pStatic->setWindowText(m_strMessage.c_str());

		QYRect rectWindow;
		GetWindowRect(rectWindow);

		QYRect rectClient;
		GetClientRect(rectClient);

		QYPaintDC dc(pStatic);

		HFONT oldFont = (HFONT)dc.SelectObject(pStatic->GetFont());

		QYString strMsg;
		const char *msg = QYLanguage::instance()->getText(m_strMessage.c_str());

		SET_TEXT(strMsg, msg);
		QYSize sizeMsg = dc.GetTextExtent(strMsg);
		int diffX = textLeft + sizeMsg.cx + padWidth + m_nBorderWidth + padWidth;

		dc.SelectObject(oldFont);

		auto cfsX = GetSystemMetrics(SM_CXFULLSCREEN);
		auto cfsY = GetSystemMetrics(SM_CYFULLSCREEN);

		int offsetX = diffX / 2;
		
		if (240 > rectWindow.Width() + diffX)
		{
			diffX = 240 - rectWindow.Width();
		}
		else if (cfsX < rectWindow.Width() + diffX)
		{
			diffX = cfsX - rectWindow.Width();
		}

		int diffY = 0;
		if (160 > rectWindow.Height())
		{
			diffY = 160 - rectWindow.Height();
		}

		int dstWidth = rectWindow.Width() + diffX;
		int dstHeight = rectWindow.Height() + diffY;

		SetWindowPos(nullptr, (cfsX - dstWidth) / 2, (cfsY - dstHeight) / 2, dstWidth, dstHeight);

	}


	return FALSE;
}


LRESULT QYMessageBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
        if (nullptr != m_buttonCancel && m_buttonCancel->GetHwnd() == (HWND)lParam)
		{
			EndDialog(IDCANCEL);
		}
        else  if (nullptr != m_buttonOK && m_buttonOK->GetHwnd() == (HWND)lParam)
		{
			EndDialog(IDOK);
		}
	}
	break;
	case WM_SIZE:
	{

	}
	break;
	default:
		break;
	}
    return QYDialog::WindowProc(message, wParam, lParam);
}


#endif
