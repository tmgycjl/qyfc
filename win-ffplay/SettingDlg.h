#pragma once


class SettingDlg :
	public QYDialog
{
public:
	SettingDlg();
	~SettingDlg();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	void onEvent(QYPropertyList *propertyList);

	volatile bool _mainThreadRun = false;
	HANDLE m_hListenerThread = nullptr;
	std::string _url;
private:
	QYEdit *_editClientCount;
	QYEdit *m_edPin;
	QYSwitchButton *m_switchRememberPin;
	QYSwitchButton *m_switchAutoLogin;
	BOOL m_bReLogin;
	QYListCtrl *_devList = nullptr;
	QYMenu *m_pPopMenu;
	QYIniFile _iniFile;
	
};

