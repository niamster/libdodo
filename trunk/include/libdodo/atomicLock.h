/***************************************************************************
 *            atomicLock.h
 *
 *  Sat Oct 20 10:50:55 2007
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

#ifndef _ATOMICLOCK_H_
#define _ATOMICLOCK_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @class atomicLock is to perform atomic locks
	 */
	class atomicLock
	{
		public:

		/**
		 * destructor
		 */
		virtual ~atomicLock() = 0;

		/**
		 * locks critical section
		 */
		void lock() = 0;

		/**
		 * unlocks critical section
		 */
		void unLock() = 0;
	};
};

#endif /*_ATOMICLOCK_H_*/
