/***************************************************************************
 *            systemRaceHazardGuard.h
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
	 * @class systemRaceHazardGuardHolder provides mutex lock and threadGuard class
	 */
	class systemRaceHazardGuardHolder
	{
		protected:

			systemAtomicLock *mutex; ///< lock

			/**
			 * destructor
			 */
			virtual ~systemRaceHazardGuardHolder() = 0;

			/**
			 * @class systemRaceHazardGuard provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class systemRaceHazardGuard
			{
				public:

					/**
					 * contructor
					 */
					systemRaceHazardGuard(const systemRaceHazardGuardHolder *parent);

					/**
					 * destructor
					 */
					virtual ~systemRaceHazardGuard();

				protected:

					systemRaceHazardGuardHolder *parent; ///< lock
			};
	};
};

#endif
