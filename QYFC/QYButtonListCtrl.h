#pragma once
#include "QYButton.h"
#include "QYListCtrl.h"
#include <vector>

class QYUI_EXPORT_CLASS QYListCtrlButton
{
public:
	QYListCtrlButton()
	{
		m_ID = 0;
		m_pPic = nullptr;
		m_nColmun = -1;
		m_nWidth = 0;
		m_colorText = RGB(50, 89, 138);
		m_strText = L"";
	}
	 ~QYListCtrlButton(){SAFE_DELETE(m_pPic); }
	UINT   m_ID;
	QYPicture *m_pPic;
	int    m_nColmun;
	int    m_nWidth;
	COLORREF m_colorText;
	QYString m_strText;
protected:
private:
};

class QYUI_EXPORT_CLASS QYButtonListCtrl :public QYListCtrl
{
public:
	QYButtonListCtrl();
	virtual ~QYButtonListCtrl();

	void AddItemButton(int colmun, UINT ctrlID,LPCTSTR strText,LPCTSTR strImageName = nullptr);
	void SetItemButtonText(int colmun,LPCTSTR strText);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	virtual LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	virtual LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	virtual LRESULT OnPaint();
	LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
private:
	std::vector<QYListCtrlButton*> m_vectorButton;
	QYFont *m_pButtonFont;
	QYFont *m_pButtonSelFont;
	int    m_selButtonItem;
	int   m_downButtonItem;
};

