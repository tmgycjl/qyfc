#include "stdafx.h"
#include "win-ffplay-dlg.h"
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
}

BOOL  StartFFPLAY(std::string &filePath, std::string &strCmdLine)
{
	HANDLE hChildApp = NULL;

	filePath += "/ffplay.exe";

	
	char ExePath[256] = { 0 };
	char CmdLine[256] = { 0 };

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);


#if 0
	char temp[512];
	sprintf_s(temp, 511, "%s %s", szExePath.c_str(), szCmdLine);

	WinExec(temp, SW_SHOW);/////////////////////////////
#else
	//// Start the child process
	if (!CreateProcessA(filePath.c_str(), (char*)strCmdLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		return FALSE;
	}
#endif

	if (nullptr ==  pi.hProcess)
	{
		return FALSE;
	}


	return TRUE;
}

BOOL MainDlg::OnInitDialog()
{
	QYDialog::OnInitDialog();

	if (!loadLayout("ffplay.xml", "ffplay"))
	{
		EndDialog(IDCANCEL);
		return false;
	}


	char cmdBuf[1024] = { 0 };
	sprintf_s(cmdBuf, 1024, " -window %d -loglevel quiet -rtsp_transport tcp -analyzeduration 50000"
		" -i rtsp://admin:123@192.168.86.133:5540/ -x 800 -y 600",GetHwnd() );
	StartFFPLAY(std::string("F:\\ffmpeg_installed\\bin"), std::string(cmdBuf));

	return TRUE;
}


LRESULT MainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

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