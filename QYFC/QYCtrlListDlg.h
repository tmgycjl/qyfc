#pragma once
#include "QYDialog.h"
#include "QYCtrlList.h"
#include "QYTabCtrl.h"
#include "QYString.h"



class QYUI_EXPORT_CLASS QYCtrlListDlg :
	public QYDialog
{
public:
	QYCtrlListDlg();
	~QYCtrlListDlg();
	virtual void DoDataExchange(QYDataExchange* pDX);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL OnSize(UINT nType, int cx, int cy);
	LRESULT OnPaint();
	QYCtrlList* AddCtrlList(LPCTSTR strTitle = L"");
	void    UpdateChildWindowPos();
	QYCtrlList* ShowCtrlList(QYCtrlList *pCtrlList);
	QYButton* AddButton(UINT nID, LPCTSTR szText);
	void SetButtonText(UINT nID,LPCTSTR szText );
	QYButton* GetButton(UINT nID);
protected:
	QYTabCtrl m_CtrlListTab;
	std::list<QYCtrlList*> m_listCtrlList;
	int       m_ButtonPaneHeight;
	std::map<UINT ,QYButton*> m_mapButton;
};

