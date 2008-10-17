/***************************************************************************
 *            toolsLogger.h
 *
 *  Fri Mar 21 00:57:57 2008
 *  Copyright  2008  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _TOOLSLOGGER_H_
#define _TOOLSLOGGER_H_

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/toolsTime.h>
#include <libdodo/pcSyncThreadStack.h>
#include <libdodo/types.h>

#include <syslog.h>

namespace dodo
{
	namespace tools
	{
#define LOGGER_LEVELS 9

		/**
		 * @enum loggerLogLevelEnum defines log levels
		 */
		enum loggerLogLevelEnum
		{
			LOGGER_INFO = 0,
			LOGGER_NOTICE,
			LOGGER_DEBUG,
			LOGGER_WARNING,
			LOGGER_ERROR,
			LOGGER_ALERT,
			LOGGER_CRITICAL,
			LOGGER_EMERGENCY,
			LOGGER_USER
		};

		/**
		 * @struct __logMap
		 * @brief defines map between log level and log handler
		 */
		struct __logMap
		{
			short level;                                ///< log level[see loggerLogLevelEnum]
			io::channel *handler;                       ///< log handler
			long position;                              ///< log handler identificator
		};

		/**
		 * @class logger
		 * @brief provides logging function
		 */
		class logger : virtual public pc::sync::thread::stack,
					   public singleton<logger>
		{
			public:

				/**
				 * constructor
				 */
				logger();

				/**
				 * destructor
				 */
				virtual ~logger();

				/**
				 * register log handler
				 * @return log handler identificator
				 * @param level defines log level[see loggerLogLevelEnum]
				 * @param handler defines log handler
				 * @note if handler is NULL logger will pass message to the syslog
				 */
				virtual unsigned long add(short level, io::channel *handler);

				/**
				 * unregister log handler
				 * @param position defines log handler identificator
				 */
				virtual void remove(unsigned long position);

				/**
				 * log message
				 * @param level defines log level[see loggerLogLevelEnum]
				 * @param msg defines log message
				 */
				virtual void log(short level, const dodoString &msg);

				/**
				 * set date/time format for log messages
				 * @param format defines date/time format[see tools::time]
				 * @note date/time format is not used for syslog
				 */
				virtual void setTimeFormat(const dodoString &format);

				bool forward;                ///< forward message to global log instance[false by default]

			private:

				dodoString timeFormat;                                                      ///< date/time format for log messages; "%d/%m/%Y.%H-%M-%S" by default

				dodoList<__logMap> handlers;                                                ///< list of log maps

				unsigned long handlersNum;                                                  ///< number of registered handlers

				static const dodoString levels[LOGGER_LEVELS];                              ///< log levels statements
				static const int syslogLevels[LOGGER_LEVELS];                               ///< syslog log levels
		};
	};
};

#endif
