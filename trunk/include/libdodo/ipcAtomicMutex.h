/***************************************************************************
 *            ipcAtomicMutex.h
 *
 *  Mon Jul 09 11:31:55 2007
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

#ifndef _IPCATOMICMUTEX_H_
#define _IPCATOMICMUTEX_H_

#include <libdodo/directives.h>

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#include <libdodo/ipcAtomicLock.h>
#include <libdodo/ipcAtomicMutexEx.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace ipc
	{
		namespace atomic
		{
			/**
			 * @class mutex performs atomic locks using mutexes
			 */
			class mutex : public lock
			{
				public:

					/**
					 * consructor
					 */
					mutex();

					/**
					 * destructor
					 */
					virtual ~mutex();

					/**
					 * lock critical section
					 */
					virtual void acquire();

					/**
					 * unlock critical section
					 */
					virtual void release();

				protected:

		#ifdef PTHREAD_EXT

					pthread_mutex_t keeper; ///< mutex

		#endif

			};
		};
	};
};

#endif
