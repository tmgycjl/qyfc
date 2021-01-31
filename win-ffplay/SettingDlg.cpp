#include "stdafx.h"
#include "SettingDlg.h"
#include "App.h"
#include "Mainframe.h"


// cloudServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <map>


#include <queue>

SettingDlg::SettingDlg() :QYDialog()
{
	
}


SettingDlg::~SettingDlg()
{

}

BOOL SettingDlg::OnInitDialog()
{
	QYDialog::OnInitDialog();

	if (!loadLayout("ffplay.xml", "setting"))
	{
		EndDialog(IDCANCEL);
		return false;
	}

	m_eventCB.callback = std::bind(&SettingDlg::onEvent, this, std::placeholders::_1);
	QYComboBox *render = (QYComboBox*)getObjectPart("render_combobox");
	if (nullptr != render)
	{
		//render->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
		auto _listRecentUrl = CMainframe::instance()->getRecentUrlPtr();

		for (auto i: _vecRender)
		{
			render->addString("1", i.c_str());
		}

		QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);
		render->SetCurSel(iniFile.Get_int(L"setting", L"render",0));
	}

	QYSwitchButton *sdl = (QYSwitchButton*)getObjectPart("enable_sdl");
	if (nullptr != sdl)
	{
		QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);
		sdl->SetCheck(iniFile.Get_int(L"setting", L"sdl", 0));
	}

	QYButton *ok = (QYButton*)getObjectPart("ok");
	if (nullptr != ok)
	{
		ok->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}


	QYButton *cancel = (QYButton*)getObjectPart("cancel");
	if (nullptr != cancel)
	{
		cancel->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}

	return TRUE;
}


LRESULT SettingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return QYDialog::WindowProc(message, wParam, lParam);
}

void SettingDlg::onEvent(QYPropertyList *propertyList)
{
	std::string id = propertyList->getValueWithDefaultString("id", "");
	if ("cancel" == id)
	{
		EndDialog(IDCANCEL);
	}
	else if ("ok" == id)
	{
		QYComboBox *render = (QYComboBox*)getObjectPart("render_combobox");
		if (nullptr != render)
		{
			QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);
			iniFile.Write_int(L"setting",L"render",render->GetCurSel());

			
			EndDialog(IDCANCEL);
		}
		 

		QYSwitchButton *sdl = (QYSwitchButton*)getObjectPart("enable_sdl");
		if (nullptr != sdl)
		{
			QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);
			iniFile.Write_int(L"setting", L"sdl", sdl->GetCheck());


			EndDialog(IDCANCEL);
		}
	}
}