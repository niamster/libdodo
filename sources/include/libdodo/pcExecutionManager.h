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

namespace dodo {
    namespace pc {
        namespace sync {
            class protector;
        };

        namespace execution {
            class job;

            /**
             * @class manager
             * @brief provides interface for jobs management
             */
            class manager {
              public:

                /**
                 * constructor
                 */
                manager();

                /**
                 * destructor
                 */
                ~manager();

                /**
                 * add a job
                 * @return job identificator
                 * @param job defines job for managing
                 */
                unsigned long add(const job &job);

                /**
                 * remove registered job
                 * @param id defines job identificator
                 * @param terminate defines termination condition
                 */
                void remove(unsigned long id,
                            bool          terminate = false);

                /**
                 * execute job
                 * @param id defines job identificator
                 */
                void run(unsigned long id);

                /**
                 * stop job
                 * @param id defines job identificator
                 */
                void stop(unsigned long id);

                /**
                 * stop all registered jobs
                 */
                void stop();

                /**
                 * wait for job termination
                 * @return status of the job
                 * @param id defines job identificator
                 */
                int wait(unsigned long id);

                /**
                 * wait for all registered jobs termination
                 */
                void wait();

                /**
                 * @return true if job is running
                 * @param id defines job identificator
                 */
                bool isRunning(unsigned long id) const;

                /**
                 * @return amount of running jobs
                 */
                unsigned long running() const;

                /**
                 * @return list of jobs in object
                 */
                dodoList<unsigned long> jobs();

                /**
                 * @return job object
                 * @param id defines job identificator
                 */
                execution::job *job(unsigned long id);

              protected:

                dodoMap<unsigned long, execution::job *> handles;  ///< managed jobs

                unsigned long counter;              ///< job id counter

                sync::protector *keeper;            ///< section locker
            };
        };
    };
};

#endif
