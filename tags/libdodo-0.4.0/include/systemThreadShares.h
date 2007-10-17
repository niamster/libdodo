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
	 * @struct __sharedInfo dewscribes shared info that can be locked
	 */
	struct __shareInfo
	{
		void *data;///< data that will be shared
		int position;///< position in queue
		pthread_mutex_t mutex;///< lock
		bool isLocked;///< whether share is locked
	};

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
			 * @return position of shared in queue
			 * @paraqm data describes data to be shared
			 */
			virtual int add(void *data);
			
			/**
			 * removes registered shared[do not deletes data]
			 * @param position indicates on shared to remove
			 * @param force if is set to true remove if this shared is locked
			 * @note - exception if it's currently locked
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool 
			#endif						 
							del(int position, bool force=false);
			
			/**
			 * lock and return shared data [if locked and force==false, wait until unlocked]
			 * @param position indicates on shared to return
			 * @param data points on shared data or NULL in error case
			 * @param microseconds indicates how many time to wait for locking; if time expired and can't unlock - error =(; only if realization of pthreads supports it!
			 * @note if microseconds==0 - infinite sleep
			 */
			virtual void *lock(int position, unsigned long microseconds=0);
							
			
			/**
			 * lock and return shared data [if locked and force==false, wait until unlocked]
			 * @param position indicates on shared to return
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool 
			#endif						 
							unlock(int position);			
			
		protected:

			/**
			 * searches shared resources by position
			 * @return true if found
			 * @param position describes position of wanted shared
			 * @param iter is iterator that points on found shared
			 */
			virtual bool getShared(int position);
			
			std::list<__shareInfo> shareds;///< vector of shareads
			
			int sharedNum;///< number of registered shareads
						
			pthread_mutexattr_t attr;///attribute for mutex
			timespec timeout;///timeout to lock mutex
						
			std::list<__shareInfo>::iterator n;///< iterator for list of shared data[for matched]
	};

};

#endif

