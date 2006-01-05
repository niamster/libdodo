/***************************************************************************
 *            systemThreadShares.h
 *
 *  Tue Nov 29 23:31:55 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SYSTEMTHREADSHARES_H_
#define _SYSTEMTHREADSHARES_H_

#include <directives.h>

#include <types.h>
#include <systemThreadSharesEx.h>

#include <pthread.h>
#include <vector>

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
			 * @param force if is set to true get if this shared is locked
			 */
			#ifndef NO_EX
				virtual void
			#else
				virtual bool 
			#endif						 
							lock(int position, void *data, bool force=false);
			
			
		protected:

			/**
			 * searches shared resources by position
			 * @return true if found
			 * @param position describes position of wanted shared
			 * @param iter is iterator that points on found shared
			 */
			virtual bool getShared(int position);
			
			std::vector<__shareInfo> shareds;///< vector of threads
			__shareInfo shared;///< temp storage for thread
			int sharedNum;///< number of registered threads			
						
			std::vector<__shareInfo>::iterator l;///< iterator for list of shared data
			std::vector<__shareInfo>::iterator m;///< iterator for list of shared data
			std::vector<__shareInfo>::iterator n;///< iterator for list of shared data[for matched]
	};

};

#endif /* _XMLTOOLS_H */
