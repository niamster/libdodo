/***************************************************************************
 *            pcSyncProcessDataSingle.h
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

#ifndef _PCSYNCPROCESSDATASINGLE_H_
#define _PCSYNCPROCESSDATASINGLE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcSyncDataSingle.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace process
			{
				struct __lock;

				namespace data
				{
					/**
					 * @class single
					 * @brief provides shared data management functionality for processes
					 * @note you should use data::shared
					 * @note if the key was not autogenerated the semaphore is not removed from the system
					 * you should call remove to remove it from the system
					 */
					class single : public sync::data::single
					{
					  private:

						/**
						 * copy constructor
						 * @note to prevent copying
						 */
						single(single &sts);

					  public:

						/**
						 * constructor
						 * @param key defines semaphore key
						 * @note if key is 0 - key will be autogenerated
						 * if you want to share semaphore between different instances of process - set own key,
						 * otherwise(like fork) - it may be autogenerated
						 */
						single(int key);

						/**
						 * constructor
						 */
						single();

						/**
						 * destructor
						 */
						virtual ~single();

						/**
						 * open or create semaphore object
						 * @param key defines semaphore key
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

						/**
						 * remove the semaphore from the system
						 * @param key defines the key of the semaphore
						 */
						static void remove(int key);

					  protected:

						void *data;             ///< shared data

						__lock *semaphore;

						bool autogenerated;     ///< true if key was autogenerated
						bool acquired;          ///< true if the semaphore was aquired
					};
				};
			};
		};
	};
};

#endif

