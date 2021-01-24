#pragma once


class CVideoPane;
class CReplayOperPane;
class CPtzPane;
class CRecordTable;
class CReplayVideoPane;
class CSystemInfoCtrl;

class CMainframe :
	public QYDialog
{
public:
	static CMainframe* instance(void);
	~CMainframe();
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void onEvent(QYPropertyList *propertyList);
	std::list<std::string>* getRecentUrlPtr(){ return &_listRecentUrl; }
	virtual BOOL OnSize(UINT nType, int cx, int cy);
private:
	static CMainframe *m_instance ;
	CMainframe();

	void loadRecentUrl();
	void saveRecentUrl();
	void OnTimer(UINT_PTR nIDEvent);
	void OnClose();
	bool playUrl(std::string &url);
	bool playFile(std::string &filePath);
	void updateVideoSize();
	QYMenu *m_pPopMenu = nullptr;
	std::list<std::string>  _listRecentUrl;
	HWND _playWnd = nullptr;
	QYStatic *_videoWnd = nullptr;
	QYWidget *_videoWidget = nullptr;
	QYProgressCtrl *_playProcess = nullptr;
	QYStatic *_timeText = nullptr;
	QYButton *_play = nullptr;

	QYButton *_stop = nullptr;
	bool _lastOpenUrl = false;
	bool _pause = false;
	std::string _lastFilePath;
	QYSize _videoSize ;

};

