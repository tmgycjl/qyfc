#include "stdafx.h"
#include "Mainframe.h"
#include "App.h"

#include <fstream>


#include "SettingDlg.h"
#include "UrlDlg.h"


CMainframe *CMainframe::m_instance = nullptr;;


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

			getline(inFile, line);
			if (0 < line.length())
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


bool CMainframe::playUrl(std::string &url)
{
	if (nullptr != _videoWnd)
	{
		QYRect rc;
		_videoWnd->GetClientRect(rc);

		_playWnd = _videoWnd->GetHwnd();
		QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);

		//StartFFPLAY(std::string("c:\\ffmpeg_installed\\bin\\ffplay.exe"), std::string(cmdBuf));
		if (0 == ffplayPlay(_playWnd, "%s %s,%s %s,%s %s,%s %s,%s %s",
			QYApp::getAppPath().c_str(),
			"-render", _vecRender[iniFile.Get_int(L"setting", L"render", 0)].c_str(),
			"-loglevel", "quiet",
			"-rtsp_transport", "tcp",
			"-analyzeduration", "50000",
			"-i", url.c_str(),
			" "
			))
		{
			_lastOpenUrl = true;
			_lastFilePath = url;
			SetTimer(1, 500, nullptr);

			auto it = _listRecentUrl.begin();
			for (; it != _listRecentUrl.end(); it++)
			{
				if (*it == url)
				{
					break;
				}
			}

			if (it == _listRecentUrl.end())
			{
				_listRecentUrl.push_back(url);
			}

			saveRecentUrl();

			return true;
		}

		
	}

	return false;
}

bool CMainframe::playFile(std::string &filePath)
{
	if (nullptr != _videoWnd)
	{
		char cmdBuf[1024] = { 0 };
		_playWnd = _videoWnd->GetHwnd();

		QYIniFile iniFile(QYApp::GetAppPath() + CONFIG_INI);
	
		if (0 == ffplayPlay(_playWnd, "%s %s,%s %s,%s %s,%s %s,%s %s",
			QYApp::getAppPath().c_str(),
			"-render", _vecRender[iniFile.Get_int(L"setting", L"render", 0)].c_str(),
			"-loglevel", "quiet",
			"-i", filePath.c_str(),
			" "
			))
		{
			_lastOpenUrl = false;
			_lastFilePath = filePath;
			SetTimer(1, 500, nullptr);

			return true;
		}

	}

	return false;
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

	ffplayStartUp(this->GetHwnd());

	m_eventCB.callback = std::bind(&CMainframe::onEvent, this, std::placeholders::_1);

	_videoWidget = (QYWidget*)getObjectPart("video_widget");
	if (nullptr != _videoWidget)
	{
		_videoWidget->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);

		_videoWnd = (QYStatic*)_videoWidget->Append("video_wnd", QY_CONTROL_TYPE_STATIC);
		if (nullptr != _videoWnd)
		{
			_videoWnd->SetBkColor(RGB(0, 0, 0));
			_videoWnd->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
		}
	}

	_playProcess = (QYProgressCtrl*)getObjectPart("play_process");
	if (nullptr != _playProcess)
	{
		_playProcess->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}

	_timeText = (QYStatic*)getObjectPart("play_time");
	if (nullptr != _timeText)
	{
		_timeText->setWindowText("00:00/00:00");
	}


	_play = (QYButton*)getObjectPart("play");
	if (nullptr != _play)
	{
		_play->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}

	_stop = (QYButton*)getObjectPart("stop");
	if (nullptr != _stop)
	{
		_stop->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}


	QYButton *setting = (QYButton*)getObjectPart("setting");
	if (nullptr != setting)
	{
		setting->registerCallback(QY_CALLBACK_EVENT, &m_eventCB);
	}


	loadRecentUrl();

	//Zoom();

	
	return TRUE;
}


