#pragma once

#include "QYDialog.h"
#include "QYWidgetList.h"
#include "QYTabCtrl.h"
#include "QYString.h"



class QYUI_EXPORT_CLASS QYWidgetListDialog :
	public QYDialog
{
public:
	QYWidgetListDialog();
	~QYWidgetListDialog();
	virtual void DoDataExchange(QYDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL OnSize(UINT nType, int cx, int cy);
	LRESULT OnPaint();
	QYWidgetList* AddWidgetList(LPCTSTR strTitle = L"");
	void    UpdateChildWindowPos();
	QYWidgetList* ShowWidgetList(QYWidgetList *pWidgetList);
	QYButton* addButton(UINT nID, const char* text);
    QYButton* AddButton(UINT nID, LPCTSTR szText);
	void SetButtonText(UINT nID, LPCTSTR szText);
    void setButtonText(UINT nID, const char* text);
	QYButton* GetButton(UINT nID);
    void enableBottomPane(BOOL enable){ m_enableBottomPane = enable; }
protected:
	QYTabCtrl m_WidgetListTab;
	std::list<QYWidgetList*> m_listWidgetList;
	int       m_ButtonPaneHeight;
	std::map<UINT, QYButton*> m_mapButton;
    BOOL  m_enableBottomPane = TRUE;
};
