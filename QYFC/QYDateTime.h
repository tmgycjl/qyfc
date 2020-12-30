#pragma once

#include "qydefine.h"

#define   MONTHS_IN_YEAR  12
#define   DAYS_IN_WEEK  7
#define   WEEK_NUMS_IN_MONTH  6
#define   MAX_DAYS_IN_MONTH   31
#define   HOURS_IN_DAY  24
#define   END_TIME_OF_DAY   235959
#define   MINIUTE_IN_HOUR   60
#define   MINIUTE_IN_DAY    (HOURS_IN_DAY * MINIUTE_IN_HOUR)
#define   SECONDS_IN_MINUTE   MINIUTE_IN_HOUR
#define   SECONDS_IN_HOUR  (MINIUTE_IN_HOUR * SECONDS_IN_MINUTE)
#define   SECONDS_IN_DAY    (HOURS_IN_DAY * SECONDS_IN_HOUR)

class QYUI_EXPORT_CLASS QYDate
{
public:
	QYDate(QYDate& date)
	{
		this->m_nDay = date.m_nDay;
		this->m_nYear = date.m_nYear;
		this->m_nMonth = date.m_nMonth;
	}

	QYDate(WORD year = 0, WORD month = 0, WORD day = 0)
	{
		m_nDay = day;
		m_nYear = year;
		m_nMonth = month;
	}
	virtual ~QYDate(){ ; }
	static int GetDaysFromTowDates(unsigned int  date1, unsigned int date2);
	static int GetDaysByYear(int year);
	static int GetStartDayOfWeekByYearAndMonth(int year, int month);
	static int GetDayOfWeekByDate(int year, int month, int day);
	static BOOL IsWeekEnd(int year, int month, int day);
	static int GetDaysByYearAndMonth(int year, int month);

	WORD GetYear(){ return m_nYear; }
	WORD GetMonth(){ return m_nMonth; }
	WORD GetDay(){ return m_nDay; }
	void SetDate(WORD year = 0, WORD month = 0, WORD day = 0);
	void LastMonth();
	void NextMonth();
	BOOL operator == (QYDate& date)
	{
		return (this->m_nYear == date.GetYear() && this->m_nMonth == date.GetMonth() && this->m_nDay == date.GetDay());
	}
	QYDate& operator -- ();
	QYDate& operator ++ ();
protected:
	WORD  m_nYear;
	WORD  m_nMonth;
	WORD  m_nDay;
private:
};


class QYUI_EXPORT_CLASS QYTime
{
public:
	QYTime(WORD hour = 0, WORD mintue = 0, WORD second = 0)
	{
		m_nHour = hour;
		m_nMinute = mintue;
		m_nSecond = second;
	}
	virtual ~QYTime(){ ; }
	

	WORD GetHour(){ return m_nHour; }
	WORD GetMinute(){ return m_nMinute; }
	WORD GetSecond(){ return m_nSecond; }
	void SetTime(WORD hour = 0, WORD mintue = 0, WORD second = 0);
protected:
	WORD  m_nHour;
	WORD  m_nMinute;
	WORD  m_nSecond;
private:
};


class QYUI_EXPORT_CLASS QYDateTime : public QYDate, public QYTime
{
public:
	QYDateTime::QYDateTime(WORD year = 0, WORD month = 0, WORD day = 0,WORD hour = 0, WORD mintue = 0, WORD second = 0) 
		:QYDate(year, month, day), 
		QYTime(hour, mintue, second)
	{

	}

	virtual ~QYDateTime(){ ; }
	
	

	QYDateTime& operator = (QYDateTime& DateTime);
	 int  operator - ( QYDateTime &DateTime);

	static void GetLocalTime(QYDateTime *DateTime);
	
	static int GetDates(QYDateTime *startDate,QYDateTime *endDate,QYDateTime *date,int maxDates );
	static int GetDuration(unsigned int timeBegin, unsigned int timeEnd);
	void  OffDate( int offday);
	void  OffTime( int offsecond);
	unsigned int GetInitegerTime();
	unsigned int GetInitegerDate();
	unsigned int GetDays();
	unsigned int GetTimeStamp(unsigned int originYear = 2018);

};

