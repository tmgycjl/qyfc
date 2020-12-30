#include "stdafx.h"
#include "qyfc-demo-dlg.h"
#include "App.h"


// cloudServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>


#include <queue>

MainDlg::MainDlg(BOOL bReLogin) :QYDialog()
{
	
}


MainDlg::~MainDlg()
{
	_mainThreadRun = false;
	WaitForSingleObject(m_hListenerThread, INFINITE);
}


BOOL MainDlg::OnInitDialog()
{
	QYDialog::OnInitDialog();

	if (!loadLayout("demo.xml", "demo"))
	{
		EndDialog(IDCANCEL);
		return false;
	}

	m_eventCB.callback = std::bind(&MainDlg::onEvent, this, std::placeholders::_1);

	_devList = (QYListCtrl*)getObjectPart("list");

	int item = 0;
	_devList->SetRedraw(FALSE);
	for (int i = 0; i < 50; i++)
	{
		item = _devList->InsertItem(L"item");
		_devList->setItemText(item,1,std::to_string(i + 1).c_str());
	}
	_devList->SetRedraw(TRUE);
	

	QYEdit *ed = (QYEdit*)getObjectPart("edit1");
	ed->setText("");

	ed = (QYEdit*)getObjectPart("edit2");
	ed->SetReadOnly(TRUE);
	ed->setText("readonly editbox");

	QYButton *button = (QYButton*)getObjectPart("button2");
	button->Enable(FALSE);

	QYComboBox *cbBox = (QYComboBox*)getObjectPart("combobox");
	
	for (int i = 0; i < 10; i++)
	{
		cbBox->addString(std::to_string(i).c_str(), (std::string("list item") + std::to_string(i + 1)).c_str());
	}
	cbBox->SetCurSel(0);

	QYTreeCtrl *tree = (QYTreeCtrl*)getObjectPart("tree");
	tree->insertItem(0, "tree item 1", 0);
	DWORD itemID = tree->insertItem(0, "tree item 2", 1);
	tree->insertItem(itemID, "tree item 3", 2);
	tree->insertItem(0, "tree item 4", 2);
	tree->insertItem(0, "tree item 5", 2);
	itemID = tree->insertItem(0, "tree item 6", 2);
	tree->insertItem(itemID, "tree item 7", 2);
	tree->insertItem(0, "tree item 8", 2);

	button = (QYButton*)getObjectPart("popup_menu");
	button->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	//ShowWindow(GetHwnd(),SW_SHOWMAXIMIZED);

	return TRUE;
}


LRESULT MainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:
		break;
	}
	return QYDialog::WindowProc(message, wParam, lParam);
}

void MainDlg::onEvent(QYPropertyList *propertyList)
{
	std::string id = propertyList->getValueWithDefaultString("id", "");
	if ("cancel" == id)
	{
		EndDialog(IDCANCEL);
	}
	else if ("popup_menu" == id)
	{
		SAFE_DESTROY_WINDOW_PTR(m_pPopMenu);
		m_pPopMenu = new QYMenu;
		assert(nullptr != m_pPopMenu);

		m_pPopMenu->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);

		if (m_pPopMenu->loadMenu("menu.xml", "system"))
		{
			m_pPopMenu->enableItem("relogin", false);

			std::string lang = QYLanguage::instance()->getLangID();
			QYMenu *pLangMenu = (QYMenu*)m_pPopMenu->getSubMenu("language");
			if ("en" == lang)
			{
				pLangMenu->checkItem("english", true);
			}
			else if ("zh" == lang)
			{
				pLangMenu->checkItem("chinese", true);
			}
			POINT pt;
			::GetCursorPos(&pt);
			m_pPopMenu->TrackPopup(QYPoint(pt.x, pt.y), 0, TPMENU_RIGHT_BOTTOM, this, true);
		}
	}
	else if ("english" == id)
	{
		QYLanguage::instance()->setLang("en", this);
	}
	else if ("chinese" == id)
	{
		QYLanguage::instance()->setLang("zh",this);
	}
}