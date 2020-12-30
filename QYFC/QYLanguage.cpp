
#include "QYLanguage.h"
#include "QYFileStudio.h"
#include "QYWindow.h"
#include "QYToolBar.h"
#include "QYDateTable.h"
#include <fstream>

QYLanguage* QYLanguage::m_pInstance = nullptr;
QYPropertyList QYLanguage::m_langMap;

static int childWndCount = 0;
BOOL CALLBACK QYLanguage::EnumChildProc(HWND hwndChild, LPARAM lParam)
{
	QYWindow *pWindow = (QYWindow*)GetWindowLong(hwndChild, GWL_USERDATA);
	if (nullptr != pWindow)
	{
		TCHAR className[1024] = { 0 };
		::GetClassName(hwndChild, className, 1024);
		if (0 == SafeStrcmp(className, L"QYStatic") 
			|| 0 == SafeStrcmp(className, L"QYButton")
			|| 0 == SafeStrcmp(className, L"QYDialog")
			|| 0 == SafeStrcmp(className, L"QYToolBar")
			|| 0 == SafeStrcmp(className, L"QYHeaderCtrl")
			|| 0 == SafeStrcmp(className, L"QYDateTable"))
		{
			pWindow->updateText();
		}
	}

	return TRUE;
}

QYLanguage* QYLanguage::instance()
{
	if (nullptr == m_pInstance)
	{
        m_pInstance = new QYLanguage;

		
	}

	return m_pInstance;
}

void QYLanguage::setLang(std::string   langKey)
{
	m_langKey = langKey;
	loadString();

	QYDateTable::SetLanguge(langKey);
}

void QYLanguage::setLang(std::string langKey,QYWindow *pWnd)
{
	m_langKey = langKey;
	loadString();

	QYDateTable::SetLanguge(langKey);

	if (nullptr != pWnd)
	{
		pWnd->updateText();
		::EnumChildWindows(pWnd->GetHwnd(), EnumChildProc, (LPARAM)this);
	}
	
	
}


void QYLanguage::setLangDir(const char *langDir)
{
    m_langDir = langDir;
}

std::string QYLanguage::getLangID()
{
	return m_langKey;
}

void QYLanguage::addLang(const char *langKey)
{
    m_langMap.addProperty(langKey, langKey);
}

const char* QYLanguage::getText(const char *msgID)
{
	const char *text =  m_stringMap.getValueString(msgID);
	if (nullptr == text)
	{
		return msgID;
	}

	return text;
}

void QYLanguage::loadString()
{
#if 1
	char lanPath[1024] = { 0 };
	sprintf_s(lanPath, "%s\\%s\\string.csv", m_langDir.c_str(), m_langMap.getValueWithDefaultString(m_langKey, "en").c_str());

	int stringCount = 0;
	std::ifstream inFile( lanPath );
	if ( !inFile.is_open() )
	{
		return;
	}
	else
	{
		m_stringMap.clearProperty();

		for ( int x = 0; !inFile.eof(); x++ )
		{
			std::string line;			std::string idValue;
			std::string strValue;

			std::getline( inFile, line );
			auto posIDValueEnd = line.find( "," );
			if ( std::string::npos == posIDValueEnd )
			{
				continue;
			}
			else
			{
				idValue.append( line, 0, posIDValueEnd );
				strValue.append( line, posIDValueEnd + 1, line.length() - posIDValueEnd - 1 );
				m_stringMap.addProperty(idValue, strValue);
			}
		}
		inFile.close();
	}
#else

	char lanPath[1024] = { 0 };
    sprintf_s(lanPath, "%s\\%s\\string.po", m_langDir.c_str(), m_langMap.getValueWithDefaultString(m_langKey, "en").c_str());

	FILE *pFile = nullptr;
	if (0 != fopen_s(&pFile, lanPath, "r"))
	{
		return;
	}

	if (nullptr != pFile)
	{
		char *readBuf = nullptr;
		int readLen = 0;
		int leaveLen = 0;
		char msgid[] = "msgid";
		char msgstr[] = "msgstr";
		char *pID = nullptr;
		char *pIDEnd = nullptr;
		char szKey[1024] = { 0 };
		char szValue[1024] = { 0 };

		m_stringMap.clearProperty();

		fseek(pFile, 0, SEEK_END);
		int fileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
	
		readBuf = new char[fileSize];

		do
		{
			readLen = fread(readBuf, 1, fileSize, pFile);

			if (0 < readLen)
			{
				char *pID = strstr(readBuf, msgid);
				//DBG( pID );

				while (nullptr != pID)
				{
					pID = strchr(pID, '\"');

					if (nullptr != pID)
					{
						pIDEnd = strchr(pID + 1, '\"');

						if (nullptr != pIDEnd)
						{
							memset(szKey, 0, 1024);
							memcpy(szKey, pID + 1, pIDEnd - pID - 1);
							//DBG( szKey );
							pID = pIDEnd + 1;

							if (nullptr != pID)
							{
								pID = strstr(pID, msgstr);

								if (nullptr != pID)
								{
									pID = strchr(pID, '\"');

									if (nullptr != pID)
									{
										pIDEnd = strchr(pID + 1, '\"');

										if (nullptr != pIDEnd)
										{
											memset(szValue, 0, 1024);
											memcpy(szValue, pID + 1, pIDEnd - pID - 1);
											// DBG( szValue );
											m_stringMap.addProperty(szKey, szValue);
										}
									}

									pID = pIDEnd + 1;
								}

							}
						}

					}

					pID = strstr(pID, msgid);
				}
			}
		} while (0);

		delete[] readBuf;

		fclose(pFile);
		return;
	}

#endif
}