/***************************************************************************
 *            systemThreadCollectionEx.h
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

#ifndef _SYSTEMTHREADCOLLECTIONEX_H_
#define _SYSTEMTHREADCOLLECTIONEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemThreadCollectionExR
	{
		SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING,
		SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING,
		SYSTEMTHREADCOLLECTIONEX_NOTFOUND,
		SYSTEMTHREADCOLLECTIONEX_ISDETACHED,
		SYSTEMTHREADCOLLECTIONEX_SWEPT,
	};

	/**
	 * libdodo defined errors' explanation
	 */
#define SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING_STR    "The thread is currently running. Please wait."
#define SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING_STR        "The thread is currently not running."
#define SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR            "Thread not found."
#define SYSTEMTHREADCOLLECTIONEX_ISDETACHED_STR          "Thread is detached. Cannot join."
#define SYSTEMTHREADCOLLECTIONEX_SWEPT_STR               "Limit of execution exceeded. Thread swept."

	/**
	 * ID of function where exception was thrown
	 */
	enum systemThreadCollectionFunctionsID
	{
		SYSTEMTHREADCOLLECTIONEX_REPLACE,
		SYSTEMTHREADCOLLECTIONEX_RUN,
		SYSTEMTHREADCOLLECTIONEX_ADDNRUN,
		SYSTEMTHREADCOLLECTIONEX_DEL,
		SYSTEMTHREADCOLLECTIONEX_WAIT,
		SYSTEMTHREADCOLLECTIONEX_STOP,
		SYSTEMTHREADCOLLECTIONEX_ISRUNNING,
		SYSTEMTHREADCOLLECTIONEX_CONSTRUCTOR,
		SYSTEMTHREADCOLLECTIONEX__ISRUNNING,
		SYSTEMTHREADCOLLECTIONEX_SETEXECUTIONLIMIT,

#ifdef DL_EXT

		SYSTEMTHREADCOLLECTIONEX_GETMODULEINFO,
		SYSTEMTHREADCOLLECTIONEX_ADD,

#endif
	};
};

#endif

