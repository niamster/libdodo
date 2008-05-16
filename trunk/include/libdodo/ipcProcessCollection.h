/***************************************************************************
 *            ipcProcessCollection.h
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

#ifndef _IPCPROCESSCOLLECTION_H_
#define _IPCPROCESSCOLLECTION_H_

#ifdef DL_EXT

#include <dlfcn.h>

#endif

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <libdodo/directives.h>
#include <libdodo/types.h>
#include <libdodo/ipcJobCollection.h>
#include <libdodo/ipcProcessCollectionEx.h>

namespace dodo
{
	namespace ipc
	{
		namespace process
		{
			/**
			 * @enum collectionOnDestructEnum defines action with processes on object destruction
			 */
			enum collectionOnDestructEnum
			{
				COLLECTION_ONDESTRUCT_KEEP_ALIVE,
				COLLECTION_ONDESTRUCT_STOP, ///< send SIGINT to process
				COLLECTION_ONDESTRUCT_WAIT
			};

			/**
			 * @struct __processInfo defines process information
			 */
			struct __processInfo
			{
				/**
				 * constuctor
				 */
				__processInfo();

				pid_t pid;                      ///< process pid
				void *data;                     ///< process data
				bool isRunning;                 ///< true process is running
				unsigned long position;         ///< identificator
				job::routine func;              ///< function to execute
				short action;                   ///< action on object destruction[see collectionOnDestructEnum]
				unsigned long executed;         ///< amount of times process was executed
				unsigned long executeLimit;     ///< if greater than one will be a atomatically deleted or deleted with `sweepTrash` method; default is 0(unlimit);

		#ifdef DL_EXT
				void *handle;    ///< handle to library
		#endif
			};

		#ifdef DL_EXT

			/**
			 * @struct __processMod defines data that is returned from initIpcProcessCollectionModule in the library
			 */
			struct __processMod
			{
				char name[64];                  ///< name of module
				char discription[256];          ///< discription of module
				char hook[64];                  ///< name of function in module that will be a hook
				unsigned long executeLimit;     ///< if greater than one will be a atomatically deleted or deleted with `sweepTrash` method
				short action;                   ///< action on object destruction[see collectionOnDestructEnum]
			};

			/**
			 * @typedef initIpcProcessCollectionModule defines type of init function for library
			 */
			typedef __processMod (*initIpcProcessCollectionModule)(void *);

			/**
			 * @typedef deinitIpcProcessCollectionModule defines type of deinit function for library
			 */
			typedef void (*deinitIpcProcessCollectionModule)();

		#endif

			/**
			 * @class collection provides processes management functionality
			 */
			class collection : public job::collection
			{
				private:

					/**
					 * copy constructor
					 * to prevent copying
					 */
					collection(collection &sp);

				public:

					/**
					 * constructor
					 */
					collection();

					/**
					 * destructor
					 */
					virtual ~collection();

					/**
					 * add function as a process
					 * @return process identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param action defines action on object destruction if process is running[see collectionOnDestructEnum]
					 * @note func must not call `exit` family call
					 */
					virtual unsigned long add(job::routine func, void *data, short action);

					/**
					 * add function as a process
					 * @return process identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param limit defines limit on executions
					 * @param action defines action on object destruction if process is running[see collectionOnDestructEnum]
					 * @note func must not call `exit` family call
					 * this will immediately execute the process
					 */
					virtual unsigned long addNRun(job::routine func, void *data, unsigned long limit = 1, short action = COLLECTION_ONDESTRUCT_WAIT);

					/**
					 * add function as a process
					 * @return process identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @note func must not call `exit` family call
					 * action = COLLECTION_ONDESTRUCT_WAIT
					 */
					virtual unsigned long add(job::routine func, void *data);

					/**
					 * add function as a process
					 * @return process identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @note func must not call `exit` family call
					 * this will immediately execute the process
					 * action = COLLECTION_ONDESTRUCT_WAIT
					 */
					virtual unsigned long addNRun(job::routine func, void *data);

					/**
					 * remove registered process
					 * @param position defines process identificator
					 * @param force defines termination condition; if true and process is running stop execution of the process
					 */
					virtual void del(unsigned long position, bool force = false);

					/**
					 * replace process function
					 * @param position defines process identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param force defines termination condition; if true and process is running stop execution of the process
					 * @param action defines action on object destruction if process is running[see collectionOnDestructEnum]
					 */
					virtual void replace(unsigned long position, job::routine func, void *data, bool force = false, short action = COLLECTION_ONDESTRUCT_WAIT);

					/**
					 * execute process
					 * @param position defines process identificator
					 * @param force defines run condition; if true and job is running run process anyway
					 */
					virtual void run(unsigned long position, bool force = false);

					/**
					 * stop process
					 * @param position defines process identificator
					 * @note sends SIGKILL to process
					 */
					virtual void stop(unsigned long position);

					/**
					 * stop all registered processes
					 */
					virtual void stop();

					/**
					 * waits for process termination
					 * @param position defines process identificator
					 */
					virtual void wait(unsigned long position);

					/**
					 * wait for all registered processes termination
					 */
					virtual void wait();

					/**
					 * @return true if process is running
					 * @param position defines process identificator
					 */
					virtual bool isRunning(unsigned long position) const;

					/**
					 * @return amount of running processes
					 */
					virtual unsigned long running() const;

					/**
					 * sweep processes if their time has been already passed
					 */
					virtual void sweepTrash();

					/**
					 * @return list of processes in object
					 */
					virtual dodoList<unsigned long> getJobsIds();

					/**
					 * set maximum execution time
					 * @param position defines process identificator
					 * @param limit defines the limit on executions of the process
					 */
					virtual void setExecutionLimit(unsigned long position, unsigned long limit = 1);

		#ifdef DL_EXT

					/**
					 * add function as a process from library
					 * @return process identificator
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param data defines process data
					 * @param toInit defines library init data
					 */
					virtual unsigned long add(const dodoString &module, void *data, void *toInit = NULL);

					/**
					 * @return info about library
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param toInit defines library init data
					 */
					static __processMod getModuleInfo(const dodoString &module, void *toInit = NULL);

		#endif

				protected:

					/**
					 * @return true if process is running
					 * @param position defines process identificator
					 */
					virtual bool _isRunning(dodoList<__processInfo>::iterator &position) const;

					/**
					 * search processes by identificator
					 * @return true if process has been found
					 * @param position defines process identificator
					 * @note this sets internal class parameter 'current' to found process
					 */
					virtual bool getProcess(unsigned long position) const;

					unsigned long processNum;                           ///< number of registered processes

					dodoList<__processInfo> processes;                  ///< identificators of processes

					mutable dodoList<__processInfo>::iterator current;  ///< iterator for list of processes[for matched with getProcess method]
			};
		};
	};
};

#endif
