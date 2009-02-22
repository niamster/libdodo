/***************************************************************************
 *            dataMemoryShared.h
 *
 *  Wed Nov 21 00:39:14 2007
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

#ifndef _DATAMEMORYSHARED_H_
#define _DATAMEMORYSHARED_H_ 1

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/mman.h>

#ifdef XSI_IPC

#include <sys/ipc.h>
#include <sys/shm.h>

#else /*POSIX_SEMAPHORES*/

#include <fcntl.h>

#endif

#include <libdodo/dataMemorySharedEx.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>

namespace dodo
{
	namespace data
	{
		namespace memory
		{
			/**
			 * @class shared
			 * @brief provides shared data functionality
			 * @note if the key was not autogenerated the semaphore is not removed from the system
			 * you should call remove to remove it from the system
			 */
			class shared
			{
			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				shared(shared &sts);

			  public:

				/**
				 * constructor
				 * @param key defines shared memory key
				 * @note if key is 0 - key will be generated
				 * if you want to share data between different instances of process - set own key,
				 * otherwise(like fork) - it may be generated
				 */
				shared(const int key);

				/**
				 * constructor
				 */
				shared();

				/**
				 * destructor
				 */
				virtual ~shared();

				/**
				 * open or create shared memory object
				 * @param key defines shared memory key
				 * @note if key is 0 - key will be generated
				 * if you want to share data between different instances of process - set own key,
				 * otherwise(like fork) - it may be generated
				 */
				virtual void open(const int key);

				/**
				 * close shared memory object
				 */
				virtual void close();

				/**
				 * map shared data
				 * @return shared data
				 * @param size defines size of shared data
				 * @note if map is called twice - old data will be unmapped
				 * is size is 0 it will be detected automatically
				 */
				virtual void *map(unsigned long size = 0);

				/**
				 * unmap shared data
				 */
				virtual void unmap();

				/**
				 * @return shared data
				 */
				virtual void *get();

				/**
				 * @return size of shared data
				 * @note this should be called before map to obtain the size of the already allocated shared memory
				 */
				virtual unsigned long getSize();

				/**
				 * remove the shared data from the system
				 * @param key defines the key of the shared data
				 */
				static void remove(int key);

			  protected:

				int shm;            ///< shared data descriptor
				void *mshared;      ///< shared data

#ifdef XSI_IPC

				int key;            ///< key for the shared data

#else

				dodoString key;     ///< key for the shared data
				unsigned long size; ///< size of shared data

#endif

				bool autogenerated; ///< true if key was autogenerated
			};
		};
	};
};

#endif

