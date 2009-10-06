/***************************************************************************
 *            pcSyncThreadDataSingle.cc
 *
 *  Wed Nov 30 2005
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

#include <libdodo/directives.h>

#ifdef PTHREAD_EXT
#include <pthread.h>
#endif

#include <time.h>
#include <errno.h>
#include <string.h>

#include "pcSyncThreadLock.inline"

#include <libdodo/pcSyncThreadDataSingle.h>
#include <libdodo/pcSyncThreadDataSingleEx.h>
#include <libdodo/pcSyncDataSingle.h>
#include <libdodo/types.h>

using namespace dodo::pc::sync::thread::data;

single::single(single &sts)
{
}

//-------------------------------------------------------------------

single::single() : data(NULL),
				   lock(new pc::sync::thread::__lock__)
{
#ifdef PTHREAD_EXT
	pthread_mutexattr_t attr;
	errno = pthread_mutexattr_init(&attr);
	if (errno != 0) {
		delete lock;

		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (errno != 0) {
		delete lock;

		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutex_init(&lock->keeper, &attr);
	if (errno != 0) {
		delete lock;

		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutexattr_destroy(&attr);
	if (errno != 0) {
		delete lock;

		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

single::~single()
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
single::set(void *a_data)
{
#ifdef PTHREAD_EXT
	errno = pthread_mutex_lock(&lock->keeper);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	errno = pthread_mutex_unlock(&lock->keeper);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
single::remove()
{
#ifdef PTHREAD_EXT
	errno = pthread_mutex_lock(&lock->keeper);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;

	errno = pthread_mutex_unlock(&lock->keeper);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void *
single::acquire(unsigned long microseconds)
{
#ifdef PTHREAD_EXT
	if (microseconds == 0) {
		errno = pthread_mutex_lock(&lock->keeper);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	} else {
		bool locked = true;
		unsigned long slept = 0;
		timespec timeout = {
			0, 10
		};

		while (locked) {
			errno = pthread_mutex_trylock(&lock->keeper);
			if (errno != 0) {
				if (errno != EBUSY)
					throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, SINGLEEX_CANNOTLOCK, PCSYNCTHREADDATASINGLEEX_CANNOTLOCK_STR, __LINE__, __FILE__);
			} else
				locked = false;
		}
	}
#endif

	return data;
}

//-------------------------------------------------------------------

void
single::release()
{
#ifdef PTHREAD_EXT
	errno = pthread_mutex_unlock(&lock->keeper);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_UNLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

