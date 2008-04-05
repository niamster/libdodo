/***************************************************************************
 *            systemAtomicMutex.cc
 *
 *  Mon Jul 09 11:31:55 2007
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

#include <libdodo/systemAtomicMutex.h>

using namespace dodo;

systemAtomicMutex::systemAtomicMutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mutex, &attr);

	pthread_mutexattr_destroy(&attr);
}

//-------------------------------------------------------------------

systemAtomicMutex::~systemAtomicMutex()
{
	pthread_mutex_destroy(&mutex);
}

//-------------------------------------------------------------------

void
systemAtomicMutex::lock()
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0 && errno != EDEADLK)
		throw baseEx(ERRMODULE_SYSTEMATOMICMUTEX, SYSTEMATOMICMUTEXEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemAtomicMutex::unlock()
{
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMATOMICMUTEX, SYSTEMATOMICMUTEXEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

