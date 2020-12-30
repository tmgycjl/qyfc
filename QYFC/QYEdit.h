#pragma once

#include "QYWindow.h"

#define SHIFTED 0x8000
class QYUI_EXPORT_CLASS QYEdit :public QYWindow
{
public:
	QYEdit();
	virtual ~QYEdit();
	static LRESULT CALLBACK EditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void SetText(const TCHAR *lpString); 
	void setText(const char *text);
	BOOL getText(std::string &text);
	int  SetFlag(int flag);
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	int GetText(QYString &strText, int size = 1024);
	TCHAR* GetText();
	LRESULT OnPaint();
	LRESULT OnEraseBackRound(HDC hDC);
	LRESULT OnNCPaint();
	LRESULT LoseFocus();
	LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	LRESULT OnLButtonDBClick(UINT nFlags, QYPoint point);
	virtual BOOL SubClassWindow(HWND hWnd);
	LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	LRESULT OnChar(WPARAM wparam, LPARAM lparam,BOOL bPasswordFlag);
	LRESULT OnSize(int cx, int cy);
	LRESULT OnSetFocus();
	LRESULT OnKeyDown(WPARAM wparam, LPARAM lparam);
	LRESULT OnSetCursor(WPARAM wparam, LPARAM lparam);
	BOOL    SetReadOnly(BOOL bReadOnly = TRUE);
	BOOL    SetPasswordFlag(BOOL bPasswordFlag = TRUE);
	BOOL    SetInComboBox(BOOL bInComboBox = TRUE);
	void    SetMaxChars(short maxChars );
private:
	BOOL  IsTextLargeThanWndRect();
	void CopyString();
	int  PosFromChar(UINT nChar);
	int  CharFromPos(int nPosX);
	void PasteString();
	void CutString();
	int  GetMaxShowCharCount();
	void OnTimer(UINT_PTR nIDEvent);
	int         m_Flag;
	BOOL        m_bFocus;
	BOOL        m_bHot;
	BOOL        m_bInComboBox;
	QYString m_strBackEdit;		// 当前包含的文本。
	QYString m_strEdit;		// 当前包含的文本。
	QYString m_strSelect;	// 当前选择的文本。

	QYPoint m_ptCursor;		// 当前光标的位置。
	QYPoint m_ptTxtBegin;	// 文本的起始位置。
	QYPoint m_ptFirst;		// 鼠标左键按下时的第一点位置。
	QYPoint m_ptSecond;		// 鼠标当前的位置。

	int m_nCharPos;			// 当前光标处于第几个字符的后面，从0开始计数。
	int m_nShowChar;		// 在编辑框中第一个显示的字符，从0开始计数。
	int m_nMaxShowChars;	// 在编辑框中一次最多显示的字符数量	。
	int m_nSelectBegin;		// 选择的字符的起始位置。
	int m_nSelectEnd;		// 选择的字符的终止位置。

	//CBitmap m_BmpBK;		// 背景图片。

	//CFont m_TxtFont;		// 文本的字体。
	TEXTMETRIC m_tm;		// 文本字体的信息。

	QYRect	m_rcWnd;		// 窗口的大小。

	bool	m_bLBtDown;		// 鼠标左键按下。
	int		m_nShiftBegin;	// shift键第一次时光标所在的字符的位置。
	BOOL    m_bReadOnly;
	BOOL    m_bPassword;
	int     m_Type;
	SYSTEMTIME m_Date;
	 int    nMargin;
	 HCURSOR m_hCursor;
	 short   m_nMaxChars;
	 BOOL   m_bTimerEnd;
};

