
#include "QYXML.h"
#include <windows.h>

#pragma  comment(lib,"expat")

#include <iostream>
#include "expat.h"

#ifdef XML_LARGE_SIZE
#  define XML_FMT_INT_MOD "ll"
#else
#  define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
#  include <wchar.h>
#  define XML_FMT_STR "ls"
#else
#  define XML_FMT_STR "s"
#endif

static void XMLCALL
startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	QYXMLTreeNode *root = (QYXMLTreeNode*)userData;
	QYXMLTreeNode *pNode = nullptr;

	pNode = new QYXMLTreeNode;

	if (nullptr != root->currentNode)
	{
		pNode->parent = root->currentNode;
	}
	else
	{
		pNode->parent = root;
	}
	
	pNode->name = name;

	for (int i = 0; atts[i] != 0; i += 2)
	{
		pNode->mapAttribute.addProperty(atts[i], atts[i + 1]);
	}

	if (nullptr != pNode->parent)
	{
		pNode->parent->vectorChildren.push_back(pNode);
	}
	
	root->currentNode = pNode;
}

static void XMLCALL
endElement(void *userData, const XML_Char *name) {

	QYXMLTreeNode *root = (QYXMLTreeNode*)userData;
	if (nullptr != root->currentNode)
	{
		root->currentNode = root->currentNode->parent;
	}
	
}

QYXML::QYXML()
{
	m_nMaxBuff = 4096;;
	
	m_pCurrentParentNode = nullptr;
	m_pCurrentNode = nullptr;

	m_pRoot = new QYXMLTreeNode;
}

QYXML::~QYXML(void)
{
	Clear(nullptr);
	
	SAFE_DELETE(m_pRoot);
}

void QYXML::Clear(QYXMLTreeNode  *pRoot)
{
	if (nullptr == pRoot )
	{
		m_pCurrentParentNode = nullptr;
		m_pCurrentNode = nullptr;
		pRoot = m_pRoot;
	}

	if (nullptr != pRoot)
	{
		for (QYXMLTreeNode *p : pRoot->vectorChildren)
		{
			//printf("%s\n",(*it)->name.c_str());
			Clear(p);
			delete p;
		}

		pRoot->vectorChildren.clear();
		pRoot->mapAttribute.clearProperty();
	}
}

bool  QYXML::AddElem(char  *name, char *data)
{
	int nSize = strlen(name);

	if (nullptr == name || 0 >= nSize)
	{
		return false;
	}

	if (nSize > m_nMaxBuff)
	{
		return false;
	}

	if (nullptr != data && strlen(data) > m_nMaxBuff)
	{
		return false;
	}

	if (nullptr == m_pCurrentParentNode)
	{
		if (nullptr == m_pRoot)
		{
			return false;
		}

		if (name == m_pRoot->name)
		{
			return false;
		}

		m_pRoot->name = name;
		if (nullptr == data)
		{
			m_pRoot->data = "";
		}
		else
		{
			m_pRoot->data = data;
			m_pRoot->type = XML_TABLE_TYPE_DATA;
		}
		m_pRoot->parent = nullptr;
		
		m_pCurrentNode = m_pRoot;
	}
	else
	{
		QYXMLTreeNode *pNode = new QYXMLTreeNode;

		pNode->name = name;
		if (nullptr == data)
		{
			pNode->data = "";
		}
		else
		{
			pNode->data = data;
			pNode->type = XML_TABLE_TYPE_DATA;
		}
		pNode->parent = m_pCurrentParentNode;

		m_pCurrentParentNode->vectorChildren.push_back(pNode);

		m_pCurrentNode = pNode;
	}

	 return true;
}

bool  QYXML::AddElem(char  *name, int value)
{
	int nSize = strlen(name);

	if (nullptr == name || 0 >= nSize)
	{
		return false;
	}
	
	if (nSize > m_nMaxBuff)
	{
		return false;
	}

	char cValue[128] = { 0 };
	sprintf_s(cValue, 128, "%d", value);
	
	if (nullptr == m_pCurrentParentNode)
	{
		if (nullptr == m_pRoot)
		{
			return false;
		}

		if (name == m_pRoot->name)
		{
			return false;
		}

		m_pRoot->name = name;
		m_pRoot->data = cValue;
		m_pRoot->parent = nullptr;
		m_pRoot->type = XML_TABLE_TYPE_DATA;
	}
	else
	{
		QYXMLTreeNode *pNode = new QYXMLTreeNode;

		pNode->name = name;
		pNode->data = cValue;
		pNode->parent = m_pCurrentParentNode;
		pNode->type = XML_TABLE_TYPE_DATA;
		m_pCurrentParentNode->vectorChildren.push_back(pNode);
		m_pCurrentNode = pNode;
	}

	return true;
}

