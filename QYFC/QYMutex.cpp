#include "QYMutex.h"


QYMutex::QYMutex()
{
	InitializeCriticalSection(&m_Mutex);
}


QYMutex::~QYMutex()
{
	DeleteCriticalSection(&m_Mutex);
}


void QYMutex::Lock()
{
	EnterCriticalSection(&m_Mutex);
}

void QYMutex::Unlock()
{
	LeaveCriticalSection(&m_Mutex);
}

