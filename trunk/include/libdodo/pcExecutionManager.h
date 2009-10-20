/***************************************************************************
 *            pcExecutionManager.h
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

#ifndef _PCEXECUTIONMANAGER_H_
#define _PCEXECUTIONMANAGER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/pcSyncStack.h>

namespace dodo {
	namespace pc {
		namespace execution {
			/**
			 * @class manager
			 * @brief provides interface for jobs management
			 */
			template <typename T>
			class manager {
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
				 * add a job
				 * @return job identificator
				 * @param job defines job for managing
				 */
				virtual unsigned long add(const T &job);

				/**
				 * remove registered job
				 * @param id defines job identificator
				 * @param terminate defines termination condition
				 */
				virtual void remove(unsigned long id,
								 bool          terminate = false);

				/**
				 * execute job
				 * @param id defines job identificator
				 * @param force defines run condition; if true and job is running run job anyway
				 */
				virtual void run(unsigned long id);

				/**
				 * stop job
				 * @param id defines job identificator
				 */
				virtual void stop(unsigned long id);

				/**
				 * stop all registered jobs
				 */
				virtual void stop();

				/**
				 * wait for job termination
				 * @return status of the job
				 * @param id defines job identificator
				 */
				virtual int wait(unsigned long id);

				/**
				 * wait for all registered jobs termination
				 */
				virtual void wait();

				/**
				 * @return true if job is running
				 * @param id defines job identificator
				 */
				virtual bool isRunning(unsigned long id) const;

				/**
				 * @return amount of running jobs
				 */
				virtual unsigned long running() const;

				/**
				 * @return list of jobs in object
				 */
				virtual dodoList<unsigned long> jobs();

			  protected:

				dodoMap<unsigned long, T> handles; ///< managed jobs

				unsigned long counter; ///< job id counter

				sync::protector *keeper; ///< section locker
			};
		};
	};
};

#include <libdodo/pcExecutionManager.inline>

#endif
