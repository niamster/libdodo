/***************************************************************************
 *            systemThreadShare.cc
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


#include <systemThreadShare.h>
	
using namespace dodo;

systemThreadShare::systemThreadShare(systemThreadShare &sts)
{
}

//-------------------------------------------------------------------

systemThreadShare::systemThreadShare() : data(NULL),
										userData(false)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mutex,&attr);

	pthread_mutexattr_destroy(&attr);
	
	timeout.tv_nsec = 1000;
	timeout.tv_sec = 0;	
}

//-------------------------------------------------------------------

systemThreadShare::~systemThreadShare()
{
	pthread_mutex_lock(&mutex);

	if (!userData && data != NULL)
		free(data);
	
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_destroy(&mutex);		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif 
systemThreadShare::set(void *a_data)
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (!userData && data != NULL)
		free(data);
	
	data = a_data;
	
	userData = true;
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif 
systemThreadShare::set(unsigned long size)
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (!userData && data != NULL)
		free(data);
	
	data = malloc(size);
	if (data == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	userData = false;
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_SET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreadShare::del()
{
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (!userData && data != NULL)
		free(data);
		
	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

void *						 
systemThreadShare::lock(unsigned long microseconds)
{
	if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
										
				if (nanosleep(&timeout, NULL) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_LOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
				
				slept += 1;
				
				if (slept > microseconds)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_LOCK,ERR_ERRNO,SYSTEMTHREADSHARE_CANNOTLOCK,SYSTEMTHREADSHARE_CANNOTLOCK_STR,__LINE__,__FILE__);
					#else
						return NULL;
					#endif
			}
			else
				locked = false;
		}	
	}
	
	return data;
}
							
//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool 
#endif						 
systemThreadShare::unlock()
{
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADSHARE,SYSTEMTHREADSHARE_UNLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		return true;
	#endif
}
							
//-------------------------------------------------------------------
