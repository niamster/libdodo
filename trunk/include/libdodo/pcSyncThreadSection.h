/***************************************************************************
 *            pcSyncThreadSection.h
 *
 *  Mon Jul 09 2007
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

#ifndef _PCSYNCTHREADSECTION_H_
#define _PCSYNCTHREADSECTION_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcSyncSection.h>

namespace dodo {
	namespace pc {
		namespace sync {
			namespace thread {
				struct __lock__;

				/**
				 * @class section
				 * @brief performs atomic locks using mutexes
				 */
				class section : public sync::section {
				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					section(section &);

				  public:

					/**
					 * consructor
					 */
					section();

					/**
					 * destructor
					 */
					virtual ~section();

					/**
					 * lock critical section
					 * @note same thread can acquire section recursively
					 * in this case the same number of releases reqired before another thread can aquire the section
					 */
					virtual void acquire();

					/**
					 * unlock critical section
					 */
					virtual void release();

				  protected:

					__lock__ *lock; ///< lock
				};
			};
		};
	};
};
#endif
