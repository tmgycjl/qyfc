#pragma once


class PicDlg :
	public QYDialog
{
public:
	PicDlg(QYPropertyList *properties = nullptr);
	~PicDlg();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	void onEvent(QYPropertyList *propertyList);
private:
	QYTreeCtrl *_picTree = nullptr;
	QYWidgetList *_widgetList = nullptr;
	std::string _picDir;
	QYButton *_buttonBic = nullptr;
	QYPicture *_pic = nullptr;
};

