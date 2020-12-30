#pragma once

#include "qydefine.h"

class QYUI_EXPORT_CLASS QYMutex
{
public:
	QYMutex();
	~QYMutex();
	void Lock();
	void Unlock();
private:
	CRITICAL_SECTION  m_Mutex;
};


class QYUI_EXPORT_CLASS QYJAutoLock
{
public:
	QYJAutoLock(QYMutex &mutex)
		:m_Mutex(mutex)
	{
		m_Mutex.Lock();
	}
	~QYJAutoLock()
	{
		m_Mutex.Unlock();
	}
private:
	QYMutex &m_Mutex;
};
