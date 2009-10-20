/***************************************************************************
 *            pcSyncThread.h
 *
 *  Tue Nov 29 2005
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

#ifndef _PCSYNCTHREAD_H_
#define _PCSYNCTHREAD_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcSyncProtector.h>

namespace dodo {
	namespace pc {
		namespace sync {
			/**
			 * @class thread
			 * @brief provides lock mechanism for threads
			 */
			class thread : public sync::protector {
			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				thread(thread &);

			  public:

				/**
				 * constructor
				 */
				thread();

				/**
				 * destructor
				 */
				virtual ~thread();

				/**
				 * lock
				 * @param timeout defines wait timeout for unlock in microseconds
				 * @note if timeout is 0 it will wait infinitely
				 */
				virtual void acquire(unsigned long timeout = 0);

				/**
				 * unlock
				 */
				virtual void release();

			  protected:

				struct __lock__;
				__lock__ *lock;         ///< lock
			};
		};
	};
};
#endif

