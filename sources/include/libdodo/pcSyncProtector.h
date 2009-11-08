/***************************************************************************
 *            pcSyncProtector.h
 *
 *  Mon Oct 22 2007
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

#ifndef _PCSYNCPROTECTOR_H_
#define _PCSYNCPROTECTOR_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    namespace pc {
        namespace sync {
            /**
             * @class protector
             * @brief provides shared data management functionality
             */
            class protector {
              public:

                /**
                 * destructor
                 */
                virtual ~protector() = 0;

                /**
                 * lock
                 * @param timeout defines wait timeout for unlock in microseconds
                 * @note if timeout is 0 it will wait infinitely
                 */
                virtual void acquire(unsigned long timeout) = 0;

                /**
                 * unlock
                 */
                virtual void release() = 0;
            };
        };
    };
};
#endif

