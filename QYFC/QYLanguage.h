#pragma once

#include "qydefine.h"
#include "QYString.h"
#include "QYPropertyList.h"

class QYWindow;
class QYUI_EXPORT_CLASS QYLanguage
{
public:
    static QYLanguage* instance();
    void setLang(std::string   langKey);
	void setLang(std::string   langKey,QYWindow *pWnd);
	const char* getText(const char *msgID);
    void setLangDir(const char *langDir);
    void addLang(const char *langKey);
	std::string getLangID();
	static BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam);
private:
	void loadString();
    static QYLanguage* m_pInstance;
	std::string  m_langKey;
    std::string  m_langDir;
	QYPropertyList m_stringMap;
	static QYPropertyList m_langMap;
};

