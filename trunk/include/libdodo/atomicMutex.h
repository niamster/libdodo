/***************************************************************************
 *            atomicMutex.h
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

#ifndef _ATOMICMUTEX_H_
#define _ATOMICMUTEX_H_

#include <libdodo/directives.h>

#include <pthread.h>

#include <libdodo/atomicLock.h>
#include <libdodo/atomicMutexEx.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @class atomicMutex is to perform atomic locks
	 */
	class atomicMutex : public atomicLock
	{
		public:

			/**
			 * consructor
			 */
			atomicMutex();

			/**
			 * destructor
			 */
			virtual ~atomicMutex();

			/**
			 * locks critical section
			 */
			virtual void lock();

			/**
			 * unlocks critical section
			 */
			virtual void unLock();

		protected:

			pthread_mutex_t mutex; ///< lock
	};

};

#endif /*_ATOMICMUTEX_H_*/
