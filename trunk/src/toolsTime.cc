/***************************************************************************
 *            toolsTime.cc
 *
 *  Sun Nov 27 05:05:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libdodo/toolsTime.h>

using namespace dodo;

tools::__time::__time() : sec(0),
					   min(0),
					   hour(0),
					   day(1),
					   month(1),
					   year(1990),
					   daylight(true)
{
}

//-------------------------------------------------------------------

tools::__time::__time(unsigned int a_sec,
				   unsigned int a_min,
				   unsigned int a_hour,
				   unsigned int a_day,
				   unsigned int a_month,
				   unsigned int a_year,
				   bool a_daylight) : sec(a_sec),
									  min(a_min),
									  hour(a_hour),
									  day(a_day),
									  month(a_month),
									  year(a_year),
									  daylight(a_daylight)
{
}

//-------------------------------------------------------------------

dodoString
tools::time::byFormat(const dodoString &format,
					long timestamp,
					bool local)
{
	tm *tTime;

	if (local)
		tTime = ::localtime((const time_t *)&timestamp);
	else
		tTime = gmtime((const time_t *)&timestamp);

	if (tTime == NULL)
		throw baseEx(ERRMODULE_TOOLSTIME, TIMEEX_GETBYFORMAT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	char formatted[30];

	strftime(formatted, 30, format.c_str(), tTime);

	return formatted;
}

//-------------------------------------------------------------------

long
tools::time::now()
{
	time_t tTime = ::time(NULL);
	if (tTime == (time_t)-1)
		throw baseEx(ERRMODULE_TOOLSTIME, TIMEEX_NOW, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return tTime;
}

//-------------------------------------------------------------------

dodoStringArray
tools::time::week(long date,
				const dodoString &format,
				bool local)
{
	long daynum = string::stringToL(tools::time::byFormat("%w", date, local));
	if (daynum == 0)
		daynum = 7;

	dodoStringArray week;
	long mon = date - (daynum - 1) * 86400;

	for (short int i(0); i < 7; ++i, mon += 86400)
		week.push_back(tools::time::byFormat(format, mon, local));

	return week;
}

//-------------------------------------------------------------------

dodoStringArray
tools::time::dates(long dateFrom,
					long dateTo,
					const dodoString &format,
					bool local)
{
	dodoStringArray result;

	if ((dateFrom == dateTo) || (dateFrom - dateTo < 86400))
	{
		result.push_back(tools::time::byFormat(format, dateFrom, local));
		return result;
	}

	if (dateFrom > dateTo)
	{
		long tmp = dateFrom;
		dateFrom = dateTo;
		dateTo = tmp;
	}

	while (dateFrom < dateTo)
	{
		result.push_back(tools::time::byFormat(format, dateFrom, local));
		dateFrom += 86400;
	}
	result.push_back(tools::time::byFormat(format, dateTo, local));

	return result;
}

//-------------------------------------------------------------------

long
tools::time::timestamp(const __time &timeInfo)
{
	tm tTime;

	tTime.tm_sec = timeInfo.sec;
	tTime.tm_min = timeInfo.min;
	tTime.tm_hour = timeInfo.hour;
	tTime.tm_mday = timeInfo.day;
	tTime.tm_mon = timeInfo.month - 1;
	tTime.tm_year = timeInfo.year;
	tTime.tm_isdst = timeInfo.daylight ? 1 : 0;

	return ::mktime(&tTime);
}

//-------------------------------------------------------------------

tools::__time
tools::time::timestamp(long seconds,
					bool local)
{
	tm *tTime;

	if (local)
		tTime = ::localtime((const time_t *)&seconds);
	else
		tTime = gmtime((const time_t *)&seconds);

	if (tTime == NULL)
		throw baseEx(ERRMODULE_TOOLSTIME, TIMEEX_MAKETIME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__time timeInfo;

	timeInfo.sec = tTime->tm_sec;
	timeInfo.min = tTime->tm_min;
	timeInfo.hour = tTime->tm_hour;
	timeInfo.day = tTime->tm_mday;
	timeInfo.month = tTime->tm_mon + 1;
	timeInfo.year = tTime->tm_year;
	timeInfo.daylight = tTime->tm_isdst > 0 ? true : false;

	return timeInfo;
}

//-------------------------------------------------------------------

unsigned short int
tools::time::daysInMonth(unsigned int year,
					   unsigned short int month)
{
	unsigned short int day(0);

	switch (month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:

			day = 31;

			break;

		case 4:
		case 6:
		case 9:
		case 11:

			day = 30;

			break;

		case 2:

			unsigned int isleap = 0;
			if (year % 4 == 0)
			{
				isleap = 1;
				if (year % 100 == 0 && year % 400 != 0)
					isleap = 0;
			}
			if (isleap == 1)
				day = 29;
			else
				day = 28;
			break;

	}

	return day;
}

//-------------------------------------------------------------------

long
tools::time::byFormat(const dodoString &format,
					const dodoString &dt)
{
	tm tTime;
	strptime(dt.c_str(), format.c_str(), &tTime);

	return ::mktime(&tTime);
}

//-------------------------------------------------------------------

