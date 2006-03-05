/***************************************************************************
 *            timeTools.h
 *
 *  Sun Nov 27 04:45:19 2005
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

#ifndef _TIMETOOLS_H_
#define _TIMETOOLS_H_

#include <time.h>
#include <stdlib.h>

#include <directives.h>

#include <types.h>
#include <timeToolsEx.h>

namespace dodo
{

	/**
	 * @note time/date format can be specified:
	 *    
	 * 
	 * %w     Weekday number (Sunday = [0,6]).
	 * %a     Abbreviated weekday name.
	 * %A     Full weekday name.
	 * 
	 * %m     Month number [01,12].
	 * %b     Abbreviated month name.
	 * %B     Full month name.
	 * 
	 * %y     Year within century. When a century is not otherwise specified, values in the range [69,99] shall refer to years 1969 to  1999
	 *           inclusive, and values in the range [00,68] shall refer to years 2000 to 2068 inclusive.
	 * %Y     Year as "ccyy" (for example, 2001).
	 * %C     Century number [00,99]; leading zeros are permitted but not required.
	 * 
	 * %d     Day of month [01,31]; the leading 0 is optional.
	 * 
	 * %H     Hour [00,23].
	 * %M     Minute [00,59].
	 * %S     Seconds  [00,60]. The range goes to 60 (rather than stopping at 59) to allow positive leap seconds to be expressed. Since leap
	 * seconds cannot be predicted by any algorithm, leap second data must come from some external source.
	 * 
	 */

	/**
	 * @struct __mktime holds info about time
	 */
	struct __mktime
	{
		/**
		 * constructor
		 */
		__mktime();
		
		/**
		 * constructor
		 * @note set structure's data with user data
		 */
		__mktime(unsigned int sec, unsigned int min, unsigned int hour, unsigned int day, unsigned int month, unsigned int year, bool daylight=true); 
		
		unsigned int sec;///< Seconds [0,60]
		unsigned int min;///< Minutes [0,59]
		unsigned int hour;///< Hour [0,23]
		unsigned int day;///< Day of month [1,31]
		unsigned int month;///< Month of year [1,12]
		unsigned int year;///< Years since 1900
		
		bool daylight;///< Daylight Savings flag; true by default
	};
	
	/**
	 * @class timeTools present different tools to manipulate with time
	 */
	class timeTools
	{
		
		public:
			
			/**
			 * @return number of days in the month [detecting leap year]
			 * @param year is to detect leap year
			 * @param month is describes month where to calculate number of days
			 */
			static unsigned short int daysInMonth(unsigned int year, unsigned short int month);
			 
			/**
			 * @return formated time[date...]
			 * @param format is string contains format presented above
			 * @param timestamp is amount of seconds from 01.01.1970
			 * @param local if true, time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
			 * @note if compiled with NO_EX and error occured empty string will return
			 */
			static std::string byFormat(const std::string &format, long timestamp, bool local = true);

			/**
			 * @return timestamp[amount of seconds from 01.01.1970]
			 * @param format is string contains format presented above
			 * @param dt contais formated time[date...]
			 */
			static long byFormat(const std::string &format, const std::string &dt);
			
			/**
			 * @return amount of seconds from 00:00:00 UTC, January 1, 1970
			 */
			static long now();
			
			/**
			 * @return array of dates that specifies week, where date(in seconds) is specified
			 * @param date indicates date for what to build array
			 * @param format is string contains format presented above
			 * @param local if true, time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
			 * @note monday is first in the array
			 */
			static stringArr week(long date, const std::string &format, bool local=true); 
			
			/**
			 * @return array of dates between specified period
			 * @param dateFrom indicates date for start
			 * @param dateTo indicates date for end
			 * @param format is string contains format presented above
			 * @param local if true, time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
			 * @note you should't set the date in 00:00, add some hours; you can get strange result in periods whe daytime changes
			 */
			static stringArr datesArr(long dateFrom, long dateTo, const std::string &format, bool local=true);
			
			/**
			 * @return second that describes __mktime
			 * @timeInfo holds parameters about time that will be converted into seconds
			 */
			static long makeTime(const __mktime &timeInfo);
			 
			/**
			 * @return parameter about given time
			 * @seconds is amount of secound that will be translated
			 * @param local if true, time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
			 */ 
			static __mktime makeTime(long seconds, bool local = true); 
	};

};

#endif /*TIMETOOLS_H_*/
