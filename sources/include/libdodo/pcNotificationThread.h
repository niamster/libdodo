/***************************************************************************
 *            pcNotificationThread.h
 *
 *  Sun Aug 29 2010
 *  Copyright  2010  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#ifndef _PCNOTIFICATIONTHREAD_H_
#define _PCNOTIFICATIONTHREAD_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class thread;
        };

        namespace notification {
            /**
             * @class thread
             * @brief provides lock mechanism for threads
             */
            class thread {
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
                thread(sync::thread &lock);

                /**
                 * destructor
                 */
                virtual ~thread();

                /**
                 * wait for notification
                 * @return true if nofitication was received
                 * @param timeout defines wait timeout for waiting for notification in microseconds
                 * @note if timeout is 0 it will wait infinitely
                 * the lock should be acquired before this call
                 * the lock is still acquired after the call
                 * if call may block the lock is released for the blocking time
                 */
                virtual bool wait(unsigned long timeout = 0);

                /**
                 * notify waiters
                 * @param all defines whether all waiters should be notified
                 */
                virtual void notify(bool all = false);

              protected:

                sync::thread &lock;         ///< lock

                struct __wake__;
                __wake__ *wake; ///< thread wake handler
            };
        };
    };
};
#endif

