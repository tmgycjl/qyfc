#pragma once


class MainDlg :
	public QYDialog
{
public:
	MainDlg(BOOL bReLogin = FALSE);
	~MainDlg();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	void onEvent(QYPropertyList *propertyList);

	volatile bool _mainThreadRun = false;
	HANDLE m_hListenerThread = nullptr;
private:
	QYEdit *_editClientCount;
	QYEdit *m_edPin;
	QYSwitchButton *m_switchRememberPin;
	QYSwitchButton *m_switchAutoLogin;
	BOOL m_bReLogin;
	QYListCtrl *_devList = nullptr;
	QYMenu *m_pPopMenu;
};

