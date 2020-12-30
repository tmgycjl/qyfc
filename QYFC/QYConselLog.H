
#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "qydefine.h"

#define LOGFILE(s)	(__FILE__ " %d :\t" s)
class QYUI_EXPORT_CLASS QYConsoleLog
{
public:
    static const int ToDebug;
    static const int ToFile;
    static const int ToConsole;

	QYConsoleLog(int mode = ToDebug, int level = 1, LPSTR filename = NULL, bool append = false);

    inline void Print( LPSTR format, ...) 
	{
#ifdef _DEBUG
		if (1 > m_level) return;
		va_list ap;
		va_start(ap, format);
		ReallyPrint(format, ap);
		va_end(ap);
#endif // !_DEBUG

    }
    
    // Change the log level
    void SetLevel(int level);
    int  GetLevel();

    // Change the logging mode
    void SetMode(int mode);
    int  GetMode();

	// Change the appearence of log records
	enum Style {
		TIME_INLINE = 0x01,
		NO_FILE_NAMES = 0x02,
		NO_TAB_SEPARATOR = 0x04
	};
	void SetStyle(int style);
	int GetStyle();

    // Change or set the logging filename.  This enables ToFile mode if
    // not already enabled.
    void SetFile(const char *filename, bool append = false);

	virtual ~QYConsoleLog();

private:
	void ReallyPrintLine(char *line);
    void ReallyPrint(char *format, va_list ap);
	void OpenFile();
    void CloseFile();
    bool m_tofile, m_todebug, m_toconsole;
    int m_level;
    int m_mode;
	int m_style;
    HANDLE hlogfile;
	char *m_filename;
	bool m_append;

	// Path prefix to remove from log records
	char *m_prefix;
	size_t m_prefix_len;

	time_t m_lastLogTime;
};


