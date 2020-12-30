#pragma once

#include <string>
#include "qydefine.h"

class QYUI_EXPORT_CLASS QYHeapBuffer
{
public:
	QYHeapBuffer();
	QYHeapBuffer(unsigned int newSize);
	~QYHeapBuffer();
	char* operator & ();
	int Length();
private:
	char *m_pBuffer;
};