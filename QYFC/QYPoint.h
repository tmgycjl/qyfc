#pragma once

#include "qydefine.h"

class QYUI_EXPORT_CLASS QYPoint
{
public:
	QYPoint(){ ; }
	QYPoint(POINT pt){ this->x = pt.x; this->y = pt.y; }
	QYPoint(int x, int y){ this->x = x; this->y = y; }


	QYPoint(POINT *pt){ this->x = pt->x; this->y = pt->y; }

	QYPoint& operator = (POINT point){ this->x = point.x; this->y = point.y; return *this; }
public:
	int x;
	int y;
};
