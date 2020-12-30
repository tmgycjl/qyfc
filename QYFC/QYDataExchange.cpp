#include "QYDataExchange.h"


QYDataExchange::QYDataExchange()
{
	m_mapControl.clear();
}

QYDataExchange::~QYDataExchange()
{
	Clear();
}

void QYDataExchange::Clear()
{
	for (auto it = m_mapControlNew.begin(); it != m_mapControlNew.end(); it++)
	{
		QYWindow *pWnd = it->second;

		SAFE_DELETE(pWnd);
	}

	m_mapControlNew.clear();
	m_mapControl.clear();
}

void QYDataExchange::PushCtrl(HWND hWnd, QYWindow *pWnd, BOOL bNew)
{
	if (bNew)
	{
		m_mapControlNew.insert(std::map<HWND, QYWindow*>::value_type(hWnd, pWnd));
	}
	else
	{
		m_mapControl.insert(std::map<HWND, QYWindow*>::value_type(hWnd, pWnd));
	}
	
}

QYWindow* QYDataExchange::FindCtrl(HWND hWnd, BOOL bNew)
{
	if (bNew)
	{
		auto it = m_mapControlNew.find(hWnd);

		return (it != m_mapControlNew.end()) ? it->second : nullptr;
	}
	else
	{
		auto it = m_mapControl.find(hWnd);

		return (it != m_mapControl.end()) ? it->second : nullptr;
	}

}