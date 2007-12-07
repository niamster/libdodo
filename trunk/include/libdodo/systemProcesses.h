/***************************************************************************
 *            systemProcesses.h
 *
 *  Tue Feb 27 08:38:55 2007
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

#ifndef _SYSTEMPROCESSES_H_
#define _SYSTEMPROCESSES_H_

#ifdef DL_EXT

	#include <dlfcn.h>

#endif

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <libdodo/directives.h>
#include <libdodo/types.h>
#include <libdodo/systemJobs.h>
#include <libdodo/systemProcessesEx.h>

namespace dodo
{

	/**
	 * @typedef describes function to be passed as process
	 */
	typedef void *(*processFunc)(void *);

	/**
	 *
	 */
	enum systemProcessOnDestructEnum
	{
		SYSTEMPROCESS_KEEP_ALIVE,
		SYSTEMPROCESS_STOP, ///< send SIGINT to process
		SYSTEMPROCESS_WAIT
	};

	/**
	 * @struct __processInfo describes process
	 */
	struct __processInfo
	{
		/**
		 * constuctor
		 */
		__processInfo();

		pid_t pid;                  ///< process pid
		void *data;                 ///< data that will be passed on run
		bool isRunning;             ///< whether process is running
		unsigned long position;     ///< position in queue
		processFunc func;           ///< function to execute
		short action;               ///< action on class destruction[see systemProcessOnDestructEnum]
		unsigned long executed;     ///< amount of times thread was executed
		unsigned long executeLimit; ///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);

		#ifdef DL_EXT
		void *handle;    ///< handle to module
		#endif
	};

	#ifdef DL_EXT

	/**
	 * @struct systemProcessesMod must be returned from initSystemProcessesModule in the module
	 */
	struct systemProcessesMod
	{
		char name[64];                  ///< name of module
		char discription[256];          ///< discription of module
		char hook[64];                  ///< name of function in module that will be a hook
		unsigned long executeLimit;     ///< if more than one will be autodleted or with `sweepTrash` method; default is 0(unlimit);
		short action;                   ///< action on class destruction[see systemProcessOnDestructEnum]
	};

	/**
	 * @typedef describes function in module that must return info for the hook
	 */
	typedef systemProcessesMod (*initSystemProcessesModule)(void *);

	/**
	 * @typedef describes function in module that will be called during module unloading
	 */
	typedef void (*deinitSystemProcessesModule)();

	#endif

	class systemProcesses : public systemJobs
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			systemProcesses(systemProcesses &sp);

		public:

			/**
			 * constructor
			 */
			systemProcesses();

			/**
			 * destructor
			 */
			virtual ~systemProcesses();


			/**
			 * adds function to became a process[not executing]
			 * @return position of process in queue
			 * @param func indicates function to be executed[function should return NULL to exit]
			 * @param data describes data to be passed to func
			 * @param action describes action with thread on destruction if process is running[see systemProcessOnDestructEnum]
			 * @note func must not call `exit` family call
			 */
			virtual unsigned long add(processFunc func, void *data, short action);

			/**
			 * adds function to became a process[executing]
			 * @return position of process in queue
			 * @param func indicates function to be executed[function should return NULL to exit]
			 * @param data describes data to be passed to func
			 * @param limit indicates the thread's limit on executions
			 * @param action describes action with thread on destruction if process is running[see systemProcessOnDestructEnum]
			 * @note func must not call `exit` family call
			 */
			virtual unsigned long addNRun(processFunc func, void *data, unsigned long limit = 1, short action = SYSTEMPROCESS_WAIT);

			/**
			 * adds function to became a job[not executing]
			 * @return position of jobFunc in queue[function should return NULL to exit]
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 * @note func must not call `exit` family call
			 * action=SYSTEMPROCESS_WAIT
			 */
			virtual unsigned long add(jobFunc func, void *data);

			/**
			 * adds function to became a job[executing]
			 * @return position of job in queue[function should return NULL to exit]
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 * @note func must not call `exit` family call
			 * limit=1
			 * action=SYSTEMPROCESS_WAIT
			 */
			virtual unsigned long addNRun(jobFunc func, void *data);

			/**
			 * removes registered process
			 * @param position indicates on process to remove
			 * @param force if is set to true stops execution if this process is running
			 * @note - exception if it's currently running
			 * send SIGINT to process
			 */
			virtual void
			del(unsigned long position, bool force = false);

			/**
			 * replaces function to became a process[not executing]
			 * @param position indicates on process to replace
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func[function should return NULL to exit]
			 * @param force if is set to true stops execution if this process is running
			 * @param action describes action with thread on destruction if process is running[see systemProcessOnDestructEnum]
			 * @note - exception if it's currently running
			 */
			virtual void
			replace(unsigned long position, processFunc func, void *data, bool force = false, short action = SYSTEMPROCESS_WAIT);

			/**
			 * executes process
			 * @param position indicates what process to run
			 * @param force if is set to true permits execution even if this process is running
			 * @note - exception if it's currently running
			 */
			virtual void
			run(unsigned long position, bool force = false);

			/**
			 * stops process
			 * @param position indicates what process to stop
			 * @note sends signal 9 to preocess
			 */
			virtual void
			stop(unsigned long position);

			/**
			 * stops all registered processes
			 */
			virtual void
			stop();

			/**
			 * waits for process's termination
			 * @param position indicates for what process to wait
			 */
			virtual void
			wait(unsigned long position);

			/**
			 * waits for all registered process' termination
			 */
			virtual void
			wait();

			/**
			 * @return true if process is running
			 * @param position indicates for what process to indicate
			 */
			virtual bool isRunning(unsigned long position) const;

			/**
			 * @return amount of running processes
			 */
			virtual unsigned long running() const;

			/**
			 * sweep processes if their time are already passed
			 */
			virtual void sweepTrash();

			/**
			 * set maximum execution time
			 * @param position indicates for what process to set limit
			 * @param limit indicates the process' limit on executions
			 */
			virtual void
			setExecutionLimit(unsigned long position, unsigned long limit = 1);


			#ifdef DL_EXT

			/**
			 * adds function to became a process[not executing] from module
			 * @return position of process in queue
			 * @param module indicates mudule where is function to be executed
			 * @param data describes data to be passed to func
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual unsigned long add(const dodoString &module, void *data, void *toInit = NULL);


			/**
			 * @return info about module
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 */
			static systemProcessesMod getModuleInfo(const dodoString &module, void *toInit = NULL);

			#endif

		protected:

			/**
			 * @return true if thread is process
			 * @param position indicates for what process to indicate
			 */
			virtual bool _isRunning(std::list<__processInfo>::iterator &position) const;

			/**
			 * searches processes by position
			 * @return true if found
			 * @param position describes position of wanted process
			 * @note sets internal parameter 'current' to found process
			 */
			virtual bool getProcess(unsigned long position) const;

			unsigned long processNum;                           ///< number of registered processes

			std::list<__processInfo> processes;                 ///< vector of processes

			mutable std::list<__processInfo>::iterator current; ///< iterator for list of processes[for matched with getProcess method]
	};

};

#endif
