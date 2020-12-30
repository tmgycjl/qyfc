#pragma once



#define  _USED_IN_WINXP   1

#ifdef _USED_IN_WINXP
#include "../sqlite3/sqlite3.h"
#else
#include "../sqlcipher/sqlite3.h"
#endif

#include "qydefine.h"

#include <string>
#include <vector>
#include "QYPropertyList.h"

class QYUI_EXPORT_CLASS QYSqlite3RecordRow
{
public:
	QYSqlite3RecordRow();
	virtual ~QYSqlite3RecordRow();
	BOOL SetField(const char* fdName, const char* fdValue);
	std::string GetField(const char* fdName);
	int GetCount(){ return m_vectorField.size(); }
private:
	std::vector<std::pair<std::string, std::string>> m_vectorField;
};


class QYUI_EXPORT_CLASS QYSqlite3Record
{
public:
	QYSqlite3Record();
	virtual ~QYSqlite3Record();
	BOOL AddRow(QYSqlite3RecordRow *pRow);
	std::string GetField(const char* fdName);
	void MoveFirst();
	void MoveNext();
	BOOL IsEnd();
	int GetCount(){ return m_vectorRow.size(); }
    void clear();
private:
	std::vector<QYSqlite3RecordRow*> m_vectorRow;
	std::vector<QYSqlite3RecordRow*>::iterator m_currentRow;
};


class QYUI_EXPORT_CLASS QYSqlite3
{
public:
	QYSqlite3();
	virtual ~QYSqlite3();
	BOOL Open(LPCTSTR strDBFilePath);
	int Close();
	int Exec(const char* sql,int (*pSqlite3Callback)(void*, int, char**, char**) ,void *context ,char **errorMsg);
	int GetTable(const char* sql, QYSqlite3Record *pRecord ,char **errorMsg);
	int GetTable(const char* sql, QYPropertyList *pPropertyList, char **errorMsg);
	void FreeTable(char **dbResult);
private:
	sqlite3 *m_DB;
};

