#pragma once
#include "qydefine.h"

class QYUI_EXPORT_CLASS QYEvent
{
public:
	QYEvent()
	{
		m_hEvent = NULL;
	}
	~QYEvent()
	{
		SAFE_CLOSE(m_hEvent);
	}
	BOOL Create(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE)
	{
		m_hEvent = ::CreateEvent(NULL, bManualReset, bInitialState, NULL);
		return (m_hEvent != NULL);
	}
	BOOL Reset()
	{
		return ::ResetEvent(m_hEvent);
	}
	BOOL Set()
	{
		return ::SetEvent(m_hEvent);
	}
	BOOL Wait(DWORD dwMilliseconds = INFINITE)
	{
		return (::WaitForSingleObject(m_hEvent, dwMilliseconds) == WAIT_OBJECT_0);
	}
private:
	HANDLE m_hEvent;
};