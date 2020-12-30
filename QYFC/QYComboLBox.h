#pragma once
#include "QYWindow.h"


class QYUI_EXPORT_CLASS QYComboLBox :public QYWindow
{
public:
	QYComboLBox();
	virtual ~QYComboLBox();
	static LRESULT CALLBACK ComboLBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	LRESULT OnPaint();
	LRESULT OnNCPaint();
	LRESULT LoseFocus();
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
private:
	int         m_Flag;
	BOOL        m_bFocus;
	BOOL        m_bHot;
};

