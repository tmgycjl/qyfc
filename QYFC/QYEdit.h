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
	QYString m_strBackEdit;		// ��ǰ�������ı���
	QYString m_strEdit;		// ��ǰ�������ı���
	QYString m_strSelect;	// ��ǰѡ����ı���

	QYPoint m_ptCursor;		// ��ǰ����λ�á�
	QYPoint m_ptTxtBegin;	// �ı�����ʼλ�á�
	QYPoint m_ptFirst;		// ����������ʱ�ĵ�һ��λ�á�
	QYPoint m_ptSecond;		// ��굱ǰ��λ�á�

	int m_nCharPos;			// ��ǰ��괦�ڵڼ����ַ��ĺ��棬��0��ʼ������
	int m_nShowChar;		// �ڱ༭���е�һ����ʾ���ַ�����0��ʼ������
	int m_nMaxShowChars;	// �ڱ༭����һ�������ʾ���ַ�����	��
	int m_nSelectBegin;		// ѡ����ַ�����ʼλ�á�
	int m_nSelectEnd;		// ѡ����ַ�����ֹλ�á�

	//CBitmap m_BmpBK;		// ����ͼƬ��

	//CFont m_TxtFont;		// �ı������塣
	TEXTMETRIC m_tm;		// �ı��������Ϣ��

	QYRect	m_rcWnd;		// ���ڵĴ�С��

	bool	m_bLBtDown;		// ���������¡�
	int		m_nShiftBegin;	// shift����һ��ʱ������ڵ��ַ���λ�á�
	BOOL    m_bReadOnly;
	BOOL    m_bPassword;
	int     m_Type;
	SYSTEMTIME m_Date;
	 int    nMargin;
	 HCURSOR m_hCursor;
	 short   m_nMaxChars;
	 BOOL   m_bTimerEnd;
};

