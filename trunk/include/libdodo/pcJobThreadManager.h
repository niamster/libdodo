/***************************************************************************
 *            pcJobThreadManager.h
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

#ifndef _PCJOBTHREADMANAGER_H_
#define _PCJOBTHREADMANAGER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcJobManager.h>
#include <libdodo/types.h>

namespace dodo {
	namespace pc {
		namespace job {
			namespace thread {
				struct __thread__;

#ifdef DL_EXT
				/**
				 * @struct __module__
				 * @brief defines data that is returned from initModule in the library
				 */
				struct __module__ {
					char          name[64];         ///< name of module
					char          discription[256]; ///< discription of module
					char          hook[64];         ///< name of function in module that will be a hook
					bool          detached;         ///< true if thread is detached
					int           stackSize;        ///< size of stack for thread[in bytes]
					short         action;           ///< action on object destruction, @see job::onDestructionEnum
				};

				/**
				 * @typedef initModule
				 * @brief defines type of init function for library
				 * @param data defines user data
				 * @note name in the library must be initPcJobThreadManagerModule
				 */
				typedef __module__ (*initModule)(void *data);

				/**
				 * @typedef deinitModule
				 * @brief defines type of deinit function for library
				 * @note name in the library must be deinitPcJobThreadManagerModule
				 */
				typedef void (*deinitModule)();
#endif

				/**
				 * @class manager
				 * @brief provides threads management functionality
				 */
				class manager : public job::manager {
				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					manager(manager &);

				  public:

					/**
					 * constructor
					 */
					manager();

					/**
					 * destructor
					 */
					virtual ~manager();

					/**
					 * add function to run as a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param action defines action on object destruction if thread is running, @see job::onDestructionEnum
					 */
					virtual unsigned long add(job::routine func,
											  void         *data,
											  short        action);

					/**
					 * add function to run as a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param action defines action on object destruction if thread is running, @see job::onDestructionEnum
					 * @note this will immediately execute the process
					 */
					virtual unsigned long addNRun(job::routine  func,
												  void          *data,
												  short         action);

					/**
					 * add function to run as a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param action defines action on object destruction if thread is running, @see job::onDestructionEnum
					 * @note this will immediately execute the process
					 * the thread will be detached
					 */
					virtual unsigned long addNRunDetached(job::routine  func,
														  void          *data,
														  short         action);

					/**
					 * remove registered thread
					 * @param id defines thread identificator
					 * @param terminate defines termination condition; if true and thread is running stop execution of the process
					 */
					virtual void remove(unsigned long id,
										bool          terminate = false);

					/**
					 * execute thread
					 * @param id defines thread identificator
					 * @param force defines run condition; if true and thread is running run thread anyway
					 */
					virtual void run(unsigned long id,
									 bool          force = false);

					/**
					 * stop thread
					 * @param id defines thread identificator
					 */
					virtual void stop(unsigned long id);

					/**
					 * stop all registered threads
					 */
					virtual void stop();

					/**
					 * wait for thread termination
					 * @return status of the job
					 * @param id defines thread identificator
					 */
					virtual int wait(unsigned long id);

					/**
					 * wait for all registered threads termination
					 */
					virtual void wait();

					/**
					 * @return true if thread is running
					 * @param id defines thread identificator
					 */
					virtual bool isRunning(unsigned long id) const;

					/**
					 * @return amount of running threads
					 */
					virtual unsigned long running() const;

					/**
					 * @return list of threads in object
					 */
					virtual dodoList<unsigned long> jobs();

					/**
					 * set maximum stack size
					 * @param id defines thread identificator
					 * @param size defines stack size of the thread
					 */
					virtual void setStackSize(unsigned long id,
											  unsigned long size);


#ifdef DL_EXT
					/**
					 * add function as a thread from library
					 * @return thread identificator
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param data defines thread data
					 * @param toInit defines library init data
					 */
					virtual unsigned long add(const dodoString &module,
											  void             *data,
											  void             *toInit = NULL);

					/**
					 * @return info about library
					 * @param module defines path to the library[if not in ldconfig db] or library name
					 * @param toInit defines library init data
					 */
					static __module__ module(const dodoString &module,
											 void             *toInit = NULL);
#endif

					/**
					 * block signals to thread
					 * @param signals defines signals to block/unblock, @see tools::osSignalsEnum
					 * @param block defines block condition
					 */
					static void blockSignal(int  signals,
											bool block = true);

				  protected:

					/**
					 * add function to run as a thread
					 * @return thread identificator
					 * @param func defines function to execute
					 * @param data defines process data
					 * @param action defines action on object destruction if thread is running, @see job::onDestructionEnum
					 * @param detached defines whether thread will be detached or not
					 * @note this will immediately execute the process
					 */
					unsigned long addNRun(job::routine  func,
										  void          *data,
										  short         action,
										  bool detached);


					/**
					 * @return true if thread is running
					 * @param id indicates for what thread to set limit
					 */
					bool _isRunning(dodoList<__thread__ *>::iterator &id) const;

					/**
					 * search threads by identificator
					 * @return true if thread has been found
					 * @param id defines thread identificator
					 * @note this sets internal class parameter 'current' to found thread
					 */
					bool getThread(unsigned long id) const;

					mutable dodoList<__thread__ *> threads;             ///< identificators of threads

					unsigned long threadNum;                            ///< number of registered threads

#ifdef PTHREAD_EXT
					pthread_attr_t attr;                                ///< thread join attribute
#endif

					mutable dodoList<__thread__ *>::iterator current;   ///< iterator for list of threads[for matched with getThread method]
				};
			};
		};
	};
};
#endif
