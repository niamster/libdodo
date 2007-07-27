/***************************************************************************
 *            systemProcessShareDataGuard.h
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
 
#ifndef _SYSTEMPROCESSSHAREDATAGUARD_H_
#define _SYSTEMPROCESSSHAREDATAGUARD_H_

#include <semaphore.h>
	
#include <time.h>

#include <directives.h>

#include <systemProcessShareDataGuardEx.h>
#include <types.h>

namespace dodo
{
	/**
	 * @class systemThreads is to manage threads(based on POSIX threads)
	 */
	class systemProcessShareDataGuard
	{
		private:
		
			/**
			 * copy constructor
			 * to prevent copying
			 */
			systemProcessShareDataGuard(systemProcessShareDataGuard &sts);
			
		public:
		
			/**
			 * constructor
			 * @param id is given to generate proper key
			 */
			systemProcessShareDataGuard(int id='l'+'i'+'b'+'d'+'o'+'d'+'o');
			
			/**
			 * destructor
			 */
			virtual ~systemProcessShareDataGuard();

			/**
			 * sets data to became a shared
			 * @param data describes data to be shared
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool
			#endif
                            set(void *data);
			
			/**
			 * sets shared data to NULL
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool 
			#endif						 
							del();
			
			/**
			 * lock and return shared data
			 * @return data points on shared data or NULL in error case
			 * @param microseconds indicates how many time to wait for locking; if time expired and can't unlock - error =(; only if realization of pthreads supports it!
			 * @note if microseconds==0 - infinite sleep
			 */
			virtual void *lock(unsigned long microseconds=0);
							
			
			/**
			 * lock and return shared data
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool 
			#endif						 
							unlock();
			
		protected:
			
    		void *data;///< data that will be shared
    		sem_t semaphore;///< semaphore
						
			timespec timeout;///< timeout to lock mutex check	
	};

};

#endif

