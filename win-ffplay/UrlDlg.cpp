#include "stdafx.h"
#include "UrlDlg.h"
#include "App.h"
#include "Mainframe.h"


// cloudServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>


#include <queue>

UrlDlg::UrlDlg() :QYDialog()
{
	
}


UrlDlg::~UrlDlg()
{

}

BOOL UrlDlg::OnInitDialog()
{
	QYDialog::OnInitDialog();

	if (!loadLayout("ffplay.xml", "url_dlg"))
	{
		EndDialog(IDCANCEL);
		return false;
	}



	m_eventCB.callback = std::bind(&UrlDlg::onEvent, this, std::placeholders::_1);


	QYString lastUrl = L"";
	QYTreeCtrl *urlTree = (QYTreeCtrl*)getObjectPart("history_url");
	if (nullptr != urlTree)
	{
		urlTree->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
			auto _listRecentUrl = CMainframe::instance()->getRecentUrlPtr();

			for (auto it = _listRecentUrl->begin(); it != _listRecentUrl->end(); it++)
			{
				urlTree->insertItem(0, it->c_str());
			}
		
		
	}

	QYEdit *edit_url = (QYEdit*)getObjectPart("edit_url");
	if (nullptr != edit_url)
	{
		edit_url->SetText(lastUrl);
	}

	QYButton *btnPlay = (QYButton*)getObjectPart("play");
	if (nullptr != btnPlay)
	{
		btnPlay->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}
	
	return TRUE;
}


LRESULT UrlDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return QYDialog::WindowProc(message, wParam, lParam);
}

void UrlDlg::onEvent(QYPropertyList *propertyList)
{
	std::string id = propertyList->getValueWithDefaultString("id", "");
	if ("cancel" == id)
	{
		EndDialog(IDCANCEL);
	}
	else if ("play" == id)
	{
		QYEdit *edit_url = (QYEdit*)getObjectPart("edit_url");
		if (nullptr != edit_url)
		{
			edit_url->getText(_url);
			EndDialog(IDOK);
		}
		 
	}
	else if ("history_url" == id)
	{
		if ("double_click" == propertyList->getValueWithDefaultString("action", ""))
		{
			QYTreeCtrl *urlTree = (QYTreeCtrl*)getObjectPart("history_url");
			if (nullptr != urlTree)
			{
				const TCHAR *text = urlTree->GetItemText(urlTree->GetSelectItem());
				if (nullptr != text)
				{
					QYEdit *edit_url = (QYEdit*)getObjectPart("edit_url");
					if (nullptr != edit_url)
					{
						edit_url->SetText(text);
					}
				}
				

			}
		}
	}
}