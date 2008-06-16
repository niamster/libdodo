/***************************************************************************
 *            ipcSharedData.h
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

#ifndef _IPCSHAREDDATA_H_
#define _IPCSHAREDDATA_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <libdodo/ipcSharedDataEx.h>
#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace ipc
	{
		namespace shared
		{
			/**
			 * @class data provides shared data functionality
			 */
			class data
			{
				private:

					/**
					 * copy constructor
					 * to prevent copying
					 */
					data(data &sts);

				public:

					/**
					 * constructor
					 * @param key defines shared memory key
					 * @note if key is NULL - key will be generated
					 * if you want to share semaphore between different instances of process - set own key,
					 * otherwise(like fork) - it may be generated
					 */
					data(const char *key = NULL);

					/**
					 * destructor
					 */
					virtual ~data();

					/**
					 * map shared data
					 * @return shared data
					 * @param size defines size of shared data
					 * @note if map is called twice - old data will be unmapped
					 */
					virtual void *map(unsigned long size);

					/**
					 * unmap shared data
					 */
					virtual void unmap();

					/**
					 * @return shared data
					 */
					virtual void *getMapped();

					/**
					 * @return size of shared data
					 */
					virtual unsigned long getSize();

				protected:

					int shm;                                    ///< shared data descriptor
					void *mdata;                                ///< shared data
					unsigned long size;                         ///< size of shared data

					char *key;                                  ///< key for the shared data
			};
		};
	};
};

#endif

