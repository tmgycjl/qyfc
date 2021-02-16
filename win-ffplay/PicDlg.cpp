#include "stdafx.h"
#include "PicDlg.h"
#include "App.h"
#include "Mainframe.h"


// cloudServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>


#include <queue>


#include "../MyMediaFile/MyMediaFile.h"


#if _DEBUG
#pragma  comment(lib,"MyMediaFile-d")
#else
#pragma  comment(lib,"MyMediaFile")
#endif


PicDlg::PicDlg(QYPropertyList *properties) :QYDialog()
{
	if (nullptr != properties)
	{
		_picDir = properties->getValueWithDefaultString("pic_dir", "");
	}
}


PicDlg::~PicDlg()
{

}

BOOL PicDlg::OnInitDialog()
{
	ModifyStyle(0, WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);


	QYDialog::OnInitDialog();

	if (!loadLayout("ffplay.xml", "pic_dlg"))
	{
		EndDialog(IDCANCEL);
		return false;
	}

	m_eventCB.callback = std::bind(&PicDlg::onEvent, this, std::placeholders::_1);


	_picTree = (QYTreeCtrl*)getObjectPart("pic_tree");
	if (nullptr != _picTree)
	{
		_picTree->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);


		QYFileStudio fileStudio;

		std::string picDir = _picDir + "\\*.*";
		if (fileStudio.FindFirst(picDir))
		{
			do
			{
				if (fileStudio.IsDirectory() || fileStudio.IsDot())
				{
					continue;
				}

				_picTree->InsertItem(0, fileStudio.GetFileName());
			} while (fileStudio.FindNext());
		}

	}

	_buttonBic = new QYButton;
	_buttonBic->Create(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, 0, GetHwnd());
	_buttonBic->Enable(FALSE);

	return TRUE;
}


LRESULT PicDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_SIZE == message)
	{
		if (nullptr != _buttonBic )
		{
			QYRect rcClient;
			GetClientRect(rcClient);

			QYRect rcTree;
			if (nullptr != _picTree)
			{
				_picTree->GetClientRect(rcTree);
			}

			_buttonBic->MoveWindow(rcClient.left + rcTree.Width() + 5,rcClient.top+5,rcClient.Width() - rcTree.Width()-10,rcClient.Height()-10);
		}
	}
	return QYDialog::WindowProc(message, wParam, lParam);
}

void PicDlg::onEvent(QYPropertyList *propertyList)
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
			EndDialog(IDOK);
		}
		 
	}
	else if ("pic_tree" == id)
	{
		if ("click" == propertyList->getValueWithDefaultString("action", ""))
		{
			std::string text = _picTree->getItemText(_picTree->GetSelectItem());

				QYPicture *pic = new QYPicture;
				text = _picDir + "\\" + text;
				if (nullptr != strstr(text.c_str(),".pchp"))
				{
					char *streamBuffer = nullptr;
					int readLen = 0;
					if (0 <  (readLen = myMediaPicRead(text.c_str(), &streamBuffer)))
					{
						pic->Load((BYTE*)streamBuffer, readLen);
						SAFE_DELETE_ARRAY(streamBuffer);
					}
					
				}
				else
				{
					pic->load(text.c_str());
				}
				
				_buttonBic->SetImage(pic);
			}
		

	}
}