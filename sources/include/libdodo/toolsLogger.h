/***************************************************************************
 *            toolsLogger.h
 *
 *  Fri Mar 21 2008
 *  Copyright  2008  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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
#define _TOOLSLOGGER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace pc {
        namespace sync {
            class protector;
        };
    };

    namespace tools {
        /**
         * @class logger
         * @brief provides logging function
         */
        class logger : public singleton<logger>{
          public:

            /**
             * @enum logLevelEnum defines log levels
             */
            enum logLevelEnum {
                LOG_LEVEL_INFO = 0,
                LOG_LEVEL_NOTICE,
                LOG_LEVEL_DEBUG,
                LOG_LEVEL_WARNING,
                LOG_LEVEL_ERROR,
                LOG_LEVEL_ALERT,
                LOG_LEVEL_CRITICAL,
                LOG_LEVEL_EMERGENCY,
                LOG_LEVEL_USER,

                LOG_LEVEL_ENUMSIZE
            };

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
             * @param level defines log level, @see logger::logLevelEnum
             * @param handler defines log handler
             * @note if handler is NULL logger will pass message to the syslog
             */
            virtual unsigned long add(short       level,
                                      io::channel *handler);

            /**
             * unregister log handler
             * @param position defines log handler identificator
             */
            virtual void remove(unsigned long position);

            /**
             * log message
             * @param level defines log level, @see logger::logLevelEnum
             * @param msg defines log message
             */
            virtual void log(short            level,
                             const dodo::string &msg);

            /**
             * set date/time format for log messages
             * @param format defines date/time format, @see tools::time
             * @note date/time format is not used for syslog
             */
            virtual void setTimeFormat(const dodo::string &format);

            bool forward;                                               ///< forward message to global log instance[false by default]

          private:

            dodo::string timeFormat;                                      ///< date/time format for log messages; "%d/%m/%Y.%H-%M-%S" by default

            /**
             * @struct __log_map__
             * @brief defines map between log level and log handler
             */
            struct __log_map__ {
                short         level;                                    ///< log level, @see logger::logLevelEnum
                io::channel   *handler;                                 ///< log handler
                unsigned long position;                                 ///< log handler identificator
            };

            dodo::slList<__log_map__> handlers;                             ///< list of log maps

            unsigned long handlersNum;                                  ///< number of registered handlers

            static const dodo::string levels[LOG_LEVEL_ENUMSIZE];         ///< log levels statements
            static const int syslogLevels[LOG_LEVEL_ENUMSIZE];          ///< syslog log levels

            pc::sync::protector *keeper;                                ///< section locker
        };
    };
};
#endif
