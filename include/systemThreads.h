/***************************************************************************
 *            systemThreads.h
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
 
#ifndef _SYSTEMTHREADS_H_
#define _SYSTEMTHREADS_H_

#include <directives.h>

#include <types.h>
#include <systemThreadsEx.h>

#include <pthread.h>
#include <vector>

namespace dodo
{

	/**
	 * @typedef describes function to be passed as thread
	 */
	typedef void *(*threadFunc)(void *);

	/**
	 * @struct __thredInfo describes thread
	 */
	struct __threadInfo
	{
		pthread_t thread;///< thread descriptor
		void *data;///< data that will be passed on run
		bool isRunning;///< whether thread is running
		int position;///< position in queue
		threadFunc func;///< function to execute
	};
	/**
	 * @class systemThreads is to manage threads(based on POSIX threads)
	 */
	class systemThreads
	{
		public:
		
			/**
			 * constructor
			 */
			systemThreads();
			
			/**
			 * destructor
			 */
			virtual ~systemThreads();
		
			/**
			 * adds function to became a thread[not executing]
			 * @return position of thread in queue
			 * @param func indicates function to be executed
			 * @paraqm data describes data to be passed to func
			 */
			virtual int add(threadFunc func, void *data);
			
			/**
			 * removes registered thread
			 * @param position indicates on thread to remove
			 * @param force if is set to true stops execution if this thread is running
			 * @note - exception if it's currently running
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 
							del(int position, bool force=false);
							
			/**
			 * replaces function to became a thread[not executing]
			 * @param position indicates on thread to replace
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 * @param force if is set to true stops execution if this thread is running
			 * @note - exception if it's currently running
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 
							replace(int position, threadFunc func, void *data, bool force=false);
			
			/**
			 * executes thread
			 * @param position indicates what thread to run
			 * @param force if is set to true permits execution even if this thread is running
			 * @note - exception if it's currently running
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 
							run(int position, bool force=false);

			/**
			 * stops thread
			 * @param position indicates what thread to stop
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 		
							stop(int position);

			/**
			 * stops all registered threads
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 		
							stop();
										
			/**
			 * waits for thread's termination
			 * @param position indicates for what thread to wait
			 * @param data points on data that thread returned
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 	
							wait(int position, void **data=NULL);
										
			/**
			 * @return true if thread is running
			 * @param position indicates for what thread to indicate
			 */
			virtual bool isRunning(int position);
							
			/**
			 * waits for all registered threads' termination
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 	
							wait();
			
			/**
			 * returns from thread and sets return data
			 * @param data points on data that thread returned
			 */
			static void returnFromThread(void *data);			
			
		protected:
		
			/**
			 * searches threads by position
			 * @return true if found
			 * @param position describes position of wanted thread
			 * @param iter is iterator that points on found thread
			 */
			virtual bool getThread(int position);
						
			std::vector<__threadInfo> threads;///< vector of threads
			__threadInfo thread;///< temp storage for thread
			int threadNum;///< number of registered threads

			std::vector<__threadInfo>::iterator i;///< iterator for list of threads
			std::vector<__threadInfo>::iterator j;///< iterator for list of threads
			std::vector<__threadInfo>::iterator k;///< iterator for list of threads[for matched]
	};

};

#endif /* _XMLTOOLS_H */
