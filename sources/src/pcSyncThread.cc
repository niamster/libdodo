/***************************************************************************
 *            pcSyncThread.cc
 *
 *  Wed Nov 30 2005
 *  Copyright  2005  Dmytro Milinevskyy
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
#include <libdodo/pcSyncThreadEx.h>
#include <libdodo/pcSyncProtector.h>
#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace sync {
            /**
             * @struct thread::__lock__
             * @brief defines thread mutex
             */
            struct thread::__lock__ {
#ifdef PTHREAD_EXT
                pthread_mutex_t keeper;  ///< lock
#endif
            };
        };
    };
};

using namespace dodo::pc::sync;

thread::thread(thread &)
{
}

//-------------------------------------------------------------------

thread::thread() : lock(new thread::__lock__)
{
#ifdef PTHREAD_EXT
    pthread_mutexattr_t attr;

    errno = pthread_mutexattr_init(&attr);
    if (errno != 0) {
        delete lock;

        dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (errno != 0) {
        delete lock;

        dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    errno = pthread_mutex_init(&lock->keeper, &attr);
    if (errno != 0) {
        delete lock;

        dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    errno = pthread_mutexattr_destroy(&attr);
    if (errno != 0) {
        delete lock;

        dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
#endif
}

//-------------------------------------------------------------------

thread::~thread()
{
#ifdef PTHREAD_EXT
    if (pthread_mutex_trylock(&lock->keeper) == 0)
        pthread_mutex_unlock(&lock->keeper);

    pthread_mutex_destroy(&lock->keeper);
#endif

    delete lock;
}

//-------------------------------------------------------------------

void
thread::acquire(unsigned long microseconds)
{
#ifdef PTHREAD_EXT
    if (microseconds == 0) {
        errno = pthread_mutex_lock(&lock->keeper);
        if (errno != 0)
            dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        bool locked = true;
        unsigned long slept = 0;

        while (locked) {
            errno = pthread_mutex_trylock(&lock->keeper);
            if (errno != 0) {
                if (errno != EBUSY)
                    dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

                if (usleep(1) == -1)
                    dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

                slept += 1;

                if (slept > microseconds)
                    dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_ACQUIRE, exception::ERRNO_ERRNO, THREADEX_CANNOTLOCK, PCSYNCTHREADEX_CANNOTLOCK_STR, __LINE__, __FILE__);
            } else {
                locked = false;
            }
        }
    }
#endif
}

//-------------------------------------------------------------------

void
thread::release()
{
#ifdef PTHREAD_EXT
    errno = pthread_mutex_unlock(&lock->keeper);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCSYNCTHREAD, THREADEX_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

