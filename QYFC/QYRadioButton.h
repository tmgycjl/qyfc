#pragma once
#include "QYCheckButton.h"
class QYUI_EXPORT_CLASS QYRadioButton :
	public QYButton
{
public:
	QYRadioButton();
	virtual ~QYRadioButton();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	BOOL SetCheck(BOOL bCheck = TRUE);
	BOOL GetCheck();
private:
	QYPicture   *m_picNormalBox;
	QYPicture   *m_picSelectBox;
	BOOL         m_bCheck;
	int          m_circleDiameter;
};

