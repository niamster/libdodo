/***************************************************************************
 *            systemProcessCollectionEx.h
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

#ifndef _SYSTEMPROCESSCOLLECTIONEX_H_
#define _SYSTEMPROCESSCOLLECTIONEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemProcessCollectionExR
	{
		SYSTEMPROCESSCOLLECTIONEX_ISALREADYRUNNING,
		SYSTEMPROCESSCOLLECTIONEX_NOTFOUND,
		SYSTEMPROCESSCOLLECTIONEX_SWEPT,
		SYSTEMPROCESSCOLLECTIONEX_ISNOTRUNNING,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define SYSTEMPROCESSCOLLECTIONEX_ISALREADYRUNNING_STR "The process is currently running. Please wait."
#define SYSTEMPROCESSCOLLECTIONEX_NOTFOUND_STR "Process not found."
#define SYSTEMPROCESSCOLLECTIONEX_SWEPT_STR "Limit of execution exceeded. Process swept."
#define SYSTEMPROCESSCOLLECTIONEX_ISNOTRUNNING_STR "The process is currently not running."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum systemProcessCollectionFunctionsID
	{
		SYSTEMPROCESSCOLLECTIONEX__ISRUNNING,
		SYSTEMPROCESSCOLLECTIONEX_ADDNRUN,
		SYSTEMPROCESSCOLLECTIONEX_RUN,
		SYSTEMPROCESSCOLLECTIONEX_DEL,
		SYSTEMPROCESSCOLLECTIONEX_REPLACE,
		SYSTEMPROCESSCOLLECTIONEX_STOP,
		SYSTEMPROCESSCOLLECTIONEX_WAIT,
		SYSTEMPROCESSCOLLECTIONEX_SETEXECUTIONLIMIT,
		SYSTEMPROCESSCOLLECTIONEX_ISRUNNING,

#ifdef DL_EXT

		SYSTEMPROCESSCOLLECTIONEX_GETMODULEINFO,
		SYSTEMPROCESSCOLLECTIONEX_ADD,

#endif
	};
};

#endif

