#pragma once

#include "qydefine.h"
#include "QYWindow.h"
#include "qystring.h"
#include "QYSize.h"

class QYUI_EXPORT_CLASS QYGDIObject
{
public:
	HGDIOBJ    m_hObject;
	QYGDIObject()
	{
		m_hObject = nullptr;
	}
	virtual ~QYGDIObject()
	{
		m_hObject = nullptr;
	}

	BOOL DeleteObject()
	{ 
	//	g_ConselLog.Print(0, "delete :%d,Object:%d\n", this, m_hObject);
		SAFE_DELETE_OBJECT(m_hObject);
		return TRUE;
	}

	
};

class QYUI_EXPORT_CLASS QYPen : public QYGDIObject
{
public:
	QYPen()
	{

	}
	QYPen(HGDIOBJ pen)
	{
		m_hObject = pen;
	}
	QYPen(int style,int width,COLORREF color)
	{
		m_hObject = ::CreatePen(style, width, color);
	}

	virtual ~QYPen()
	{

	}

	BOOL CreatePen(int style, int width, COLORREF color)
	{ 
		m_hObject = ::CreatePen(style, width, color);
		return (nullptr != m_hObject);
	}
};


class QYUI_EXPORT_CLASS QYBrush : public QYGDIObject
{
public:
	QYBrush()
	{

	}
	QYBrush(COLORREF  color)
	{
		m_hObject = (HGDIOBJ)::CreateSolidBrush(color);
		//g_ConselLog.Print(0, "create brush:%d,Object:%d\n", this, m_hObject);
	}
	QYBrush(HGDIOBJ brush)
	{
		m_hObject = brush;
	}

	virtual ~QYBrush()
	{
	}

	 BOOL GetBrush(int i)
	{
		return (nullptr != (m_hObject = GetStockObject(i)));
	}

	BOOL CreateSolidBrush(COLORREF  color)
	{
		
		m_hObject = ::CreateSolidBrush(color); 
	//	g_ConselLog.Print(0, "create brush:%d,Object:%d\n", this, m_hObject);
		return (nullptr != m_hObject); 
	}
};

class QYUI_EXPORT_CLASS QYFont : public QYGDIObject
{
public:
	QYFont()
	{

	}
	QYFont(_In_ int cHeight,
		_In_ int cWidth,
		_In_ int cWeight = FW_NORMAL,
		_In_opt_  LPCTSTR pszFaceName = L"MS Sans Serif",
		_In_ DWORD bUnderline = 0,
		_In_ int cEscapement = 0,
		_In_ int cOrientation = 0,
		_In_ DWORD bItalic = 0,
		_In_ DWORD bStrikeOut = 0,
		_In_ DWORD iCharSet = ANSI_CHARSET,
		_In_ DWORD iOutPrecision = OUT_TT_PRECIS,
		_In_ DWORD iClipPrecision = CLIP_DEFAULT_PRECIS,
		_In_ DWORD iQuality = DEFAULT_QUALITY,
		_In_ DWORD iPitchAndFamily = VARIABLE_PITCH | FF_SWISS)
	{
		m_hObject = ::CreateFont(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, \
			bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
	}
	QYFont(HGDIOBJ font)
	{
		m_hObject = font;
	}

	virtual ~QYFont()
	{
	}
	
	inline BOOL CreateFont(_In_ int cHeight,
		_In_ int cWidth,
		_In_ int cWeight = FW_NORMAL,
		_In_opt_ LPCTSTR pszFaceName = L"MS Shell Dlg 2",
		_In_ DWORD bUnderline = 0,
		_In_ int cEscapement = 0, 
		_In_ int cOrientation = 0,
		_In_ DWORD bItalic = 0,
		_In_ DWORD bStrikeOut = 0,
		_In_ DWORD iCharSet = ANSI_CHARSET,
		_In_ DWORD iOutPrecision = OUT_TT_PRECIS,
		_In_ DWORD iClipPrecision = CLIP_DEFAULT_PRECIS,
		_In_ DWORD iQuality = DEFAULT_QUALITY,
		_In_ DWORD iPitchAndFamily = VARIABLE_PITCH | FF_SWISS)
	{
		m_hObject = ::CreateFont(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, \
			bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);

		return (nullptr != m_hObject);
	}
};


enum QY_ARROW_POINT
{
	QY_ARROW_POINT_LEFT = 0,
	QY_ARROW_POINT_RIGHT,
	QY_ARROW_POINT_UP,
	QY_ARROW_POINT_DOWN,
};

class QYUI_EXPORT_CLASS QYDC
{
protected:
	HDC   m_hDC;
public:
	QYDC()
	{
	
	}
	QYDC(HDC hDC)
	{
		m_hDC = hDC;
	}
	virtual ~QYDC()
	{
		
	}
	HDC GetDC(){ return m_hDC; }
	BOOL  DeleteDC(){ return ::DeleteDC(m_hDC); }
	inline HGDIOBJ SelectObject(QYGDIObject *object){ return ::SelectObject(m_hDC, object->m_hObject); }
	inline HGDIOBJ SelectObject(HGDIOBJ hobject){ return ::SelectObject(m_hDC, hobject); }

