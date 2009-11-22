/***************************************************************************
 *            pcExecutionManagerEx.h
 *
 *  Wed Oct 07 2009
 *  Copyright  2009  Ni@m
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

#ifndef _PCEXECUTIONMANAGEREX_H_
#define _PCEXECUTIONMANAGEREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * libdodo defined errors
             */
            enum managerExR {
                MANAGEREX_NOTFOUND,
                MANAGEREX_UNKNOWNJOB,
            };

            /**
             * libdodo defined errors explanation
             */
#define PCEXECUTIONMANAGEREX_NOTFOUND_STR    "Job was not found"
#define PCEXECUTIONMANAGEREX_UNKNOWNJOB_STR  "Unknown type of job"

            /**
             * ID of function where exception was thrown
             */
            enum managerFunctionsID {
                MANAGEREX_RUN,
                MANAGEREX_WAIT,
                MANAGEREX_STOP,
                MANAGEREX_ISRUNNING,
                MANAGEREX_JOB,
                MANAGEREX_ADD,
            };
        };
    };
};
#endif

