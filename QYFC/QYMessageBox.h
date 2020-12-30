#pragma once

#include "qydefine.h"
#include "QYWidgetListDialog.h"

#if 0
class QYUI_EXPORT_CLASS QYMessageBox :
	public QYDialog
{
public:
	QYMessageBox(const char  *strCaption, const char  *strMessage, UINT nFlag);
	~QYMessageBox();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
private:
	std::string m_strCaption;
	std::string m_strMessage;
	UINT m_nFlag;
};


#else
class QYUI_EXPORT_CLASS QYMessageBox :
    public QYDialog
{
public:
	QYMessageBox(const char  *strCaption, const char  *strMessage, UINT nFlag);
	~QYMessageBox();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
private:
    QYButton *m_buttonOK = nullptr;
    QYButton *m_buttonCancel = nullptr;
	std::string m_strCaption;
	std::string m_strMessage;
	UINT m_nFlag;
};


#endif