	inline BOOL Rectangle( _In_ int left, _In_ int top, _In_ int right, _In_ int bottom,HBRUSH brush)
	{
		HBRUSH oldbrush = (HBRUSH)::SelectObject(m_hDC, brush);
		BOOL ret = ::Rectangle(m_hDC, left, top, right, bottom);
		::SelectObject(m_hDC, oldbrush);
		return ret;
	}


	inline BOOL Rectangle(QYRect &rect, HBRUSH brush)
	{
		HBRUSH oldbrush = (HBRUSH)::SelectObject(m_hDC, brush);
		BOOL ret = ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
		::SelectObject(m_hDC, oldbrush);
		return ret;
	}

	inline BOOL Rectangle(QYRect &rect, COLORREF  brushColor,COLORREF penColor)
	{
		HBRUSH brush = ::CreateSolidBrush(brushColor);
		HPEN pen = ::CreatePen(PS_SOLID, 1, penColor);
		HBRUSH oldbrush = (HBRUSH)::SelectObject(m_hDC, brush);
		HPEN oldpen = (HPEN)::SelectObject(m_hDC, pen);
		BOOL ret = ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
		::SelectObject(m_hDC, oldbrush);
		::DeleteObject(brush);
		::SelectObject(m_hDC, oldpen);
		::DeleteObject(pen);
		return ret;
	}



	inline BOOL Rectangle(_In_ int left, _In_ int top, _In_ int right, _In_ int bottom)
	{
		return ::Rectangle(m_hDC, left, top, right, bottom);
	}

