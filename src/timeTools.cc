/***************************************************************************
 *            timeTools.cc
 *
 *  Sun Nov 27 05:05:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <timeTools.h>

using namespace dodo;

__mktime::__mktime() : sec(0),
						min(0),
						hour(0),
						day(1),
						month(1),
						year(1990),
						daylight(true)
{
}

//-------------------------------------------------------------------

__mktime::__mktime(unsigned int a_sec, 
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

std::string 
timeTools::byFormat(const std::string &format, 
						long timestamp,
						bool local)
{
	tm *tTime;
	
	if (local)
		tTime = ::localtime(&timestamp);
	else
		tTime = gmtime(&timestamp);
		
	if (tTime == NULL)	
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TIMETOOLS,TIMETOOLS_GETBYFORMAT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;
		#endif
	
	char formatted[30];
	
	strftime(formatted, 30, format.c_str(), tTime);
	
	return formatted;
}

//-------------------------------------------------------------------

long 
timeTools::now()
{
	time_t tTime = ::time(NULL);
	if (tTime==((time_t)-1))
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TIMETOOLS,TIMETOOLS_NOW,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;
		#endif
	
	return tTime;
}

//-------------------------------------------------------------------

stringArr
timeTools::week(long date, 
				const std::string &format,
				bool local)
{
	register long daynum = atol(timeTools::byFormat("%w",date,local).c_str());
	if (daynum == 0)
		daynum = 7;

	stringArr week;
	register long mon = date - (daynum-1)*86400;
	
	for (register int i(0);i<7;++i,mon+=86400)
		week.push_back(timeTools::byFormat(format,mon,local));
	
	return week;	
}

//-------------------------------------------------------------------

stringArr 
timeTools::datesArr(long dateFrom, 
					long dateTo, 
					const std::string &format, 
					bool local)
{
	stringArr result;
	
	if ( (dateFrom == dateTo) || (dateFrom - dateTo<86400) )
	{
		result.push_back(timeTools::byFormat(format,dateFrom,local));
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
		result.push_back(timeTools::byFormat(format,dateFrom,local));
	    dateFrom += 86400;
	}
	result.push_back(timeTools::byFormat(format,dateTo,local));

	return result;	
}

//-------------------------------------------------------------------

long 
timeTools::makeTime(const __mktime &timeInfo)
{
	tm tTime;
	
	tTime.tm_sec = timeInfo.sec;
	tTime.tm_min = timeInfo.min;
	tTime.tm_hour = timeInfo.hour;
	tTime.tm_mday = timeInfo.day;
	tTime.tm_mon = timeInfo.month-1;
	tTime.tm_year = timeInfo.year;
	tTime.tm_isdst = timeInfo.daylight?1:0;
	
	return ::mktime(&tTime);
}

//-------------------------------------------------------------------

__mktime 
timeTools::makeTime(long seconds, 
					bool local)
{
	tm *tTime;
	
	if (local)
		tTime = ::localtime(&seconds);
	else
		tTime = gmtime(&seconds);
		
	if (tTime == NULL)	
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TIMETOOLS,TIMETOOLS_MAKETIME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __mktime;
		#endif
	
	__mktime timeInfo;
	
	timeInfo.sec = tTime->tm_sec;
	timeInfo.min = tTime->tm_min;
	timeInfo.hour = tTime->tm_hour;
	timeInfo.day = tTime->tm_mday;
	timeInfo.month = tTime->tm_mon+1;
	timeInfo.year = tTime->tm_year;
	timeInfo.daylight = (tTime->tm_isdst>0)?true:false;
		
	return timeInfo;
}

//-------------------------------------------------------------------

unsigned short int 
timeTools::daysInMonth(unsigned int year, 
					unsigned short int month)
{
	register unsigned short int day;
	
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
		{
			register unsigned int isleap = 0;
			if (year % 4 == 0) 
			{
				isleap = 1;
				if ( year % 100 == 0 && year % 400 != 0)
					isleap = 0;
			}
			if (isleap == 1)
				day = 29;
			else
				day = 28;
			break;
		}
	}
	
	return day;	
}

//-------------------------------------------------------------------
