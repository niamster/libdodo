/***************************************************************************
 *            pcExecutionProcessEx.h
 *
 *  Wed Oct 08 2009
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

#ifndef _PCEXECUTIONPROCESSEX_H_
#define _PCEXECUTIONPROCESSEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * libdodo defined errors
             */
            enum processExR {
                PROCESSEX_ISALREADYRUNNING,
                PROCESSEX_ISNOTLAUNCHED,
            };

            /**
             * explanations for libdodo defined errors
             */
#define PCEXECUTIONPROCESSEX_ISALREADYRUNNING_STR "Process is currently running"
#define PCEXECUTIONPROCESSEX_ISNOTLAUNCHED_STR "Process is not launched"

            /**
             * IDs of functions where exception might be thrown
             */
            enum processFunctionsID {
                PROCESSEX_ISRUNNING,
                PROCESSEX_RUN,
                PROCESSEX_STOP,
                PROCESSEX_WAIT,
#ifdef DL_EXT
                PROCESSEX_MODULE,
                PROCESSEX_CONSTRUCTOR,
#endif
            };
        };
    };
};
#endif

