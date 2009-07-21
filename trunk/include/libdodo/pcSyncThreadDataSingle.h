/***************************************************************************
 *            pcSyncThreadDataSingle.h
 *
 *  Tue Nov 29 23:31:55 2005
 *  Copyright  2005  Ni@m
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

#ifndef _PCSYNCTHREADDATASINGLE_H_
#define _PCSYNCTHREADDATASINGLE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcSyncDataSingle.h>

namespace dodo {
	namespace pc {
		namespace sync {
			namespace thread {
				struct __lock__;

				namespace data {
					/**
					 * @class single
					 * @brief provides shared data management functionality for threads
					 */
					class single : public sync::data::single {
					  private:

						/**
						 * copy constructor
						 * @note to prevent copying
						 */
						single(single &sts);

					  public:

						/**
						 * constructor
						 */
						single();

						/**
						 * destructor
						 */
						virtual ~single();

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

						void *data;             ///< shared data

						__lock__ *lock;         ///< lock
					};
				};
			};
		};
	};
};

#endif

