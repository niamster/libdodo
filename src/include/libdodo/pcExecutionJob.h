/***************************************************************************
 *            pcExecutionJob.h
 *
 *  Wed Oct 07 2009
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

#ifndef _PCEXECUTIONJOB_H_
#define _PCEXECUTIONJOB_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace execution {
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
             * @class job
             * @brief provides interface for job job management
             */
            class job {
              public:

                /**
                 * constructor
                 */
                job();

                /**
                 * copy constructor
                 */
                job(const job &);

                /**
                 * destructor
                 */
                virtual ~job() = 0;

                /**
                 * execute job
                 */
                virtual void run() = 0;

                /**
                 * stop the job
                 */
                virtual void stop() = 0;

                /**
                 * wait for the job termination
                 * @return exit status
                 */
                virtual int wait() = 0;

                /**
                 * @return true if job is running
                 */
                virtual bool isRunning() const = 0;

              protected:

                mutable bool cloned; ///< true if object was cloned
            };
        };
    };
};
#endif
