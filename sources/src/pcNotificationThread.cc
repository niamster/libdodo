/***************************************************************************
 *            pcNotificationThread.cc
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

#include <libdodo/directives.h>

#ifdef PTHREAD_EXT
#include <pthread.h>
#endif

#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <libdodo/pcSyncThread.h>
#include <libdodo/pcNotificationThreadEx.h>
#include <libdodo/pcNotificationThread.h>
#include <libdodo/types.h>

#include "pcSyncThread.inline"

namespace dodo {
    namespace pc {
        namespace notification {
            /**
             * @struct thread::__wake__
             * @brief defines conditional locking
             */
            struct thread::__wake__ {
#ifdef PTHREAD_EXT
                pthread_cond_t cond;  ///< lock
#endif
            };
        };
    };
};

using namespace dodo::pc::notification;

thread::thread(thread &) : lock(* new sync::thread)
{
}

//-------------------------------------------------------------------

thread::thread(sync::thread &lock) : lock(lock),
                                     wake(new __wake__)
{
#ifdef PTHREAD_EXT
    errno = pthread_cond_init(&wake->cond, NULL);
    if (errno != 0) {
        delete wake;

        dodo_throw exception::basic(exception::MODULE_PCNOTIFICATIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
#endif
}

//-------------------------------------------------------------------

thread::~thread()
{
#ifdef PTHREAD_EXT
    pthread_cond_destroy(&wake->cond);
#endif

    delete wake;
}

//-------------------------------------------------------------------

bool
thread::wait(unsigned long microseconds)
{
#ifdef PTHREAD_EXT
    if (microseconds == 0) {
        errno = pthread_cond_wait(&wake->cond, &lock.lock->keeper);
        if (errno != 0)
            dodo_throw exception::basic(exception::MODULE_PCNOTIFICATIONTHREAD, THREADEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        timespec ts = {microseconds/1000000, (microseconds%1000000)*1000};
        timespec now;

        clock_gettime(CLOCK_REALTIME, &now);
        ts.tv_sec += now.tv_sec;
        ts.tv_nsec += now.tv_nsec;
        if (ts.tv_nsec > 999999999) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 999999999;
        }

        errno = pthread_cond_timedwait(&wake->cond, &lock.lock->keeper, &ts);
        if (errno != 0) {
            if (errno == ETIMEDOUT)
                return false;
            else
                dodo_throw exception::basic(exception::MODULE_PCNOTIFICATIONTHREAD, THREADEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }
#endif

    return true;
}

//-------------------------------------------------------------------

void
thread::notify(bool all)
{
#ifdef PTHREAD_EXT
    if (all)
        errno = pthread_cond_broadcast(&wake->cond);
    else
        errno = pthread_cond_signal(&wake->cond);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCNOTIFICATIONTHREAD, THREADEX_NOTIFY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

