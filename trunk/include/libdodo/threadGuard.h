/***************************************************************************
 *            threadGuard.h
 *
 *  Tue Jul  10 22:00:57 2007
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

#ifndef _THREADGUARD_H_
#define _THREADGUARD_H_

#include <directives.h>

#include <types.h>
#include <atomicMutex.h>

namespace dodo
{
	/**
	 * @class threadGuardHolder contains mutex lock and threadGuard class
	 */
	class threadGuardHolder
	{

		protected:

		atomicMutex mutex;    ///< lock

		/**
		 * @class threadGuard provides thread safe behaviour
		 */
		class threadGuard
		{
			public:

			/**
			 * contructor
			 */
			threadGuard(threadGuardHolder *parent);

			/**
			 * destructor
			 */
			~threadGuard();

			protected:

			threadGuardHolder *parent;        ///< class to lock
		};
	};

};

#endif /*THREADGUARD_H_*/
