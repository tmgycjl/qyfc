#include "QYHeapBuffer.h"

QYHeapBuffer::QYHeapBuffer()
{
	m_pBuffer = nullptr;
}

QYHeapBuffer::QYHeapBuffer(unsigned int newSize)
{
	if (0 < newSize && newSize < 10 * 1024 * 1024)
	{
		m_pBuffer = new char[newSize];
		memset(m_pBuffer, 0, newSize);
	}
}


QYHeapBuffer::~QYHeapBuffer()
{
	if (nullptr != m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}

}

char* QYHeapBuffer::operator & ()
{
	return this->m_pBuffer;
}

int QYHeapBuffer::Length()
{
	return strlen(m_pBuffer);
}