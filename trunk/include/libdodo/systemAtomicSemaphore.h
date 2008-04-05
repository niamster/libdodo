/***************************************************************************
 *            systemAtomicSemaphore.h
 *
 *  Sat Oct 20 02:00:55 2007
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

#ifndef _SYSTEMATOMICSEMAPHORE_H_
#define _SYSTEMATOMICSEMAPHORE_H_

#include <libdodo/directives.h>

#include <fcntl.h>
#include <semaphore.h>

#include <libdodo/systemAtomicLock.h>
#include <libdodo/systemAtomicSemaphoreEx.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @class systemAtomicSemaphore performs atomic locks using semaphores
	 */
	class systemAtomicSemaphore : public systemAtomicLock
	{
		public:

			/**
			 * consructor
			 */
			systemAtomicSemaphore(unsigned int value, const char *key);

			/**
			 * destructor
			 */
			virtual ~systemAtomicSemaphore();

			/**
			 * lock critical section
			 */
			virtual void lock();

			/**
			 * unlock critical section
			 */
			virtual void unlock();

		protected:

			sem_t *semaphore;   ///< semaphore
			char *key;          ///< key for the semaphore
	};
};

#endif
