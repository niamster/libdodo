/***************************************************************************
 *            pcProcessCollection.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCPROCESSCOLLECTION_H_
#define _PCPROCESSCOLLECTION_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/pcJobCollection.h>

namespace dodo {
	namespace pc {
		namespace process {
			struct __process__;

#ifdef DL_EXT
			/**
			 * @struct __module__
			 * @brief defines data that is returned from initPcProcessCollectionModule in the library
			 */
			struct __module__ {
				char          name[64];         ///< name of module
				char          discription[256]; ///< discription of module
				char          hook[64];         ///< name of function in module that will be a hook
				short         action;           ///< action on object destruction, @see job::collection::onDestructionEnum
			};

			/**
			 * @typedef initModule
			 * @brief defines type of init function for library
			 * @param data defines user data
			 * @note name in the library must be initPcProcessModule
			 */
			typedef __module__ (*initModule)(void *data);

			/**
			 * @typedef deinitModule
			 * @brief defines type of deinit function for library
			 * @note name in the library must be deinitPcProcessModule
			 */
			typedef void (*deinitModule)();
#endif

			/**
			 * @class collection
			 * @brief provides processes management functionality
			 */
			class collection : public job::collection {
			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				collection(collection &);

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
				 * @param action defines action on object destruction if process is running, @see job::collection::onDestructionEnum
				 * @note func must not call `exit` family call
				 */
				virtual unsigned long add(job::routine func,
										  void         *data,
										  short        action);

				/**
				 * add function as a process
				 * @return process identificator
				 * @param func defines function to execute
				 * @param data defines process data
				 * @param action defines action on object destruction if process is running, @see job::collection::onDestructionEnum
				 * @note func must not call `exit` family call
				 * this will immediately execute the process
				 */
				virtual unsigned long addNRun(job::routine  func,
											  void          *data,
											  short         action);

				/**
				 * remove registered process
				 * @param id defines process identificator
				 * @param terminate defines termination condition; if true and process is running stop execution of the process
				 */
				virtual void remove(unsigned long id,
								 bool          terminate = false);

				/**
				 * execute process
				 * @param id defines process identificator
				 * @param force defines run condition; if true and job is running run process anyway
				 */
				virtual void run(unsigned long id,
								 bool          force = false);

				/**
				 * stop process
				 * @param id defines process identificator
				 * @note sends SIGKILL to process
				 */
				virtual void stop(unsigned long id);

				/**
				 * stop all registered processes
				 */
				virtual void stop();

				/**
				 * waits for process termination
				 * @return status of the job
				 * @param id defines process identificator
				 */
				virtual int wait(unsigned long id);

				/**
				 * wait for all registered processes termination
				 */
				virtual void wait();

				/**
				 * @return true if process is running
				 * @param id defines process identificator
				 */
				virtual bool isRunning(unsigned long id) const;

				/**
				 * @return amount of running processes
				 */
				virtual unsigned long running() const;

				/**
				 * @return list of processes in object
				 */
				virtual dodoList<unsigned long> jobs();

#ifdef DL_EXT
				/**
				 * add function as a process from library
				 * @return process identificator
				 * @param module defines path to the library[if not in ldconfig db] or library name
				 * @param data defines process data
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

			  protected:

				/**
				 * @return true if process is running
				 * @param id defines process identificator
				 */
				bool _isRunning(dodoList<__process__ *>::iterator &id) const;

				/**
				 * search processes by identificator
				 * @return true if process has been found
				 * @param id defines process identificator
				 * @note this sets internal class parameter 'current' to found process
				 */
				bool getProcess(unsigned long id) const;

				unsigned long processNum;                           ///< number of registered processes

				mutable dodoList<__process__ *> processes;          ///< identificators of processes

				mutable dodoList<__process__ *>::iterator current;  ///< iterator for list of processes[for matched with getProcess method]
			};
		};
	};
};
#endif
