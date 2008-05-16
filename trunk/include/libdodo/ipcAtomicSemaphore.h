/***************************************************************************
 *            ipcAtomicSemaphore.h
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

#ifndef _IPCATOMICSEMAPHORE_H_
#define _IPCATOMICSEMAPHORE_H_

#include <libdodo/directives.h>

#include <fcntl.h>
#include <semaphore.h>

#include <libdodo/ipcAtomicLock.h>
#include <libdodo/ipcAtomicSemaphoreEx.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace ipc
	{
		namespace atomic
		{
			/**
			 * @class semaphore performs atomic locks using semaphores
			 */
			class semaphore : public lock
			{
				public:

					/**
					 * consructor
					 */
					semaphore(unsigned int value, const char *key);

					/**
					 * destructor
					 */
					virtual ~semaphore();

					/**
					 * lock critical section
					 */
					virtual void acquire();

					/**
					 * unlock critical section
					 */
					virtual void release();

				protected:

					sem_t *keeper;      ///< semaphore
					char *key;          ///< key for the semaphore
			};
		};
	};
};

#endif