void CMainframe::onEvent(QYPropertyList *propertyList)
{
	std::string id = propertyList->getValueWithDefaultString("id", "");
	if ("video_wnd" == id || "video_widget" == id )
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
		if (QYFileStudio::openFileDialog(GetHwnd(), filePath, "media files (*.mp4*)\0*.mp4*\0\0"))
		{
			if (playFile(filePath))
			{
				_play->setImage("replay_pause.png");
			}
			
		}
	}
	else if ("open_url" == id)
	{
		//rtsp://admin:123@192.168.86.133:5540/
		UrlDlg dlg;
		if (IDOK == dlg.DoModal(this))
		{
			if(playUrl(dlg._url))
			{
				_play->setImage("replay_pause.png");
			}
			
		}
		
	}
	else if ("play_process" == id)
	{
		if ("lbuttondown" == propertyList->getValueWithDefaultString("action", ""))
		{
			int pos = propertyList->getValueWithDefaultInt("pos", 0);
			if (0 < pos)
			{
				ffplaySeekTime(pos);
			}
		}
	}
	else if ("play" == id)
	{
		if (!ffplayIsPlaying())
		{
			if (0 < _lastFilePath.length())
			{
				QYPropertyList eventProp;
				if (_lastOpenUrl)
				{
					if (playUrl(_lastFilePath))
					{
						_play->setImage("replay_pause.png");
					}
				}
				else
				{
					if (playFile(_lastFilePath))
					{
						_play->setImage("replay_pause.png");
					}
				}
			}
		}
		else
		{
			_pause = ffplayIsPause();
			ffplayPause();
		}
		
	}
	else if ("stop" == id)
	{
		ffplayStop();
		_pause = false;
		_play->setImage("replay_play.png");
		KillTimer(1);
		_timeText->setWindowText("00:00/00:00");
		_playProcess->SetRange(0);
		_playProcess->SetPos(0);
		_videoWnd->Invalidate();
	}
	else if ("setting" == id)
	{
		SettingDlg dlg;
		dlg.DoModal(this);
	}
}

void CMainframe::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		int totalTime = 0;
		int playTime = 0;
		ffplayGetTime(&totalTime, &playTime);
	
		if (nullptr != _timeText)
		{
			char szTime[128] = { 0 };
			if (0 < totalTime)
			{
				sprintf_s(szTime, 128, "%d:%d/%d:%d", playTime / 60, playTime % 60, totalTime / 60, totalTime % 60);
				_timeText->setWindowText(szTime);
			}
		
		}

		if (nullptr != _playProcess)
		{
			_playProcess->SetRange(totalTime);
			_playProcess->SetPos(playTime);
		}

		if (_pause != ffplayIsPause())
		{
			if (ffplayIsPause())
			{
				_play->setImage("replay_play.png");
			}
			else
			{
				_play->setImage("replay_pause.png");
			}

			_pause = ffplayIsPause();
		}
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
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_SPACE:ffplayPause(); _pause = ffplayIsPause(); break;
		case VK_LEFT:ffplaySeekLeft(); break;
		case VK_RIGHT:ffplaySeekRight(); break;
		case VK_UP:ffplaySeekUp(); break; 
		case VK_DOWN:ffplaySeekDown(); break;
		default:
			break;
		}
		
	}
		break;
	case WM_FFPLAY_VIDEO_RESIZE:
	{
		_videoSize.cx = wParam;
		_videoSize.cy = lParam;
		updateVideoSize();
	}
		break;
	default:
		break;
	}

	return QYDialog::WindowProc(message, wParam, lParam);
}

void CMainframe::updateVideoSize()
{
	if (nullptr != _videoWidget && _videoSize.cx > 0 && _videoSize.cy > 0)
	{
		QYRect rcWiget;
		_videoWidget->GetClientRect(rcWiget);
		QYRect rcDisplay = rcWiget;
		float ratioImage = (float)_videoSize.cx / (float)_videoSize.cy;
		float ratioWnd = (float)rcWiget.Width() / (float)rcWiget.Height();
		int off = 0;
		if (ratioImage > ratioWnd)
		{
			off = (rcWiget.Height() - (rcWiget.Width() / ratioImage)) / 2;
			rcDisplay.top += off;
			rcDisplay.bottom -= off;
		}
		else
		{
			off = (rcWiget.Width() - rcWiget.Height() * ratioImage) / 2;
			rcDisplay.left += off;
			rcDisplay.right -= off;
		}

		if (nullptr != _videoWnd)
		{
			_videoWnd->MoveWindow(rcDisplay);
		}
	}
}

BOOL CMainframe::OnSize(UINT nType, int cx, int cy)
{
	QYDialog::OnSize(nType, cx, cy);

	updateVideoSize();

	return TRUE;
}

void  CMainframe::OnClose()
{
	ffplayStop();
}