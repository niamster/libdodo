/***************************************************************************
 *            systemSharedDataGuard.h
 *
 *  Mon Oct 22 00:02:14 2007
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

#ifndef _SYSTEMSHAREDDATAGUARD_H_
#define _SYSTEMSHAREDDATAGUARD_H_

#include <libdodo/directives.h>

namespace dodo
{
	/**
	 * @class systemSharedDataGuard is to manage data between jobs
	 */
	class systemSharedDataGuard
	{
		public:

			/**
			 * destructor
			 */
			virtual ~systemSharedDataGuard() = 0;

			/**
			 * sets data to became a shared
			 * @param data describes data to be shared
			 */
			virtual void set(void *data) = 0;

			/**
			 * sets shared data to NULL
			 */
			virtual void del() = 0;

			/**
			 * lock and return shared data
			 * @return data points on shared data or NULL in error case
			 * @param microseconds indicates how many time to wait for locking; if time expired and can't unlock - error =(; only if realization of pthreads supports it!
			 * @note if microseconds==0 - infinite sleep
			 */
			virtual void *lock(unsigned long microseconds) = 0;


			/**
			 * lock and return shared data
			 */
			virtual void unlock() = 0;
	};

};

#endif

