/***************************************************************************
 *            systemJobs.h
 *
 *  Mon Mar 05 13:01:55 2007
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

#ifndef _systemJobs_H_
#define _systemJobs_H_

#include <libdodo/directives.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @typedef jobFunc defines job function
	 */
	typedef void *(*jobFunc)(void *);

	/**
	 * @class systemJobs provides interface for jobs management
	 */
	class systemJobs
	{
		public:

			/**
			 * destructor
			 */
			virtual ~systemJobs() = 0;

			/**
			 * add function as a job
			 * @return job identificator
			 * @param func defines function to execute
			 * @param data defines job data
			 */
			virtual unsigned long add(jobFunc func, void *data) = 0;

			/**
			 * add function as a job
			 * @return job identificator
			 * @param func defines function to execute
			 * @param data defines job data
			 * @note this will immediately execute the job
			 */
			virtual unsigned long addNRun(jobFunc func, void *data) = 0;

			/**
			 * remove registered job
			 * @param position defines job identificator
			 * @param force defines termination condition; if true and job is running stop execution of the job 
			 */
			virtual void del(unsigned long position, bool force = false) = 0;

			/**
			 * execute job
			 * @param position defines job identificator
			 * @param force defines run condition; if true and job is running run job anyway
			 */
			virtual void run(unsigned long position, bool force = false) = 0;

			/**
			 * stop job
			 * @param position defines job identificator
			 */
			virtual void stop(unsigned long position) = 0;

			/**
			 * stop all registered jobs
			 */
			virtual void stop() = 0;

			/**
			 * wait for job termination
			 * @param position defines job identificator
			 */
			virtual void wait(unsigned long position) = 0;

			/**
			 * wait for all registered jobs termination
			 */
			virtual void wait() = 0;

			/**
			 * @return true if job is running
			 * @param position defines job identificator
			 */
			virtual bool isRunning(unsigned long position) const = 0;

			/**
			 * @return amount of running jobs
			 */
			virtual unsigned long running() const = 0;

			/**
			 * sweep jobs if their time has been already passed
			 */
			virtual void sweepTrash() = 0;

			/**
			 * @return list of jobs in object
			 */
			virtual dodoList<unsigned long> getJobsIds() = 0;

			/**
			 * set maximum execution time
			 * @param position defines job identificator
			 * @param limit defines the limit on executions of the job 
			 */
			virtual void setExecutionLimit(unsigned long position, unsigned long limit = 1) = 0;


#ifdef DL_EXT

			/**
			 * add function as a job from library
			 * @return job identificator
			 * @param module defines path to the library[if not in ldconfig db] or library name
			 * @param data defines job data
			 * @param toInit defines library init data
			 */
			virtual unsigned long add(const dodoString &module, void *data, void *toInit = NULL) = 0;

#endif
	};
};

#endif
