#pragma once

#include "QYSplitterCtrl.h"

#define  MAX_SPLITTER_COUNT        64

class QYUI_EXPORT_CLASS QYSplitterCtrlManager
{
public:
	QYSplitterCtrlManager();
	virtual ~QYSplitterCtrlManager();
	QYSplitterCtrl* AddSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY);
	QYSplitterCtrl* AddFirstSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY);
	QYSplitterCtrl* AddSecondSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY);
	void ClearSplitter();
	void SetSplitterWidth();
	void UpdateLayout(QYRect &rect);
private:
	QYSplitterCtrl* m_RootSplitter;
	int m_nSplitterWidth;
};

