#pragma once

#include "QYWindow.h"

enum Button_state
{
	button_state_normal,
	button_state_hot,
	button_state_down,
	button_state_disable
};

enum ButtonState  { bsNormal, bsHot, bsDown, bsDisable };

class QYUI_EXPORT_CLASS QYButton :
	public QYWindow
{
public:
	QYButton();
	virtual ~QYButton();
	virtual BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnLButtonDown(UINT nFlags, QYPoint point);
	BOOL OnLButtonUp(UINT nFlags, QYPoint point);
	BOOL LoseFocus();
	BOOL OnMouseMove(UINT nFlags, QYPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	BOOL SetImage(UINT uResID, int states = IMAGE_STATUS_PRESS);
	BOOL setImage(const char *imagePath, int states = IMAGE_STATUS_PRESS);
	BOOL SetImage(QYPicture *pImage);
	void setFlag(int flag){ m_Flag = flag; }
    void Enable(BOOL bEnable = TRUE);
	enum ButtonFlag
	{
		BUTTON_DEFAULT = 0,
		BUTTON_TRANSPRENT_BACKGROUND,
		BUTTON_LEFT_ICON,
		BUTTON_TOP_ICON
	};
protected:
	BOOL        m_bDown;
	BOOL        m_bHover;
	int         m_Flag;            //是否释放关联按钮的CDrawButton对象
	int         m_Style;           //按钮风格
	BOOL	    m_bGetText;			//是否已经获取了文本
	BOOL        m_bFocus;
	BOOL        m_bDisable;
	BOOL        m_showBorder = TRUE;
	QYPicture  *m_pImage;
};

