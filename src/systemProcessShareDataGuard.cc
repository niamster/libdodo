/***************************************************************************
 *            systemProcessShareDataGuard.cc
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


#include <systemProcessShareDataGuard.h>
	
using namespace dodo;

systemProcessShareDataGuard::systemProcessShareDataGuard(systemProcessShareDataGuard &sts)
{
}

//-------------------------------------------------------------------

systemProcessShareDataGuard::systemProcessShareDataGuard(int id) : data(NULL)
{
	key_t key = ftok("/tmp", id);
	
	semaphore = semget(key, 1, 0660 | IPC_EXCL);
	if (semaphore == -1 && errno == EEXIST)
		semaphore = semget(key, 1, 0660);
	else
	{
		semOptions.val = 1;
		
		semctl(semaphore, 1, SETVAL, semOptions);
	}
}

//-------------------------------------------------------------------

systemProcessShareDataGuard::~systemProcessShareDataGuard()
{	
	semctl(semaphore, 1, IPC_RMID, semOptions);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif 
systemProcessShareDataGuard::set(void *a_data)
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	data = a_data;
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_SET, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
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
systemProcessShareDataGuard::del()
{
	/*errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_DEL, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif*/
}

//-------------------------------------------------------------------

void *						 
systemProcessShareDataGuard::lock(unsigned long microseconds)
{
	/*if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
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
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
										
				if (nanosleep(&timeout, NULL) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_LOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
					#else
						return NULL;
					#endif
				
				slept += 1;
				
				if (slept > microseconds)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_LOCK, ERR_ERRNO, SYSTEMPROCESSSHAREDATAGUARD_CANNOTLOCK, SYSTEMPROCESSSHAREDATAGUARD_CANNOTLOCK_STR,__LINE__,__FILE__);
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
systemProcessShareDataGuard::unlock()
{
	/*errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDATAGUARD, SYSTEMPROCESSSHAREDATAGUARD_UNLOCK, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		return true;
	#endif*/
}
							
//-------------------------------------------------------------------
