#pragma once
#include "includedef.h"
#include "QYWindow.h"
#include "QYpicture.h"

enum
{
	SHADOW_ID_LEFT_TOP,
	SHADOW_ID_LEFT_BOTTOM,
	SHADOW_ID_RIGHT_TOP,
	SHADOW_ID_RIGHT_BOTTOM,
	SHADOW_ID_LEFT,
	SHADOW_ID_TOP,
	SHADOW_ID_RIGHT,
	SHADOW_ID_BOTTOM,
	SHADOW_ID_SMALL_LEFT,
	SHADOW_ID_SMALL_TOP,
	SHADOW_ID_SMALL_RIGHT,
	SHADOW_ID_SMALL_BOTTOM,
	SHADOW_ID_SMALL_LEFT_TOP,
	SHADOW_ID_SMALL_LEFT_BOTTOM,
	SHADOW_ID_SMALL_RIGHT_TOP,
	SHADOW_ID_SMALL_RIGHT_BOTTOM,
	SHADOW_ID_END
};

enum
{
	SHADOW_ON_RIGHT_BOTTOM,
	SHADOW_ON_WHOLE,
};

class QYUI_EXPORT_CLASS QYShadowBorder
{
public:
	QYShadowBorder();
	~QYShadowBorder();
	BOOL Create();
	void Release();
	void Draw(HDC hDC,int left,int top,int right,int bottom);
	void Draw(HDC hDC, QYRect &rect);
private:
	QYPicture  *m_picShadow;
};

