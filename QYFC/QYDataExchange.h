#pragma once

#include <map>
#include <vector>
#include "QYWindow.h"

class QYDataExchange
{
public:
	QYDataExchange();
	virtual ~QYDataExchange();
	QYWindow* FindCtrl(HWND hWnd, BOOL bNew = TRUE);
	void PushCtrl(HWND hWnd, QYWindow *pWnd,BOOL bNew = TRUE);
	void Clear();
private:
	std::map<HWND, QYWindow*> m_mapControlNew;
	std::map<HWND, QYWindow*> m_mapControl;
	std::map<QYWindow*, QYWindow*> m_mapValue;
};

