#pragma once
#include "QYWindow.h"
#include <vector>

class QYUI_EXPORT_CLASS QYToolTip :
	public QYWindow
{
public:
	QYToolTip();
	~QYToolTip();

	BOOL Create();
	BOOL AddTool(QYWindow* window, LPCTSTR strText);
	BOOL DeleteTool(QYWindow* window);
	void SetText(LPCTSTR strText);
	virtual BOOL Show(int nCmdShow = SW_SHOW);
	void TrackPopup(QYWindow *pRefWind,QYPoint point = QYPoint(0,0),int delay = 500);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnTimer(int nEventID);
	void OnWindowPosChanged();
private:
	class ToolTipNode
	{
	public:
		QYWindow* pWind;
		QYString  strText;
	protected:
	private:
	};
	QYWindow  *m_pSourceWnd;
	std::vector<ToolTipNode*> m_vectorToolTipNode;
	QYString m_strText;
	QYWindow *m_pRefWind;
	QYPoint  m_pt;
public:
	virtual LRESULT OnPaint();
	LRESULT OnSize(UINT nType, int cx, int cy);
};

