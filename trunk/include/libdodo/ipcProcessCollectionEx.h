/***************************************************************************
 *            ipcProcessCollectionEx.h
 *
 *  Tue Feb 27 08:42:14 2007
 *  Copyright  2007  Ni@m
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

#ifndef _IPCPROCESSCOLLECTIONEX_H_
#define _IPCPROCESSCOLLECTIONEX_H_

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace ipc
	{
		namespace process
		{
			/**
			 * libdodo defined errors
			 */
			enum collectionExR
			{
				COLLECTIONEX_ISALREADYRUNNING,
				COLLECTIONEX_NOTFOUND,
				COLLECTIONEX_SWEPT,
				COLLECTIONEX_ISNOTRUNNING,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define IPCPROCESSCOLLECTIONEX_ISALREADYRUNNING_STR "The process is currently running. Please wait."
#define IPCPROCESSCOLLECTIONEX_NOTFOUND_STR         "Process not found."
#define IPCPROCESSCOLLECTIONEX_SWEPT_STR            "Limit of execution exceeded. Process swept."
#define IPCPROCESSCOLLECTIONEX_ISNOTRUNNING_STR     "The process is currently not running."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum collectionFunctionsID
			{
				COLLECTIONEX__ISRUNNING,
				COLLECTIONEX_ADDNRUN,
				COLLECTIONEX_RUN,
				COLLECTIONEX_DEL,
				COLLECTIONEX_REPLACE,
				COLLECTIONEX_STOP,
				COLLECTIONEX_WAIT,
				COLLECTIONEX_SETEXECUTIONLIMIT,
				COLLECTIONEX_ISRUNNING,

#ifdef DL_EXT

				COLLECTIONEX_GETMODULEINFO,
				COLLECTIONEX_ADD,

#endif
			};
		};
	};
};

#endif

