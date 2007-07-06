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

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

	namespace dodo
	{
		/**
		 * libdodo defined errors
		 */
		enum systemThreadsExR
		{
			SYSTEMTHREADS_ISALREADYRUNNING,
			SYSTEMTHREADS_ISNOTRUNNING,
			SYSTEMTHREADS_NOTFOUND,
			SYSTEMTHREADS_ISDETACHED,
			SYSTEMTHREADS_SWEPT,
		};
		
		/**
		 * libdodo defined errors' explanation
		 */
		#define SYSTEMTHREADS_ISALREADYRUNNING_STR "The thread is currently running. Please wait."
		#define SYSTEMTHREADS_ISNOTRUNNING_STR "The thread is currently not running."
		#define SYSTEMTHREADS_NOTFOUND_STR "Thread not found."
		#define SYSTEMTHREADS_ISDETACHED_STR "Thread is detached. Cannot join."
		#define SYSTEMTHREADS_SWEPT_STR "Limit of execution exceeded. Thread swept."
			
		/**
		 * ID of function where exception was thrown
		 */			
		enum systemThreadsFunctionsID
		{
			SYSTEMTHREADS_REPLACE,
			SYSTEMTHREADS_RUN,
			SYSTEMTHREADS_ADDNRUN,
			SYSTEMTHREADS_DEL,
			SYSTEMTHREADS_WAIT,
			SYSTEMTHREADS_STOP,
			SYSTEMTHREADS_ISRUNNING,
			SYSTEMTHREADS_CONSTRUCTOR,
			SYSTEMTHREADS__ISRUNNING,
			SYSTEMTHREADS_SETEXECUTIONLIMIT,
			
			#ifdef DL_EXT
			
				SYSTEMTHREADS_GETMODULEINFO,
				SYSTEMTHREADS_ADD,
				
			#endif			
		};
	};

#endif
#endif 

