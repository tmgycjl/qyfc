#pragma once

#include "QYButton.h"
#include "QYpicture.h"
class QYUI_EXPORT_CLASS QYCheckButton :
	public QYButton
{
public:
	QYCheckButton();
	virtual ~QYCheckButton();//
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	LRESULT OnNCPaint();
	BOOL SetCheck(BOOL bCheck = TRUE);
	BOOL GetCheck();
protected:
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
	BOOL         m_bCheck;
};

