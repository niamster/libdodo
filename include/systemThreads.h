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

#include <signal.h>
#include <pthread.h>
#include <vector>

namespace dodo
{

	/**
	 * @typedef describes function to be passed as thread
	 */
	typedef void *(*threadFunc)(void *);
	
	/**
	 * @enum systemThreadOnDestructEnum describes what to do with thread if class destructs
	 */
	enum systemThreadOnDestructEnum
	{
		THREAD_KEEP_ALIVE,
		THREAD_STOP,
		THREAD_WAIT
	};
	
	/**
	 * @struct __thredInfo describes thread
	 */
	struct __threadInfo
	{
		/**
		 * contructor
		 */
		__threadInfo();
		 
		pthread_t thread;///< thread descriptor
		void *data;///< data that will be passed on run
		bool isRunning;///< whether thread is running
		bool detached;///< if thread is detached
		unsigned long position;///< position in queue
		threadFunc func;///< function to execute
		int stackSize;///< amount of stack for thread[in bytes]
		systemThreadOnDestructEnum action;///< action on class destruction
		unsigned long executed;///< amount of times thread was executed
		unsigned long executeLimit;///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);
		
		#ifdef DL_EXT
			void *handle;///< handle to module
		#endif		
	};
	
	#ifdef DL_EXT
	
		/**
		 * @struct systemThreadsMod must be returned from initSystemThreadsModule in the module
		 */
		struct systemThreadsMod
		{
			char name[64];///< name of module
			char discription[256];///< discription of module
			char hook[64];///< name of function in module that will be a hook
			unsigned long executeLimit;///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);
			bool detached;///< if thread is detached
			int stackSize;///< amount of stack for thread[in bytes]
			systemThreadOnDestructEnum action;///< action on class destruction			
		};
		
		/**
		 * @typedef describes function in module that must return info for the hook
		 */
		typedef systemThreadsMod (*initSystemThreadsModule)();

		/**
		 * @typedef describes function in module that will be called during module unloading
		 */
		typedef void (*deinitSystemThreadsModule)();
	
	#endif
	
	/**
	 * @class systemThreads is to manage threads(based on POSIX threads)
	 */
	class systemThreads
	{
		private:
		
			/**
			 * copy constructor
			 * to prevent copying
			 */
			systemThreads(systemThreads &st);

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
			virtual unsigned long add(threadFunc func, void *data, bool detached = false, systemThreadOnDestructEnum action=THREAD_WAIT, int stackSize=2097152);
			
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
							del(unsigned long position, bool force=false);
							
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
							replace(unsigned long position, threadFunc func, void *data, bool force=false);
			
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
							run(unsigned long position, bool force=false);

			/**
			 * stops thread
			 * @param position indicates what thread to stop
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 		
							stop(unsigned long position);

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
							wait(unsigned long position, void **data=NULL);
										
			/**
			 * @return true if thread is running
			 * @param position indicates for what thread to indicate
			 */
			virtual bool isRunning(unsigned long position);
			
			/**
			 * @return amount of running threads
			 */
			virtual unsigned long running();
							
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
			
			/**
			 * sweep threads if their time are already passed
			 */
			virtual void sweepTrash();
			
			/**
			 * set maximum execution time
			 * @param position indicates for what thread to set limit
			 * @param limit indicates the thread's limit on executions
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif			
							setExecutionLimit(unsigned long position, unsigned long limit=1); 

			
			#ifdef DL_EXT
			
				/**
				 * @return info about module
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 */
				static systemThreadsMod getModuleInfo(const std::string &module);
			
			#endif
										
		protected:

										
			/**
			 * @return true if thread is running
			 * @param position indicates for what thread to indicate
			 */
			virtual bool _isRunning(std::vector<__threadInfo>::iterator &position);
			
			/**
			 * searches threads by position
			 * @return true if found
			 * @param position describes position of wanted thread
			 * @param iter is iterator that points on found thread
			 */
			virtual bool getThread(unsigned long position);
						
			std::vector<__threadInfo> threads;///< vector of threads
			__threadInfo thread;///< temp storage for thread
			unsigned long threadNum;///< number of registered threads
			pthread_attr_t attr;///< attribute that indicates joinability

			std::vector<__threadInfo>::iterator i;///< iterator for list of threads
			std::vector<__threadInfo>::iterator j;///< iterator for list of threads
			std::vector<__threadInfo>::iterator k;///< iterator for list of threads[for matched]
	};

};

#endif /* _XMLTOOLS_H */
