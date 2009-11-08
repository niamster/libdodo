/***************************************************************************
 *            pcExecutionScheduler.h
 *
 *  Sun Nov 08 2009
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

#ifndef _PCEXECUTIONSCHEDULER_H_
#define _PCEXECUTIONSCHEDULER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class protector;
        };

        struct __manager__;

        namespace execution {
            class job;

            /**
             * @class scheduler
             * @brief provides interface for jobs management
             */
            class scheduler {
                friend struct __manager__;

              public:

                /**
                 * constructor
                 */
                scheduler();

                /**
                 * destructor
                 */
                ~scheduler();

                /**
                 * schedule a job
                 * @return job identificator
                 * @param job defines job for scheduling
                 * @param timeout defines timeout of job scheduling in milliseconds
                 * @param repeat defines if job should be rescheduled again
                 */
                unsigned long schedule(execution::job *job,
                                       unsigned long timeout,
                                       bool repeat = false);

                /**
                 * remove registered job
                 * @param id defines job identificator
                 * @param terminate defines termination condition
                 */
                void remove(unsigned long id,
                            bool          terminate = false);

              protected:

                struct __job__ {
                    execution::job *job; ///< job for scheduling
                    unsigned long timeout; ///< timeout for job scheduling
                    unsigned long ts; ///< timestamp of last time job was executed or added
                    bool repeat; ///< if the job is to be rescheduled
                };

                dodoMap<unsigned long, __job__> handles;  ///< managed jobs

                unsigned long counter;              ///< job id counter

                sync::protector *keeper;            ///< section locker

                __manager__ *manager; ///< schedule manager handle
            };
        };
    };
};

#endif
