/***************************************************************************
 *            systemThreadSharedDataGuard.cc
 *
 *  Wed Nov 30 22:02:16 2005
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


#include <systemThreadSharedDataGuard.h>

using namespace dodo;

systemThreadSharedDataGuard::systemThreadSharedDataGuard(systemThreadSharedDataGuard &sts)
{
}

//-------------------------------------------------------------------

systemThreadSharedDataGuard::systemThreadSharedDataGuard() : data(NULL)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mutex, &attr);

	pthread_mutexattr_destroy(&attr);

	timeout.tv_nsec = 1000;
	timeout.tv_sec = 0;
}

//-------------------------------------------------------------------

systemThreadSharedDataGuard::~systemThreadSharedDataGuard()
{
	if (pthread_mutex_trylock(&mutex) == 0)
		pthread_mutex_unlock(&mutex);

	pthread_mutex_destroy(&mutex);
}

//-------------------------------------------------------------------

void
systemThreadSharedDataGuard::set(void *a_data)
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadSharedDataGuard::del()
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void *
systemThreadSharedDataGuard::lock(unsigned long microseconds)
{
	if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		bool locked = true;
		unsigned long slept = 0;

		while (locked)
		{
			errno = pthread_mutex_trylock(&mutex);
			if (errno != 0)
			{
				if (errno != EBUSY)
					throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_LOCK, ERR_ERRNO, SYSTEMTHREADSHAREDDATAGUARD_CANNOTLOCK, SYSTEMTHREADSHAREDDATAGUARD_CANNOTLOCK_STR, __LINE__, __FILE__);
			}
			else
				locked = false;
		}
	}

	return data;
}

//-------------------------------------------------------------------

void
systemThreadSharedDataGuard::unlock()
{
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATAGUARD, SYSTEMTHREADSHAREDDATAGUARD_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------