bool  QYXML::IntoElem()
{
	if (m_pCurrentParentNode == nullptr)
	{
		if (nullptr != m_pRoot)
		{
			if (!m_pRoot->name.empty())
			{
				m_pCurrentParentNode = m_pRoot;
				return true;
			}
			
		}
		
	}
	else
	{
		m_pCurrentParentNode = m_pCurrentNode;
		return true;
	}

	return false;
}

bool  QYXML::OutElem()
{
	if (m_pCurrentParentNode == nullptr)
	{
		return false;
	}
	else
	{
		m_pCurrentNode = m_pCurrentParentNode;
		m_pCurrentParentNode = m_pCurrentNode->parent;
		return true;
	}

	return true;
}


QYXMLTreeNode* QYXML::GetElem(char  *name)
{
	if (nullptr == m_pCurrentParentNode)
	{
		return nullptr;
	}

	for (QYXMLTreeNode *p : m_pCurrentParentNode->vectorChildren)
	{
		if (name == p->name)
		{
			m_pCurrentNode = p;
			return  p;
		}
	}

	return nullptr;
}

QYXMLTreeNode* QYXML::GetFirstElem()
{
	if (nullptr == m_pCurrentParentNode || 0 >= m_pCurrentParentNode->vectorChildren.size())
	{
		return nullptr;
	}

	m_pCurrentNode = m_pCurrentParentNode->vectorChildren.front();
	return m_pCurrentNode;
}

QYXMLTreeNode* QYXML::GetNextElem()
{
	if (nullptr == m_pCurrentParentNode)
	{
		return nullptr;
	}

	for (auto it = m_pCurrentParentNode->vectorChildren.begin(); it != m_pCurrentParentNode->vectorChildren.end(); it++)
	{
		if (m_pCurrentNode == (*it))
		{
			it++;
			if (it != m_pCurrentParentNode->vectorChildren.end())
			{
				m_pCurrentNode = (*it);
				return  (*it);
			}
			else
			{
				return nullptr;
			}
			
		}
	}

	return nullptr;
}

const char* QYXML::GetAttribute(QYXMLTreeNode *pNode, const char *name)
{
	return pNode->mapAttribute.getValueWithDefaultString(name, "").c_str();
}

int  QYXML::Print(char *pBuffer, QYXMLTreeNode *pRoot)
{
	int size = 0;

	if (nullptr == pRoot)
	{
		pRoot = m_pRoot;
	}

	if (nullptr != pRoot)
	{
		for (auto it = pRoot->vectorChildren.begin(); it != pRoot->vectorChildren.end(); it++)
		{
			memcpy(pBuffer + size, "<", 1);
			size += 1;
			memcpy(pBuffer + size, (*it)->name.c_str(), (*it)->name.length());
			size += (*it)->name.length();

			if (XML_TABLE_TYPE_ATTRIBUTE != (*it)->type)
			{
				memcpy(pBuffer + size, ">", 1);
				size += 1;

				if (XML_TABLE_TYPE_ROOT == (*it)->type)
				{
					memcpy(pBuffer + size, "\r\n", 2);
					size += 2;

					size += Print(pBuffer + size, (*it));

					if ('\r' != *(pBuffer + size - 2) && '\n' != *(pBuffer + size - 1) && 0 < (*it)->vectorChildren.size())
					{
						memcpy(pBuffer + size, "\r\n", 2);
						size += 2;
					}
				}
				else
				{
					memcpy(pBuffer + size, (*it)->data.c_str(), (*it)->data.length());
					size += (*it)->data.length();
				}

				memcpy(pBuffer + size, "</", 2);
				size += 2;
				memcpy(pBuffer + size, (*it)->name.c_str(), (*it)->name.length());
				size += (*it)->name.length();
				memcpy(pBuffer + size, ">", 1);
				size += 1;

				if (0 < pRoot->vectorChildren.size())
				{
					memcpy(pBuffer + size, "\r\n", 2);
					size += 2;
				}
			}
			else
			{
				memcpy(pBuffer + size, " ", 1);
				size += 1;
				//write attribute

				memcpy(pBuffer + size, "/>", 2);
				size += 2;

				memcpy(pBuffer + size, "\r\n", 2);
				size += 2;
			}
		}
	}

	return size;
}

