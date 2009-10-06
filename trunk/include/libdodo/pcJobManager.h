/***************************************************************************
 *            pcJobManager.h
 *
 *  Mon Mar 05 2007
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

#ifndef _PCJOBMANAGER_H_
#define _PCJOBMANAGER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
	namespace pc {
		namespace job {
			/**
			 * @typedef routine
			 * @brief defines job function
			 * @param data defines user data
			 */
			typedef int (*routine)(void *data);

			/**
			 * @enum onDestructionEnum defines action with processes on object destruction
			 */
			enum onDestructionEnum {
				ON_DESTRUCTION_KEEP_ALIVE,
				ON_DESTRUCTION_STOP,     ///< send SIGINT to process
				ON_DESTRUCTION_WAIT
			};

			/**
			 * @class manager
			 * @brief provides interface for jobs management
			 */
			class manager {
			  public:
				/**
				 * destructor
				 */
				virtual ~manager() = 0;

				/**
				 * add function as a job
				 * @return job identificator
				 * @param func defines function to execute
				 * @param data defines job data
				 * @param action defines action on object destruction if process is running, @see job::onDestructionEnum
				 */
				virtual unsigned long add(routine func,
										  void    *data,
										  short        action) = 0;

				/**
				 * add function as a job
				 * @return job identificator
				 * @param func defines function to execute
				 * @param data defines job data
				 * @param action defines action on object destruction if process is running, @see job::onDestructionEnum
				 * @note this will immediately execute the job
				 */
				virtual unsigned long addNRun(routine func,
											  void    *data,
											  short        action) = 0;

				/**
				 * remove registered job
				 * @param id defines job identificator
				 * @param terminate defines termination condition; if true and job is running stop execution of the job
				 */
				virtual void remove(unsigned long id,
								 bool          terminate = false) = 0;

				/**
				 * execute job
				 * @param id defines job identificator
				 * @param force defines run condition; if true and job is running run job anyway
				 */
				virtual void run(unsigned long id,
								 bool          force = false) = 0;

				/**
				 * stop job
				 * @param id defines job identificator
				 */
				virtual void stop(unsigned long id) = 0;

				/**
				 * stop all registered jobs
				 */
				virtual void stop() = 0;

				/**
				 * wait for job termination
				 * @return status of the job
				 * @param id defines job identificator
				 */
				virtual int wait(unsigned long id) = 0;

				/**
				 * wait for all registered jobs termination
				 */
				virtual void wait() = 0;

				/**
				 * @return true if job is running
				 * @param id defines job identificator
				 */
				virtual bool isRunning(unsigned long id) const = 0;

				/**
				 * @return amount of running jobs
				 */
				virtual unsigned long running() const = 0;

				/**
				 * @return list of jobs in object
				 */
				virtual dodoList<unsigned long> jobs() = 0;

#ifdef DL_EXT
				/**
				 * add function as a job from library
				 * @return job identificator
				 * @param module defines path to the library[if not in ldconfig db] or library name
				 * @param data defines job data
				 * @param toInit defines library init data
				 */
				virtual unsigned long add(const dodoString &module,
										  void             *data,
										  void             *toInit = NULL) = 0;
#endif
			};
		};
	};
};
#endif
