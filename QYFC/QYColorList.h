#pragma once

#include "qydefine.h"
#include <vector>

class QYUI_EXPORT_CLASS QYColorList
{
public:
	QYColorList();
	virtual ~QYColorList();
	int  AddColor(COLORREF color);
private:
	std::vector<COLORREF> m_vectorColor;
};

