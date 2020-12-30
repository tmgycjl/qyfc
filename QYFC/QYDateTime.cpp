#include "QYDateTime.h"


 void QYDateTime::GetLocalTime(QYDateTime *DateTime)
{
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	DateTime->SetDate(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	DateTime->SetTime(sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
}

 void QYTime::SetTime(WORD hour, WORD mintue, WORD second)
 {
	 m_nHour = hour;
	 m_nMinute = mintue;
	 m_nSecond = second;
 }

 void QYDate::SetDate(WORD year, WORD month, WORD day)
{
	if (0 != year)
	{
		m_nYear = year;
	}

	if (0 != month)
	{
		m_nMonth = month;
	}

	if (0 != day)
	{
		m_nDay = day;
	}
}

 int QYDate::GetDaysByYear(int year)
{
	int days = 0;
	for (int i = 1; i <= MONTHS_IN_YEAR; i++)
	{
		days += GetDaysByYearAndMonth(year, i);
	}

	return days;
}

int QYDate::GetStartDayOfWeekByYearAndMonth(int year, int month)
{
	int dayofweek = 0;
	int totaldays = 0;

	int day = 1;

	int i = 0;
	for (i = 1900; i < year; i++)
	{
		totaldays += GetDaysByYear(i);
	}

	for (i = 1; i < month; i++)
	{
		totaldays += GetDaysByYearAndMonth(year, i);
	}

	//totaldays++;

	dayofweek = totaldays % 7;

	return dayofweek;
}

int QYDate::GetDayOfWeekByDate(int year, int month, int day)
{
	int dayofweek = GetStartDayOfWeekByYearAndMonth(year, month);
	int days = GetDaysByYearAndMonth(year, month);
	if (days < day)
	{
		return 0;
	}

		for (int i = 1; i < day; i++)
		{
			if (dayofweek >= DAYS_IN_WEEK - 1)
			{
				dayofweek = 0;
			}
			else
			{
				dayofweek++;
			}
		}
	

	return dayofweek;
}

int QYDate::GetDaysByYearAndMonth(int year, int month)
{
	switch (month)
	{
	case 2:
	{
		if (((0 == year % 4) && (0 != year % 100))
			|| (0 == year % 400))
		{
			return 29;
		}
		else
		{
			return 28;
		}
	}
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	default:
		return 30;
	}
}

int QYDate::GetDaysFromTowDates(unsigned int  date1, unsigned int date2)
{
	return 0;
}

BOOL  QYDate::IsWeekEnd(int year, int month, int day)
{
	int dayofweek = GetDayOfWeekByDate(year, month, day);
	if (0 == dayofweek || 6 == dayofweek)
	{
		return TRUE;
	}

	return FALSE;
}

int QYDateTime::GetDates(QYDateTime *startDate, QYDateTime *endDate, QYDateTime *date, int maxDates)
{
	int count = 0;

	for (int year = startDate->m_nYear; year <= endDate->m_nYear; year++)
	{
		if (startDate->m_nYear < endDate->m_nYear)
		{
			if (year == startDate->m_nYear)
			{
				for (int month = startDate->m_nMonth; month <= MONTHS_IN_YEAR; month++)
				{
					int dayCount = GetDaysByYearAndMonth(year, month);
					if (month == startDate->m_nMonth)
					{
						for (int day = startDate->m_nDay; day <= dayCount; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{
								date[count++].SetDate(year, month, day);
							}
							
						}
					}
					else
					{
						for (int day = 1; day <= dayCount; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
				}
			}
			else if (year == endDate->m_nYear)
			{
				for (int month = 1; month <= endDate->m_nMonth; month++)
				{
					int dayCount = GetDaysByYearAndMonth(year, month);
					if (month == endDate->m_nMonth)
					{
						for (int day = 1; day <= endDate->m_nDay; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
					else
					{
						for (int day = 1; day <= dayCount; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
				}
			}
			else
			{
				for (int month = 1; month <= MONTHS_IN_YEAR; month++)
				{
					int dayCount = GetDaysByYearAndMonth(year, month);
					for (int day = 1; day <= dayCount; day++)
					{
						if (maxDates <= count)
						{
							return 0;
						}
						else
						{date[count++].SetDate(year, month, day);}
					}
				}
			}

		}
		else
		{
			for (int month = startDate->m_nMonth; month <= endDate->m_nMonth; month++)
			{
				if (startDate->m_nMonth < endDate->m_nMonth)
				{
					int dayCount = GetDaysByYearAndMonth(year, month);
					if (month == startDate->m_nMonth)
					{
						for (int day = startDate->m_nDay; day <= dayCount; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
					else if (month == endDate->m_nMonth)
					{
						for (int day = 1; day <= endDate->m_nDay; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
					else
					{
						for (int day = 1; day <= dayCount; day++)
						{
							if (maxDates <= count)
							{
								return 0;
							}
							else
							{date[count++].SetDate(year, month, day);}
						}
					}
				}
				else
				{
					for (int day = startDate->m_nDay; day <= endDate->m_nDay; day++)
					{
						if (maxDates <= count)
						{
							return 0;
						}
						else
						{date[count++].SetDate(year, month, day);}
					}
				}
			}
		}

	}

	return count;
}

void QYDateTime::OffDate(int offday)
{

}

void QYDateTime::OffTime(int offsecond)
{
	unsigned int nTime = m_nHour * 3600 + m_nMinute * 60 + m_nSecond;
	nTime += offsecond;

	m_nHour = nTime / 3600;
	nTime %= 3600;
	m_nMinute = nTime / 60;
	nTime %= 60;
	m_nSecond = nTime;
}

unsigned int QYDateTime::GetInitegerTime()
{
	return (m_nHour * 10000 + m_nMinute * 100 + m_nSecond);
}

unsigned int QYDateTime::GetInitegerDate()
{
	return (m_nYear * 10000 + m_nMonth * 100 + m_nDay);
}


QYDateTime& QYDateTime::operator = (QYDateTime& DateTime)
{
	this->m_nDay = DateTime.m_nDay;
	this->m_nYear = DateTime.m_nYear;
	this->m_nMonth = DateTime.m_nMonth;
	this->m_nHour = DateTime.m_nHour;
	this->m_nMinute = DateTime.m_nMinute;
	this->m_nSecond = DateTime.m_nSecond;

	return *this;
}


QYDate& QYDate::operator -- ()
{
	int days = GetDaysByYearAndMonth(m_nYear, m_nMonth);

	if (1 == m_nDay)
	{
		if (1 == m_nMonth)
		{
			m_nYear--;
			m_nMonth = MONTHS_IN_YEAR;
		}
		else
		{
			m_nMonth--;
		}
		m_nDay = GetDaysByYearAndMonth(m_nYear, m_nMonth);
	}
	else
	{
		m_nDay--;
	}

	return *this;
}

QYDate& QYDate::operator ++ ()
{
	int days = GetDaysByYearAndMonth(m_nYear, m_nMonth);

		if (days == m_nDay)
		{
			if (MONTHS_IN_YEAR == m_nMonth)
			{
				m_nYear++;
				m_nMonth = 1;
			}
			else
			{
				m_nMonth++;
			}
			m_nDay = 1;
		}
		else
		{
			m_nDay++;
		}


	return *this;
}


void  QYDate::LastMonth()
{
		if (1 == m_nMonth)
		{
			m_nYear--;
			m_nMonth = MONTHS_IN_YEAR;
		}
		else
		{
			m_nMonth--;
		}
		m_nDay = GetDaysByYearAndMonth(m_nYear, m_nMonth);
}

void QYDate::NextMonth()
{
		if (MONTHS_IN_YEAR == m_nMonth)
		{
			m_nYear++;
			m_nMonth = 1;
		}
		else
		{
			m_nMonth++;
		}
		m_nDay = 1;
}

 int QYDateTime::operator - ( QYDateTime &DateTime)
{
	return (this->GetTimeStamp() - DateTime.GetTimeStamp());
}

int QYDateTime::GetDuration(unsigned int timeBegin, unsigned int timeEnd)
{
	if (timeBegin >= timeEnd)
	{
		return 0;
	}
	int duration = 0;

	duration += ((timeEnd / 10000) - (timeBegin / 10000)) * 3600;
	duration += (((timeEnd % 10000) / 100) - ((timeBegin % 10000) / 100)) * 60;
	duration += ((timeEnd % 100) - (timeBegin % 100));

	return duration;
}

unsigned int QYDateTime::GetDays()
{
	unsigned int days = 0;
	int originMonth = 1;
	int originDay = 1;
	int leaveSecond = 0;
	int originYear = 0;

	for (int year = originYear; year <= m_nYear; year++)
	{
		if (m_nYear == year)
		{
			for (int month = 1; month <= m_nMonth; month++)
			{
				if (month == m_nMonth)
				{
					for (int day = 1; day <= m_nDay; day++)
					{
						days++;
					}
				}
				else
				{
					days += GetDaysByYearAndMonth(year, month);
				}

			}
		}
		else
		{
			for (int month = 1; month <= MONTHS_IN_YEAR; month++)
			{
				days += GetDaysByYearAndMonth(year, month);
			}
		}

	}

	return days;
}

unsigned int QYDateTime::GetTimeStamp(unsigned int originYear)
{
	unsigned int timeStamp = 0;
	int originMonth = 1;
	int originDay = 1;
	int leaveSecond = 0;

	for (int year = originYear; year <= m_nYear;year++)
	{
		if (m_nYear == year)
		{
			for (int month = 1; month <= m_nMonth; month++)
			{
				if (month == m_nMonth)
				{
					for (int day = 1; day <= m_nDay; day++)
					{
						if (day == m_nDay)
						{
							leaveSecond = m_nHour * SECONDS_IN_HOUR + m_nMinute * SECONDS_IN_MINUTE + m_nSecond;
						}
						else
						{
							timeStamp++;
						}
					}
				}
				else
				{
					timeStamp += GetDaysByYearAndMonth(year, month);
				}
				
			}
		}
		else
		{
			for (int month = 1; month <= MONTHS_IN_YEAR; month++)
			{
				timeStamp += GetDaysByYearAndMonth(year, month);
			}
		}
		
	}

	timeStamp *= SECONDS_IN_DAY ;
	timeStamp += leaveSecond;

	return timeStamp;
}