int QYXML::Print(char **ppBuffer)
{
	int size = 0;

	if (nullptr == m_pRoot)
	{
		return 0;
	}

	*ppBuffer = new char[4 * MB];
	memset(*ppBuffer, 0, 4 * MB);

	memcpy(*ppBuffer, "<", 1);
	size += 1;
	memcpy(*ppBuffer + size, m_pRoot->name.c_str(), m_pRoot->name.length());
	size += m_pRoot->name.length();

	if (XML_TABLE_TYPE_ATTRIBUTE != m_pRoot->type)
	{
		memcpy(*ppBuffer + size, ">", 1);
		size += 1;

		if (XML_TABLE_TYPE_ROOT == m_pRoot->type)
		{
			memcpy(*ppBuffer + size, "\r\n", 2);
			size += 2;

			size += Print(*ppBuffer + size, nullptr);
			if ('\r' != *(*ppBuffer + size - 2) && '\n' != *(*ppBuffer + size - 1) && 0 < m_pRoot->vectorChildren.size())
			{
				memcpy(*ppBuffer + size, "\r\n", 2);
				size += 2;
			}
		}
		else
		{
			memcpy(*ppBuffer + size, m_pRoot->data.c_str(), m_pRoot->data.length());
			size += m_pRoot->data.length();
		}

		memcpy(*ppBuffer + size, "</", 2);
		size += 2;
		memcpy(*ppBuffer + size, m_pRoot->name.c_str(), m_pRoot->name.length());
		size += m_pRoot->name.length();
		memcpy(*ppBuffer + size, ">", 1);
		size += 1;
	}
	else
	{
		memcpy(*ppBuffer + size, " ", 1);
		size += 1;
		//write attribute

		memcpy(*ppBuffer + size, "/>", 2);
		size += 2;
	}

	return size;
}

bool QYXML::Save(const char *strFileName)
{
	FILE *pFile = nullptr;

	char mode[10] = { 0 };

	if (0 != fopen_s(&pFile, strFileName, "w"))
	{
		return false;
	}

	char *writeBuf = nullptr;
	
	int size = Print(&writeBuf);

	try
	{
		fwrite(writeBuf, 1, size,pFile);
	}
	catch (...)
	{
		SAFE_CLOSE_FILE(pFile);
		return false;
	}
	
	SAFE_CLOSE_FILE(pFile);
	SAFE_DELETE_ARRAY(writeBuf);

	return true;
}

bool QYXML::Parse(char *pBuffer)
{
	char mode[10] = { 0 };

	XML_Parser parser = XML_ParserCreate(NULL);
	int done = 0;
	int len = 0;

	XML_SetUserData(parser, m_pRoot);
	XML_SetElementHandler(parser, startElement, endElement);
	do {
		done = len < sizeof(pBuffer);
		if (XML_Parse(parser, pBuffer, (int)len, done) == XML_STATUS_ERROR)
		{
			// 			fprintf(stderr, "%" XML_FMT_STR " at line %" XML_FMT_INT_MOD "u\n",
			// 				XML_ErrorString(XML_GetErrorCode(parser)),
			// 				XML_GetCurrentLineNumber(parser));
			XML_ParserFree(parser);
			return false;
		}
	} while (!done);
	XML_ParserFree(parser);

	return true;
}


bool QYXML::Load(const char *strFileName)
{
	FILE *pFile = nullptr;

	char mode[10] = { 0 };

	if (0 != fopen_s(&pFile, strFileName, "r"))
	{
		return false;
	}

	char buf[BUFSIZ] = { 0 };
	XML_Parser parser = XML_ParserCreate(NULL);
	int done;
	int depth = 0;

	XML_SetUserData(parser, m_pRoot);
	XML_SetElementHandler(parser, startElement, endElement);
	do {
		size_t len = fread(buf, 1, sizeof(buf), pFile);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, (int)len, done) == XML_STATUS_ERROR)
		{
// 			fprintf(stderr, "%" XML_FMT_STR " at line %" XML_FMT_INT_MOD "u\n",
// 				XML_ErrorString(XML_GetErrorCode(parser)),
// 				XML_GetCurrentLineNumber(parser));
			XML_ParserFree(parser);
			return false;
		}
	} while (!done);
	XML_ParserFree(parser);
	SAFE_CLOSE_FILE(pFile);

	return true;

	char *readBuf = nullptr;

	int pos = ftell(pFile);
	fseek(pFile, 0, SEEK_END);
	int size = ftell(pFile);
	fseek(pFile, pos, SEEK_SET);

	if (4*MB < size)
	{
		return false;
	}

	readBuf = new char[size];
	try
	{
		fread(readBuf, 1, size, pFile);
	}
	catch (...)
	{
		SAFE_CLOSE_FILE(pFile);
		return false;
	}

    Parse(readBuf);

	SAFE_CLOSE_FILE(pFile);
	SAFE_DELETE_ARRAY(readBuf);

	return true;
}