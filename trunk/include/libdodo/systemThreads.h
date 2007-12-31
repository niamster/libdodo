/***************************************************************************
 *            systemThreads.h
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

#ifndef _SYSTEMTHREADS_H_
#define _SYSTEMTHREADS_H_

#include <libdodo/directives.h>

#ifdef PTHREAD_EXT

	#ifdef DL_EXT

		#include <dlfcn.h>

	#endif

	#include <signal.h>
	#include <pthread.h>

	#include <libdodo/systemJobs.h>
	#include <libdodo/systemTools.h>
	#include <libdodo/systemThreadsEx.h>
	#include <libdodo/types.h>

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
		SYSTEMTHREADS_KEEP_ALIVE,
		SYSTEMTHREADS_STOP,
		SYSTEMTHREADS_WAIT
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

		pthread_t thread;               ///< thread descriptor
		void *data;                     ///< data that will be passed on run
		bool isRunning;                 ///< whether thread is running
		bool detached;                  ///< if thread is detached
		unsigned long position;         ///< position in queue
		threadFunc func;                ///< function to execute
		int stackSize;                  ///< amount of stack for thread[in bytes]
		short action;                   ///< action on class destruction[see systemThreadOnDestructEnum]
		unsigned long executed;         ///< amount of times thread was executed
		unsigned long executeLimit;     ///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);

			#ifdef DL_EXT
		void *handle;        ///< handle to module
			#endif
	};

		#ifdef DL_EXT

	/**
	 * @struct __systemThreadsMod must be returned from initSystemThreadsModule in the module
	 */
	struct __systemThreadsMod
	{
		char name[64];                      ///< name of module
		char discription[256];              ///< discription of module
		char hook[64];                      ///< name of function in module that will be a hook
		unsigned long executeLimit;         ///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);
		bool detached;                      ///< if thread is detached
		int stackSize;                      ///< amount of stack for thread[in bytes]
		short action;                       ///< action on class destruction[see systemThreadOnDestructEnum]
	};

	/**
	 * @typedef describes function in module that must return info for the hook
	 */
	typedef __systemThreadsMod (*initSystemThreadsModule)(void *);

	/**
	 * @typedef describes function in module that will be called during module unloading
	 */
	typedef void (*deinitSystemThreadsModule)();

		#endif

	/**
	 * @class systemThreads is to manage threads(based on POSIX threads)
	 */
	class systemThreads : public systemJobs
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
			 * @param data describes data to be passed to func[function should return NULL to exit]
			 * @param detached indicates whether thread will be detached
			 * @param action describes action with thread on destruction if thread is running[see systemThreadOnDestructEnum]
			 * @param stackSize describes stack siae of the thread
			 */
			virtual unsigned long add(threadFunc func, void *data, bool detached, short action, int stackSize=2097152);

			/**
			 * adds function to became a thread[executing]
			 * @return position of thread in queue
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func[function should return NULL to exit]
			 * @param limit indicates the thread's limit on executions
			 * @param detached indicates whether thread will be detached
			 * @param action describes action with thread on destruction if thread is running[see systemThreadOnDestructEnum]
			 * @param stackSize describes stack siae of the thread
			 */
			virtual unsigned long addNRun(threadFunc func, void *data, unsigned long limit=1, bool detached=false, short action=SYSTEMTHREADS_WAIT, int stackSize=2097152);

			/**
			 * adds function to became a job[not executing]
			 * @return position of jobFunc in queue[function should return NULL to exit]
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 * @note
			 * detached=false
			 * action=SYSTEMTHREADS_WAIT
			 * stackSize=2097152
			 */
			virtual unsigned long add(jobFunc func, void *data);

			/**
			 * adds function to became a job[executing]
			 * @return position of job in queue[function should return NULL to exit]
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 * @note
			 * limit=1
			 * detached=false
			 * action=SYSTEMTHREADS_WAIT
			 * stackSize=2097152
			 */
			virtual unsigned long addNRun(jobFunc func, void *data);

			/**
			 * removes registered thread
			 * @param position indicates on thread to remove
			 * @param force if is set to true stops execution if this thread is running
			 * @note - exception if it's currently running
			 */
			virtual void del(unsigned long position, bool force=false);

			/**
			 * replaces function to became a thread[not executing]
			 * @param position indicates on thread to replace
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func[function should return NULL to exit]
			 * @param force if is set to true stops execution if this thread is running
			 * @param detached indicates whether thread will be detached
			 * @param action describes action with thread on destruction if thread is running[see systemThreadOnDestructEnum]
			 * @param stackSize describes stack siae of the thread
			 * @note - exception if it's currently running
			 */
			virtual void replace(unsigned long position, threadFunc func, void *data, bool force=false, bool detached=false, short action=SYSTEMTHREADS_WAIT, int stackSize=2097152);

			/**
			 * executes thread
			 * @param position indicates what thread to run
			 * @param force if is set to true permits execution even if this thread is running
			 * @note - exception if it's currently running
			 */
			virtual void run(unsigned long position, bool force=false);

			/**
			 * stops thread
			 * @param position indicates what thread to stop
			 */
			virtual void stop(unsigned long position);

			/**
			 * stops all registered threads
			 */
			virtual void stop();

			/**
			 * waits for thread's termination
			 * @param position indicates for what thread to wait
			 */
			virtual void wait(unsigned long position);

			/**
			 * waits for all registered threads' termination
			 */
			virtual void wait();

			/**
			 * @return true if thread is running
			 * @param position indicates for what thread to indicate
			 */
			virtual bool isRunning(unsigned long position) const;

			/**
			 * @return amount of running threads
			 */
			virtual unsigned long running() const;

			/**
			 * sweep threads if their time are already passed
			 */
			virtual void sweepTrash();
			
			/**
			 * @return list of jobs in object
			 */
			virtual dodoList<unsigned long> getJobsIds();

			/**
			 * set maximum execution time
			 * @param position indicates for what thread to set limit
			 * @param limit indicates the thread's limit on executions
			 */
			virtual void setExecutionLimit(unsigned long position, unsigned long limit=1);


				#ifdef DL_EXT

			/**
			 * adds function to became a thread[not executing] from module
			 * @return position of thread in queue
			 * @param module indicates mudule where is function to be executed
			 * @param data describes data to be passed to func
			 * @param toInit indicates data that will path to initialize function
			 * @param detached indicates whether thread will be detached
			 * @param action describes action with thread on destruction if thread is running
			 * @param stackSize describes stack siae of the thread
			 */
			virtual unsigned long add(const dodoString &module, void *data, void *toInit=NULL, bool detached=false, short action=SYSTEMTHREADS_WAIT, int stackSize=2097152);

			/**
			 * adds function to became a thread[not executing] from module
			 * @return position of thread in queue
			 * @param module indicates mudule where is function to be executed
			 * @param data describes data to be passed to func
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual unsigned long add(const dodoString &module, void *data, void *toInit=NULL);

			/**
			 * @return info about module
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 */
			static __systemThreadsMod getModuleInfo(const dodoString &module, void *toInit=NULL);

				#endif

			/**
			 * block or unblock signals to thread
			 * @param signal indicates what signals to block/unblock; can be or'ed;
			 * @param block indicates whether to block or unblock
			 */
			static void blockSignal(int signals, bool block=true);

		protected:

			/**
			 * @return true if thread is running
			 * @param position indicates for what thread to indicate
			 */
			virtual bool _isRunning(dodoList<__threadInfo>::iterator &position) const;

			/**
			 * searches threads by position
			 * @return true if found
			 * @param position describes position of wanted thread
			 * @note sets internal parameter 'current' to found thread
			 */
			virtual bool getThread(unsigned long position) const;

			mutable dodoList<__threadInfo> threads;                        ///< vector of threads

			unsigned long threadNum;                                ///< number of registered threads

			pthread_attr_t attr;                                    ///< attribute that indicates joinability

			mutable dodoList<__threadInfo>::iterator current;      ///< iterator for list of threads[for matched with getThread method]
	};

};

#endif

#endif
