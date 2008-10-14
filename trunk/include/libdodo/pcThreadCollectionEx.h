/***************************************************************************
 *            pcThreadCollectionEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCTHREADCOLLECTIONEX_H_
#define _PCTHREADCOLLECTIONEX_H_

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace pc
	{
		namespace thread
		{
			/**
			 * libdodo defined errors
			 */
			enum collectionExR
			{
				COLLECTIONEX_ISALREADYRUNNING,
				COLLECTIONEX_ISNOTRUNNING,
				COLLECTIONEX_NOTFOUND,
				COLLECTIONEX_ISDETACHED,
				COLLECTIONEX_SWEPT,
			};

			/**
			 * libdodo defined errors' explanation
			 */
#define PCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR    "The thread is currently running. Please wait."
#define PCTHREADCOLLECTIONEX_ISNOTRUNNING_STR        "The thread is currently not running."
#define PCTHREADCOLLECTIONEX_NOTFOUND_STR            "Thread not found."
#define PCTHREADCOLLECTIONEX_ISDETACHED_STR          "Thread is detached. Cannot join."
#define PCTHREADCOLLECTIONEX_SWEPT_STR               "Limit of execution exceeded. Thread swept."

			/**
			 * ID of function where exception was thrown
			 */
			enum collectionFunctionsID
			{
				COLLECTIONEX_REPLACE,
				COLLECTIONEX_RUN,
				COLLECTIONEX_ADDNRUN,
				COLLECTIONEX_DEL,
				COLLECTIONEX_WAIT,
				COLLECTIONEX_STOP,
				COLLECTIONEX_ISRUNNING,
				COLLECTIONEX_CONSTRUCTOR,
				COLLECTIONEX__ISRUNNING,
				COLLECTIONEX_SETEXECUTIONLIMIT,

#ifdef DL_EXT

				COLLECTIONEX_GETMODULEINFO,
				COLLECTIONEX_ADD,

#endif
			};
		};
	};
};

#endif
