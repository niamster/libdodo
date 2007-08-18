/***************************************************************************
 *            systemProcessSharedDataGuard.cc
 *
 *  Sun Jul 22 23:14:14 2007
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


#include <systemProcessSharedDataGuard.h>
	
using namespace dodo;

systemProcessSharedDataGuard::systemProcessSharedDataGuard(systemProcessSharedDataGuard &sts)
{
}

//-------------------------------------------------------------------

systemProcessSharedDataGuard::systemProcessSharedDataGuard(const char *a_key) : data(NULL)
{
	//sem_init(&semaphore, 1, 0);
	
	if (a_key == NULL)
	{
		key = new char[32];
		tools::random(key, 31);
		key[31] = '\0';
	}
	else
	{
		unsigned long len = strlen(a_key);
		key = new char[len + 1];
		strcpy(key, a_key);
	}
	
	
}

//-------------------------------------------------------------------

systemProcessSharedDataGuard::~systemProcessSharedDataGuard()
{
	//sem_destroy(&semaphore);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif 
systemProcessSharedDataGuard::set(void *a_data)
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	data = a_data;
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif*/
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemProcessSharedDataGuard::del()
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif*/
}

//-------------------------------------------------------------------

void *						 
systemProcessSharedDataGuard::lock(unsigned long microseconds)
{
	/*if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
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
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
										
				if (nanosleep(&timeout, NULL) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
				
				slept += 1;
				
				if (slept > microseconds)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_LOCK, ERR_ERRNO, SYSTEMPROCESSSHAREDDATAGUARD_CANNOTLOCK, SYSTEMPROCESSSHAREDDATAGUARD_CANNOTLOCK_STR,__LINE__,__FILE__);
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
systemProcessSharedDataGuard::unlock()
{
	/*errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARD_UNLOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		return true;
	#endif*/
}
							
//-------------------------------------------------------------------
