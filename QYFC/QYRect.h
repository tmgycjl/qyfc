#pragma once

#include "QYPoint.h"


class QYUI_EXPORT_CLASS QYRect
{
public:
	QYRect()
	{
		this->left = 0;
		this->right = 0;
		this->bottom = 0;
		this->top = 0;
	}
	QYRect(const QYRect &rect)
	{
		this->left = rect.left;
		this->right = rect.right;
		this->bottom = rect.bottom;
		this->top = rect.top;
	}
	QYRect(int rect)
	{
		this->left = rect;
		this->right = rect;
		this->bottom = rect;
		this->top = rect;
	}
	QYRect(RECT rect)
	{
		this->left = rect.left;
		this->right = rect.right;
		this->bottom = rect.bottom;
		this->top = rect.top;
	}
	QYRect(int left,int top,int right,int bottom)
	{
		this->left = left;
		this->right = right;
		this->bottom = bottom;
		this->top = top;
	}
	~QYRect();
	inline bool PtInRect(int x, int y){ return left <= x && right >= x && top <= y && bottom >= y; }
	inline bool PtInRect(QYPoint &pt){ return left <= pt.x && right >= pt.x && top <= pt.y && bottom >= pt.y; }
	inline bool PtInRect(POINT &pt){ return left <= pt.x && right >= pt.x && top <= pt.y && bottom >= pt.y; }
	int Width(){ return right - left; }
	int Height(){ return bottom - top; }
	inline void OffsetRect(int x, int y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}
	inline void DeflateRect(int x, int y)
	{
		left += x;
		right -= x;
		top += y;
		bottom -= y;
	}
	inline void InflateRect(int x, int y)
	{
		left -= x;
		right += x;
		top  -= y;
		bottom += y;
	}

	
	inline QYRect& operator = (RECT &rect)
	{
		this->left = rect.left;
		this->right = rect.right;
		this->bottom = rect.bottom;
		this->top = rect.top;

		return *this;
	}

	inline QYRect& operator = (QYRect &rect)
	{
		this->left = rect.left;
		this->right = rect.right;
		this->bottom = rect.bottom;
		this->top = rect.top;

		return *this;
	}

	inline QYRect& operator = (int rect)
	{
		this->left = rect;
		this->right = rect;
		this->bottom = rect;
		this->top = rect;
		return *this;
	}

	inline QYRect& operator += (QYRect &rect)
	{
		this->left += rect.left;
		this->right += rect.right;
		this->bottom += rect.bottom;
		this->top += rect.top;
		return *this;
	}

	inline QYRect operator + (QYRect &rect)
	{
		QYRect rectDes;
		rectDes.left = this->left + rect.left;
		rectDes.right = this->right + rect.right;
		rectDes.bottom = this->bottom + rect.bottom;
		rectDes.top = this->top + rect.top;
		return rectDes;
	}

	inline BOOL operator == (QYRect &rect)
	{
		if (this->left == rect.left&&
			this->right == rect.right&&
			this->bottom == rect.bottom&&
			this->top == rect.top)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	inline BOOL operator != ( QYRect &rect)
	{
		if (this->left == rect.left&&
			this->right == rect.right&&
			this->bottom == rect.bottom&&
			this->top == rect.top)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	inline BOOL operator != ( int rect)
	{
		if (this->left == rect&&
			this->right == rect&&
			this->bottom == rect&&
			this->top == rect)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	inline BOOL operator == (int rect)
	{
		if (this->left == rect&&
			this->right == rect&&
			this->bottom == rect&&
			this->top == rect)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void calculateDisplayRect(int width, int height);

	int left;
	int top; 
	int right;
	int bottom;
};

