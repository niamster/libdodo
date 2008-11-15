/***************************************************************************
 *            pcThreadCollection.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCTHREADCOLLECTION_H_
#define _PCTHREADCOLLECTION_H_ 1

#include <libdodo/directives.h>

#ifdef DL_EXT

#include <dlfcn.h>

#endif

#include <signal.h>

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#include <libdodo/pcJobCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/pcThreadCollectionEx.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace pc
	{
		namespace thread
		{
			/**
			 * @enum collectionOnDestructEnum defines action with thread on object destruction
			 */
			enum collectionOnDestructEnum
			{
				COLLECTION_ONDESTRUCT_KEEP_ALIVE,
				COLLECTION_ONDESTRUCT_STOP,
				COLLECTION_ONDESTRUCT_WAIT
			};

			/**
			 * @struct __threadInfo
			 * @brief defines process information
			 */
			struct __threadInfo
			{
				/**
				 * contructor
				 */
				__threadInfo();

#ifdef PTHREAD_EXT

				pthread_t thread;                                               ///< thread descriptor

#endif

				void *data;                                                             ///< thread data
				bool isRunning;                                                         ///< true if thread is running
				bool detached;                                                          ///< true if thread is detached
				unsigned long position;                                                 ///< identificator
				job::routine func;                                                      ///< function to execute
				int stackSize;                                                          ///< size of stack for thread[in bytes]
				short action;                                                           ///< action on object destruction[see collectionOnDestructEnum]
				unsigned long executed;                                                 ///< amount of times thread was executed
				unsigned long executeLimit;                                             ///< if greater than one will be a atomatically deleted or deleted with `sweepTrash` method; default is 0(unlimit);

#ifdef DL_EXT
				void *handle;                                        ///< handle to library
#endif
			};

#ifdef DL_EXT

			/**
			 * @struct __threadMod
			 * @brief defines data that is returned from initIpcThreadCollectionModule in the library
			 */
			struct __threadMod
			{
				char name[64];                                                          ///< name of module
				char discription[256];                                                  ///< discription of module
				char hook[64];                                                          ///< name of function in module that will be a hook
				unsigned long executeLimit;                                             ///< if greater than one will be a atomatically deleted or deleted with `sweepTrash` method
				bool detached;                                                          ///< true if thread is detached
				int stackSize;                                                          ///< size of stack for thread[in bytes]
				short action;                                                           ///< action on object destruction[see collectionOnDestructEnum]
			};

			/**
			 * @typedef initIpcThreadCollectionModule
			 * @brief defines type of init function for library
			 * @param data defines user data
			 */
			typedef __threadMod (*initIpcThreadCollectionModule)(void *data);

			/**
			 * @typedef deinitIpcThreadCollectionModule
			 * @brief defines type of deinit function for library
			 */
			typedef void (*deinitIpcThreadCollectionModule)();

#endif

			/**
			 * @class collection
			 * @brief provides threads management functionality
			 */
			class collection : public job::collection
			{
				private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					collection(collection &st);

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
					 * add function to became a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param detached defines whether thread will be detached
					 * @param action defines action on object destruction if thread is running[see collectionOnDestructEnum]
					 * @param stackSize defines stack thread size
					 */
					virtual unsigned long add(job::routine func, void *data, bool detached, short action, int stackSize = 2097152);

					/**
					 * add function to became a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param limit defines limit on executions
					 * @param detached defines whether thread will be detached
					 * @param action defines action on object destruction if thread is running[see collectionOnDestructEnum]
					 * @param stackSize defines stack thread size
					 * @note this will immediately execute the process
					 */
					virtual unsigned long addNRun(job::routine func, void *data, unsigned long limit, bool detached, short action, int stackSize = 2097152);

					/**
					 * add function to became a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @note
					 * detached=false
					 * action=COLLECTION_ONDESTRUCT_WAIT
					 * stackSize=2097152
					 */
					virtual unsigned long add(job::routine func, void *data);

					/**
					 * add function to became a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @note this will immediately execute the process
					 * limit=1
					 * detached=false
					 * action=COLLECTION_ONDESTRUCT_WAIT
					 * stackSize=2097152
					 */
					virtual unsigned long addNRun(job::routine func, void *data);

					/**
					 * remove registered thread
					 * @param position defines thread identificator
					 * @param force defines termination condition; if true and thread is running stop execution of the process
					 */
					virtual void del(unsigned long position, bool force = false);

					/**
					 * replace thread function
					 * @param position defines thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param force defines termination condition; if true and thread is running stop execution of the process
					 * @param detached defines whether thread will be detached
					 * @param action defines action on object destruction if thread is running[see collectionOnDestructEnum]
					 * @param stackSize defines stack thread size
					 */
					virtual void replace(unsigned long position, job::routine func, void *data, bool force = false, bool detached = false, short action = COLLECTION_ONDESTRUCT_WAIT, int stackSize = 2097152);

					/**
					 * execute thread
					 * @param position defines thread identificator
					 * @param force defines run condition; if true and thread is running run thread anyway
					 */
					virtual void run(unsigned long position, bool force = false);

					/**
					 * stop thread
					 * @param position defines thread identificator
					 */
					virtual void stop(unsigned long position);

					/**
					 * stop all registered threads
					 */
					virtual void stop();

					/**
					 * wait for thread termination
					 * @param position defines thread identificator
					 */
					virtual void wait(unsigned long position);

					/**
					 * wait for all registered threads termination
					 */
					virtual void wait();

					/**
					 * @return true if thread is running
					 * @param position defines thread identificator
					 */
					virtual bool isRunning(unsigned long position) const;

					/**
					 * @return amount of running threads
					 */
					virtual unsigned long running() const;

					/**
					 * sweep threads if their time has been already passed
					 */
					virtual void sweepTrash();

					/**
					 * @return list of threads in object
					 */
					virtual dodoList<unsigned long> getIds();

					/**
					 * set maximum execution time
					 * @param position defines thread identificator
					 * @param limit defines the limit on executions of the thread
					 */
					virtual void setExecutionLimit(unsigned long position, unsigned long limit = 1);


#ifdef DL_EXT

					/**
					 * add function as a thread from library
					 * @return thread identificator
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param data defines thread data
					 * @param toInit defines library init data
					 */
					virtual unsigned long add(const dodoString &module, void *data, void *toInit = NULL);

					/**
					 * @return info about library
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param toInit defines library init data
					 */
					static __threadMod getModuleInfo(const dodoString &module, void *toInit = NULL);

#endif

					/**
					 * block signals to thread
					 * @param signals defines signals to block/unblock[see tools::osSignalsEnum]
					 * @param block defines block condition
					 */
					static void blockSignal(int signals, bool block = true);

				protected:

					/**
					 * @return true if thread is running
					 * @param position indicates for what thread to set limit
					 */
					virtual bool _isRunning(dodoList<__threadInfo>::iterator &position) const;

					/**
					 * search threads by identificator
					 * @return true if thread has been found
					 * @param position defines thread identificator
					 * @note this sets internal class parameter 'current' to found thread
					 */
					virtual bool getThread(unsigned long position) const;

					mutable dodoList<__threadInfo> threads;                                                                 ///< identificators of threads

					unsigned long threadNum;                                                                                ///< number of registered threads

#ifdef PTHREAD_EXT

					pthread_attr_t attr;                                                                                    ///< thread join attribute

#endif

					mutable dodoList<__threadInfo>::iterator current;                                                       ///< iterator for list of threads[for matched with getThread method]
			};
		};
	};
};

#endif
