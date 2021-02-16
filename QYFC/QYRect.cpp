#include "QYRect.h"



QYRect::~QYRect()
{
}


void QYRect::calculateDisplayRect(int width, int height)
{
	float rcW = this->right - this->left;
	float rcH = this->bottom - this->top;
	float ratioImage = (float)width / (float)height;
	float ratioWnd = (float)rcW / (float)rcH;
	float off = 0;
	float realXY = 0;
	if (ratioImage > ratioWnd)
	{
		realXY = rcW / ratioImage;

		off = (rcH - realXY) / 2;
		this->top += off;
		this->bottom = this->top + realXY;
	}
	else
	{
		realXY = rcH * ratioImage;

		off = (rcW - realXY) / 2;
		this->left += off;
		this->right = this->left + realXY;
	}
}