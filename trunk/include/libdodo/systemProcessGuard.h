/***************************************************************************
 *            systemProcessGuard.h
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

#ifndef _SYSTEMPROCESSGUARD_H_
#define _SYSTEMPROCESSGUARD_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/systemRaceHazardGuard.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/systemAtomicSemaphore.h>

namespace dodo
{
	/**
	 * @class systemProcessGuardHolder provides concurrent lock for processes interconnection
	 */
	class systemProcessGuardHolder : public systemRaceHazardGuardHolder
	{
		protected:

			/**
			 * contructor
			 */
			systemProcessGuardHolder();

			/**
			 * destructor
			 */
			virtual ~systemProcessGuardHolder();

			char key[32]; ///< key for the semaphore
	};

};

#endif 
