#include "QYShadowBorder.h"


QYShadowBorder::QYShadowBorder()
{
	m_picShadow = nullptr;
}


QYShadowBorder::~QYShadowBorder()
{
	Release();
}


BOOL QYShadowBorder::Create()
{
	m_picShadow = new QYPicture[SHADOW_ID_END];

	BOOL bRet = TRUE;
	
	m_picShadow[SHADOW_ID_LEFT_TOP].Load(IDP_SMALL_SHADOW_LEFT_TOP, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_LEFT_BOTTOM].Load(IDP_SMALL_SHADOW_LEFT_BOTTOM, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_RIGHT_TOP].Load(IDP_SMALL_SHADOW_RIGHT_TOP, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_RIGHT_BOTTOM].Load(IDP_SMALL_SHADOW_RIGHT_BOTTOM, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_TOP].Load(IDP_SMALL_SHADOW_TOP, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_BOTTOM].Load(IDP_SMALL_SHADOW_BOTTOM, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_RIGHT].Load(IDP_SMALL_SHADOW_RIGHT, QY_IMAGE_PNG);
	m_picShadow[SHADOW_ID_LEFT].Load(IDP_SMALL_SHADOW_LEFT, QY_IMAGE_PNG);

	return  TRUE;
}

void QYShadowBorder::Release()
{
	SAFE_DELETE_ARRAY(m_picShadow);
}

void QYShadowBorder::Draw(HDC hDC, int left, int top, int right, int bottom)
{
	QYRect rct(left,  top,  right,  bottom);

	Draw(hDC, rct);
}

void QYShadowBorder::Draw(HDC hDC, QYRect &rect)
{
	QYPicture *p = m_picShadow + SHADOW_ID_LEFT_TOP;

	int alignLen = 0;
#if 1
	int hornLen = p->GetWidth();
	p->Draw(hDC,
		0, 0, p->GetWidth(), p->GetHeight(),
		0, 0, p->GetWidth(), p->GetHeight());

	p = m_picShadow + SHADOW_ID_LEFT_BOTTOM;
	p->Draw(hDC,
		0, rect.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
		0, 0, p->GetWidth(), p->GetHeight());
	
	p = m_picShadow + SHADOW_ID_RIGHT_TOP;
	p->Draw(hDC,
		rect.Width() - p->GetWidth(), 0, p->GetWidth(), p->GetHeight(),
		0, 0, p->GetWidth(), p->GetHeight());
	p = m_picShadow + SHADOW_ID_RIGHT_BOTTOM;
	p->Draw(hDC,
		rect.Width() - p->GetWidth(), rect.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
		0, 0, p->GetWidth(), p->GetHeight());

	p = m_picShadow + SHADOW_ID_TOP;

	int left = hornLen;
	int drawLen = rect.Width() - 2 * hornLen;
	while (drawLen > 0)
	{
		p->Draw(hDC,
			left, 0, drawLen, p->GetHeight(),
			0, alignLen, p->GetWidth(), p->GetHeight());

		drawLen -= p->GetWidth();
		left += p->GetWidth();
	}
	

	p = m_picShadow + SHADOW_ID_BOTTOM;

	 left = hornLen;
	 drawLen = rect.Width() - 2 * hornLen;
	while (drawLen > 0)
	{
		p->Draw(hDC,
			hornLen, rect.Height() - p->GetHeight() + alignLen, rect.Width() - 2 * hornLen, p->GetHeight(),
			0, 0, p->GetWidth(), p->GetHeight());

		drawLen -= p->GetWidth();
		left += p->GetWidth();
	}

	

	p = m_picShadow + SHADOW_ID_LEFT;
	int top = hornLen;
	drawLen = rect.Height() - 2 * hornLen;
	while (drawLen > 0)
	{
		p->Draw(hDC,
			0, hornLen, p->GetWidth(), rect.Height() - 2 * hornLen,
			alignLen, 0, p->GetWidth(), p->GetHeight());

		drawLen -= p->GetHeight();
		top += p->GetHeight();
	}

	p = m_picShadow + SHADOW_ID_RIGHT;
	top = hornLen;
	drawLen = rect.Height() - 2 * hornLen;
	while (drawLen > 0)
	{
		p->Draw(hDC,
			rect.Width() - p->GetWidth() + alignLen, hornLen, p->GetWidth(), rect.Height() - 2 * hornLen,
			0, 0, p->GetWidth(), p->GetHeight());

		drawLen -= p->GetHeight();
		top += p->GetHeight();
	}


#endif
}