/***************************************************************************
 *            pcJobThreadManagerEx.h
 *
 *  Wed Oct 5 2005
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCJOBTHREADMANAGEREX_H_
#define _PCJOBTHREADMANAGEREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace pc {
		namespace job {
			namespace thread {
				/**
				 * libdodo defined errors
				 */
				enum managerExR {
					MANAGEREX_ISALREADYRUNNING,
					MANAGEREX_ISNOTLAUNCHED,
					MANAGEREX_NOTFOUND,
					MANAGEREX_ISDETACHED,
				};

				/**
				 * libdodo defined errors' explanation
				 */
#define PCJOBTHREADMANAGEREX_ISALREADYRUNNING_STR    "Thread is currently running"
#define PCJOBTHREADMANAGEREX_ISNOTLAUNCHED_STR		"Thread is not launched"
#define PCJOBTHREADMANAGEREX_NOTFOUND_STR            "Thread not found"
#define PCJOBTHREADMANAGEREX_ISDETACHED_STR          "Thread is detached"

				/**
				 * ID of function where exception was thrown
				 */
				enum managerFunctionsID {
					MANAGEREX_RUN,
					MANAGEREX_ADDNRUN,
					MANAGEREX_REMOVE,
					MANAGEREX_WAIT,
					MANAGEREX_STOP,
					MANAGEREX_ISRUNNING,
					MANAGEREX_CONSTRUCTOR,
					MANAGEREX__ISRUNNING,
					MANAGEREX_SETSTACKSIZE,
#ifdef DL_EXT
					MANAGEREX_MODULE,
					MANAGEREX_ADD,
#endif
				};
			};
		};
	};
};
#endif

