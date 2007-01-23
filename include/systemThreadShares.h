/***************************************************************************
 *            systemThreadShares.h
 *
 *  Tue Nov 29 23:31:55 2005
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
 
#ifndef _SYSTEMTHREADSHARES_H_
#define _SYSTEMTHREADSHARES_H_
	
#include <pthread.h>
#include <time.h>

#include <directives.h>

#include <systemThreadSharesEx.h>
#include <types.h>
#include <baseEx.h>

namespace dodo
{
	/**
	 * @class systemThreads is to manage threads(based on POSIX threads)
	 */
	class systemThreadShares
	{
		private:
		
			/**
			 * copy constructor
			 * to prevent copying
			 */
			systemThreadShares(systemThreadShares &sts);
			
		public:
		
			/**
			 * constructor
			 */
			systemThreadShares();
			
			/**
			 * destructor
			 */
			virtual ~systemThreadShares();

			/**
			 * adds data to became a shared
			 * @paraqm data describes data to be shared
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
			 * lock and return shared data [if locked and force==false, wait until unlocked]
			 * @param data points on shared data or NULL in error case
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
    		pthread_mutex_t mutex;///< lock
						
			timespec timeout;///timeout to lock mutex check
	};

};

#endif

