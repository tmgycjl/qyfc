#pragma once

#include "qydefine.h"

class QYUI_EXPORT_CLASS QYRelative
{
public:
	QYRelative();
	~QYRelative();

	QYRelative(const QYRelative &rel)
	{
		this->m_X = rel.m_X;
		this->m_Y = rel.m_Y;
	}

	QYRelative(double X, double Y)
	{
		this->m_X = X;
		this->m_Y = Y;
	}

	QYRelative(double rel)
	{
		this->m_X = rel;
		this->m_Y = rel;
	}

	QYRelative& operator = (double rel)
	{
		this->m_X = rel;
		this->m_Y = rel;

		return *this;
	}

	double m_X;
	double m_Y;
};

