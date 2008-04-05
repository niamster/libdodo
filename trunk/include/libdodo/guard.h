/***************************************************************************
 *            guard.h
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

#ifndef _GUARD_H_
#define _GUARD_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/systemAtomicLock.h>
#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * @class guardHolder provides mutex lock and threadGuard class
	 */
	class guardHolder
	{
		protected:

			systemAtomicLock *mutex; ///< lock

			/**
			 * destructor
			 */
			virtual ~guardHolder() = 0;

			/**
			 * @class guard provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class guard
			{
				public:

					/**
					 * contructor
					 */
					guard(const guardHolder *parent);

					/**
					 * destructor
					 */
					virtual ~guard();

				protected:

					guardHolder *parent; ///< lock
			};
	};
};

#endif
