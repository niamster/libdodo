/***************************************************************************
 *            ipcSharedDataGuard.h
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

#ifndef _IPCSHAREDDATAGUARD_H_
#define _IPCSHAREDDATAGUARD_H_

#include <libdodo/directives.h>

namespace dodo
{
	namespace ipc
	{
		namespace shared
		{
			/**
			 * @class dataGuard provides shared data management functionality
			 */
			class dataGuard
			{
				public:

					/**
					 * destructor
					 */
					virtual ~dataGuard() = 0;

					/**
					 * set shared data
					 * @param data defines shared data
					 */
					virtual void set(void *data) = 0;

					/**
					 * set shared data to NULL
					 */
					virtual void del() = 0;

					/**
					 * lock and return shared data
					 * @return shared data
					 * @param microseconds defines wait timeout for unlock
					 * @note if microseconds is 0 it will wait infinitely
					 */
					virtual void *acquire(unsigned long microseconds) = 0;

					/**
					 * unlock shared data
					 */
					virtual void release() = 0;
			};
		};
	};
};

#endif

