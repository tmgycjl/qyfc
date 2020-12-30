#pragma once

#include "qydefine.h"
#include <vector>
#include "QYPropertyList.h"

enum enumXMLTableType
{
	XML_TABLE_TYPE_ROOT = 0,
	XML_TABLE_TYPE_DATA,
	XML_TABLE_TYPE_ATTRIBUTE 
};

class QYUI_EXPORT_CLASS QYXMLTreeNode
{
public:
	QYXMLTreeNode()
	{
		type = XML_TABLE_TYPE_ROOT;
		parent = nullptr;
		currentNode = nullptr;
	}

	std::string getAttribute(std::string name){ return mapAttribute.getValueWithDefaultString(name, ""); }
	int getElemCount(){ return vectorChildren.size(); }
	std::vector <QYXMLTreeNode*>* getChildrenList(){ return &vectorChildren; }
	char        type;
	std::string name;
	std::string data;
	std::vector <QYXMLTreeNode*> vectorChildren;
	QYXMLTreeNode *parent;
	QYXMLTreeNode *currentNode;
	QYPropertyList mapAttribute;
};


class QYUI_EXPORT_CLASS QYXML
{
public:
	QYXML();
	~QYXML();
	enum
	{
		modeWrite = 0,
		modeRead,
		modeAppend
	};

	bool Save(const char *strFileName);
	bool Load(const char *strFileName);
	void Clear(QYXMLTreeNode  *pRoot);
	bool  AddElem(char  *name, char *data );
	bool  AddElem(char  *name, int value );
	bool  IntoElem();
	bool  OutElem();
	int  Print(char **ppBuffer);
	bool Parse(char *pBuffer);
	QYXMLTreeNode* GetElem(char  *name);
	QYXMLTreeNode* GetNextElem();
	QYXMLTreeNode* GetFirstElem();
	QYXMLTreeNode* GetRoot(){ return m_pRoot; }
	const char* GetAttribute(QYXMLTreeNode *pNode, const char *name);
private:
	int  Print(char *pBuffer, QYXMLTreeNode *pRoot);
	int  Parse(const char *pBuffer, QYXMLTreeNode *pRoot);
	int  ParseAttribute(QYXMLTreeNode *pNode, const char *pBuffer);
	void Trim(std::string &str);
	void Trim(std::string &str,char c);
	bool CheckTag(const char *pBuffer);
	QYXMLTreeNode  *m_pRoot;
	QYXMLTreeNode  *m_pCurrentNode;
	QYXMLTreeNode  *m_pCurrentParentNode;
	int    m_nMaxBuff;
};

