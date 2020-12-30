#include "QYCaptionCtrl.h"
#include "QYDC.h"
#include "qycolor.h"

QYCaptionCtrl::QYCaptionCtrl()
{
}


QYCaptionCtrl::~QYCaptionCtrl()
{
}

void QYCaptionCtrl::Draw(HDC hDC, int left, int top, int right, int bottom, LPCTSTR strCaption, COLORREF color)
{
	QYRect rct(left, top, right, bottom);

	Draw(hDC, rct, strCaption,color);
}

void QYCaptionCtrl::Draw(HDC hDC, QYRect  &rect, LPCTSTR strCaption, COLORREF color)
{
	QYFont ft(QY_THEME.CAPTION_FONT_SIZE_2, 0, FW_HEAVY, QY_THEME.CAPTION_FONT_TYPE_2);

	HFONT hOldFont = (HFONT)::SelectObject(hDC, ft.m_hObject);

	QYDC dc(hDC);
	QYSize size = dc.GetTextExtent(strCaption);

	dc.DrawText(strCaption, SafeStrlen(strCaption), &rect, DT_SINGLELINE | DT_TOP | DT_LEFT);

	int splitterLen = (size.cx + 20 > 150) ? (size.cx + 50 ) : 150;

	rect.top = rect.top + size .cy+ 5;
	rect.right = rect.left + splitterLen;
	rect.bottom = rect.top + 3;

	dc.FillSolidRect(rect, color);

	::SelectObject(hDC, hOldFont);
	::DeleteObject(ft.m_hObject);
}