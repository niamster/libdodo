/***************************************************************************
 *            systemThreads.h
 *
 *  Tue Nov 29 23:31:55 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SYSTEMTHREADS_H_
#define _SYSTEMTHREADS_H_

#include <directives.h>

#include <pthread.h>
	
namespace dodo
{

	/**
	 * @struct __thredInfo describes thread
	 */
	struct __thredInfo
	{
		pthread_t thread;///< thread descriptor
		void *data;///< data that will be passed on run
		bool isRunning;///< whether thread is running
		
	};

};

#endif /* _XMLTOOLS_H */
