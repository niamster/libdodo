/***************************************************************************
 *            ipcProcessSharedDataGuard.h
 *
 *  Sun Jul 22 23:14:14 2007
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _IPCPROCESSSHAREDDATAGUARD_H_
#define _IPCPROCESSSHAREDDATAGUARD_H_

#include <libdodo/directives.h>

#include <semaphore.h>
#include <time.h>

#include <libdodo/ipcProcessSharedDataGuardEx.h>
#include <libdodo/ipcSharedDataGuard.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace ipc
	{
		namespace process
		{
			namespace shared
			{
				/**
				 * @class dataGuard provides shared data management functionality for processes
				 * @note you should use data
				 */
				class dataGuard : public ipc::shared::dataGuard
				{
					private:

						/**
						 * copy constructor
						 * @note to prevent copying
						 */
						dataGuard(dataGuard &sts);

					public:

						/**
						 * constructor
						 * @param value defines initial value of semaphore
						 * @param key defines semaphore key
						 * @note if key is NULL - key will be generated
						 * if you want to share semaphore between different instances of process - set own key,
						 * otherwise(like fork) - it may be generated
						 */
						dataGuard(unsigned int value = 1, const char *key = NULL);

						/**
						 * destructor
						 */
						virtual ~dataGuard();

						/**
						 * set shared data
						 * @param data defines shared data
						 */
						virtual void set(void *data);

						/**
						 * set shared data to NULL
						 */
						virtual void del();

						/**
						 * lock and return shared data
						 * @return shared data
						 * @param microseconds defines wait timeout for unlock
						 * @note if microseconds is 0 it will wait infinitely
						 */
						virtual void *acquire(unsigned long microseconds = 0);

						/**
						 * unlock shared data
						 */
						virtual void release();

					protected:

						void *data;                                     ///< shared data
						sem_t *semaphore;                               ///< lock

						timespec timeout;                               ///< lock timeout

						char *key;                                      ///< key for the semaphore
				};
			};
		};
	};
};

#endif