	inline BOOL Rectangle(QYRect &rect)
	{
		return ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
	}
	inline BOOL  Ellipse( __in int left, __in int top, __in int right, __in int bottom)
	{
		return ::Ellipse(m_hDC, left, top, right, bottom);
	}
	inline BOOL RoundRect( __in int left, __in int top, __in int right, __in int bottom, __in int width, __in int height)
	{
		return ::RoundRect(m_hDC, left, top, right, bottom, width, height);
	}
	inline BOOL RoundRect(QYRect &rect ,QYPoint point)
	{
		return ::RoundRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom, point.x, point.y);
	}
	inline BOOL RoundRect(QYRect &rect, __in int width, __in int height)
	{
		return ::RoundRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom, width, height);
	}
	inline BOOL  Ellipse(QYRect &rect)
	{
		return ::Ellipse(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
	}

	inline int FillRect(_In_ CONST QYRect &rc, QYBrush &Brush)
	{
		RECT rc2;
		rc2.left = rc.left;
		rc2.top = rc.top;
		rc2.right = rc.right;
		rc2.bottom = rc.bottom;

		return ::FillRect(m_hDC, &rc2, (HBRUSH)Brush.m_hObject);
	}

	inline int FillRect(_In_ CONST QYRect &rc, HBRUSH brush)
	{
		RECT rc2;
		rc2.left = rc.left;
		rc2.top = rc.top;
		rc2.right = rc.right;
		rc2.bottom = rc.bottom;

		return ::FillRect(m_hDC, &rc2, brush);
	}

	

	inline int FillRect( int left,int top,int right,int bottom, QYBrush &Brush)
	{
		RECT rc2;
		rc2.left = left;
		rc2.top = top;
		rc2.right = right;
		rc2.bottom = bottom;

		return ::FillRect(m_hDC, &rc2, (HBRUSH)Brush.m_hObject);
	}



	inline int FillSolidRect(_In_ CONST QYRect &rc, COLORREF color)
	{
		QYBrush Brush(color);
		int ret =  FillRect(rc, Brush);
		Brush.DeleteObject();
		return ret;
	}

	inline int FillSolidRect(int left, int top, int right, int bottom, COLORREF color)
	{
		QYBrush Brush(color);
		int ret = FillRect(left,top,right,bottom, Brush);
		Brush.DeleteObject();
		return ret;
	}

	inline BOOL  LineTo(_In_ int x, _In_ int y)
	{
		return ::LineTo(m_hDC, x, y);
	}
	inline BOOL MoveToEx(_In_ int x, _In_ int y, _Out_opt_ QYPoint *lppt)
	{
		POINT pt;
		pt.x = lppt->x;
		pt.y = lppt->y;
		return ::MoveToEx(m_hDC, x, y, &pt);
	}

	inline UINT   SetTextAlign(_In_ UINT align)
	{
		return ::SetTextAlign(m_hDC, align);
	}

	 inline COLORREF  SetTextColor(_In_ COLORREF color)
	 {
		 return ::SetTextColor(m_hDC, color);
	}

	 inline int   SetBkMode(_In_ int mode)
	 {
		return ::SetBkMode(m_hDC, mode);
	}
	inline BOOL  TextOut(_In_ int x, _In_ int y, _In_reads_(c) const TCHAR * lpString)
	{
		return ::TextOut(m_hDC, x, y, lpString, SafeStrlen(lpString));
	}
	inline int DrawText(
		_When_((format & DT_MODIFYSTRING), _At_((const TCHAR *)lpchText, _Inout_grows_updates_bypassable_or_z_(cchText, 4)))
		_When_((!(format & DT_MODIFYSTRING)), _In_bypassable_reads_or_z_(cchText))
		const TCHAR * lpchText,
		_In_ int cchText,
		_Inout_ QYRect* lprc,
		_In_ UINT format)
	{
		RECT rc2;
		rc2.left = lprc->left;
		rc2.top = lprc->top;
		rc2.right = lprc->right;
		rc2.bottom = lprc->bottom;
		return ::DrawText(m_hDC, lpchText, cchText, &rc2, format | DT_NOPREFIX);
	}

	void DrawMultiText(TCHAR * lpchText, QYRect &rect, int rowSpac = 2);

	inline BOOL  BitBlt( __in int x, __in int y, __in int cx, __in int cy, QYDC *pDc, __in int x1, __in int y1, __in DWORD rop)
	{
		return ::BitBlt(m_hDC, x, y, cx, cy, pDc->GetDC(), x1, y1, rop);
	}
	inline BOOL DrawIcon(_In_ int X, _In_ int Y, _In_ HICON hIcon)
	{
		return ::DrawIcon(m_hDC, X, Y, hIcon);
	}
	inline BOOL  DrawIconEx(
		__in int xLeft,
		__in int yTop,
		__in HICON hIcon,
		__in int cxWidth,
		__in int cyWidth,
		__in UINT istepIfAniCur = 0,
		__in_opt HBRUSH hbrFlickerFreeDraw = nullptr,
		__in UINT diFlags = DI_NORMAL)
	{
		return ::DrawIconEx(m_hDC, xLeft, yTop, hIcon, cxWidth, cyWidth, istepIfAniCur, hbrFlickerFreeDraw, diFlags);
	}
	inline QYSize GetTextExtent(const TCHAR *string,int len = -1)
	{
		SIZE size;
		if (-1 == len)
		{
			::GetTextExtentPoint32(m_hDC, string, SafeStrlen(string), &size);
		}
		else
		{	
			::GetTextExtentPoint32(m_hDC, string, len, &size);
		}

		return size;
	}

	inline BOOL GetTextMetrics(TEXTMETRIC *tm)
	{
		return ::GetTextMetrics(m_hDC, tm);
	}

	void DrawArrow(QYRect *rect, QY_ARROW_POINT arrowPt,int width,COLORREF color);
};

class QYUI_EXPORT_CLASS QYPaintDC : public QYDC
{
public:
	QYPaintDC(QYWindow * const pWnd)
	{
		m_hwnd = pWnd->GetHwnd();
		m_hDC = BeginPaint(m_hwnd, &m_ps);
	}
	virtual ~QYPaintDC()
	{
		EndPaint(m_hwnd, &m_ps);
	}

	
private:
	PAINTSTRUCT m_ps;
	HWND        m_hwnd;
};

class QYUI_EXPORT_CLASS QYMemDC : public QYDC
{
public:
	QYMemDC(QYDC *pDc, QYRect rect);
	QYMemDC(HDC hDC, QYRect rect);
	virtual ~QYMemDC();
protected:
private:
	HBITMAP m_bitmapBk;
	HBITMAP m_OldBitmap;
};

