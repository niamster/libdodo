/***************************************************************************
 *            pcExecutionProcess.h
 *
 *  Wed Oct 08 2009
 *  Copyright  2009  Ni@m
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

#ifndef _PCEXECUTIONPROCESS_H_
#define _PCEXECUTIONPROCESS_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/pcExecutionJob.h>

namespace dodo {
	namespace pc {
		namespace execution {
			struct __process__;

			/**
			 * @class process
			 * @brief provides process management functionality
			 */
			class process : public job {
			  public:

				/**
				 * constructor
				 * @param func defines function to execute
				 * @param data defines process data
				 * @param action defines action on object destruction if process is running, @see onDestructionEnum
				 * @note func must not call `exit` family call
				 */
				process(routine func,
										  void         *data,
										  short        action);

#ifdef DL_EXT
				/**
				 * constructor
				 * @param module defines path to the library[if not in ldconfig db] or library name
				 * @param data defines process data
				 * @param toInit defines library init data
				 */
				process(const dodoString &module,
										  void             *data,
										  void             *toInit = NULL);
#endif

				/**
				 * copy constructor
				 */
				process(const process &);

				/**
				 * destructor
				 */
				virtual ~process();

				/**
				 * execute process
				 */
				virtual void run();

				/**
				 * stop all registered processes
				 */
				virtual void stop();

				/**
				 * wait for all registered processes termination
				 * @return exit status
				 */
				virtual int wait();

				/**
				 * @return true if process is running
				 */
				virtual bool isRunning() const;

#ifdef DL_EXT
				/**
				 * @struct __module__
				 * @brief defines data that is returned from initModule in the library
				 */
				struct __module__ {
					char          name[64];         ///< name of module
					char          discription[256]; ///< discription of module
					char          hook[64];         ///< name of function in module that will be a hook
					short         action;           ///< action on object destruction, @see onDestructionEnum
				};

				/**
				 * @typedef initModule
				 * @brief defines type of init function for library
				 * @param data defines user data
				 * @note name in the library must be initPcExecutionProcessModule
				 */
				typedef __module__ (*initModule)(void *data);

				/**
				 * @typedef deinitModule
				 * @brief defines type of deinit function for library
				 * @note name in the library must be deinitPcExecutionProcessModule
				 */
				typedef void (*deinitModule)();

				/**
				 * @return info about library
				 * @param module defines path to the library[if not in ldconfig db] or library name
				 * @param toInit defines library init data
				 */
				static __module__ module(const dodoString &module,
										 void             *toInit = NULL);
#endif

			  private:

				mutable __process__  *handle;          ///< process handle
			};
		};
	};
};
#endif
