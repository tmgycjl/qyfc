#include "QYSqlite3.h"
#include "QYString.h"



#ifndef _USED_IN_WINXP

#pragma comment(lib,"libcrypto_static.lib")
#ifdef _DEBUG
#pragma comment(lib,"sqlcipher-d")
#else
#pragma comment(lib,"sqlcipher")
#endif
const char sqliteKey[] = "tmgycjl";
#else
#ifdef _DEBUG
#pragma comment(lib,"sqlite3-d")
#else
#pragma comment(lib,"sqlite3")
#endif
#endif


QYSqlite3RecordRow::QYSqlite3RecordRow()
{
	

}


QYSqlite3RecordRow::~QYSqlite3RecordRow()
{

}

BOOL QYSqlite3RecordRow::SetField(const char* fdName, const char* fdValue)
{
	std::pair<std::string, std::string> pairField = std::make_pair(fdName, "");
	if (nullptr != fdValue)
	{
		pairField = std::make_pair(fdName, fdValue);
	}
	
	m_vectorField.push_back(pairField);

	return TRUE;
}


std::string QYSqlite3RecordRow::GetField(const char* fdName)
{
	for (auto it = m_vectorField.begin(); it != m_vectorField.end(); it++)
	{
		if (it->first == fdName)
		{
			return it->second;
		}
	}
	return "";
}



QYSqlite3Record::QYSqlite3Record()
{
	m_currentRow = m_vectorRow.end();
}


QYSqlite3Record::~QYSqlite3Record()
{
    clear();
}


void QYSqlite3Record::clear()
{
    for (auto it = m_vectorRow.begin(); it != m_vectorRow.end(); it++)
    {
        SAFE_DELETE(*it);
    }

    m_vectorRow.clear();
}

BOOL QYSqlite3Record::AddRow(QYSqlite3RecordRow *pRow)
{
	if (nullptr == pRow)
	{
		return FALSE;
	}

	if (m_vectorRow.end() != std::find(m_vectorRow.begin(), m_vectorRow.end(), pRow))
	{
		return FALSE;
	}

	m_vectorRow.push_back(pRow);

	return TRUE;
}


void QYSqlite3Record::MoveFirst()
{
	m_currentRow = m_vectorRow.begin();
}

void QYSqlite3Record::MoveNext()
{
	m_currentRow++;
}

BOOL QYSqlite3Record::IsEnd()
{
	return (m_currentRow == m_vectorRow.end());
}

std::string QYSqlite3Record::GetField(const char* fdName)
{
	if (IsEnd())
	{
		return "";
	}

	if (m_currentRow != m_vectorRow.end())
	{
		QYSqlite3RecordRow *record = *m_currentRow;
		return record->GetField(fdName);
	}
	
	return "";
}

QYSqlite3::QYSqlite3()
{
	m_DB = nullptr;
}


QYSqlite3::~QYSqlite3()
{
	Close();
}


BOOL QYSqlite3::Open(LPCTSTR strDBFilePath)
{
	char filePath[MAX_PATH] = { 0 };

	QYString::SafeW2AUTF8(filePath, MAX_PATH, strDBFilePath);

	int result = sqlite3_open(filePath, &m_DB);
	if (result != SQLITE_OK)
	{
		return FALSE;
	}

	return TRUE;
}

int QYSqlite3::Close()
{
	int ret = SQLITE_OK;
	if (nullptr != m_DB)
	{
		int ret = sqlite3_close(m_DB);
		m_DB = nullptr;
	}

	return ret;
}


int QYSqlite3::Exec(const char* sql, int(*pSqlite3Callback)(void*, int, char**, char**), void *context, char **errorMsg)
{
	return sqlite3_exec(m_DB, sql, pSqlite3Callback, context, errorMsg);
}

int QYSqlite3::GetTable(const char* sql, QYSqlite3Record *pRecord, char **errorMsg)
{
	char **dbResult = nullptr;
	int row = 0;
	int col = 0;
	int res = SQLITE_OK;
	if (SQLITE_OK != (res = (sqlite3_get_table(m_DB, sql, &dbResult, &row, &col, errorMsg))))
	{
		return res;
	}

	int index = col; 

	for (int i = 0; i < row; i++)
	{
		QYSqlite3RecordRow *pRow = new QYSqlite3RecordRow;

		for (int j = 0; j < col; j++)
		{
			pRow->SetField(dbResult[j], dbResult[index]);
			++index;
		}

		pRecord->AddRow(pRow);
	}

	sqlite3_free_table(dbResult);

	return res;
}


void QYSqlite3::FreeTable(char **dbResult)
{
	return sqlite3_free_table(dbResult);
}