/***************************************************************************
 *            pcSyncProcessSection.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCSYNCPROCESSSECTION_H_
#define _PCSYNCPROCESSSECTION_H_ 1

#include <libdodo/directives.h>

#ifdef XSI_IPC

#include <sys/ipc.h>
#include <sys/sem.h>

#else /*POSIX_SEMAPHORES*/

#include <fcntl.h>
#include <semaphore.h>

#endif

#include <libdodo/pcSyncSection.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/pcSyncProcessSectionEx.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace process
			{
				/**
				 * @class section
				 * @brief performs atomic locks using semaphores
				 * @note if the key was not autogenerated the semaphore is not removed from the system
				 * you should call remove to remove it from the system
				 */
				class section : public sync::section
				{
				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					section(section &sts);

				  public:

					/**
					 * consructor
					 * @param key defines shared memory key
					 * @note if key is 0 - key will be autogenerated
					 * if you want to share semaphore between different instances of process - set own key,
					 * otherwise(like fork) - it may be autogenerated
					 */
					section(int key);

					/**
					 * consructor
					 */
					section();

					/**
					 * destructor
					 */
					virtual ~section();

					/**
					 * open or create semaphore
					 * @param key defines shared memory key
					 * @note if key is 0 - key will be autogenerated
					 * if you want to share semaphore between different instances of process - set own key,
					 * otherwise(like fork) - it may be autogenerated
					 */
					virtual void open(int key);

					/**
					 * close opened semaphore
					 */
					virtual void close();

					/**
					 * lock critical section
					 */
					virtual void acquire();

					/**
					 * unlock critical section
					 */
					virtual void release();

					/**
					 * remove the semaphore from the system
					 * @param key defines the key of the semaphore
					 */
					static void remove(int key);

				  protected:

#ifdef XSI_IPC
					int keeper;             ///< semaphore
					int key;                ///< key for the semaphore
					union semun
					{
						int             val;
						struct semid_ds *buf;
						ushort          * array;
					} control;              ///< semaphore control structure

					sembuf operations[1];   ///< semaphore operations
#else
					sem_t * keeper;         ///< semaphore
					dodoString key;         ///< key for the semaphore
#endif

					bool autogenerated;     ///< true if key was autogenerated
					bool acquired;          ///< true if the semaphore was aquired
				};
			};
		};
	};
};

#endif
