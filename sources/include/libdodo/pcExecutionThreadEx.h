/***************************************************************************
 *            pcExecutionThreadEx.h
 *
 *  Wed Oct 07 2009
 *  Copyright  2009  Dmytro Milinevskyy
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

#ifndef _PCEXECUTIONTHREADEX_H_
#define _PCEXECUTIONTHREADEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * libdodo defined errors
             */
            enum threadExR {
                THREADEX_ISALREADYRUNNING,
                THREADEX_ISNOTLAUNCHED,
                THREADEX_ISDETACHED,
            };

            /**
             * libdodo defined errors explanation
             */
#define PCEXECUTIONTHREADEX_ISALREADYRUNNING_STR    "Thread is currently running"
#define PCEXECUTIONTHREADEX_ISNOTLAUNCHED_STR       "Thread is not launched"
#define PCEXECUTIONTHREADEX_ISDETACHED_STR          "Thread is detached"

            /**
             * ID of function where exception was thrown
             */
            enum threadFunctionsID {
                THREADEX_RUN,
                THREADEX_WAIT,
                THREADEX_STOP,
                THREADEX_ISRUNNING,
                THREADEX_CONSTRUCTOR,
#ifdef DL_EXT
                THREADEX_MODULE,
#endif
            };
        };
    };
};
#endif

