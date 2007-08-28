/***************************************************************************
 *            atomicMutex.cc
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

#include <atomicMutex.h>

using namespace dodo;

atomicMutex::atomicMutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mutex,&attr);

	pthread_mutexattr_destroy(&attr);	
}

//-------------------------------------------------------------------

atomicMutex::~atomicMutex()
{
	pthread_mutex_destroy(&mutex);
}

//-------------------------------------------------------------------

void
atomicMutex::lock()
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_ATOMICMUTEX, ATOMICMUTEX_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return ;
		#endif
}

//-------------------------------------------------------------------

void
atomicMutex::unLock()
{
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_ATOMICMUTEX, ATOMICMUTEX_UNLOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return ;
		#endif
}

//-------------------------------------------------------------------
