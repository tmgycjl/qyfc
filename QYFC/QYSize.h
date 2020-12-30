#pragma once

#include "qydefine.h"


class QYUI_EXPORT_CLASS QYSize
{
public:
	QYSize()
	{
		this->cx = 0;
		this->cy = 0;
	}
	QYSize(SIZE size)
	{
		this->cx = size.cx;
		this->cy = size.cy;
	}
	QYSize(int size)
	{
		this->cx = size;
		this->cy = size;
	}
	QYSize(int cx, int cy)
	{
		this->cx = cx;
		this->cy = cy;
	}
	QYSize(QYSize &size)
	{
		this->cx = size.cx;
		this->cy = size.cy;
	}

	~QYSize(){ ; }

	QYSize& operator = (int size)
	{
		this->cx = size;
		this->cy = size;

		return *this;
	}

	QYSize& operator += (QYSize &size)
	{
		this->cx += size.cx;
		this->cy += size.cy;

		return *this;
	}

	int cx;
	int cy;
private:

};
