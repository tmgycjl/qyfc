#include "QYMessageMap.h"

QYMessageMap::QYMessageMap(void)
{
}

QYMessageMap::~QYMessageMap(void)
{
    m_callbacks.clear();
}

void QYMessageMap::registerCallback(const std::string &purpose, w_funcCB_t *funCB)
{
    if (nullptr != funCB && funCB->callback && !purpose.empty())   //insert will return false if map already exists
    {
        m_callbacks.insert(std::pair<std::string, w_funcCB_t*>(purpose, funCB));
    }
}

w_funcCB_t* QYMessageMap::getCallback(const std::string &purpose)
{
	if (m_callbacks.end() == m_callbacks.find(purpose))
	{
		return nullptr;
	}
    return m_callbacks.at(purpose);
}