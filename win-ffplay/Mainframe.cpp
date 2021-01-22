#include "stdafx.h"
#include "Mainframe.h"
#include "App.h"

CMainframe *CMainframe::m_instance = nullptr;;

enum ffplay_pip_cmd
{
	FFPLAY_PIPE_CMD_GET_FILE_TIME = 0,
	FFPLAY_PIPE_CMD_GET_PLAY_TIME,
	FFPLAY_PIPE_CMD_SET_VIDEO_SIZE,
	FFPLAY_PIPE_CMD_END
};

typedef struct
{
	int cmd;
	int nParam;
	char sParam[128];
}FFPLAY_PIPE_INFO;

//============================================================================
CMainframe*
CMainframe::instance(
void
)
//============================================================================
{
	if (!m_instance)
	{
		m_instance = new CMainframe;
	}
	return m_instance;
}


CMainframe::CMainframe()
{
}


CMainframe::~CMainframe()
{
}


BOOL  StartFFPLAY(std::string &filePath, std::string &strCmdLine)
{
	HANDLE hChildApp = NULL;

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

	if (nullptr == pi.hProcess)
	{
		return FALSE;
	}


	return TRUE;
}



void sendPipeMessage(HWND hWnd, FFPLAY_PIPE_INFO *pipeInfo)
{
	if (nullptr == hWnd)
	{
		return;
	}
	TCHAR pipeName[1024] = { 0 };
	SafeSprintf(pipeName, L"\\\\.\\pipe\\ffplay_%d", hWnd);

	if (!WaitNamedPipe(pipeName, NMPWAIT_WAIT_FOREVER))
	{
		return;
	}

	HANDLE hPip = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hPip)
	{
		return;
	}

	DWORD dwNum;
	if (!WriteFile(hPip, (char*)pipeInfo, sizeof(FFPLAY_PIPE_INFO), &dwNum, NULL))
	{
		return;
	}

}


BOOL CMainframe::OnInitDialog()
{
	ModifyStyle(0, WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX );

	QYDialog::OnInitDialog();

	if (!loadLayout("ffplay.xml", "ffplay"))
	{
		EndDialog(IDCANCEL);
		return false;
	}


	m_eventCB.callback = std::bind(&CMainframe::onEvent, this, std::placeholders::_1);

	_videoWnd = (QYStatic*)getObjectPart("video_wnd");
	if (nullptr != _videoWnd)
	{
		_videoWnd->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}

	loadRecentUrl();

	//Zoom();
	return TRUE;
}


#include <fstream>



void CMainframe::loadRecentUrl()
{
	_listRecentUrl.clear();

	std::string strPath = QYApp::getAppPath() + "recent_url.dat";
	std::ifstream inFile(strPath.c_str());
	if (inFile.is_open())
	{
		for (int x = 0; !inFile.eof(); x++)
		{
			std::string line;

			getline(inFile, line);			if (0 < line.length())
			{
				_listRecentUrl.push_back(line);
			}
		}

		inFile.close();
	}
}


void CMainframe::saveRecentUrl()
{
	std::string strPath = QYApp::getAppPath() + "recent_url.dat";


	std::ofstream outFile(strPath.c_str());

	if (outFile.is_open())
	{
	
		for (auto it = _listRecentUrl.begin(); it != _listRecentUrl.end(); it++)
		{
			outFile << (*it) << "\n";
		}
		outFile.close();
	}
}


