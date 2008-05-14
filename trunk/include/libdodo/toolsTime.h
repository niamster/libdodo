/***************************************************************************
 *            toolsTime.h
 *
 *  Sun Nov 27 04:45:19 2005
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

#ifndef _TOOLSTIME_H_
#define _TOOLSTIME_H_

#include <libdodo/directives.h>

#include <time.h>
#include <stdlib.h>

#include <libdodo/toolsTimeEx.h>
#include <libdodo/toolsString.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * @note time/date format can be specified as:
		 * %w     Weekday number (Sunday=[0, 6]).
		 * %a     Abbreviated weekday name.
		 * %A     Full weekday name.
		 *
		 * %m     Month number [01, 12].
		 * %b     Abbreviated month name.
		 * %B     Full month name.
		 *
		 * %y     Year within century. When a century is not otherwise specified, values in the range [69, 99] shall refer to years 1969 to  1999
		 *           inclusive, and values in the range [00, 68] shall refer to years 2000 to 2068 inclusive.
		 * %Y     Year as "ccyy" (for example, 2001).
		 * %C     Century number [00, 99]; leading zeros are permitted but not required.
		 *
		 * %d     Day of month [01, 31]; the leading 0 is optional.
		 *
		 * %H     Hour [00, 23].
		 * %M     Minute [00, 59].
		 * %S     Seconds  [00, 60]. The range goes to 60 (rather than stopping at 59) to allow positive leap seconds to be expressed. Since leap
		 * seconds cannot be predicted by any algorithm, leap second data must come from some external source.
		 */

		/**
		 * @struct __mktime defines time info
		 */
		struct __time
		{
			/**
			 * constructor
			 */
			__time();

			/**
			 * constructor
			 * @param sec defines seconds
			 * @param min defines minutes
			 * @param hour defines hours
			 * @param day defines day of month
			 * @param month defines month
			 * @param year defines year
			 * @param daylight defines daylight savings condition
			 */
			__time(unsigned int sec, unsigned int min, unsigned int hour, unsigned int day, unsigned int month, unsigned int year, bool daylight = true);

			unsigned int sec;   ///< seconds [0, 60]
			unsigned int min;   ///< minutes [0, 59]
			unsigned int hour;  ///< hours [0, 23]
			unsigned int day;   ///< day of month [1, 31]
			unsigned int month; ///< month [1, 12]
			unsigned int year;  ///< year [since 1900]

			bool daylight;      ///< daylight saving flag; true by default
		};

		/**
		 * @class time provides time functionality
		 */
		class time
		{
			public:

				/**
				 * @return number of days in the month [detecting leap year]
				 * @param year defines year
				 * @param month defines month[1, 12]
				 */
				static unsigned short int daysInMonth(unsigned int year, unsigned short int month);

				/**
				 * @return formated time/date/...
				 * @param format defines time/date/... format string
				 * @param timestamp defines number of seconds from 00:00:00 UTC, January 1, 1970
				 * @param local defines local time condition; if true time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
				 */
				static dodoString byFormat(const dodoString &format, long timestamp, bool local = true);

				/**
				 * @return number of seconds from 00:00:00 UTC, January 1, 1970
				 * @param format defines time/date/... format string
				 * @param dt defines formated string of time/date/...
				 */
				static long byFormat(const dodoString &format, const dodoString &dt);

				/**
				 * @return number of seconds from 00:00:00 UTC, January 1, 1970 till now
				 */
				static long now();

				/**
				 * @return dates of the week
				 * @param date defines timestamp in the weekv
				 * @param format defines time/date/... format string
				 * @param local defines local time condition; if true time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
				 * @note monday is first
				 */
				static dodoStringArray week(long date, const dodoString &format, bool local = true);

				/**
				 * @return dates of the specified period
				 * @param dateFrom defines start date
				 * @param dateTo defines end date
				 * @param format defines time/date/... format string
				 * @param local defines local time condition; if true time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
				 * @note you should't set the date in 00:00: add some hours to avoid undefined behaivior in periods of daytime changes
				 */
				static dodoStringArray dates(long dateFrom, long dateTo, const dodoString &format, bool local = true);

				/**
				 * @return number of seconds from 00:00:00 UTC, January 1, 1970
				 * @param timeInfo defines time info
				 */
				static long timestamp(const __time &timeInfo);

				/**
				 * @return time info
				 * @param seconds defines number of seconds from 00:00:00 UTC, January 1, 1970
				 * @param local defines local time condition; if true time represents as local(according to timezone), otherwice as Coordinated Universal Time (UTC)
				 */
				static __time timestamp(long seconds, bool local = true);
		};
	};
};

#endif
