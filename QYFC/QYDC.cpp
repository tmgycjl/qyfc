#include "QYDC.h"


void QYDC::DrawMultiText(TCHAR * lpchText, QYRect &rect, int rowSpac )
{
	TCHAR szSubText[1024] = { 0 };
	int length = SafeStrlen(lpchText);
	int i = 0;
	int nFirst = 0;
	QYRect rectTemp = rect;

	QYSize size = GetTextExtent(lpchText);
	if (size.cx > rectTemp.Width())
	{
		TCHAR *pStart = lpchText;
		for (; i < length; i++)
		{
			memset(szSubText, 0, sizeof(TCHAR) * 1024);
			memcpy(szSubText, pStart + nFirst, (i - nFirst) * sizeof(TCHAR));
			//strTemp = (*it)->strDesc.Mid(nFirst, i - nFirst);
			size = GetTextExtent(szSubText);
			if (size.cx > rectTemp.Width())
			{
				DrawText(szSubText, SafeStrlen(szSubText) - 2, &rectTemp, DT_LEFT | DT_TOP);
				rectTemp.OffsetRect(0, size.cy + rowSpac);
				nFirst = i - 2;
			}
		}

		if (0 < i - nFirst)
		{
			memcpy(szSubText, pStart + nFirst, (i - nFirst) * sizeof(TCHAR));
			DrawText(szSubText, SafeStrlen(szSubText), &rectTemp, DT_LEFT | DT_TOP);
		}
		
	}
	else
	{
		DrawText(lpchText, length, &rectTemp, DT_LEFT | DT_TOP);
	}
}

void QYDC::DrawArrow(QYRect *rect, QY_ARROW_POINT arrowPt, int width, COLORREF color)
{
	HPEN hBtnPen = CreatePen(PS_SOLID, width, color);

	int sideLen = 0;
	RECT rectReal;
	QYPoint pt; 
	HPEN oldpen =  (HPEN)SelectObject(hBtnPen);

	if (QY_ARROW_POINT_LEFT == arrowPt)
	{
		sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
		sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
		rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
		rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
		rectReal.bottom = rectReal.top + sideLen;
		rectReal.right = rectReal.left + sideLen;
		
		MoveToEx(rectReal.right, rectReal.top, &pt);
		LineTo(rectReal.right - sideLen / 2, rectReal.top + sideLen / 2);

		LineTo(rectReal.right, rectReal.bottom);
	}
	else if (QY_ARROW_POINT_RIGHT == arrowPt)
	{
		sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
		sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
		rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
		rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
		rectReal.bottom = rectReal.top + sideLen;
		rectReal.right = rectReal.left + sideLen;

		MoveToEx(rectReal.left, rectReal.top, &pt);
		LineTo(rectReal.left + sideLen / 2, rectReal.top + sideLen / 2);
		LineTo(rectReal.left, rectReal.bottom);
	}
	else if (QY_ARROW_POINT_UP == arrowPt)
	{

	}
	else if (QY_ARROW_POINT_DOWN == arrowPt)
	{

	}

	SelectObject( oldpen);
	DeleteObject(hBtnPen);
}

QYMemDC::QYMemDC(QYDC *pDc, QYRect rect)
{
	m_hDC = ::CreateCompatibleDC(pDc->GetDC());

	m_bitmapBk = (HBITMAP)::CreateCompatibleBitmap(pDc->GetDC(), rect.Width(), rect.Height());

	m_OldBitmap = (HBITMAP)::SelectObject(m_hDC,m_bitmapBk);
}

QYMemDC::QYMemDC(HDC hDC, QYRect rect)
{
	m_hDC = ::CreateCompatibleDC(hDC);

	m_bitmapBk = (HBITMAP)::CreateCompatibleBitmap(hDC, rect.Width(), rect.Height());

	m_OldBitmap = (HBITMAP)::SelectObject(m_hDC, m_bitmapBk);
}

QYMemDC::~QYMemDC()
{
	//SelectObject(m_pOldBitmap);
	::DeleteDC(this->GetDC());
	::DeleteObject(m_bitmapBk);
}