#include "UrlDlg.h"
void CMainframe::onEvent(QYPropertyList *propertyList)
{
	std::string id = propertyList->getValueWithDefaultString("id", "");
	if ("video_wnd" == id)
	{
		if ("rbuttondown" == propertyList->getValueWithDefaultString("action", ""))
		{
			SAFE_DESTROY_WINDOW_PTR(m_pPopMenu);
			m_pPopMenu = new QYMenu;
			assert(nullptr != m_pPopMenu);

			m_pPopMenu->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);

			if (m_pPopMenu->loadMenu("menu.xml", "ffplay_oper"))
			{
				
				POINT pt;
				::GetCursorPos(&pt);
				m_pPopMenu->TrackPopup(QYPoint(pt.x, pt.y), 0, TPMENU_RIGHT_BOTTOM, this, true);
			}
		}
	}
	else if ("open_file" == id)
	{
		std::string filePath;
		if (QYFileStudio::openFileDialog(GetHwnd(), filePath, "media files (*.*)\0*.*\0\0"))
		{
			
			if (nullptr != _videoWnd)
			{
				QYRect rc;
				_videoWnd->GetClientRect(rc);
				char cmdBuf[1024] = { 0 };
				_playWnd = _videoWnd->GetHwnd();
				sprintf_s(cmdBuf, 1024, " -window %d -render direct3d  -x %d -y %d -i \"%s\"", _playWnd, rc.Width(), rc.Height(), filePath.c_str());
				StartFFPLAY(std::string("F:\\ffmpeg_installed\\bin\\ffplay.exe"), std::string(cmdBuf));

			}
		}
	}
	else if ("open_url" == id)
	{
		//rtsp://admin:123@192.168.86.133:5540/
		UrlDlg dlg;
		if (IDOK == dlg.DoModal(this))
		{
			if (nullptr != _videoWnd)
			{
				QYRect rc;
				_videoWnd->GetClientRect(rc);
				char cmdBuf[1024] = { 0 };

				_playWnd = _videoWnd->GetHwnd();
				sprintf_s(cmdBuf, 1024, " -window %d -render direct3d11 -x %d -y %d -loglevel quiet -rtsp_transport tcp -analyzeduration 50000"
					" -i  %s", _playWnd, rc.Width(), rc.Height(), dlg._url.c_str());
				StartFFPLAY(std::string("F:\\ffmpeg_installed\\bin\\ffplay.exe"), std::string(cmdBuf));

				SetTimer(1, 1000, nullptr);
				auto it = _listRecentUrl.begin();
				for (; it != _listRecentUrl.end(); it++)
				{
					if (*it == dlg._url)
					{
						break;
					}
				}

				if (it == _listRecentUrl.end())
				{
					_listRecentUrl.push_back(dlg._url);
				}
				
				saveRecentUrl();
			}
		}
		
	}
}

void CMainframe::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		FFPLAY_PIPE_INFO pipInfo;
		pipInfo.cmd = FFPLAY_PIPE_CMD_GET_PLAY_TIME;

		//sendPipeMessage(_playWnd, &pipInfo);
	}
}


LRESULT CMainframe::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_NCDESTROY:OnClose(); break;
	case WM_GETMINMAXINFO:
	{
		auto cX = GetSystemMetrics(SM_CXSCREEN);
		auto cY = GetSystemMetrics(SM_CYSCREEN);

		MINMAXINFO *maxminInfo = (MINMAXINFO*)lParam;
		maxminInfo->ptMinTrackSize.x = 1280;
		maxminInfo->ptMinTrackSize.y = 800;

		return TRUE;
	}
	break;
	case WM_TIMER:OnTimer(wParam); break;
	default:
		break;
	}

	return QYDialog::WindowProc(message, wParam, lParam);
}


BOOL CMainframe::OnSize(UINT nType, int cx, int cy)
{
	QYDialog::OnSize(nType, cx, cy);

	if(nullptr != _videoWnd)
	{
		QYRect rc;
		_videoWnd->GetClientRect(rc);
		FFPLAY_PIPE_INFO pipInfo;
		pipInfo.cmd = FFPLAY_PIPE_CMD_SET_VIDEO_SIZE;
		sprintf_s(pipInfo.sParam, "%dx%d", rc.Width(), rc.Height());
		sendPipeMessage(_playWnd, &pipInfo);
	}

	return TRUE;
}

void  CMainframe::OnClose()
{
	FFPLAY_PIPE_INFO pipInfo;
	pipInfo.cmd = FFPLAY_PIPE_CMD_END;

	sendPipeMessage(_playWnd, &pipInfo);
	_playWnd = nullptr;
}