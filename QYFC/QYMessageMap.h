#pragma once


#include "qydefine.h"
#include "QYPropertyList.h"
#include <map>
#include <functional>

struct w_funcCB_t
{
    std::function<void(QYPropertyList*)> callback;
};

struct listenerCB_t
{
    std::function<void(void)> callback;
};


class QYUI_EXPORT_CLASS QYMessageMap
{
public:
	QYMessageMap(void);
	~QYMessageMap(void);

    void registerCallback(const std::string &purpose, w_funcCB_t *funCB);
    w_funcCB_t *getCallback(const std::string &purpose);

protected:
    std::map<std::string, w_funcCB_t*> m_callbacks;
};
