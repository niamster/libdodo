/***************************************************************************
 *            systemThreadShares.cc
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


#include <systemThreadShares.h>
	
using namespace dodo;

systemThreadShares::threadGuard::threadGuard(systemThreadShares *a_parent) : locked(false),
																			parent(a_parent)
{
	parent->mutex.lock();
	
	locked = true;
}

systemThreadShares::threadGuard::~threadGuard()
{
	if (locked)
		parent->mutex.unLock();
}

void
systemThreadShares::threadGuard::unlock()
{
	locked = false;
	
	parent->mutex.unLock();
}

systemThreadShares::systemThreadShares(systemThreadShares &sts)
{
}

//-------------------------------------------------------------------

systemThreadShares::systemThreadShares()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	/*pthread_mutex_init(&mutex,&attr);

	pthread_mutexattr_destroy(&attr);*/
	
	timeout.tv_nsec = 1000;
	timeout.tv_sec = 0;	
}

//-------------------------------------------------------------------

systemThreadShares::~systemThreadShares()
{
	//pthread_mutex_destroy(&mutex);		
}

//-------------------------------------------------------------------

unsigned long
systemThreadShares::set(void *a_data)
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return 0;
		#endif
	
	data = a_data;
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return 0;
		#endif*/
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreadShares::del(unsigned long position)
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif*/
}

//-------------------------------------------------------------------

void *						 
systemThreadShares::lock(unsigned long position,
						unsigned long microseconds)
{
	/*if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return NULL;
			#endif
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
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
										
				if (nanosleep(&timeout, NULL) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
				
				slept += 1;
				
				if (slept > microseconds)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_LOCK,ERR_ERRNO,SYSTEMTHREADSHARES_CANNOTLOCK,SYSTEMTHREADSHARES_CANNOTLOCK_STR,__LINE__,__FILE__);
					#else
						return NULL;
					#endif
			}
			else
				locked = false;
		}	
	}
	
	return data;*/
}
							
//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool 
#endif						 
systemThreadShares::unlock(unsigned long position)
{
	/*errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARES,SYSTEMTHREADSHARES_UNLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		return true;
	#endif*/
}
							
//-------------------------------------------------------------------

bool						 
systemThreadShares::getShare(unsigned long position)
{
	std::list<__shareInfo>::const_iterator i(shares.begin()), j(shares.end());
	for (;i!=j;++i)
		if (i->position == position)
		{
			current = *((std::list<__shareInfo>::iterator *)&i);
			return true;
		}
	
	return false;	
}

//-------------------------------------------------------------------
