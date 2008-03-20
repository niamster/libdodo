/***************************************************************************
 *            systemThreadsEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _SYSTEMTHREADSEX_H_
#define _SYSTEMTHREADSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemThreadsExR
	{
		SYSTEMTHREADSEX_ISALREADYRUNNING,
		SYSTEMTHREADSEX_ISNOTRUNNING,
		SYSTEMTHREADSEX_NOTFOUND,
		SYSTEMTHREADSEX_ISDETACHED,
		SYSTEMTHREADSEX_SWEPT,
	};

	/**
	 * libdodo defined errors' explanation
	 */
#define SYSTEMTHREADSEX_ISALREADYRUNNING_STR    "The thread is currently running. Please wait."
#define SYSTEMTHREADSEX_ISNOTRUNNING_STR        "The thread is currently not running."
#define SYSTEMTHREADSEX_NOTFOUND_STR            "Thread not found."
#define SYSTEMTHREADSEX_ISDETACHED_STR          "Thread is detached. Cannot join."
#define SYSTEMTHREADSEX_SWEPT_STR               "Limit of execution exceeded. Thread swept."

	/**
	 * ID of function where exception was thrown
	 */
	enum systemThreadsFunctionsID
	{
		SYSTEMTHREADSEX_REPLACE,
		SYSTEMTHREADSEX_RUN,
		SYSTEMTHREADSEX_ADDNRUN,
		SYSTEMTHREADSEX_DEL,
		SYSTEMTHREADSEX_WAIT,
		SYSTEMTHREADSEX_STOP,
		SYSTEMTHREADSEX_ISRUNNING,
		SYSTEMTHREADSEX_CONSTRUCTOR,
		SYSTEMTHREADSEX__ISRUNNING,
		SYSTEMTHREADSEX_SETEXECUTIONLIMIT,

#ifdef DL_EXT

		SYSTEMTHREADSEX_GETMODULEINFO,
		SYSTEMTHREADSEX_ADD,

#endif
	};
};

#endif

